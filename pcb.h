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
    string process_name; //进程名
    int arrive_time; //到达时间
    int serve_time; //已运行的时间 服务时间
    int length; //估计的需要运行的时间
    int status; // 0就绪 1运行 -1 终止 -2阻塞
    int lastExecuteTime = -1; //-1 表示未执行过 , 当status = -1 该值代表完成时间
    int execTime = 0; //一次时间片中执行次数
    int pid; // 进程在系统中的编号

    static const int WAIT = 0;
    static const int RUN = 1;
    static const int FIN = -1;

    int alloc[RES_SIZE]; //已分配
    int request[RES_SIZE] ;//进程到达请求的大小
    int max[RES_SIZE] ; //最大需求
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
            // 随机请求一些, 要<=缺的
            // 随机初始化已分配的一些
            int randReq = rand() % 10;
            randReq < 0 ? randReq = 0 : randReq > max[i] - randAlloc ? randReq = max[i] - randAlloc : randReq= randReq ;
            request[i] = randReq;
        }
    }

    void allocate(int index, int count) {
        if(count > request[index]) cout << "分配的超过请求的" << endl;
        if(count + alloc[index] > max[index] ) cout << "分配的超过最大的所需的" << endl;

        alloc[index] += count; //分配

        //更改下次请求的数量

            // 随机请求一些, 要<=缺的
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
        return "未知状态";
    }
};
#endif //UNTITLED_PCB_H
