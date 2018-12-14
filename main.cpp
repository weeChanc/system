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
    string process_name; //������
    int arrive_time; //����ʱ��
    int serve_time; //�����е�ʱ�� ����ʱ��
    int length; //���Ƶ���Ҫ���е�ʱ��
    int status; // 0���� 1���� -1 ��ֹ
    int lastExecuteTime = -1; //-1 ��ʾδִ�й� , ��status = -1 ��ֵ�������ʱ��
    int execTime = 0; //һ��ʱ��Ƭ��ִ�д���


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
        return "δ֪״̬";
    }
};

//
class JobManager {
public:
    PCB *pcbs;
    PCB *activedPCB; //���pcb
    int currentTime = -1; //һ��ʼ��ʱ��Ϊ0,++����
    int size; //�����pcb��С
    int type; //���õ��㷨
    int chipTime= 3; //ʱ��Ƭ��С

    JobManager(PCB *ps, int length, int t) {
        //��ʼ��
        activedPCB = nullptr;
        size = length;
        type = t;
        for (int i = 0; i < length; i++) {
            pcbs = ps;
            pcbs[i].process_name = 'A' + i;
        }
    }

    /*��һ��ʱ�� һ��ʼΪ0 ʱ��*/
    void next() {
        currentTime++; //ִ��ʱ��+1

        addToWait();

        if (activedPCB != nullptr) {
            //������һ��,��֮ǰ��PCB����1�ķ���ʱ��
            activedPCB->serve_time++;
            // ֮ǰ��PCB����ִ��ʱ��Ϊ��ǰʱ��
            activedPCB->lastExecuteTime = currentTime;
            activedPCB->execTime++;
            if (activedPCB->serve_time == activedPCB->length) {
                activedPCB->status = -1;
                finishJob(activedPCB);
                activedPCB = nullptr;
            }
        }

        //����
        findNextPCB(type);

        if (activedPCB == nullptr) {
            printProcesses();
            return;
        }

        if (activedPCB->status == 0) {
            activedPCB->status = 1;
            //�л������е�״̬
            //��һʱ�̵������޸�
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
        cout << "��ǰʱ��: ��" + to_string(currentTime) + "��" << endl;
        cout << "����    ����ʱ��     ����״̬     ���̳���     ����ʱ��    ���ʱ��    ��תʱ��     ��Ȩʱ��" << endl;
        for (int i = 0; i < size; i++) {
            pcbs[i].print();
        }
        cout << "==========================================================================" << endl;
    }

private:
    //�ȴ�����
    vector<PCB *> waitingPcbs;

    //ģ��һ�����̵ĵ���
    void addToWait() {
        for (int i = 0; i < size; i++) {
            if (pcbs[i].arrive_time == currentTime) {
                waitingPcbs.push_back(&pcbs[i]);
            }
        }
    }

    //���ݵ�ǰ��PCB������һ��ִ�еĽ���
    PCB *findNextPCB(int type) {
        switch (type) {
            // ����ҵ����
            case 0:
                activedPCB = findNextPCBShort(activedPCB);
                break;
                //��ת�����㷨
            case 1:
                activedPCB = findNextPCBRR(activedPCB);
                break;
                //����Ӧ������
            case 2:
                activedPCB = findNextPCBHR(activedPCB);
        }
        return activedPCB;
    }

    PCB *findNextPCBShort(PCB *currentPCB) {

        //���ڶ���ҵ�����㷨,��Ҫ��һ����ҵ���������ҵ,��������ռʽ��
        sort(waitingPcbs.begin(), waitingPcbs.end(), JobManager::comp);

        for (int i = 0; i < waitingPcbs.size(); i++) {
            if (waitingPcbs[i]->status == 0 || waitingPcbs[i]->status == 1) {
                //�����һ��Ҫִ�еĽ���(��һ������̬�������еĽ���)���ǸոյĽ���,�ҸոյĽ���ûִ����,��ô��Ϊ����̬,��������һ��Ҫִ�еĽ���
                if (currentPCB != nullptr && currentPCB != waitingPcbs[i] && currentPCB->status != -1) {
                    currentPCB->status = 0;
                }
                return waitingPcbs[i];
            }
        }
        return nullptr;
    }

    PCB *findNextPCBRR(PCB *currentPCB) {
        //����RR��ת��,��Ҫָ��ʱ��Ƭ��С
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
        //���ڸ���Ӧ�������㷨,Ҫ�ҵ������Ӧ�ȵĽ�������
        //RP = (�ȴ�ʱ��+Ҫ�����ʱ��)/Ҫ�����ʱ��
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

    //���ݳ�������
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




