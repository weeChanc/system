//
// Created by c on 2018/12/24.
//
#include <stdio.h>
#include <string>
#ifndef UNTITLED_SYSTEMRESOURCE_H
#define UNTITLED_SYSTEMRESOURCE_H

using namespace std;
class SystemRes{

public:
    string resName;
    int resId = 0 ;
    int length = 0 ;
    SystemRes( int resId) : resId(resId) {}

public:
    void setResCount(int count ){
        this->length = count;
    }

    int getRes(int reqSize){
        if(length - reqSize < 0){
            return -1;
        }
        length -= reqSize;
        return reqSize;
    }
};

#endif //UNTITLED_SYSTEMRESOURCE_H
