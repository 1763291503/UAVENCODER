#define _CRT_SECURE_NO_WARNINGS
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
#include"entroy.h"
#include"other.h"
#include <sys/timeb.h>
#include <math.h>




//#define width 1280 //�ֱ�����main.h�ж���
//#define height 720
int I_frame_interval = 50; //ÿ����֡һ��I֡


#if defined(WIN32)
# define  TIMEB    _timeb
# define  ftime    _ftime
typedef __int64 TIME_T;
#else
#define TIMEB timeb
typedef long long TIME_T;
#endif
struct TIMEB ts1, ts2,ts3,ts4;
TIME_T t1, t2,t3,t4;
int ti,tj;
int deltat = 0;

int matrix_frame[height][width];//�浱ǰ����֡
int reconstructed_frame[height][width];//�����ؽ�֡����Ϊ��һ֡�Ĳο�֡
int reconstructed_frame_tmp[height][width];//��֡���˶��������м���
int intraframe_model[height / 4][width / 4];//��֡��Ԥ���9��ģʽ
int not0_num[height / 4][width / 4];//��dct16�з�0�ĸ���
int ref_frame_buffer[6][height][width];
float R_buffer[5][6];
float affine_p[6] = { 1,0,0,0,1,0 }; //������
double A[3][3] = { 0 };
int ref_coordinate[height / 4][width / 4][2] = {0};

struct Queue bitQ; //���ض��л���

//dequant_MF����
int dequant_MF[6][16] =
{ {10, 13, 10, 13, 13, 16, 13, 16, 10, 13, 10, 13, 13, 16, 13, 16 },
{ 11, 14, 11, 14, 14, 18, 14, 18, 11, 14, 11, 14, 14, 18, 14, 18 },
{ 13, 16, 13, 16, 16, 20, 16, 20, 13, 16, 13, 16, 16, 20, 16, 20 },
{ 14, 18, 14, 18, 18, 23, 18, 23, 14, 18, 14, 18, 18, 23, 18, 23 },
{ 16, 20, 16, 20, 20, 25, 20, 25, 16, 20, 16, 20, 20, 25, 20, 25 },
{ 18, 23, 18, 23, 23, 29, 23, 29, 18, 23, 18, 23, 23, 29, 23, 29 } };
//deblock �жϱ߽�������ֵ0��alpha��1��beta
int thr_alpha_beta[52][2] = { {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{4,2},{4,2},{5,2},{6,3},{7,3},{8,3},{9,3},{10,4},{12,4},{13,4},{15,6},{17,6},{20,7},{22,7},{25,8},{28,8},{32,9},{36,9},{40,10},{45,10},{50,11},{56,11},{63,12},{71,12},{80,13},{90,13},{101,14},{113,14},{127,15},{144,15},{162,16},{182,16},{203,17},{226,17},{255,18},{255,18} };

int BS[3][52] = { {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,2,2,2,2,3,3,3,4,4,4,5,6,6,7,8,9,10,11,13},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,2,2,2,2,3,3,3,4,4,5,5,6,7,8,8,10,11,12,13,15,17},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,2,2,2,2,3,3,3,4,4,4,5,6,6,7,8,9,10,11,13,14,16,18,20,23,25} };

