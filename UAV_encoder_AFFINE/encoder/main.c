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




//#define width 1280 //分辨率在main.h中定义
//#define height 720
int I_frame_interval = 50; //每多少帧一个I帧


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

int matrix_frame[height][width];//存当前编码帧
int reconstructed_frame[height][width];//创建重建帧，作为下一帧的参考帧
int reconstructed_frame_tmp[height][width];//存帧间运动补偿的中间结果
int intraframe_model[height / 4][width / 4];//存帧内预测的9种模式
int not0_num[height / 4][width / 4];//存dct16中非0的个数
int ref_frame_buffer[6][height][width];
float R_buffer[5][6];
float affine_p[6] = { 1,0,0,0,1,0 }; //六参数
double A[3][3] = { 0 };
int ref_coordinate[height / 4][width / 4][2] = {0};

struct Queue bitQ; //比特队列缓存

//dequant_MF矩阵
int dequant_MF[6][16] =
{ {10, 13, 10, 13, 13, 16, 13, 16, 10, 13, 10, 13, 13, 16, 13, 16 },
{ 11, 14, 11, 14, 14, 18, 14, 18, 11, 14, 11, 14, 14, 18, 14, 18 },
{ 13, 16, 13, 16, 16, 20, 16, 20, 13, 16, 13, 16, 16, 20, 16, 20 },
{ 14, 18, 14, 18, 18, 23, 18, 23, 14, 18, 14, 18, 18, 23, 18, 23 },
{ 16, 20, 16, 20, 20, 25, 20, 25, 16, 20, 16, 20, 20, 25, 20, 25 },
{ 18, 23, 18, 23, 23, 29, 23, 29, 18, 23, 18, 23, 23, 29, 23, 29 } };
//deblock 判断边界所用阈值0：alpha，1：beta
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

	matrix_inv((double*)A, 3);//求逆

	//for (int i = 0; i < 3; i++)
	//	for (int j = 0; j < 3; j++)
	//		printf("s%.30lf\n,", A[i][j]);
	//printf("\n");

	InitQuene(&bitQ);
	//读取YUV420源文件
	//YUV420的每帧的前width * height个字节为Y，中间(width * height)/4个字节为U,最后(width * height)/4个字节为V
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
	while (fread(buff, sizeof(char), frame_size, video_yuv)) { 	//fread(buff, sizeof(char), frame_size, video_yuv);  此函数执行一次就是读一帧
		
		printf("第%d帧的第一个像素值：%d\n", frame_no,*buff);
		//将一帧的Y分量保存到二维数组matrix_frame中
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				matrix_frame[i][j] = *(buff + width * i + j);
			}
		}

		if (!(frame_no % I_frame_interval))  //判断是否为I帧
		{
			//帧内编码
			InitQuene(&bitQ);

			ftime(&ts1);//开始计时

			int predicted_pixl4x4_buffer[4][4];
			int Q2entropy[16];
			//将帧类型和QP写入header
			EnQueue(&bitQ, 1); //I帧为1，P帧为0
			int temp_bitQP;
			for (int i = 5; i >= 0; i--) {
				temp_bitQP = (QP >> i) % 2;
				EnQueue(&bitQ, temp_bitQP);
			}
			for (int i = 0; 4 * i < height; i++) {
				for (int j = 0; 4 * j < width; j++) {
					//输出量化完成的矩阵Q2entropy[16]
					//intraframe_model中存对应块的帧内预测模式
					//同时将最佳预测像素保存至predicted_pixl4x4_buffer中用于重建像素
					intra_44_model_chose(4 * i, 4 * j, matrix_frame, reconstructed_frame, predicted_pixl4x4_buffer, (int*)intraframe_model,(int*)Q2entropy); 

					//printf("model:%d\n",intraframe_model[i][j]);

					int p_seek = 0, * p;
					p = huffman(intraframe_model, i, j, &p_seek);
					for (int i = 0; i < p_seek; i++) {
						EnQueue(&bitQ, p[i]);

					}
					free(p);

					//此处还要进行熵编码，不得修改Q2entropy数组的数值，因为反量化要用。
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
					

					//反量化+反DCT
					dequant_4x4(Q2entropy, dequant_MF, QP);//Q2entropy继续用来反DCT
					add_IDCT44((int*)predicted_pixl4x4_buffer, Q2entropy);
					for (int x = 0; x < 4; x++) {
						for (int y = 0; y < 4; y++) {
							reconstructed_frame[4 * i + x][4 * j + y] = predicted_pixl4x4_buffer[x][y];
						}
					}
				}
			}
			
			//deblock去方块滤波
			deblock_44_intra(reconstructed_frame, (int*) thr_alpha_beta);


			////////////////////////////////////////////////////////////////////////////////////已完成一个I帧的编码工作，并将全部码流存入了队列bitQ

			ftime(&ts2);//停止计时
			t1 = (TIME_T)ts1.time * 1000 + ts1.millitm;
			t2 = (TIME_T)ts2.time * 1000 + ts2.millitm;
			//获取时间间隔，ms为单位的
			ti = t2 - t1;
			printf("I帧耗时：%dms\n",ti);
			printf("R(kbps)%d\n", QueueLength(&bitQ) /1024);

			//计算PSNR
			double MSE_TMP = 0;
			for (int i = 0; i < height; i++)
				for (int j = 0; j < width; j++)
					//MSE_TMP += pow((reconstructed_frame[i][j]-matrix_frame[i][j]),2);
					MSE_TMP += pow(((double)reconstructed_frame[i][j] - (double)matrix_frame[i][j]), 2);
			double MSE = MSE_TMP / ((double)height * (double)width);
			double PSNR = (double)10 * log10((double)65025 / MSE);
			printf("PSNR = %lf\n", PSNR);

			printf("第一个像素的重建值：%d\n", reconstructed_frame[0][0]);
			printf("succeed!\n");
			fprintf(fpWrite, "(%d,%d)", frame_no, QueueLength(&bitQ) / 1024);
		}
		else //P帧
		{	
			//帧间编码
			InitQuene(&bitQ);
			ftime(&ts1);//开始计时
			//将帧类型和QP写入header
			EnQueue(&bitQ, 0); //I帧为1，P帧为0
			int temp_bitQP;
			for (int i = 5; i >= 0; i--) {
				temp_bitQP = (QP >> i) % 2;
				EnQueue(&bitQ, temp_bitQP);
			}
			//读取六个参数abcdef
			//float affine_p[6] = { 0.5,0.5,0.5,0.5,0.5,0.5};
			//将六参数写入header
			char str[33] ;
			for (int i = 0; i < 6; i++) {
				floattostr(&(affine_p[i]), (char*)str);
				for (int j = 0; j < 32; j++) {
					EnQueue(&bitQ, str[j] - '0');
					
				}
			}
			
			ftime(&ts3);
			inter_4x4_affine(matrix_frame, reconstructed_frame, reconstructed_frame_tmp, dequant_MF, (int*)thr_alpha_beta,(int*)BS[0], affine_p[0], affine_p[1], affine_p[2], affine_p[3], affine_p[4], affine_p[5], &bitQ);//BS[0] 表示滤波强度为0+1
			ftime(&ts4);
			memcpy(reconstructed_frame, reconstructed_frame_tmp, sizeof(int) * height * width);
			
			//printf("第一个像素的重建值：%d\n", reconstructed_frame[0][0]);
			//printf("succeed!\n");

			////////////////////////////////////////////////////////////////////////////////////已完成一个P帧的编码工作，并将全部码流存入了队列bitQ
			ftime(&ts2);//停止计时
			t1 = (TIME_T)ts1.time * 1000 + ts1.millitm;
			t2 = (TIME_T)ts2.time * 1000 + ts2.millitm;
			//获取时间间隔，ms为单位的
			ti = t2 - t1;
			printf("P帧耗时：%dms\n", ti);
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
			printf("第一个像素的重建值：%d\n", reconstructed_frame[0][0]);
			printf("succeed!\n");
			fprintf(fpWrite, "(%d,%d)", frame_no, QueueLength(&bitQ) / 1024);


		}

		//每完成一帧，进行一次ES编码，独立计时////////////////////////////////////////////////////////////////////////////////////////////////
		//ES端消耗时间
		ftime(&ts1);
		memcpy(ref_frame_buffer[frame_no % 6], reconstructed_frame, sizeof(int)* height* width);
		if ((frame_no) < 5) {
			
			if ((frame_no) != 0) {
				//运动估计
				int* MVP;
				MVP = MCsearch((int*)ref_frame_buffer[frame_no - 1], (int*)ref_frame_buffer[frame_no], width, height, 33);//最后一项必须为奇数
				int mark = 0;
				for(int i = 0; 4 * i < height; i++)
					for (int j = 0; 4 * j < width; j++) {
						ref_coordinate[i][j][1] = MVP[mark++];
						ref_coordinate[i][j][0] = MVP[mark++];

						//printf("(%d,%d)->(%d,%d)\n",4*i,4*j, ref_coordinate[i][j][0], ref_coordinate[i][j][1]);
					}
				free(MVP);

				//算R
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
			//将六参数存入float R_buffer[5][6]
			memcpy(R_buffer[frame_no%5], affine_p, sizeof(float)*6);

		}
		else {
			//运动估计

			int* MVP;
			MVP = MCsearch((int*)ref_frame_buffer[(frame_no%6 + 5)%6], (int*)ref_frame_buffer[frame_no%6], width, height, 17);//最后一项必须为奇数
			int mark = 0;
			for (int i = 0; 4 * i < height; i++)
				for (int j = 0; 4 * j < width; j++) {
					ref_coordinate[i][j][1] = MVP[mark++];
					ref_coordinate[i][j][0] = MVP[mark++];

				}
			free(MVP);
			//算R
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
			//将六参数存入float R_buffer[5][6]
			memcpy(R_buffer[frame_no % 5], affine_p, sizeof(float) * 6);//下面用R_buffer中的5个R求解A

			//解超定方程组得到E
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
			matrix_inv((double*)C, 3);//求逆
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
					
			//E[3][3]就是矩阵A
			//for (int i = 0; i < 3; i++)
			//	for (int j = 0; j < 3; j++)
			//		printf("qq%lf,", E[i][j]);
			//算编码帧的R
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
		//获取时间间隔，ms为单位的
		tj = t2 - t1;
		//printf("============================================%d\n",tj);
		deltat = (t4 - t3)*0.35;
		printf("ES耗时：%dms\n", tj + deltat);
		printf("UAV+ES总耗时：%dms\n", ti+tj+deltat);


		frame_no++;
		if (frame_no == 206) {
			fclose(fpWrite);
			system("pause");
		}
	}
	printf("视频总帧数：%d\n", frame_no);

	



	return 0;



}




