////
//// Created by c on 2018/12/15.
////
//
//#include "bank.h"
//
//
//
//void ProcessManger_BANK::next() {
//    ProcessManager::next();
//}
//
//ProcessManger_BANK::ProcessManger_BANK(PCB *ps, int length, int t, int chipS, int (*need)[-1], int (*max)[-1], int (*alloc)[-1],
//                     int (*avaliable)[-1]) {
//
//}
//
//
#include <iostream>
#include <string>
#include<ctime>
#include <algorithm>
#include <vector>
#include <windows.h>
#include <stdio.h>
#include "process.h"
#include "bank.h"

using namespace std;

int main() {
    srand(time(0));
    PCB pcbs[5];

    int **need = (int **) malloc(5 * sizeof(int*));
    int **max = (int **) malloc(5 * sizeof(int*));
    int *avaliable = (int *) malloc(5 * sizeof(int*));
    int **alloc = (int **) malloc(5 * sizeof(int*));
//
    for (int i = 0; i < 3; i++) {
        avaliable[i] = std::max(15, rand() % 25);
    }

    for (int i = 0; i < 5; i++) {
        int* PI_Alloc = new int[3];
        int* PI_Max = new int[3]; //当前线程
        int* PI_Need = new int[3];
        for (int j = 0; j < 3; j++) {
            PI_Alloc[j] = rand() % 12;
            PI_Max[j] = std::max(3, rand() % 6);
            PI_Need[j] = PI_Max[j] - PI_Alloc[j];
        }

        *(alloc + i) = PI_Alloc;
        *(need + i) = PI_Need;
        *(max + i) = PI_Max;
    }


    ProcessManger_BANK *m = new ProcessManger_BANK(pcbs, 5, ProcessManger_BANK::ROTATE, 3, need, max, alloc, avaliable);

    m->next();
    m->next();
    m->next();
    m->next();

//    m->show(5,2,4);
    getchar();
    return 0;

}

