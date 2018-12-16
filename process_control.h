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

#ifndef UNTITLED_PROCESS_CONTROL_H
#define UNTITLED_PROCESS_CONTROL_H

#endif //UNTITLED_PROCESS_CONTROL_H
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


    static const int WAIT = 0;
    static const int RUN = 1;
    static const int FIN = -1;


    PCB(void) {
        arrive_time = rand() % 5;
        serve_time = 0;
        length = max(1, rand() % 8);
        status = 0;
    }

    void print() {
        int T = lastExecuteTime - arrive_time;
        string Ts = status == FIN ? to_string(T) : "*";
        string fs = status == FIN ? to_string(lastExecuteTime) : "*";
        string pts = status == FIN ? to_string((T + 0.0f) / serve_time) : "*";
        printf("%2s%12d%12s%12d%12d%12s%12s%12s\r\n",
               process_name.c_str(), arrive_time, trans(status).c_str(), length, serve_time, fs.c_str(), Ts.c_str(),
               pts.c_str());
    }

private:
    string trans(int status) {
        switch (status) {
            case 0 :
                return "WAIT";
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

class JobManager {

protected :PCB *pcbs;
    PCB *activedPCB; //���pcb
    int currentTime = -1; //һ��ʼ��ʱ��Ϊ0,++����
    int size; //�����pcb��С
    int type; //���õ��㷨
    int chipTime = 3; //ʱ��Ƭ��С

public:
    static const int SHORT_FIRST = 0;
    static const int HIGHT_RESPONSE_FIRST = 2;
    static const int ROTATE = 1;

    JobManager(PCB *ps, int length, int t,int chipS) {
        //��ʼ��
        activedPCB = nullptr;
        size = length;
        type = t;
        chipTime = chipS;

        for (int i = 0; i < length; i++) {
            pcbs = ps;
            pcbs[i].process_name = 'A' + i;
        }
    }

    virtual /*��һ��ʱ�� һ��ʼΪ0 ʱ��*/
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

        if(activedPCB->status != PCB::FIN){
            activedPCB->status = PCB::RUN;
        }

        printProcesses();
        return;
    }

    static void show(int process_count,int type,int chipsize){
        srand(time(0));
        PCB pcbs[process_count];
        JobManager manager = JobManager(pcbs, process_count, type,chipsize);
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
            case SHORT_FIRST:
                activedPCB = findNextPCBShort(activedPCB);
                break;
                //��ת�����㷨
            case ROTATE:
                activedPCB = findNextPCBRR(activedPCB);
                break;
                //����Ӧ������
            case HIGHT_RESPONSE_FIRST:
                activedPCB = findNextPCBHR(activedPCB);
        }
        return activedPCB;
    }

    PCB *findNextPCBShort(PCB *currentPCB) {

        //���ڶ���ҵ�����㷨,��Ҫ��һ����ҵ���������ҵ,��������ռʽ��
        sort(waitingPcbs.begin(), waitingPcbs.end(), JobManager::comp);

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

    PCB *findNextPCBRR(PCB *currentPCB) {
        //����RR��ת��,��Ҫָ��ʱ��Ƭ��С
        if (currentPCB == nullptr) {
            if (!waitingPcbs.empty()) return waitingPcbs[0];
            return nullptr;
        }

        if (currentPCB->execTime < chipTime) {
            return currentPCB;
        } else {
            PCB *head = waitingPcbs[0];
            waitingPcbs.erase(waitingPcbs.begin());
            waitingPcbs.push_back(head);
            head->execTime = 0;
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

    void finishJob(PCB *job) {
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