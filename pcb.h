//
// Created by c on 2018/12/24.
//

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include "SysRes.h"

#ifndef UNTITLED_PCB_H
#define UNTITLED_PCB_H

#define RES_SIZE 3

using namespace std;

class PCB {
public:
    string process_name; //������
    int arrive_time; //����ʱ��
    int serve_time; //�����е�ʱ�� ����ʱ��
    int length; //���Ƶ���Ҫ���е�ʱ��
    int status; // 0���� 1���� -1 ��ֹ -2����
    int lastExecuteTime = -1; //-1 ��ʾδִ�й� , ��status = -1 ��ֵ�������ʱ��
    int execTime = 0; //һ��ʱ��Ƭ��ִ�д���
    int pid; // ������ϵͳ�еı��

    static const int WAIT = 0;
    static const int RUN = 1;
    static const int FIN = -1;

    int alloc[RES_SIZE]; //�ѷ���
    int request[RES_SIZE] ;//���̵�������Ĵ�С
    int max[RES_SIZE] ; //�������
    //lack  max-alloc

public:

    PCB() {
        arrive_time = rand() % 5;
        serve_time = 0;
        length = std::max(1, rand() % 8);
        status = 0;

        for(int i = 0 ; i < RES_SIZE ; i++){
            alloc[i] = 0;
            max[i] = rand() % 10;
            int randAlloc = 0;
            // �������һЩ, Ҫ<=ȱ��
            // �����ʼ���ѷ����һЩ
            int randReq = rand() % 10;
            randReq < 0 ? randReq = 0 : randReq > max[i] - randAlloc ? randReq = max[i] - randAlloc : randReq= randReq ;
            request[i] = randReq;
        }
    }

    void allocate(int index, int count) {
        if(count > request[index]) cout << "����ĳ��������" << endl;
        if(count + alloc[index] > max[index] ) cout << "����ĳ������������" << endl;

        alloc[index] += count; //����

        //�����´����������

            // �������һЩ, Ҫ<=ȱ��
            int randReq = rand() % 5;
            randReq < 0 ? randReq = 0 : randReq > max[index] - alloc[index] ? randReq = max[index] - alloc[index] : randReq= randReq ;
            request[index] = randReq;

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

    void printBank() {
        int T = lastExecuteTime - arrive_time;
        string Ts = status == FIN ? to_string(T) : "*";
        string fs = status == FIN ? to_string(lastExecuteTime) : "*";
        string pts = status == FIN ? to_string((T + 0.0f) / serve_time) : "*";
        printf("%2s%12d%12s%7d%12d%7d%7d%7d%7d%7d%7d%7d%7d%7d\r\n",
               process_name.c_str(), arrive_time, trans(status).c_str(),
               length,serve_time,alloc[0],alloc[1],alloc[2],request[0],request[1],request[2],max[0],max[1],max[2]);
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
            case -2:
                return "BLOCK";
            default:
                return "Unknown State";
        }
        return "δ֪״̬";
    }
};
#endif //UNTITLED_PCB_H
