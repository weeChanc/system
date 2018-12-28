//
// Created by c on 2018/12/15.
//
#include <iostream>
#include <string>
#include<ctime>
#include <algorithm>
#include <vector>
#include <windows.h>
#include <stdio.h>

#ifndef PROCESS_CONTROL_H_CWC
#define PROCESS_CONTROL_H_CWC
#define CHIP_SIZE 1 //时间片大小
#include "pcb.h"

/**
 * 调度器
 */
class ProcessManager;

/**
 * 选择调度的策略!
 */
class ProcessSelectStrategy {
public :
    virtual PCB *findNextPCB(ProcessManager& manager) = 0;
};

class Strategy_RR : public ProcessSelectStrategy {
public:
    PCB *findNextPCB(ProcessManager& manager) override;
};

class Strategy_HR : public ProcessSelectStrategy {
    PCB *findNextPCB(ProcessManager& manager) override;
    //根据长度排序
public:
    static bool comp(PCB *a, PCB *b) {
        return a->length < b->length;
    }
};

class Strategy_SF : public ProcessSelectStrategy {
public:
    PCB *findNextPCB(ProcessManager& manager) override;
};

class Strategy_BANK : public ProcessSelectStrategy {
public:
    PCB *findNextPCB(ProcessManager& manager) override;

    bool canAlloc( int* working, PCB *check);

    bool isSafe(const int working[], vector<PCB *> waitingPcbs);

    bool canFeedLack(const int working[], PCB pcb);
};

class ProcessManager {

private:
    ProcessSelectStrategy *strategy = nullptr;

public :
    PCB *pcbs;
    PCB *activedPCB; //活动的pcb
    int currentTime = -1; //一开始的时间为0,++操作
    int size; //传入的pcb大小
    //等待队列
    vector<PCB *> waitingPcbs;
    int working[3] = {10, 15, 12};

public:
    /**
     * 主要用来,对传入的pcb进行调度
     * @param ps
     * @param length  ps的长
     * @param strategy 调度算法的调度策略
     */
    ProcessManager(PCB *ps, int length, ProcessSelectStrategy *strategy) {
        //初始化
        activedPCB = nullptr;
        size = length;
        pcbs = ps;
        if (strategy != nullptr) {
            this->strategy = strategy;
        } else {
            this->strategy = new Strategy_RR();
        }

        for (int i = 0; i < 5; i++) {
            pcbs[i].process_name = 'A' + i;
            pcbs[i].pid = i;
        }
        waitingPcbs.clear();
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
        this->activedPCB = strategy->findNextPCB(*this);


        if (activedPCB == nullptr) {
            return;
        }
        if (activedPCB->status != PCB::FIN) {
            activedPCB->status = PCB::RUN;
        }

        return;
    }

    static void show(int process_count, ProcessSelectStrategy *strategy) {
        srand(time(0));
        PCB pcbs[process_count];

        for (int i = 0; i < process_count; i++) {
            pcbs[i].process_name = 'A' + i;
            pcbs[i].pid = i;
        }

        ProcessManager manager = ProcessManager(pcbs, process_count, strategy);

        while (!manager.allJobFinish()) {
            manager.next();
            manager.printProcesses();
            Sleep(500);
        }

    }

