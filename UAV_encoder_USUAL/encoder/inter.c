#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"DCT.h"
#include"deblock.h"
#include"mc.h"
#include"quant.h"
#include"main.h"
#include"math.h"
#include"intra.h"
#include"inter.h"
#include"entroy.h"

int MF[6][16] =
{ { 13107,8066,13107,8066,8066,5243,8066,5243,13107,8066,13107,8066,8066,5243,8066,5243 },
{ 11916,7490,11916,7490,7490,4660,7490,4660,11916,7490,11916,7490,7490,4660,7490,4660 },
{ 10082,6554,10082,6554,6554,4194,6554,4194,10082,6554,10082,6554,6554,4194,6554,4194 },
{ 9362,5825,9362,5825,5825,3647,5825,3647,9362,5825,9362,5825,5825,3647,5825,3647 },
{ 8192,5243,8192,5243,5243,3355,5243,3355,8192,5243,8192,5243,5243,3355,5243,3355 },
{ 7282,4559,7282,4559,4559,2893,4559,2893,7282,4559,7282,4559,4559,2893,4559,2893 } };
int not0_num_inter[height / 4][width / 4];//��dct16�з�0�ĸ���
int ref_coordinate1[height / 4][width / 4][2] = { 0 };
void inter_4x4_affine(int matrix_frame[][width], int reconstructed_frame[][width], int(*reconstructed_frame_tmp)[width], int dequant_mf[6][16], int* alpha_beta, int* BS, double a, double b, double c, double d, double e, double f , struct Queue* p) {

	int residual_matrix[4][4];
	int predicted_pixl4x4_buffer[4][4];
	int x, y, xx, yy;
	//int not0_num_inter[height / 4][width / 4];//��dct16�з�0�ĸ���

	int* MVP;
	MVP = MCsearch((int*)reconstructed_frame, (int*)matrix_frame, width, height, 33);//���һ�����Ϊ����
	int mark = 0;
	for (int i = 0; 4 * i < height; i++)
		for (int j = 0; 4 * j < width; j++) {
			ref_coordinate1[i][j][1] = MVP[mark++];
			ref_coordinate1[i][j][0] = MVP[mark++];

			//printf("(%d,%d)->(%d,%d)\n",4*i,4*j, ref_coordinate[i][j][0], ref_coordinate[i][j][1]);
		}
	free(MVP);

	for (int h = 0; 4 * h < height; h++) {
		for (int w = 0; 4 * w < width; w++) {
			//����ÿ��4x4�Ŀ�
			x = 4 * h;
			y = 4 * w;
			//��в�
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					//printf("qqqqq%d\n", matrix_frame[x + i][y + j] - reconstructed_frame[ref_coordinate1[h][w][0] + i][ref_coordinate1[h][w][1] + j]);
					predicted_pixl4x4_buffer[i][j] = reconstructed_frame[ref_coordinate1[h][w][0] + i][ref_coordinate1[h][w][1] + j];
					residual_matrix[i][j] = matrix_frame[x + i][y + j] - reconstructed_frame[ref_coordinate1[h][w][0]+i][ref_coordinate1[h][w][1] + j];
					
				}
			}
			//DCT Q
			int* DCT2Q = DCT4x4(residual_matrix); //dct[16]

			//������DCT2Q[16]��������

			//int MF[6][16] =
			//{ { 13107,8066,13107,8066,8066,5243,8066,5243,13107,8066,13107,8066,8066,5243,8066,5243 },
			//{ 11916,7490,11916,7490,7490,4660,7490,4660,11916,7490,11916,7490,7490,4660,7490,4660 },
			//{ 10082,6554,10082,6554,6554,4194,6554,4194,10082,6554,10082,6554,6554,4194,6554,4194 },
			//{ 9362,5825,9362,5825,5825,3647,5825,3647,9362,5825,9362,5825,5825,3647,5825,3647 },
			//{ 8192,5243,8192,5243,5243,3355,5243,3355,8192,5243,8192,5243,5243,3355,5243,3355 },
			//{ 7282,4559,7282,4559,4559,2893,4559,2893,7282,4559,7282,4559,4559,2893,4559,2893 } };

			int qbits = 15 + QP / 6;

			quant4x4(DCT2Q, MF, pow(2, qbits) / 6, qbits); // �������������DCT2Q[16]����

			//�˴���Ҫ�����ر��룬�����޸�DCT2Q�������ֵ����Ϊ������Ҫ�á�
			int not0 = 0;
			for (int i = 0; i < 16; i++) {
				if (DCT2Q[i] != 0) {
					not0 += 1;
				}
			}
			not0_num_inter[h][w] = not0;
			if (h == 0 && w == 0) 
				CAVLC_encode(DCT2Q, 0, 0, p);
			else if (h == 0 && w != 0)
				CAVLC_encode(DCT2Q, not0_num_inter[h][w - 1], 0, p);
			else if (h != 0 && w == 0)
				CAVLC_encode(DCT2Q, 0, not0_num_inter[h - 1][w], p);
			else
				CAVLC_encode(DCT2Q, not0_num_inter[h][w - 1], not0_num_inter[h - 1][w], p);

			//������+��DCT
			dequant_4x4(DCT2Q, dequant_mf, QP);//DCT2Q����������DCT
			add_IDCT44((int*)predicted_pixl4x4_buffer, DCT2Q);
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					reconstructed_frame_tmp[x + i][y + j] = predicted_pixl4x4_buffer[i][j];
				}
			}
			
			free(DCT2Q);
		}

	}

	//deblock
	deblock_44_inter(reconstructed_frame_tmp, (int*)alpha_beta, (int*)BS);









}