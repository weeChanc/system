#include<iostream>
#include<stdlib.h>

#include "memory.h"

using namespace std;


class MemoryManager{

public:
    MemoryNode *block_first; //头结点
    MemoryNode *block_last;  //尾结点
    int strategy = 1;
    int SIZE = 0;

    MemoryManager(int strategy,int size){
        this->strategy  = strategy;
        this->SIZE = size;
        initMemory();
    }


    bool Alloc(int requestSize, int processId)//分配主存
    {
        if (requestSize < 0 || requestSize == 0) return false;

        if (strategy == 2) {
            //选择最佳适应算法
            return Best_fit(requestSize,processId);
        } else if (strategy == 1) {
            //默认首次适应算法
            return First_fit(requestSize,processId);
        }

        return false;
    }


    bool free(int processId)//主存回收
    {
        MemoryNode *p = block_first;

        while (p != nullptr) {
            p = p->next;
            if (p->memoryData.id == processId) {
                break;
            }
        }
        if(p == nullptr) return false;

        p->memoryData.state = FREE; //释放
        if (p->pre != block_first && p->pre->memoryData.state == FREE)//与前面的空闲块相连
        {
            p->pre->memoryData.size += p->memoryData.size;//空间扩充,合并为一个
            p->pre->next = p->next;//去掉原来被合并的p
            p->next->pre = p->pre;
            std::free(p);
            p = p->pre;
        }
        if (p->next != block_last && p->next->memoryData.state == FREE)//与后面的空闲块相连
        {
            p->memoryData.size += p->next->memoryData.size;//空间扩充,合并为一个
            p->next->next->pre = p;
            auto *f = p->next;
            p->next = p->next->next;
            std::free(f);

        }
        if (p->next == block_last && p->next->memoryData.state == FREE)//与最后的空闲块相连
        {
            p->memoryData.size += p->next->memoryData.size;
            p->next = nullptr;
        }

        return true;
    }



    void show()//显示主存分配情况
    {
        int flag = 0;
        cout << "\n主存分配情况:\n";
        cout << "++++++++++++++++++++++++++++++++++++++++++++++\n\n";
        MemoryNode *p = block_first->next;
        cout << "分区号\t起始地址\t分区大小\t状态\n\n";
        while (p) {
            cout << "  " << flag++ << "\t";
            cout << "  " << p->memoryData.address << "\t\t";
            cout << " " << p->memoryData.size << "KB\t\t";
            if (p->memoryData.state == FREE) cout << "空闲\n\n";
            else cout << "已分配\n\n";
            p = p->next;
        }
        cout << "++++++++++++++++++++++++++++++++++++++++++++++\n\n";
    }

    void play(){
        cout << "请输入所使用的内存分配算法：\n";
        cout << "(1)首次适应算法\n(2)最佳适应算法\n";

        cin >> strategy;
        while (strategy < 1 || strategy > 2) {
            cout << "输入错误，请重新输入所使用的内存分配算法：\n";
            cin >> strategy;
        }

        initMemory(); //开创空间表

        int operators[] = {-1, 130, 1, -1, 60, 2, -1, 100, 3, 2, 60, 2, -1, 200, 4, 3, 100, 3, 1, 130, 1, -1, 140, 5, -1,
                           60, 6, -1, 50, 7, -1, 60, 8};

        for (int i = 0; i < 22; i += 3) {
            int opt = operators[i];
            if (opt == -1) {
                cout << "分配内存: " + to_string(operators[i + 1]) << endl;
                Alloc(operators[i + 1], operators[i + 2]);
            } else {
                cout << "释放第" + to_string(operators[i]) + " 个作业的内存 " + to_string(operators[i + 1]) << endl;
                free(operators[i + 2]);
            }
            show();
        }
    }

private:
    bool initMemory()//开创带头结点的内存空间链表
    {
        block_first = new MemoryNode();
        block_last = new MemoryNode();
        block_first->pre = NULL;
        block_first->next = block_last;
        block_last->pre = block_first;
        block_last->next = NULL;
        block_last->memoryData.address = 0;
        block_last->memoryData.size = SIZE;
        block_last->memoryData.state = FREE;
        block_last->memoryData.id = 0;
        return true;
    }
    bool First_fit(int request,int id)//首次适应算法
    {
        //为申请作业开辟新空间且初始化
        MemoryNode *node = block_first->next;
        while (node != nullptr) {
            if (node->memoryData.state == FREE && node->memoryData.size == request) {
                //有大小恰好合适的空闲块
                node->memoryData.state = USED;
                return true;
            }
            if (node->memoryData.state == FREE && node->memoryData.size > request) {
                //有空闲块能满足需求且有剩余,插入并修改始值
                MemoryNode *temp = new MemoryNode();
                temp->memoryData.id = id;
                temp->memoryData.size = request;
                temp->memoryData.state = USED;
                temp->pre = node->pre;
                temp->next = node;
                temp->memoryData.address = node->memoryData.address;
                node->pre->next = temp;
                node->pre = temp;
                node->memoryData.address = temp->memoryData.address + temp->memoryData.size;
                node->memoryData.size -= request;
                return true;
            }
            node = node->next;
        }
        return false;
    }


    bool Best_fit(int request,int id)//最佳适应算法
    {
        int rest = 0; //记录最小剩余空间
        MemoryNode *temp = new MemoryNode();
        temp->memoryData.size = request;
        temp->memoryData.state = USED;
        MemoryNode *p = block_first->next;
        MemoryNode *q = NULL; //记录最佳插入位置

        while (p) //初始化最小空间和最佳位置
        {
            if (p->memoryData.state == FREE && (p->memoryData.size >= request)) {
                if (q == NULL) {
                    q = p;
                    rest = p->memoryData.size - request;
                } else if (q->memoryData.size > p->memoryData.size) {
                    q = p;
                    rest = p->memoryData.size - request;
                }
            }
            p = p->next;
        }

        if (q == NULL) return false;//没有找到空闲块

        if (q->memoryData.size == request) {
            q->memoryData.state = USED;
            return true;
        } else {
            temp->pre = q->pre;
            temp->next = q;
            temp->memoryData.address = q->memoryData.address;
            temp->memoryData.id = id;
            q->pre->next = temp;
            q->pre = temp;
            q->memoryData.address += request;
            q->memoryData.size = rest;
            return false;
        }
    }

};



int main()//主函数
{
    MemoryManager(1,640).play();

}