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

//#define width 1280 //分辨率在main.h中定义
//#define height 720
I_frame_interval = 40; //每多少帧一个I帧



int matrix_frame[height][width];//存当前编码帧
int reconstructed_frame[height][width];//创建重建帧，作为下一帧的参考帧
int reconstructed_frame_tmp[height][width];//存帧间运动补偿的中间结果
int intraframe_model[height / 4][width / 4];//存帧内预测的9种模式

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

	//读取YUV420源文件
	//YUV420的每帧的前width * height个字节为Y，中间(width * height)/4个字节为U,最后(width * height)/4个字节为V
	int frame_size = (width * height * 3) >> 1; 
	unsigned char* buff = (unsigned char*)malloc(frame_size);
	FILE* video_yuv;
	errno_t err = fopen_s(&video_yuv, "C:/Users/Administrator/Desktop/car_test_dec.yuv", "rb");
	if (err) {
		printf("open file failed, directory or file not found.\n");
		return 0;
	}


	int frame_no=0;
	while (fread(buff, sizeof(char), frame_size, video_yuv)) { 	//fread(buff, sizeof(char), frame_size, video_yuv);  此函数执行一次就是读一帧
		frame_no++;
		printf("第%d帧的第一个像素值：%d\n", frame_no,*buff);
		//将一帧的Y分量保存到二维数组matrix_frame中
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				matrix_frame[i][j] = *(buff + width * i + j);
			}
		}

		if (!((frame_no - 1) % I_frame_interval))  //判断是否为I帧
		{
			//帧内编码
			int predicted_pixl4x4_buffer[4][4];
			int Q2entropy[16];
			//////////////////////////////////////////////////////////////////////将QP和帧类型写入header


			for (int i = 0; 4 * i < height; i++) {
				for (int j = 0; 4 * j < width; j++) {
					//输出量化完成的矩阵Q2entropy[16]
					//intraframe_model中存对应块的帧内预测模式
					//同时将最佳预测像素保存至predicted_pixl4x4_buffer中用于重建像素
					intra_44_model_chose(4 * i, 4 * j, matrix_frame, reconstructed_frame, predicted_pixl4x4_buffer, (int*)intraframe_model,(int*)Q2entropy); 
					
					//此处还要进行熵编码，不得修改Q2entropy数组的数值，因为反量化要用。

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

			printf("第一个像素的重建值：%d\n", reconstructed_frame[0][0]);
			printf("succeed!\n");
		}
		else //P帧
		{
			//帧间编码
			//读取六个参数
			double a = 0.5;
			double b = 0.5;
			double c = 0.5;
			double d = 0.5;
			double e = 0.5;
			double f = 0.5;
			inter_4x4_affine(matrix_frame, reconstructed_frame, reconstructed_frame_tmp, dequant_MF, (int*)thr_alpha_beta,(int*)BS[0], a, b, c, d, e, f);//BS[0] 表示滤波强度为0+1
			memcpy(reconstructed_frame, reconstructed_frame_tmp, sizeof(height * width));
			
			printf("第一个像素的重建值：%d\n", reconstructed_frame[0][0]);
			printf("succeed!\n");





		}
		//if (frame_no == 2) {
		//	break;
		//}

	}
	printf("视频总帧数：%d\n", frame_no);

	



	return 0;



}




