#include<stdio.h>
#include"DCT.h"
#include"deblock.h"
#include"mc.h"
#include"quant.h"
#include<stdlib.h>
#include"intra.h"
#include"inter.h"
#include"main.h"
#include<string.h>
#include<math.h>
#include"entroy.h"


//初始化
void InitQuene(struct Queue* p)
{
    p->front = p->rear = 0;			//初始化前后指针都指向0
}
//求长度
int QueueLength(struct Queue* p)
{
    return (p->rear - p->front + M_S) % M_S;	//求元素个数
}
//判断是否为空
short IsEmpty(struct Queue* p)
{
    if (p->front == p->rear)		//若前后指针指向同一个节点，则判断为空
        return 1;
    else
        return 0;
}
//判断是否为满
short IsFull(struct Queue* p)
{
    if (p->front == (p->rear + 1) % M_S)		//若前指针等于(后指针+1)%数组大小，则判断为满
        return 1;
    else
        return 0;
}
//进队
short EnQueue(struct Queue* p, int key)
{
    if (IsFull(p)) {  //若队列为满，则进队失败
        printf("==============buffer overflow===============\n");
        //在这写将队列值全部写进文档或者其他地方。
        system("pause");
        return 0;
    }
    p->a[p->rear] = key;			//否则将进队元素赋值给后指针所指的位置，后指针往后移动一格
    p->rear = (p->rear + 1) % M_S;
    return 1;
}
//出队
short DeQueue(struct Queue* p, int* value)
{
    if (IsEmpty(p))				//若队列为空，则出队失败
        return 0;
    *value = p->a[p->front];			//传入一个指针保留出队元素，然后队首指向下一个元素
    p->front = (p->front + 1) % M_S;
    return 1;
}



int huffman_code[17][10] =
{
    {1,1,0,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,0,0},
    {1,1,0,0,0,0,0},
    {1,1,0,0,0,0},
    {1,1,0,0,0},
    {1,1,0,0},
    {1,1,0},
    {1,0},
    {1,0,0},
    {1,0,0,0},
    {1,0,0,0,0},
    {1,0,0,0,0,0},
    {1,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0},
};

int* huffman(int (*matrix)[width/4], int m, int n, int* p_seek)
{   
    int tmp;
    int* p = (int*)malloc(10 * sizeof(int));
    memset(p, 0, 10 * sizeof(int));
    if (m == 0 && n == 0)
        tmp = matrix[m][n];
    else if(m == 0 && n != 0)
        tmp = matrix[m][n-1];
    else if (m != 0 && n == 0)
        tmp = matrix[m-1][n];
    else
        tmp = matrix[m][n] - (matrix[m][n - 1] < matrix[m - 1][n] ? matrix[m][n - 1] : matrix[m - 1][n]);

    switch (tmp) {

    case -1: {
        memcpy(p + *p_seek, huffman_code[7], sizeof(int) * 3);
        *p_seek += 3;
        break;
    }
    case -2: {
        memcpy(p + *p_seek, huffman_code[6], sizeof(int) * 4);
        *p_seek += 4;
        break;
    }
    case -3: {
        memcpy(p + *p_seek, huffman_code[5], sizeof(int) * 5);
        *p_seek += 5;
        break;
    }
    case -4: {
        memcpy(p + *p_seek, huffman_code[4], sizeof(int) * 6);
        *p_seek += 6;
        break;
    }
    case -5: {
        memcpy(p + *p_seek, huffman_code[3], sizeof(int) * 7);
        *p_seek += 7;
        break;
    }
    case -6: {
        memcpy(p + *p_seek, huffman_code[2], sizeof(int) * 8);
        *p_seek += 8;
        break;
    }
    case -7: {
        memcpy(p + *p_seek, huffman_code[1], sizeof(int) * 9);
        *p_seek += 9;
        break;
    }
    case -8: {
        memcpy(p + *p_seek, huffman_code[0], sizeof(int) * 10);
        *p_seek += 10;
        break;
    }


    case 0: {
        memcpy(p + *p_seek, huffman_code[8], sizeof(int) * 2);
        *p_seek += 2;
        break;
    }
    case 1: {
        memcpy(p + *p_seek, huffman_code[9], sizeof(int) * 3);
        *p_seek += 3;
        break;
    }
    case 2: {
        memcpy(p + *p_seek, huffman_code[10], sizeof(int) * 4);
        *p_seek += 4;
        break;
    }
    case 3: {
        memcpy(p + *p_seek, huffman_code[11], sizeof(int) * 5);
        *p_seek += 5;
        break;
    }
    case 4: {
        memcpy(p + *p_seek, huffman_code[12], sizeof(int) * 6);
        *p_seek += 6;
        break;
    }
    case 5: {
        memcpy(p + *p_seek, huffman_code[13], sizeof(int) * 7);
        *p_seek += 7;
        break;
    }
    case 6: {
        memcpy(p + *p_seek, huffman_code[14], sizeof(int) * 8);
        *p_seek += 8;
        break;
    }
    case 7: {
        memcpy(p + *p_seek, huffman_code[15], sizeof(int) * 9);
        *p_seek += 9;
        break;
    }
    case 8: {
        memcpy(p + *p_seek, huffman_code[16], sizeof(int) * 10);
        *p_seek += 10;
        break;
    }
    }

    return p;
}




