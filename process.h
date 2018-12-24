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

#define CHIP_SIZE 3 //时间片大小

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
    virtual PCB *findNextPCB(ProcessManager manager) = 0;
};

class Strategy_RR : public ProcessSelectStrategy {
public:
    PCB *findNextPCB(ProcessManager manager) override;
};

class Strategy_HR : public ProcessSelectStrategy {
    PCB *findNextPCB(ProcessManager manager) override;
    //根据长度排序
public:
    static bool comp(PCB *a, PCB *b) {
        return a->length < b->length;
    }
};

class Strategy_SF : public ProcessSelectStrategy {
public:
    PCB *findNextPCB(ProcessManager manager) override;
};

class Strategy_BANK : public ProcessSelectStrategy {
public:
    PCB *findNextPCB(ProcessManager manager) override;
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
            printProcesses();
            return;
        }
        if (activedPCB->status != PCB::FIN) {
            activedPCB->status = PCB::RUN;
        }

        printProcesses();
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
            Sleep(500);
        }
        manager.printProcesses();
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
    }
};

PCB *Strategy_RR::findNextPCB(ProcessManager manager) {
    PCB *currentPCB = manager.activedPCB;
    vector<PCB *> waitingPcbs = manager.waitingPcbs;
    //对于RR轮转法,需要指定时间片大小
    if (currentPCB == nullptr) {
        if (!waitingPcbs.empty()) return waitingPcbs[0];
        return nullptr;
    }

    if (currentPCB->execTime < CHIP_SIZE) {
        return currentPCB;
    } else {
        PCB *head = waitingPcbs[0];
        waitingPcbs.erase(waitingPcbs.begin());
        waitingPcbs.push_back(head);
        head->execTime = 0;
        return *waitingPcbs.begin();
    }
}

PCB *Strategy_HR::findNextPCB(ProcessManager manager) {
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

PCB *Strategy_SF::findNextPCB(ProcessManager manager) {
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

PCB *Strategy_BANK::findNextPCB(ProcessManager manager) {
    PCB *currentPCB = manager.activedPCB;
    vector<PCB *> waitingPcbs = manager.waitingPcbs;
    PCB* check;
    //对于RR轮转法,需要指定时间片大小
    if (currentPCB == nullptr) {
        if (!waitingPcbs.empty()) {
            check = waitingPcbs[0];
        }
    }

    if (currentPCB->execTime < CHIP_SIZE) {
        check = currentPCB;
    } else {
        PCB *head = waitingPcbs[0];
        waitingPcbs.erase(waitingPcbs.begin());
        waitingPcbs.push_back(head);
        head->execTime = 0;
        return *waitingPcbs.begin();
    }
}


#endif //PROCESS_CONTROL_H_CWC