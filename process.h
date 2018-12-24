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

#define CHIP_SIZE 3 //ʱ��Ƭ��С

#include "pcb.h"

/**
 * ������
 */
class ProcessManager;

/**
 * ѡ����ȵĲ���!
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
    //���ݳ�������
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
    PCB *activedPCB; //���pcb
    int currentTime = -1; //һ��ʼ��ʱ��Ϊ0,++����
    int size; //�����pcb��С
    //�ȴ�����
    vector<PCB *> waitingPcbs;
    int working[3] = {10, 15, 12};

public:
    /**
     * ��Ҫ����,�Դ����pcb���е���
     * @param ps
     * @param length  ps�ĳ�
     * @param strategy �����㷨�ĵ��Ȳ���
     */
    ProcessManager(PCB *ps, int length, ProcessSelectStrategy *strategy) {
        //��ʼ��
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
        cout << "��ǰʱ��: ��" + to_string(currentTime) + "��" << endl;
        cout << "����    ����ʱ��     ����״̬     ���̳���     ����ʱ��    ���ʱ��    ��תʱ��     ��Ȩʱ��" << endl;
        for (int i = 0; i < size; i++) {
            pcbs[i].print();
        }
        cout << "==========================================================================" << endl;
    }

    //ģ��һ�����̵ĵ���
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
    //����RR��ת��,��Ҫָ��ʱ��Ƭ��С
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
    //���ڸ���Ӧ�������㷨,Ҫ�ҵ������Ӧ�ȵĽ�������
    //RP = (�ȴ�ʱ��+Ҫ�����ʱ��)/Ҫ�����ʱ��
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
    //���ڶ���ҵ�����㷨,��Ҫ��һ����ҵ���������ҵ,��������ռʽ��
    sort(waitingPcbs.begin(), waitingPcbs.end(), Strategy_HR::comp);

    for (int i = 0; i < waitingPcbs.size(); i++) {
        if (waitingPcbs[i]->status == PCB::WAIT || waitingPcbs[i]->status == PCB::RUN) {
            //�����һ��Ҫִ�еĽ���(��һ������̬�������еĽ���)���ǸոյĽ���,�ҸոյĽ���ûִ����,��ô��Ϊ����̬,��������һ��Ҫִ�еĽ���
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
    //����RR��ת��,��Ҫָ��ʱ��Ƭ��С
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