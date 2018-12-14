#include <iostream>
#include <string>
#include<ctime>
#include <algorithm>
#include <vector>
#include <windows.h>
#include <stdio.h>

using namespace std;


class PCB {
public:
    string process_name; //进程名
    int arrive_time; //到达时间
    int serve_time; //已运行的时间 服务时间
    int length; //估计的需要运行的时间
    int status; // 0就绪 1运行 -1 终止
    int lastExecuteTime = -1; //-1 表示未执行过 , 当status = -1 该值代表完成时间
    int execTime = 0; //一次时间片中执行次数


    PCB(void) {
        arrive_time = rand() % 5;
        serve_time = 0;
        length = max(1, rand() % 8);
        status = 0;
    }

    void print() {
        int T = lastExecuteTime - arrive_time;
        string Ts = (status == -1) ? to_string(T) : "*";
        string fs = (status == -1) ? to_string(lastExecuteTime) : "*";
        string pts = status == -1 ? to_string((T + 0.0f) / serve_time) : "*";
        printf("%2s%12d%12s%12d%12d%12s%12s%12s\r\n",
               process_name.c_str(), arrive_time, trans(status).c_str(), length, serve_time, fs.c_str(), Ts.c_str(),
               pts.c_str());
    }

private:
    string trans(int status) {
        switch (status) {
            case 0 :
                return "OK";
            case 1 :
                return "RUN";
            case -1:
                return "FIN";
            default:
                return "Unknown State";
        }
        return "未知状态";
    }
};

//
class JobManager {
public:
    PCB *pcbs;
    PCB *activedPCB; //活动的pcb
    int currentTime = -1; //一开始的时间为0,++操作
    int size; //传入的pcb大小
    int type; //采用的算法
    int chipTime= 3; //时间片大小

    JobManager(PCB *ps, int length, int t) {
        //初始化
        activedPCB = nullptr;
        size = length;
        type = t;
        for (int i = 0; i < length; i++) {
            pcbs = ps;
            pcbs[i].process_name = 'A' + i;
        }
    }

    /*下一个时刻 一开始为0 时刻*/
    void next() {
        currentTime++; //执行时间+1

        addToWait();

        if (activedPCB != nullptr) {
            //到了下一轮,给之前的PCB加上1的服务时间
            activedPCB->serve_time++;
            // 之前的PCB最终执行时间为当前时刻
            activedPCB->lastExecuteTime = currentTime;
            activedPCB->execTime++;
            if (activedPCB->serve_time == activedPCB->length) {
                activedPCB->status = -1;
                finishJob(activedPCB);
                activedPCB = nullptr;
            }
        }

        //策略
        findNextPCB(type);

        if (activedPCB == nullptr) {
            printProcesses();
            return;
        }

        if (activedPCB->status == 0) {
            activedPCB->status = 1;
            //切换到运行的状态
            //下一时刻到达再修改
        }
        printProcesses();
    }

    bool allJobFinish() {
        for (int i = 0; i < size; i++) {
            if (pcbs[i].status != -1) return false;
        }
        return true;
    }

    void printProcesses() {
        cout << "当前时间: 第" + to_string(currentTime) + "秒" << endl;
        cout << "进程    到达时间     进程状态     进程长度     服务时间    完成时间    周转时间     带权时间" << endl;
        for (int i = 0; i < size; i++) {
            pcbs[i].print();
        }
        cout << "==========================================================================" << endl;
    }

private:
    //等待队列
    vector<PCB *> waitingPcbs;

    //模拟一个进程的到达
    void addToWait() {
        for (int i = 0; i < size; i++) {
            if (pcbs[i].arrive_time == currentTime) {
                waitingPcbs.push_back(&pcbs[i]);
            }
        }
    }

    //根据当前的PCB查找下一个执行的进程
    PCB *findNextPCB(int type) {
        switch (type) {
            // 短作业优先
            case 0:
                activedPCB = findNextPCBShort(activedPCB);
                break;
                //轮转调度算法
            case 1:
                activedPCB = findNextPCBRR(activedPCB);
                break;
                //高响应比优先
            case 2:
                activedPCB = findNextPCBHR(activedPCB);
        }
        return activedPCB;
    }

    PCB *findNextPCBShort(PCB *currentPCB) {

        //对于短作业优先算法,需要下一个作业就是最短作业,并且是抢占式的
        sort(waitingPcbs.begin(), waitingPcbs.end(), JobManager::comp);

        for (int i = 0; i < waitingPcbs.size(); i++) {
            if (waitingPcbs[i]->status == 0 || waitingPcbs[i]->status == 1) {
                //如果下一个要执行的进程(第一个就绪态或运行中的进程)不是刚刚的进程,且刚刚的进程没执行完,那么置为就绪态,并返回下一个要执行的进程
                if (currentPCB != nullptr && currentPCB != waitingPcbs[i] && currentPCB->status != -1) {
                    currentPCB->status = 0;
                }
                return waitingPcbs[i];
            }
        }
        return nullptr;
    }

    PCB *findNextPCBRR(PCB *currentPCB) {
        //对于RR轮转法,需要指定时间片大小
//        if(currentPCB->lastExecuteTime)
        if(currentPCB == nullptr){
            if(!waitingPcbs.empty()) return waitingPcbs[0];
            return nullptr;
        }

        if( currentPCB->execTime < chipTime){
            return currentPCB;
        }else{
            PCB* head = waitingPcbs[0];
            waitingPcbs.erase(waitingPcbs.begin());
            waitingPcbs.push_back(head);
            head->execTime = 0 ;
            return *waitingPcbs.begin();
        }
    }

    PCB *findNextPCBHR(PCB *currentPCB) {
        //对于高响应比优先算法,要找到最高响应比的进程运行
        //RP = (等待时间+要求服务时间)/要求服务时间
        if (waitingPcbs.empty()) return nullptr;

        int maxIndex = 0;
        double ratio = 0;
        for (int i = 0; i < waitingPcbs.size(); i++) {
            if (waitingPcbs[i]->status == -1) continue;
            double cur = (currentTime + waitingPcbs[i]->serve_time + 0.0f) / waitingPcbs[i]->serve_time;
            if (cur > ratio) {
                ratio = cur;
                maxIndex = i;
            }
        }

        PCB *find = waitingPcbs[maxIndex];
        if (currentPCB != nullptr && currentPCB != find && currentPCB->status != -1) {
            currentPCB->status = 0;
        }
        return find;
    }

    void finishJob(PCB* job){
        for (int i = 0; i < waitingPcbs.size(); i++) {
            if (waitingPcbs[i] == job) {
                waitingPcbs.erase(waitingPcbs.begin() + i);
                break;
            }
        }
    }

    //根据长度排序
    static bool comp(PCB *a, PCB *b) {
        return a->length < b->length;
    }
};

int main() {
    srand(time(0));
    PCB pcbs[5];
    JobManager manager = JobManager(pcbs, 5, 1);
    while (!manager.allJobFinish()) {
        manager.next();
        Sleep(500);
//        system("cls");
    }
    manager.printProcesses();
    getchar();
    return 0;
}