int h_order[16] = { 0,1,0,0,1,2,3,2,1,0,1,2,3,3,2,3 };//横向扫描顺序
int v_order[16] = { 0,0,1,2,1,0,0,1,2,3,3,2,1,2,3,3 };//纵向扫描顺序
//TotalMap[TraillingOnes][Totalcoeff][N范围][比特串]
int TotalMap[4][17][5][17] =//若末尾为0，使用-1当作截断字符
{//第一维
    {
        {{1},{1,1},{1,1,1,1},{0,0,0,0,1,1},{0,1}},//00
        {{0,0,0,1,0,1},{0,0,1,0,1,1},{0,0,1,1,1,1},{0,0,0,0,0,0,-1},{0,0,0,1,1,1}},//01
        {{0,0,0,0,1,1,1},{0,0,0,1,1,1},{0,0,1,0,1,1},{0,0,0,1,0,0,-1},{0,0,0,1,0,0,-1}},//02
        {{0,0,0,0,0,0,1,1,1},{0,0,0,0,1,1,1},{0,0,1,0,0,0,-1},{0,0,1,0,0,0,-1},{0,0,0,0,1,1}},//03
        {{0,0,0,0,0,0,0,1,1,1},{0,0,0,0,0,1,1,1},{0,0,0,1,1,1,1},{0,0,1,1,0,0,-1},{0,0,0,0,1,0,-1}},//04
        {{0,0,0,0,0,0,0,0,1,1,1},{0,0,0,0,0,1,0,0,-1},{0,0,0,1,0,1,1},{0,1,0,0,0,0,-1},{0}},//05
        {{0,0,0,0,0,0,0,0,0,1,1,1,1},{0,0,0,0,0,0,1,1,1},{0,0,0,1,0,0,1},{0,1,0,1,0,0,-1},{0}},//06
        {{0,0,0,0,0,0,0,0,0,1,0,1,1},{0,0,0,0,0,0,0,1,1,1,1},{0,0,0,1,0,0,0,-1},{0,1,1,0,0,0,-1},{0}},//07
        {{0,0,0,0,0,0,0,0,0,1,0,0,0,-1},{0,0,0,0,0,0,0,1,0,1,1},{0,0,0,0,1,1,1,1},{0,1,1,1,0,0,-1},{0}},//08
        {{0,0,0,0,0,0,0,0,0,0,1,1,1,1},{0,0,0,0,0,0,0,0,1,1,1,1},{0,0,0,0,1,0,1,1},{1,0,0,0,0,0,-1},{0}},//09
        {{0,0,0,0,0,0,0,0,0,0,1,0,1,1},{0,0,0,0,0,0,0,0,1,0,1,1},{0,0,0,0,0,1,1,1,1},{1,0,0,1,0,0,-1},{0}},//0 10
        {{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1},{0,0,0,0,0,0,0,0,1,0,0,0,-1},{0,0,0,0,0,1,0,1,1},{1,0,1,0,0,0,-1},{0}},//0 11
        {{0,0,0,0,0,0,0,0,0,0,0,1,0,1,1},{0,0,0,0,0,0,0,0,0,1,1,1,1},{0,0,0,0,0,1,0,0,0,-1},{1,0,1,1,0,0,-1},{0}},//0 12
        {{0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1},{0,0,0,0,0,0,0,0,0,1,0,1,1},{0,0,0,0,0,0,1,1,0,1},{1,1,0,0,0,0,-1},{0}},//0 13
        {{0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1},{0,0,0,0,0,0,0,0,0,0,1,1,1},{0,0,0,0,0,0,1,0,0,1},{1,1,0,1,0,0,-1},{0}},//0 14
        {{0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},{0,0,0,0,0,0,0,0,0,0,1,0,0,1},{0,0,0,0,0,0,0,1,0,1},{1,1,1,0,0,0,-1},{0}},//0 15
        {{0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,-1},{0,0,0,0,0,0,0,0,0,0,0,1,0,0,-1},{0,0,0,0,0,0,0,0,1,0,-1},{1,1,1,1,1,1},{0}},//0 16
    },//0
    {
        {{0},{0},{0},{0},{0}},//10
        {{0,1},{1,0,-1},{1,1,1,0,-1},{0,0,0,0,0,1},{1}},//11
        {{0,0,0,1,0,0,-1},{0,0,1,1,1},{0,1,1,1,1},{0,0,0,1,0,1},{0,0,1,1,0,-1}},//12
        {{0,0,0,0,0,1,1,0,-1},{0,0,1,0,1,0,-1},{0,1,1,0,0,-1},{0,0,1,0,0,1},{0,0,0,0,0,1,1}},//13
        {{0,0,0,0,0,0,1,1,0,-1},{0,0,0,1,1,0,-1},{0,1,0,1,0,-1},{0,0,1,1,0,1},{0,0,0,0,0,0,1,1}},//14
        {{0,0,0,0,0,0,0,1,1,0,-1},{0,0,0,0,1,1,0,-1},{0,1,0,0,0,-1},{0,1,0,0,0,1},{0}},//15
        {{0,0,0,0,0,0,0,0,1,1,0,-1},{0,0,0,0,0,1,1,0,-1},{0,0,1,1,1,0,-1},{0,1,0,1,0,1},{0}},//16
        {{0,0,0,0,0,0,0,0,0,1,1,1,0,-1},{0,0,0,0,0,0,1,1,0,-1},{0,0,1,0,1,0,-1},{0,1,1,0,0,1},{0}},//17
        {{0,0,0,0,0,0,0,0,0,1,0,1,0,-1},{0,0,0,0,0,0,0,1,1,1,0,-1},{0,0,1,1,1,0,-1},{0,1,1,1,0,1},{0}},//18
        {{0,0,0,0,0,0,0,0,0,0,1,1,1,0,-1},{0,0,0,0,0,0,0,1,0,1,0,-1},{0,0,0,0,1,1,1,0,-1},{1,0,0,0,0,1},{0}},//19
        {{0,0,0,0,0,0,0,0,0,0,1,0,1,0,-1},{0,0,0,0,0,0,0,0,1,1,1,0,-1},{0,0,0,0,1,0,1,0,-1},{1,0,0,1,0,1},{0}},//1 10
        {{0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,-1},{0,0,0,0,0,0,0,0,1,0,1,0,-1},{0,0,0,0,0,1,1,1,0,-1},{1,0,1,0,0,1},{0}},//1 11
        {{0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,-1},{0,0,0,0,0,0,0,0,0,1,1,1,0,-1},{0,0,0,0,0,1,0,1,0,-1},{1,0,1,1,0,1},{0}},//1 12
        {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},{0,0,0,0,0,0,0,0,0,1,0,1,0,-1},{0,0,0,0,0,0,1,1,1},{1,1,0,0,0,1},{0}},//1 13
        {{0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,-1},{0,0,0,0,0,0,0,0,0,0,1,0,1,1},{0,0,0,0,0,0,1,1,0,0,-1},{1,1,0,1,0,1},{0}},//1 14
        {{0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,-1},{0,0,0,0,0,0,0,0,0,0,1,0,0,0,-1},{0,0,0,0,0,0,1,0,0,0,-1},{1,1,0,0,1},{0}},//1 15
        {{0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,-1},{0,0,0,0,0,0,0,0,0,0,0,1,1,0,-1},{0,0,0,0,0,0,0,1,0,0,-1},{1,1,1,1,0,1},{0}},//1 16
    },//1
    {
        {{0},{0},{0},{0},{0}},//20
        {{0},{0},{0},{0},{0}},//21
        {{0,0,1},{0,1,1},{1,1,0,1},{0,0,0,1,1,0,-1},{0,0,1}},//22
        {{0,0,0,0,1,0,1},{0,0,1,0,0,1},{0,1,1,1,0,-1},{0,0,1,0,1,0,-1},{0,0,0,0,0,1,0,-1}},//23
        {{0,0,0,0,0,1,0,1},{0,0,0,1,0,1},{0,1,0,1,1},{0,0,1,1,1,0,-1},{0,0,0,0,0,0,1,0,-1}},//24
        {{0,0,0,0,0,0,1,0,1},{0,0,0,0,1,0,1},{0,1,0,0,1},{0,1,0,0,1,0,-1},{0}},//25
        {{0,0,0,0,0,0,0,1,0,1},{0,0,0,0,0,1,0,1},{0,0,1,1,0,1},{0,1,0,1,1,0,-1},{0}},//26
        {{0,0,0,0,0,0,0,0,1,0,1},{0,0,0,0,0,0,1,0,1},{0,0,1,0,0,1},{0,1,1,0,1,0,-1},{0}},//27
        {{0,0,0,0,0,0,0,0,0,1,1,0,1},{0,0,0,0,0,0,0,1,1,0,1},{0,0,0,1,1,0,1},{0,1,1,1,1,0,-1},{0}},//28
        {{0,0,0,0,0,0,0,0,0,1,0,0,1},{0,0,0,0,0,0,0,1,0,0,1},{0,0,0,1,0,1,0,-1},{1,0,0,0,1,0,-1},{0}},//29
        {{0,0,0,0,0,0,0,0,0,0,1,1,0,1},{0,0,0,0,0,0,0,0,1,1,0,1},{0,0,0,0,1,1,0,1},{1,0,0,1,1,0,-1},{0}},//2 10
        {{0,0,0,0,0,0,0,0,0,0,1,0,0,1},{0,0,0,0,0,0,0,0,1,0,0,1},{0,0,0,0,1,0,0,1},{1,0,1,0,1,0,-1},{0}},//2 11
        {{0,0,0,0,0,0,0,0,0,0,0,1,1,0,1},{0,0,0,0,0,0,0,0,0,1,1,0,1},{0,0,0,0,0,1,1,0,1},{1,0,1,1,1,0,-1},{0}},//2 12
        {{0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},{0,0,0,0,0,0,0,0,0,1,0,0,1},{0,0,0,0,0,1,0,0,1},{1,1,0,0,1,0,-1},{0}},//2 13
        {{0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1},{0,0,0,0,0,0,0,0,0,0,1,1,0,-1},{0,0,0,0,0,0,1,0,1,1},{1,1,0,1,1,0,-1},{0}},//2 14
        {{0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},{0,0,0,0,0,0,0,0,0,0,1,0,1,0,-1},{0,0,0,0,0,0,0,1,1,1},{1,1,1,0,1,0,-1},{0}},//2 15
        {{0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},{0,0,0,0,0,0,0,0,0,0,0,1,0,1},{0,0,0,0,0,0,0,0,1,1},{1,1,1,1,1,0,-1},{0}},//2 16

    },//2
    {
        {{0},{0},{0},{0},{0}},//30
        {{0},{0},{0},{0},{0}},//31
        {{0},{0},{0},{0},{0}},//32
        {{0,0,0,1,1},{0,1,0,1},{1,1,0,0,-1},{0,0,1,0,1,1},{0,0,0,1,0,1}},//33
        {{0,0,0,0,1,1},{0,1,0,0,-1},{1,0,1,1},{0,0,1,1,1,1},{0,0,0,0,0,0,0,-1}},//34
        {{0,0,0,0,1,0,0,-1},{0,0,1,1,0,-1},{1,0,1,0,-1},{0,1,0,0,1,1},{0}},//35
        {{0,0,0,0,0,1,0,0,-1},{0,0,1,0,0,0,-1},{1,0,0,1},{0,1,0,1,1,1},{0}},//36
        {{0,0,0,0,0,0,1,0,0,-1},{0,0,0,1,0,0,-1},{1,0,0,0,-1},{0,1,1,0,1,1},{0}},//37
        {{0,0,0,0,0,0,0,1,0,0,-1},{0,0,0,0,1,0,0,-1},{0,1,1,0,1},{0,1,1,1,1,1},{0}},//38
        {{0,0,0,0,0,0,0,0,1,0,0,-1},{0,0,0,0,0,0,1,0,0,-1},{0,0,1,1,0,0,-1},{1,0,0,0,1,1},{0}},//39
        {{0,0,0,0,0,0,0,0,0,1,1,0,0,-1},{0,0,0,0,0,0,0,1,1,0,0,-1},{0,0,0,1,1,0,0,-1},{1,0,0,1,1,1},{0}},//3 10
        {{0,0,0,0,0,0,0,0,0,0,1,1,0,0,-1},{0,0,0,0,0,0,0,1,0,0,0,-1},{0,0,0,0,1,1,0,0,-1},{1,0,1,0,1,1},{0}},//3 11
        {{0,0,0,0,0,0,0,0,0,0,1,0,0,0,-1},{0,0,0,0,0,0,0,0,1,1,0,0,-1},{0,0,0,0,1,0,0,0,-1},{1,0,1,1,1,1},{0}},//3 12
        {{0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,-1},{0,0,0,0,0,0,0,0,0,1,1,0,0,-1},{0,0,0,0,0,1,1,0,0,-1},{1,1,0,0,1,1},{0}},//3 13
        {{0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,-1},{0,0,0,0,0,0,0,0,0,1,0,0,0,-1},{0,0,0,0,0,0,1,0,1,0,-1},{1,1,0,1,1,1},{0}},//3 14
        {{0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,-1},{0,0,0,0,0,0,0,0,0,0,0,0,1},{0,0,0,0,0,0,0,1,1,0,-1},{1,1,1,0,1,1},{0}},//3 15
        {{0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,-1},{0,0,0,0,0,0,0,0,0,0,0,1,0,0,-1},{0,0,0,0,0,0,0,0,1,0,-1},{1,1,1,1,1,1},{0}},//3 16
    },//3
};

