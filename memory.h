//
// Created by c on 2018/12/24.
//

#ifndef UNTITLED_MEMORY_H
#define UNTITLED_MEMORY_H

#define FREE 0 //空闲状态
#define USED 1 //已用状态
class MemoryData//定义一个空闲区说明表结构
{
public:
    long size;   //分区大小
    long address; //分区地址
    int state;   //状态
    int id; //分区号
};


class MemoryNode// 线性表的双向链表存储结构
{
public:
    MemoryData memoryData;
    MemoryNode *pre; //前趋指针
    MemoryNode *next;  //后继指针
};

#endif //UNTITLED_MEMORY_H
