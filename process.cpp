#include <iostream>
#include <string>
#include<ctime>
#include <algorithm>
#include <vector>
#include <windows.h>
#include <stdio.h>

#include "process.h"

using namespace std;

int main() {

    ProcessSelectStrategy* SHORT_FIRST = new Strategy_SF();
    ProcessSelectStrategy* HIGHT_RESPONSE_FIRST = new Strategy_HR();
    ProcessSelectStrategy* ROTATE = new Strategy_RR();
    ProcessSelectStrategy* BANK_STRATEGY = new Strategy_BANK();

    ProcessManager::show(5, ROTATE);
//    ProcessManager::show(5, ROTATE);
//    ProcessManager::show(5, HIGHT_RESPONSE_FIRST);

    getchar();
    return 0;
}

