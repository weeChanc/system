#include <iostream>
#include <string>
#include<ctime>
#include <algorithm>
#include <vector>
#include <windows.h>
#include <stdio.h>

#include "process_control.h"

using namespace std;



int main() {

    JobManager::show(5,JobManager::SHORT_FIRST,1);

    JobManager::show(5,JobManager::ROTATE,5);

    JobManager::show(5,JobManager::HIGHT_RESPONSE_FIRST,5);

    getchar();
    return 0;
}