    static void showBank(int process_count, ProcessSelectStrategy *strategy) {
        srand(time(0));
        PCB pcbs[process_count];

        for (int i = 0; i < process_count; i++) {
            pcbs[i].process_name = 'A' + i;
            pcbs[i].pid = i;
        }

        ProcessManager manager = ProcessManager(pcbs, process_count, strategy);
        while (!manager.allJobFinish()) {
            manager.next();
            manager.printBankProcess();
            Sleep(500);
        }

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

    void printBankProcess() {
        cout << "当前时间: 第" + to_string(currentTime) + "秒" << endl;
        cout << "进程    到达时间     进程状态   进程长度  服务时间   分配A  分配B  分配C   请求A   请求B  请求C  最大A  最大B  最大C" << endl;
        for (int i = 0; i < size; i++) {
            pcbs[i].printBank();
        }
        cout << "==========================================================================" << endl;
    }

    //模拟一个进程的到达
    void addToWait() {
        for (int i = 0; i < size; i++) {
            if (pcbs[i].arrive_time == currentTime) {
                waitingPcbs.push_back(&pcbs[i]);
            }
        }
    }

    void finishJob(PCB *job) {
        for (int i = 0; i < waitingPcbs.size(); i++) {
            if (waitingPcbs[i] == job) {
                waitingPcbs.erase(waitingPcbs.begin() + i);
                break;
            }
        }
        for(int i = 0 ; i< 3 ;i++){
            working[i] += job->alloc[i]; // 归还资源
        }
    }
};

PCB *Strategy_RR::findNextPCB(ProcessManager& manager) {
    PCB *currentPCB = manager.activedPCB;
    //对于RR轮转法,需要指定时间片大小
    if (currentPCB == nullptr) {
        if (!manager.waitingPcbs.empty()) return manager.waitingPcbs[0];
        return nullptr;
    }

    if (currentPCB->execTime < CHIP_SIZE) {
        return currentPCB;
    } else {
        PCB *head = manager.waitingPcbs[0];
        manager.waitingPcbs.erase(manager.waitingPcbs.begin());
        manager.waitingPcbs.push_back(head);
        head->status = 0;
        head->execTime = 0;
        return *manager.waitingPcbs.begin();
    }
}

PCB *Strategy_HR::findNextPCB(ProcessManager& manager) {
    //对于高响应比优先算法,要找到最高响应比的进程运行
    //RP = (等待时间+要求服务时间)/要求服务时间
    PCB *currentPCB = manager.activedPCB;
    vector<PCB *> waitingPcbs = manager.waitingPcbs;
    int currentTime = manager.currentTime;

    if (waitingPcbs.empty()) return nullptr;

    int maxIndex = 0;
    double ratio = 0;
    for (int i = 0; i < waitingPcbs.size(); i++) {
        if (waitingPcbs[i]->status == PCB::FIN) continue;
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

PCB *Strategy_SF::findNextPCB(ProcessManager& manager) {
    PCB *currentPCB = manager.activedPCB;
    vector<PCB *> waitingPcbs = manager.waitingPcbs;
    //对于短作业优先算法,需要下一个作业就是最短作业,并且是抢占式的
    sort(waitingPcbs.begin(), waitingPcbs.end(), Strategy_HR::comp);

    for (int i = 0; i < waitingPcbs.size(); i++) {
        if (waitingPcbs[i]->status == PCB::WAIT || waitingPcbs[i]->status == PCB::RUN) {
            //如果下一个要执行的进程(第一个就绪态或运行中的进程)不是刚刚的进程,且刚刚的进程没执行完,那么置为就绪态,并返回下一个要执行的进程
            if (currentPCB != nullptr && currentPCB != waitingPcbs[i] && currentPCB->status != -1) {
                currentPCB->status = 0;
            }
            return waitingPcbs[i];
        }
    }
    return nullptr;
}

bool Strategy_BANK::canAlloc( int* working, PCB *check) {

    for (int i = 0; i < 3; i++) {
        if (check->request[i] > working[i]) {
            return false;
        }
    }
    return true;
}

bool Strategy_BANK::canFeedLack(const int working[], PCB pcb) {
    for (int i = 0; i < 3; i++) {
        if (pcb.max - pcb.alloc > working[i]) {
            return false;
        }
    }

    //feed


    return true;
}

bool Strategy_BANK::isSafe(const int working[], vector<PCB *> waitingPcbs) {
    int tempWork[3] = {0, 0, 0};

    for (int i = 0; i < 3; i++) {
        tempWork[i] = working[i];
    }

    for (int i = 0; i < waitingPcbs.size(); i++) {
        PCB *current = waitingPcbs[i];
        if (canFeedLack(tempWork, *current)) {

//            for (int k = 0; k < 3; ++k) {
//                tempWork[k] = tempWork[k] + (current->max[i] + current->alloc[i]);
//            }

            for (int j = 0; j < 3; j++) {
                int max =current->alloc[j];
                tempWork[j] = tempWork[j] + max;
            }

            waitingPcbs.erase(waitingPcbs.begin() + i);
            if (isSafe(tempWork, waitingPcbs)) {
                return true;
            } else {
                waitingPcbs.insert(waitingPcbs.begin() + i, current);
            }
        }
        if (i == waitingPcbs.size()) return false;
    }

    return true;
}


PCB *Strategy_BANK::findNextPCB(ProcessManager& manager) {
    PCB *currentPCB = manager.activedPCB;
    PCB *check;

    for(int i = 0 ; i < manager.waitingPcbs.size(); i++){
        cout << manager.waitingPcbs[i]->process_name << "   ";
    }

    cout << endl;

    for(int i = 0 ; i < 3 ; i++){
        cout << manager.working[i] << "   ";
    }

    if (currentPCB != nullptr && currentPCB->execTime < CHIP_SIZE) {
        return currentPCB;
    } else {
        if (manager.waitingPcbs.empty()) return nullptr;
        PCB *head = manager.waitingPcbs[0];
        head->status = 0; // 就绪
        manager.waitingPcbs.erase(manager.waitingPcbs.begin());
        manager.waitingPcbs.push_back(head);
        head->execTime = 0;
    }

    //check
    for (int j = 0; j < manager.waitingPcbs.size(); j++) {
        check = *manager.waitingPcbs.begin();
        //可以分配就分配

        if (canAlloc(manager.working, check)) {
            //可以分配就分配
            for (int i = 0; i < 3; i++) {
                int req = check->request[i];
                check->allocate(i, req);
                manager.working[i] -= req;
            }

            if (!isSafe(manager.working, manager.waitingPcbs)) {
                cout << "不安全" << endl;
                for (int i = 0; i < 3; i++) {
                    check->alloc[i] -= check->request[i];
                    manager.working[i] += check->request[i];
                }

                manager.waitingPcbs.erase(manager.waitingPcbs.begin());
                check->status = -2;//阻塞;
                manager.waitingPcbs.push_back(check);
            } else {
                return check;
            }

        } else {
            manager.waitingPcbs.erase(manager.waitingPcbs.begin());
            check->status = -2;//阻塞;
            manager.waitingPcbs.push_back(check);
        }
    }
    return nullptr;
}


#endif //PROCESS_CONTROL_H_CWC