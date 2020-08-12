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

//#define width 1280 //�ֱ�����main.h�ж���
//#define height 720
I_frame_interval = 40; //ÿ����֡һ��I֡



int matrix_frame[height][width];//�浱ǰ����֡
int reconstructed_frame[height][width];//�����ؽ�֡����Ϊ��һ֡�Ĳο�֡
int reconstructed_frame_tmp[height][width];//��֡���˶��������м���
int intraframe_model[height / 4][width / 4];//��֡��Ԥ���9��ģʽ

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

	//��ȡYUV420Դ�ļ�
	//YUV420��ÿ֡��ǰwidth * height���ֽ�ΪY���м�(width * height)/4���ֽ�ΪU,���(width * height)/4���ֽ�ΪV
	int frame_size = (width * height * 3) >> 1; 
	unsigned char* buff = (unsigned char*)malloc(frame_size);
	FILE* video_yuv;
	errno_t err = fopen_s(&video_yuv, "C:/Users/Administrator/Desktop/car_test_dec.yuv", "rb");
	if (err) {
		printf("open file failed, directory or file not found.\n");
		return 0;
	}


	int frame_no=0;
	while (fread(buff, sizeof(char), frame_size, video_yuv)) { 	//fread(buff, sizeof(char), frame_size, video_yuv);  �˺���ִ��һ�ξ��Ƕ�һ֡
		frame_no++;
		printf("��%d֡�ĵ�һ������ֵ��%d\n", frame_no,*buff);
		//��һ֡��Y�������浽��ά����matrix_frame��
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				matrix_frame[i][j] = *(buff + width * i + j);
			}
		}

		if (!((frame_no - 1) % I_frame_interval))  //�ж��Ƿ�ΪI֡
		{
			//֡�ڱ���
			int predicted_pixl4x4_buffer[4][4];
			int Q2entropy[16];
			//////////////////////////////////////////////////////////////////////��QP��֡����д��header


			for (int i = 0; 4 * i < height; i++) {
				for (int j = 0; 4 * j < width; j++) {
					//���������ɵľ���Q2entropy[16]
					//intraframe_model�д��Ӧ���֡��Ԥ��ģʽ
					//ͬʱ�����Ԥ�����ر�����predicted_pixl4x4_buffer�������ؽ�����
					intra_44_model_chose(4 * i, 4 * j, matrix_frame, reconstructed_frame, predicted_pixl4x4_buffer, (int*)intraframe_model,(int*)Q2entropy); 
					
					//�˴���Ҫ�����ر��룬�����޸�Q2entropy�������ֵ����Ϊ������Ҫ�á�

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

			printf("��һ�����ص��ؽ�ֵ��%d\n", reconstructed_frame[0][0]);
			printf("succeed!\n");
		}
		else //P֡
		{
			//֡�����
			//��ȡ��������
			double a = 0.5;
			double b = 0.5;
			double c = 0.5;
			double d = 0.5;
			double e = 0.5;
			double f = 0.5;
			inter_4x4_affine(matrix_frame, reconstructed_frame, reconstructed_frame_tmp, dequant_MF, (int*)thr_alpha_beta,(int*)BS[0], a, b, c, d, e, f);//BS[0] ��ʾ�˲�ǿ��Ϊ0+1
			memcpy(reconstructed_frame, reconstructed_frame_tmp, sizeof(height * width));
			
			printf("��һ�����ص��ؽ�ֵ��%d\n", reconstructed_frame[0][0]);
			printf("succeed!\n");





		}
		//if (frame_no == 2) {
		//	break;
		//}

	}
	printf("��Ƶ��֡����%d\n", frame_no);

	



	return 0;



}




