#include<iostream>
#include<stdlib.h>

#include "memory.h"

using namespace std;


class MemoryManager{

public:
    MemoryNode *block_first; //ͷ���
    MemoryNode *block_last;  //β���
    int strategy = 1;
    int SIZE = 0;

    MemoryManager(int strategy,int size){
        this->strategy  = strategy;
        this->SIZE = size;
        initMemory();
    }


    bool Alloc(int requestSize, int processId)//��������
    {
        if (requestSize < 0 || requestSize == 0) return false;

        if (strategy == 2) {
            //ѡ�������Ӧ�㷨
            return Best_fit(requestSize,processId);
        } else if (strategy == 1) {
            //Ĭ���״���Ӧ�㷨
            return First_fit(requestSize,processId);
        }

        return false;
    }


    bool free(int processId)//�������
    {
        MemoryNode *p = block_first;

        while (p != nullptr) {
            p = p->next;
            if (p->memoryData.id == processId) {
                break;
            }
        }
        if(p == nullptr) return false;

        p->memoryData.state = FREE; //�ͷ�
        if (p->pre != block_first && p->pre->memoryData.state == FREE)//��ǰ��Ŀ��п�����
        {
            p->pre->memoryData.size += p->memoryData.size;//�ռ�����,�ϲ�Ϊһ��
            p->pre->next = p->next;//ȥ��ԭ�����ϲ���p
            p->next->pre = p->pre;
            std::free(p);
            p = p->pre;
        }
        if (p->next != block_last && p->next->memoryData.state == FREE)//�����Ŀ��п�����
        {
            p->memoryData.size += p->next->memoryData.size;//�ռ�����,�ϲ�Ϊһ��
            p->next->next->pre = p;
            auto *f = p->next;
            p->next = p->next->next;
            std::free(f);

        }
        if (p->next == block_last && p->next->memoryData.state == FREE)//�����Ŀ��п�����
        {
            p->memoryData.size += p->next->memoryData.size;
            p->next = nullptr;
        }

        return true;
    }



    void show()//��ʾ����������
    {
        int flag = 0;
        cout << "\n����������:\n";
        cout << "++++++++++++++++++++++++++++++++++++++++++++++\n\n";
        MemoryNode *p = block_first->next;
        cout << "������\t��ʼ��ַ\t������С\t״̬\n\n";
        while (p) {
            cout << "  " << flag++ << "\t";
            cout << "  " << p->memoryData.address << "\t\t";
            cout << " " << p->memoryData.size << "KB\t\t";
            if (p->memoryData.state == FREE) cout << "����\n\n";
            else cout << "�ѷ���\n\n";
            p = p->next;
        }
        cout << "++++++++++++++++++++++++++++++++++++++++++++++\n\n";
    }

    void play(){
        cout << "��������ʹ�õ��ڴ�����㷨��\n";
        cout << "(1)�״���Ӧ�㷨\n(2)�����Ӧ�㷨\n";

        cin >> strategy;
        while (strategy < 1 || strategy > 2) {
            cout << "�������������������ʹ�õ��ڴ�����㷨��\n";
            cin >> strategy;
        }

        initMemory(); //�����ռ��

        int operators[] = {-1, 130, 1, -1, 60, 2, -1, 100, 3, 2, 60, 2, -1, 200, 4, 3, 100, 3, 1, 130, 1, -1, 140, 5, -1,
                           60, 6, -1, 50, 7, -1, 60, 8};

        for (int i = 0; i < 22; i += 3) {
            int opt = operators[i];
            if (opt == -1) {
                cout << "�����ڴ�: " + to_string(operators[i + 1]) << endl;
                Alloc(operators[i + 1], operators[i + 2]);
            } else {
                cout << "�ͷŵ�" + to_string(operators[i]) + " ����ҵ���ڴ� " + to_string(operators[i + 1]) << endl;
                free(operators[i + 2]);
            }
            show();
        }
    }

private:
    bool initMemory()//������ͷ�����ڴ�ռ�����
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
    bool First_fit(int request,int id)//�״���Ӧ�㷨
    {
        //Ϊ������ҵ�����¿ռ��ҳ�ʼ��
        MemoryNode *node = block_first->next;
        while (node != nullptr) {
            if (node->memoryData.state == FREE && node->memoryData.size == request) {
                //�д�Сǡ�ú��ʵĿ��п�
                node->memoryData.state = USED;
                return true;
            }
            if (node->memoryData.state == FREE && node->memoryData.size > request) {
                //�п��п���������������ʣ��,���벢�޸�ʼֵ
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


    bool Best_fit(int request,int id)//�����Ӧ�㷨
    {
        int rest = 0; //��¼��Сʣ��ռ�
        MemoryNode *temp = new MemoryNode();
        temp->memoryData.size = request;
        temp->memoryData.state = USED;
        MemoryNode *p = block_first->next;
        MemoryNode *q = NULL; //��¼��Ѳ���λ��

        while (p) //��ʼ����С�ռ�����λ��
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

        if (q == NULL) return false;//û���ҵ����п�

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



int main()//������
{
    MemoryManager(1,640).play();

}