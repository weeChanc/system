////
//// Created by c on 2018/12/15.
////
//
//#include "bank.h"
//
//
//
//void Manager_R::next() {
//    JobManager::next();
//}
//
//Manager_R::Manager_R(PCB *ps, int length, int t, int chipS, int (*need)[-1], int (*max)[-1], int (*alloc)[-1],
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

using namespace std;

int main() {
//    int a [3][3];
//    for (int i = 0; i < 3; ++i) {
//        for (int j = 0; j < 3; ++j) {
//            a[i][j] = i + j ;
//        }
//    }
//
//
//    cout << &a << endl;
//    cout << &a[0][0] << endl;
//
//
//    for (int i = 0; i < 3; ++i) {
//        for (int j = 0; j < 3; ++j) {
//            cout << a[i][j];
//        }
//        cout << "" << endl;
//    }

    int a[3][3] ;
    int * pt = (int*)a;
    // int b[4][3];
    // pt[i][j] == pt[i * (3) + j];

    cout << pt[0] << "" << pt[1] << endl;

    return 0;
}
//
//
//
