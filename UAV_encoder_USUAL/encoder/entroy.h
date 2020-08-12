struct Queue
{
    int a[M_S];
    int front;		//前指针，指向队首
    int rear;		//后指针，指向对尾
};

void InitQuene(struct Queue* p);
int QueueLength(struct Queue* p);
short IsEmpty(struct Queue* p);
short IsFull(struct Queue* p);
short EnQueue(struct Queue* p, int key);
short DeQueue(struct Queue* p, int* value);

int huffman_code[17][10];
int* huffman(int(*matrix)[width / 4], int m, int n, int* p_seek);

//#define round(x) (int)(x+0.5);
int h_order[16];
int v_order[16];
int TotalMap[4][17][5][17];
int TotalZeros_matrix[16][15][10];
int RunBefore_matrix[15][7][12];
int level_prefix_matrix[16][16];
void CAVLC_encode(int* dct, int NCa, int NCb, struct Queue* p);
void floattostr(float* a, char* str);