int TotalZeros_matrix[16][15][10] =
{
    {{1},{1,1,1},{0,1,0,1},{0,0,0,1,1},{0,1,0,1},{0,0,0,0,0,1},{0,0,0,0,0,1},{0,0,0,0,0,1},{0,0,0,0,0,1},{0,0,0,0,1},{0,0,0,0,-1},{0,0,0,0,-1},{0,0,0,-1},{0,0,-1},{0,-1}},
    {{0,1,1},{1,1,0,-1},{1,1,1},{1,1,1},{0,1,0,0,-1},{0,0,0,0,1},{0,0,0,0,1},{0,0,0,1},{0,0,0,0,0,0,-1},{0,0,0,0,0,-1},{0,0,0,1},{0,0,0,1},{0,0,1},{0,1},{1}},
    {{0,1,0,-1},{1,0,1},{1,1,0,-1},{0,1,0,1},{0,0,1,1},{1,1,1},{1,0,1},{0,0,0,0,1},{0,0,0,1},{0,0,1},{0,0,1},{0,1},{1},{1},{0}},
    {{0,0,1,1},{1,0,0,-1},{1,0,1},{0,1,0,0,-1},{1,1,1},{1,1,0,-1},{1,0,0,-1},{0,1,1},{1,1},{1,1},{0,1,0,-1},{1},{0,1},{0},{0}},
    {{0,0,1,0,-1},{0,1,1},{0,1,0,0,-1},{1,1,0,-1},{1,1,0,-1},{1,0,1},{0,1,1},{1,1},{1,0,-1},{1,0,-1},{1},{0,0,1},{0},{0},{0}},
    {{0,0,0,1,1},{0,1,0,1},{0,0,1,1},{1,0,1},{1,0,1},{1,0,0,-1},{1,1},{1,0,-1},{0,0,1},{0,1},{0,1,1},{0},{0},{0},{0}},
    {{0,0,0,1,0,-1},{0,1,0,0,-1},{1,0,0,-1},{1,0,0,-1},{1,0,0,-1},{0,1,1},{0,1,0,-1},{0,1,0,-1},{0,1},{0,0,0,1},{0},{0},{0},{0},{0}},
    {{0,0,0,0,1,1},{0,0,1,1},{0,1,1},{0,0,1,1},{0,1,1},{0,1,0,-1},{0,0,0,1},{0,0,1},{0,0,0,0,1},{0},{0},{0},{0},{0},{0}},
    {{0,0,0,0,1,0,-1},{0,0,1,0,-1},{0,0,1,0,-1},{0,1,1},{0,0,1,0,-1},{0,0,0,1},{0,0,1},{0,0,0,0,0,0,-1},{0},{0},{0},{0},{0},{0},{0}},
    {{0,0,0,0,0,1,1},{0,0,0,1,1},{0,0,0,1,1},{0,0,1,0,-1},{0,0,0,0,1},{0,0,1},{0,0,0,0,0,0,-1},{0},{0},{0},{0},{0},{0},{0},{0}},
    {{0,0,0,0,0,1,0,-1},{0,0,0,1,0,-1},{0,0,0,1,0,-1},{0,0,0,1,0,-1},{0,0,0,1},{0,0,0,0,0,0,-1},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
    {{0,0,0,0,0,0,1,1},{0,0,0,0,1,1},{0,0,0,0,0,1},{0,0,0,0,1},{0,0,0,0,0,-1},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
    {{0,0,0,0,0,0,1,0,-1},{0,0,0,0,1,0,-1},{0,0,0,0,1},{0,0,0,0,0,-1},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
    {{0,0,0,0,0,0,0,1,1},{0,0,0,0,0,1},{0,0,0,0,0,0,-1},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
    {{0,0,0,0,0,0,0,1,0,-1},{0,0,0,0,0,0,-1},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
    {{0,0,0,0,0,0,0,0,1},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},

};

int RunBefore_matrix[15][7][12] =
{
    {{1},{1},{1,1},{1,1},{1,1},{1,1},{1,1,1}},
    {{0,-1},{0,1},{1,0,-1},{1,0,-1},{1,0,-1},{0,0,0,-1},{1,1,0,-1}},
    {{0},{0,0,-1},{0,1},{0,1},{0,1,1},{0,0,1},{1,0,1}},
    {{0},{0},{0,0,-1},{0,0,1},{0,1,0,-1},{0,1,1},{1,0,0,-1}},
    {{0},{0},{0},{0,0,0,-1},{0,0,1},{0,1,0,-1},{0,1,1}},
    {{0},{0},{0},{0},{0,0,0,-1},{1,0,1},{0,1,0,-1}},
    {{0},{0},{0},{0},{0},{1,0,0,-1},{0,0,1}},
    {{0},{0},{0},{0},{0},{0},{0,0,0,1}},
    {{0},{0},{0},{0},{0},{0},{0,0,0,0,1}},
    {{0},{0},{0},{0},{0},{0},{0,0,0,0,0,1}},
    {{0},{0},{0},{0},{0},{0},{0,0,0,0,0,0,1}},
    {{0},{0},{0},{0},{0},{0},{0,0,0,0,0,0,0,1}},
    {{0},{0},{0},{0},{0},{0},{0,0,0,0,0,0,0,0,1}},
    {{0},{0},{0},{0},{0},{0},{0,0,0,0,0,0,0,0,0,1}},
    {{0},{0},{0},{0},{0},{0},{0,0,0,0,0,0,0,0,0,0,1}}
};

int level_prefix_matrix[16][16] =  //没用了，查表9-6
{
    {1},
    {0,1},
    {0,0,1},
    {0,0,0,1},
    {0,0,0,0,1},
    {0,0,0,0,0,1},
    {0,0,0,0,0,0,1},
    {0,0,0,0,0,0,0,1},
    {0,0,0,0,0,0,0,0,1},
    {0,0,0,0,0,0,0,0,0,1},
    {0,0,0,0,0,0,0,0,0,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}
};




void CAVLC_encode(int* dct, int NCa, int NCb, struct Queue* p)
{
    //for (int i = 0; i < 16; i++)
     //   printf("%d,", dct[i]);

    int NC = 0;
    int TotalCoeffs = 0;
    int TrailingOnes = 0;
    int SuffixLength = 0;
    int LevelCode = 0;
    int Level_prefix = 0;
    int Level_suffix = 0;
    int TotalZeros = 0;
    int ZeroLeft = 0;
    int Zigzag_matrix[16] = { 0 };
    int Level[16] = { 0 };//拖尾外非零系数
    int result[2048] = { 0 };
    int result_seek = 0;//指名result写到哪个位置（即长度-1）
    int table_seek = 0;//查表时临时使用
    int Trail[3] = { 0 };
    int Trail_end = 0;//最后一个拖尾系数的位置
    int Table_num = 0;//编码表根据NC值确定的那一维
    //zigzag扫描
    for (int i = 0; i < 16; i++)
        Zigzag_matrix[i] = dct[4 * v_order[i] + h_order[i]];
    //设置NC
    if (NCa != -1 && NCb != -1) {
        NC = round((NCa + NCb) / 2);
    }
    else if (NCa == -1) {
        NC = NCb;
    }
    else if (NCb == -1) {
        NC = NCa;
    }
    else
        NC = 0;
    //根据NC值判断在表里哪个子表（维度）进行查找
    if (NC == 0 || NC == 1) {
        Table_num = 0;
    }
    else if (NC == 2 || NC == 3) {
        Table_num = 1;
    }
    else if (NC >= 8) {
        Table_num = 3;
    }
    else if (NC == -1) {
        Table_num = 4;
    }
    else
        Table_num = 2;
    //计算TotalCoeffs和TrailOnes
    for (int i = 15, Trail_flag = 1; i >= 0; i--) {
        if (Zigzag_matrix[i] != 0 && Zigzag_matrix[i] != 1 && Zigzag_matrix[i] != -1)
            Trail_flag = 0;

        if ((Zigzag_matrix[i] == 1 || Zigzag_matrix[i] == -1) && TrailingOnes < 3 && Trail_flag != 0) {
            Trail[TrailingOnes] = Zigzag_matrix[i] == 1 ? 0 : 1;
            TrailingOnes++;
            Trail_end = i;
        }
        if (Zigzag_matrix[i] != 0)
            TotalCoeffs++;
    }

    //通过查表编码TotalCoeffs和TrailOnes
    for (int i = 16, copy_flag = 0; i >= 0; i--)
    {
        if (copy_flag == 1)
            result[i] = TotalMap[TrailingOnes][TotalCoeffs][Table_num][i];
        else {
            if (TotalMap[TrailingOnes][TotalCoeffs][Table_num][i] == -1) {
                result_seek = i - 1;
                copy_flag = 1;
            }
            else if (TotalMap[TrailingOnes][TotalCoeffs][Table_num][i] == 1) {
                result_seek = i - 1;
                copy_flag = 1;
                result[i] = TotalMap[TrailingOnes][TotalCoeffs][Table_num][i];
            }
        }
    }
    //编码拖尾系数符号正负性
    for (int i = 0; i < 3; i++)
        result[++result_seek] = Trail[i];

    //设置SuffixLength
    if (TotalCoeffs > 10 && TrailingOnes <= 1)
        SuffixLength = 1;

    //获取拖尾外非零系数
    for (int i = Trail_end - 1, j = 0; i >= 0; i--) {
        if (Zigzag_matrix[i] != 0) {
            Level[j] = Zigzag_matrix[i];
            j++;
        }
    }
    //编码除拖尾系数外的每一个非零系数
    for (int i = 0; i < 16; i++) {
        if (Level[i] != 0) {
            LevelCode = 0;
            Level_prefix = 0;
            Level_suffix = 0;
            LevelCode = Level[i] > 0 ? (Level[i] << 1) - 2 : -(Level[i] << 1) - 1;
            Level_prefix = LevelCode / (1 << SuffixLength);
            Level_suffix = LevelCode % (1 << SuffixLength);

            //编码Level_prefix
            for (int j = 0; j < Level_prefix; j++)
                result[++result_seek] = 0;
            result[++result_seek] = 1;
            //编码Level_suffix,就是二进制形式
            if (SuffixLength != 0) {
                do {//此处不用dowhile会少等于0的情况
                    result[++result_seek] = Level_suffix % 2;
                    Level_suffix /= 2;
                } while (Level_suffix != 0);
            }
            //更新SuffixLength
            if (SuffixLength == 0)
                SuffixLength++;
            else if (abs(Level[i]) > (3 << (SuffixLength - 1)) && SuffixLength < 6)
                SuffixLength++;

        }
    }
    //统计TotalZeros
    for (int i = 15, flag = 0; i >= 0; i--) {
        if (Zigzag_matrix[i] != 0)
            flag = 1;
        if (flag == 1 && Zigzag_matrix[i] == 0)
            TotalZeros++;
    }
    //编码TotalZeros
    int temp_result_seek = result_seek + 1;
    for (int i = 9, copy_flag = 0; i >= 0; i--)
    {
        if (copy_flag != 0) {
            result[temp_result_seek + i] = TotalZeros_matrix[TotalZeros][TotalCoeffs - 1][i];
            result_seek++;
        }
        else {
            if (TotalZeros_matrix[TotalZeros][TotalCoeffs - 1][i] == -1) {
                table_seek = i - 1;
                copy_flag = 1;
            }
            else if (TotalZeros_matrix[TotalZeros][TotalCoeffs - 1][i] == 1) {
                table_seek = i - 1;
                copy_flag = 1;
                result[temp_result_seek + i] = TotalZeros_matrix[TotalZeros][TotalCoeffs - 1][i];
                result_seek++;
            }
        }
    }
    //编码RunBefore
    ZeroLeft = TotalZeros;
    for (int i = 15, num_flag = 0, RunBefore = 0; i >= 0; i--) {
        if (num_flag == 0 && Zigzag_matrix[i] != 0) {
            num_flag = 1;
            continue;
        }
        if (num_flag != 0 && Zigzag_matrix[i] == 0)
            RunBefore++;
        if (num_flag != 0 && Zigzag_matrix[i] != 0) {
            int temp_zeroleft = ZeroLeft;
            temp_result_seek = result_seek + 1;
            temp_zeroleft = temp_zeroleft > 6 ? 6 : temp_zeroleft;
            for (int i = 11, copy_flag = 0; i >= 0; i--)
            {
                if (copy_flag != 0) {
                    result[temp_result_seek + i] = RunBefore_matrix[RunBefore][temp_zeroleft - 1][i];
                    result_seek++;
                }
                else {
                    if (RunBefore_matrix[RunBefore][temp_zeroleft - 1][i] == -1) {
                        table_seek = i - 1;
                        copy_flag = 1;
                    }
                    else if (RunBefore_matrix[RunBefore][temp_zeroleft - 1][i] == 1) {
                        table_seek = i - 1;
                        copy_flag = 1;
                        result[temp_result_seek + i] = RunBefore_matrix[RunBefore][temp_zeroleft - 1][i];
                        result_seek++;
                    }
                }
            }
            ZeroLeft -= RunBefore;
            RunBefore = 0;
        }
        if (ZeroLeft == 0)
            break;
    }
    //if(result_seek > 128)
    //    printf("sssssssssss result_seek%d\n", result_seek);
    //result数组里面存着编码结果，长度=result_seek+1
    for (int i = 0; i <= result_seek; i++) {
        //printf("wwwwwwwwwwww%d\n", result[i]);
        EnQueue(p, result[i]);
        //printf("sssssssssssss%d\n", p->a[p->rear-1]);
    }
}   




void floattostr(float* a, char* str) {
    unsigned int c;
    c = ((unsigned int*)a)[0];
    for (int i = 0; i < 32; i++) {
        str[31 - i] = (char)(c & 1) + '0';
        c >>= 1;
    }
    str[32] = '\0';
}






















































































void entroy_CAVLC_block4x4(int *dct, int *intraframe_model) {
	
	//做帧内模式预测，编码（真实模式-预测模式）写入头信息

	//对dct[16]进行CAVLC编码



	//将编码的码流输出到txt文档中（txt相当于buffer）


}