int main() {
	for (double i = 0; 4*i < height; i++) {
		for (double j = 0; 4*j < width; j++) {
			A[0][0] += 4 * i * 4 * i;
			A[0][1] += 4 * i * 4 * j;
			A[0][2] += 4 * i;
			A[1][0] = A[0][1];
			A[1][1] += 4 * j * 4 * j;
			A[1][2] += 4 * j;
			A[2][0] = A[0][2];
			A[2][1] = A[1][2];
			A[2][2] += 1;
		}
	}

	//for (int i = 0; i < 3; i++)
	//	for (int j = 0; j < 3; j++)
	//		printf("%.30lf,", A[i][j]);
	//printf("\n");

	matrix_inv((double*)A, 3);//����

	//for (int i = 0; i < 3; i++)
	//	for (int j = 0; j < 3; j++)
	//		printf("s%.30lf\n,", A[i][j]);
	//printf("\n");

	InitQuene(&bitQ);
	//��ȡYUV420Դ�ļ�
	//YUV420��ÿ֡��ǰwidth * height���ֽ�ΪY���м�(width * height)/4���ֽ�ΪU,���(width * height)/4���ֽ�ΪV
	int frame_size = (width * height * 3) >> 1; 
	unsigned char* buff = (unsigned char*)malloc(frame_size);
	FILE* video_yuv;
	errno_t err = fopen_s(&video_yuv, "C:/Users/lzb/Desktop/car_test_dec.yuv", "rb");
	if (err) {
		printf("open file failed, directory or file not found.\n");
		return 0;
	}
	FILE* fpWrite = fopen("C:/Users/lzb/Desktop/tmp1wme.txt", "w");

	int frame_no=0;
	while (fread(buff, sizeof(char), frame_size, video_yuv)) { 	//fread(buff, sizeof(char), frame_size, video_yuv);  �˺���ִ��һ�ξ��Ƕ�һ֡
		
		printf("��%d֡�ĵ�һ������ֵ��%d\n", frame_no,*buff);
		//��һ֡��Y�������浽��ά����matrix_frame��
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				matrix_frame[i][j] = *(buff + width * i + j);
			}
		}

		if (!(frame_no % I_frame_interval))  //�ж��Ƿ�ΪI֡
		{
			//֡�ڱ���
			InitQuene(&bitQ);

			ftime(&ts1);//��ʼ��ʱ

			int predicted_pixl4x4_buffer[4][4];
			int Q2entropy[16];
			//��֡���ͺ�QPд��header
			EnQueue(&bitQ, 1); //I֡Ϊ1��P֡Ϊ0
			int temp_bitQP;
			for (int i = 5; i >= 0; i--) {
				temp_bitQP = (QP >> i) % 2;
				EnQueue(&bitQ, temp_bitQP);
			}
			for (int i = 0; 4 * i < height; i++) {
				for (int j = 0; 4 * j < width; j++) {
					//���������ɵľ���Q2entropy[16]
					//intraframe_model�д��Ӧ���֡��Ԥ��ģʽ
					//ͬʱ�����Ԥ�����ر�����predicted_pixl4x4_buffer�������ؽ�����
					intra_44_model_chose(4 * i, 4 * j, matrix_frame, reconstructed_frame, predicted_pixl4x4_buffer, (int*)intraframe_model,(int*)Q2entropy); 

					//printf("model:%d\n",intraframe_model[i][j]);

					int p_seek = 0, * p;
					p = huffman(intraframe_model, i, j, &p_seek);
					for (int i = 0; i < p_seek; i++) {
						EnQueue(&bitQ, p[i]);

					}
					free(p);

					//�˴���Ҫ�����ر��룬�����޸�Q2entropy�������ֵ����Ϊ������Ҫ�á�
					int not0 = 0;
					for (int i = 0; i < 16; i++) {
						if (Q2entropy[i] != 0) {
							not0 += 1;
						}
					}
					not0_num[i][j] = not0;
					if (i == 0 && j == 0)
						CAVLC_encode(Q2entropy, 0, 0, &bitQ);
					else if (i == 0 && j != 0)
						CAVLC_encode(Q2entropy, not0_num[i][j-1], 0, &bitQ);
					else if (i != 0 && j == 0)
						CAVLC_encode(Q2entropy, 0, not0_num[i-1][j], &bitQ);
					else
						CAVLC_encode(Q2entropy, not0_num[i][j - 1], not0_num[i - 1][j], &bitQ);
					

					//������+��DCT
					dequant_4x4(Q2entropy, dequant_MF, QP);//Q2entropy����������DCT
					add_IDCT44((int*)predicted_pixl4x4_buffer, Q2entropy);
					for (int x = 0; x < 4; x++) {
						for (int y = 0; y < 4; y++) {
							reconstructed_frame[4 * i + x][4 * j + y] = predicted_pixl4x4_buffer[x][y];
						}
					}
				}
			}
			
			//deblockȥ�����˲�
			deblock_44_intra(reconstructed_frame, (int*) thr_alpha_beta);


			////////////////////////////////////////////////////////////////////////////////////�����һ��I֡�ı��빤��������ȫ�����������˶���bitQ

			ftime(&ts2);//ֹͣ��ʱ
			t1 = (TIME_T)ts1.time * 1000 + ts1.millitm;
			t2 = (TIME_T)ts2.time * 1000 + ts2.millitm;
			//��ȡʱ������msΪ��λ��
			ti = t2 - t1;
			printf("I֡��ʱ��%dms\n",ti);
			printf("R(kbps)%d\n", QueueLength(&bitQ) /1024);

			//����PSNR
			double MSE_TMP = 0;
			for (int i = 0; i < height; i++)
				for (int j = 0; j < width; j++)
					//MSE_TMP += pow((reconstructed_frame[i][j]-matrix_frame[i][j]),2);
					MSE_TMP += pow(((double)reconstructed_frame[i][j] - (double)matrix_frame[i][j]), 2);
			double MSE = MSE_TMP / ((double)height * (double)width);
			double PSNR = (double)10 * log10((double)65025 / MSE);
			printf("PSNR = %lf\n", PSNR);

			printf("��һ�����ص��ؽ�ֵ��%d\n", reconstructed_frame[0][0]);
			printf("succeed!\n");
			fprintf(fpWrite, "(%d,%d)", frame_no, QueueLength(&bitQ) / 1024);
		}
		else //P֡
		{	
			//֡�����
			InitQuene(&bitQ);
			ftime(&ts1);//��ʼ��ʱ
			//��֡���ͺ�QPд��header
			EnQueue(&bitQ, 0); //I֡Ϊ1��P֡Ϊ0
			int temp_bitQP;
			for (int i = 5; i >= 0; i--) {
				temp_bitQP = (QP >> i) % 2;
				EnQueue(&bitQ, temp_bitQP);
			}
			//��ȡ��������abcdef
			//float affine_p[6] = { 0.5,0.5,0.5,0.5,0.5,0.5};
			//��������д��header
			char str[33] ;
			for (int i = 0; i < 6; i++) {
				floattostr(&(affine_p[i]), (char*)str);
				for (int j = 0; j < 32; j++) {
					EnQueue(&bitQ, str[j] - '0');
					
				}
			}
			
			ftime(&ts3);
			inter_4x4_affine(matrix_frame, reconstructed_frame, reconstructed_frame_tmp, dequant_MF, (int*)thr_alpha_beta,(int*)BS[0], affine_p[0], affine_p[1], affine_p[2], affine_p[3], affine_p[4], affine_p[5], &bitQ);//BS[0] ��ʾ�˲�ǿ��Ϊ0+1
			ftime(&ts4);
			memcpy(reconstructed_frame, reconstructed_frame_tmp, sizeof(int) * height * width);
			
			//printf("��һ�����ص��ؽ�ֵ��%d\n", reconstructed_frame[0][0]);
			//printf("succeed!\n");

			////////////////////////////////////////////////////////////////////////////////////�����һ��P֡�ı��빤��������ȫ�����������˶���bitQ
			ftime(&ts2);//ֹͣ��ʱ
			t1 = (TIME_T)ts1.time * 1000 + ts1.millitm;
			t2 = (TIME_T)ts2.time * 1000 + ts2.millitm;
			//��ȡʱ������msΪ��λ��
			ti = t2 - t1;
			printf("P֡��ʱ��%dms\n", ti);
			printf("R(kbps)%d\n", (QueueLength(&bitQ) + height * width / 16) / 1024);

			//PSNR
			double MSE_TMP = 0;
			for (int i = 0; i < height; i++)
				for (int j = 0; j < width; j++)
					//MSE_TMP += pow((reconstructed_frame[i][j]-matrix_frame[i][j]),2);
					MSE_TMP += pow(((double)reconstructed_frame[i][j] - (double)matrix_frame[i][j]), 2);
			double MSE = MSE_TMP / ((double)height * (double)width);
			double PSNR = (double)10 * log10((double)65025 / MSE);
			printf("PSNR = %lf\n", PSNR);
			printf("��һ�����ص��ؽ�ֵ��%d\n", reconstructed_frame[0][0]);
			printf("succeed!\n");
			fprintf(fpWrite, "(%d,%d)", frame_no, QueueLength(&bitQ) / 1024);


		}

		//ÿ���һ֡������һ��ES���룬������ʱ////////////////////////////////////////////////////////////////////////////////////////////////
		//ES������ʱ��
		ftime(&ts1);
		memcpy(ref_frame_buffer[frame_no % 6], reconstructed_frame, sizeof(int)* height* width);
		if ((frame_no) < 5) {
			
			if ((frame_no) != 0) {
				//�˶�����
				int* MVP;
				MVP = MCsearch((int*)ref_frame_buffer[frame_no - 1], (int*)ref_frame_buffer[frame_no], width, height, 33);//���һ�����Ϊ����
				int mark = 0;
				for(int i = 0; 4 * i < height; i++)
					for (int j = 0; 4 * j < width; j++) {
						ref_coordinate[i][j][1] = MVP[mark++];
						ref_coordinate[i][j][0] = MVP[mark++];

						//printf("(%d,%d)->(%d,%d)\n",4*i,4*j, ref_coordinate[i][j][0], ref_coordinate[i][j][1]);
					}
				free(MVP);

				//��R
				double b1 = 0, b2 = 0, b3 = 0, b4 = 0, b5 = 0, b6 = 0;
				for (int i = 0; 4 * i < height; i++)
					for (int j = 0; 4 * j < width;j++) {
						b1 += (double)ref_coordinate[i][j][0] * 4 * (double)i;
						b2 += (double)ref_coordinate[i][j][0] * 4 * (double)j;
						b3 += ref_coordinate[i][j][0];

						b4 += (double)ref_coordinate[i][j][1] * 4 * (double)i ;
						b5 += (double)ref_coordinate[i][j][1] * 4 * (double)j;
						b6 += (double)ref_coordinate[i][j][1];
					}
				affine_p[0] = A[0][0] * b1 + A[0][1] * b2 + A[0][2] * b3;
				affine_p[1] = A[1][0] * b1 + A[1][1] * b2 + A[1][2] * b3;
				affine_p[2] = A[2][0] * b1 + A[2][1] * b2 + A[2][2] * b3;

				affine_p[3] = A[0][0] * b4 + A[0][1] * b5 + A[0][2] * b6;
				affine_p[4] = A[1][0] * b4 + A[1][1] * b5 + A[1][2] * b6;
				affine_p[5] = A[2][0] * b4 + A[2][1] * b5 + A[2][2] * b6;

				printf("a%f,b%f,c%f,d%f,e%f,f%f\n",affine_p[0], affine_p[1], affine_p[2], affine_p[3], affine_p[4], affine_p[5]);

			}
			//������������float R_buffer[5][6]
			memcpy(R_buffer[frame_no%5], affine_p, sizeof(float)*6);

		}
		else {
			//�˶�����

			int* MVP;
			MVP = MCsearch((int*)ref_frame_buffer[(frame_no%6 + 5)%6], (int*)ref_frame_buffer[frame_no%6], width, height, 17);//���һ�����Ϊ����
			int mark = 0;
			for (int i = 0; 4 * i < height; i++)
				for (int j = 0; 4 * j < width; j++) {
					ref_coordinate[i][j][1] = MVP[mark++];
					ref_coordinate[i][j][0] = MVP[mark++];

				}
			free(MVP);
			//��R
			double b1 = 0, b2 = 0, b3 = 0, b4 = 0, b5 = 0, b6 = 0;
			for (int i = 0; 4 * i < height; i++)
				for (int j = 0; 4 * j < width; j++) {
					b1 += (double)ref_coordinate[i][j][0] * 4 * (double)i;
					b2 += (double)ref_coordinate[i][j][0] * 4 * (double)j;
					b3 += ref_coordinate[i][j][0];

					b4 += (double)ref_coordinate[i][j][1] * 4 * (double)i;
					b5 += (double)ref_coordinate[i][j][1] * 4 * (double)j;
					b6 += (double)ref_coordinate[i][j][1];
				}
			affine_p[0] = A[0][0] * b1 + A[0][1] * b2 + A[0][2] * b3;
			affine_p[1] = A[1][0] * b1 + A[1][1] * b2 + A[1][2] * b3;
			affine_p[2] = A[2][0] * b1 + A[2][1] * b2 + A[2][2] * b3;

			affine_p[3] = A[0][0] * b4 + A[0][1] * b5 + A[0][2] * b6;
			affine_p[4] = A[1][0] * b4 + A[1][1] * b5 + A[1][2] * b6;
			affine_p[5] = A[2][0] * b4 + A[2][1] * b5 + A[2][2] * b6;

			printf("a%f,b%f,c%f,d%f,e%f,f%f\n", affine_p[0], affine_p[1], affine_p[2], affine_p[3], affine_p[4], affine_p[5]);
			//������������float R_buffer[5][6]
			memcpy(R_buffer[frame_no % 5], affine_p, sizeof(float) * 6);//������R_buffer�е�5��R���A

			//�ⳬ��������õ�E
			double B[12][3] = { {R_buffer[frame_no%5][0],R_buffer[frame_no % 5][1],R_buffer[frame_no % 5][2]},
				{R_buffer[frame_no % 5][3],R_buffer[frame_no % 5][4],R_buffer[frame_no % 5][5]},
				{0,0,1}, 
				{R_buffer[(frame_no % 5+4)%5][0],R_buffer[(frame_no % 5 + 4) % 5][1],R_buffer[(frame_no % 5 + 4) % 5][2]},
				{R_buffer[(frame_no % 5 + 4) % 5][3],R_buffer[(frame_no % 5 + 4) % 5][4],R_buffer[(frame_no % 5 + 4) % 5][5]},
				{0,0,1},
				{R_buffer[(frame_no % 5 + 3) % 5][0],R_buffer[(frame_no % 5 + 3) % 5][1],R_buffer[(frame_no % 5 + 3) % 5][2]},
				{R_buffer[(frame_no % 5 + 3) % 5][3],R_buffer[(frame_no % 5 + 3) % 5][4],R_buffer[(frame_no % 5 + 3) % 5][5]},
				{0,0,1},
				{R_buffer[(frame_no % 5 + 2) % 5][0],R_buffer[(frame_no % 5 + 2) % 5][1],R_buffer[(frame_no % 5 + 2) % 5][2]},
				{R_buffer[(frame_no % 5 + 2) % 5][3],R_buffer[(frame_no % 5 + 2) % 5][4],R_buffer[(frame_no % 5 + 2) % 5][5]},
				{0,0,1}};
			double B_T[3][12];
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 12; j++)
					B_T[i][j] = B[j][i];
			double C[3][3] = {0};
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
					for (int k = 0; k < 12; k++)
						C[i][j] += B_T[i][k] * B[k][j];
			matrix_inv((double*)C, 3);//����
			double D[3][12] = { 0 };
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 12; j++)
					for (int k = 0; k < 3; k++)
						D[i][j] += C[i][k] * B_T[k][j];
			double E[3][3] = {0};
			double b[12][3] = { 
				{R_buffer[(frame_no % 5 + 4) % 5][0],R_buffer[(frame_no % 5 + 4) % 5][1],R_buffer[(frame_no % 5 + 4) % 5][2]},
				{R_buffer[(frame_no % 5 + 4) % 5][3],R_buffer[(frame_no % 5 + 4) % 5][4],R_buffer[(frame_no % 5 + 4) % 5][5]},
				{0,0,1},
				{R_buffer[(frame_no % 5 + 3) % 5][0],R_buffer[(frame_no % 5 + 3) % 5][1],R_buffer[(frame_no % 5 + 3) % 5][2]},
				{R_buffer[(frame_no % 5 + 3) % 5][3],R_buffer[(frame_no % 5 + 3) % 5][4],R_buffer[(frame_no % 5 + 3) % 5][5]},
				{0,0,1},
				{R_buffer[(frame_no % 5 + 2) % 5][0],R_buffer[(frame_no % 5 + 2) % 5][1],R_buffer[(frame_no % 5 + 2) % 5][2]},
				{R_buffer[(frame_no % 5 + 2) % 5][3],R_buffer[(frame_no % 5 + 2) % 5][4],R_buffer[(frame_no % 5 + 2) % 5][5]},
				{0,0,1},
				{R_buffer[(frame_no % 5 + 1) % 5][0],R_buffer[(frame_no % 5 + 1) % 5][1],R_buffer[(frame_no % 5 + 1) % 5][2]},
				{R_buffer[(frame_no % 5 + 1) % 5][3],R_buffer[(frame_no % 5 + 1) % 5][4],R_buffer[(frame_no % 5 + 1) % 5][5]},
				{0,0,1} };

			for (int i = 0; i < 3; i++) 
				for(int j = 0;j<3;j++)
					for (int k = 0; k < 12; k++) 
						E[i][j] += D[i][k] * b[k][j];
					
			//E[3][3]���Ǿ���A
			//for (int i = 0; i < 3; i++)
			//	for (int j = 0; j < 3; j++)
			//		printf("qq%lf,", E[i][j]);
			//�����֡��R
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 3; j++)
					for (int k = 0; k < 3; k++) 
						affine_p[3*i+j] += R_buffer[frame_no%5][3*i+k] * E[k][j];
					
		
		
		}
		ftime(&ts2);
		t1 = (TIME_T)ts1.time * 1000 + ts1.millitm;
		t2 = (TIME_T)ts2.time * 1000 + ts2.millitm;
		t3 = (TIME_T)ts3.time * 1000 + ts3.millitm;
		t4 = (TIME_T)ts4.time * 1000 + ts4.millitm;
		//��ȡʱ������msΪ��λ��
		tj = t2 - t1;
		//printf("============================================%d\n",tj);
		deltat = (t4 - t3)*0.35;
		printf("ES��ʱ��%dms\n", tj + deltat);
		printf("UAV+ES�ܺ�ʱ��%dms\n", ti+tj+deltat);


		frame_no++;
		if (frame_no == 206) {
			fclose(fpWrite);
			system("pause");
		}
	}
	printf("��Ƶ��֡����%d\n", frame_no);

	



	return 0;



}




