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


void inter_4x4_affine(int matrix_frame[][width], int reconstructed_frame[][width], int(*reconstructed_frame_tmp)[width], int dequant_mf[6][16], int* alpha_beta, int* BS, double a, double b, double c, double d, double e, double f ) {
	
	int residual_matrix[4][4];
	int predicted_pixl4x4_buffer[4][4];
	int x, y, xx, yy;

	//////////////////////////////////////////////////////////////////////将QP和帧类型写入header

	for (int h = 0; 4 * h < height; h++) {
		for (int w = 0; 4 * w < width; w++) {
			//对于每个4x4的块
			x = 4 * h;
			y = 4 * w;
			//求残差
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					xx = (int)(a * ((double)x + (double)i) + b * ((double)y + (double)j) + c);
					yy = (int)(d * ((double)x + (double)i) + e * ((double)y + (double)j) + f);
					if (xx < 0 || xx >= height || yy < 0 || yy >= width) {
						predicted_pixl4x4_buffer[i][j] = 128;
						residual_matrix[i][j] = matrix_frame[x + i][y + j] - 128;
					}
					else{
						predicted_pixl4x4_buffer[i][j] = reconstructed_frame[xx][yy];
						residual_matrix[i][j] = matrix_frame[x + i][y + j] - reconstructed_frame[xx][yy];
					}
				}
			}
			//DCT Q
			int* DCT2Q = DCT4x4(residual_matrix); //dct[16]

			//对数组DCT2Q[16]进行量化

			int MF[6][16] =
			{ { 13107,8066,13107,8066,8066,5243,8066,5243,13107,8066,13107,8066,8066,5243,8066,5243 },
			{ 11916,7490,11916,7490,7490,4660,7490,4660,11916,7490,11916,7490,7490,4660,7490,4660 },
			{ 10082,6554,10082,6554,6554,4194,6554,4194,10082,6554,10082,6554,6554,4194,6554,4194 },
			{ 9362,5825,9362,5825,5825,3647,5825,3647,9362,5825,9362,5825,5825,3647,5825,3647 },
			{ 8192,5243,8192,5243,5243,3355,5243,3355,8192,5243,8192,5243,5243,3355,5243,3355 },
			{ 7282,4559,7282,4559,4559,2893,4559,2893,7282,4559,7282,4559,4559,2893,4559,2893 } };

			int qbits = 15 + QP / 6;

			quant4x4(DCT2Q, MF, pow(2, qbits) / 6, qbits); // 量化结果保存在DCT2Q[16]中了

			//此处还要进行熵编码，不得修改DCT2Q数组的数值，因为反量化要用。

			//反量化+反DCT
			dequant_4x4(DCT2Q, dequant_mf, QP);//DCT2Q继续用来反DCT
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