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
#include"intra_44_model.h"

//int** intra_44(int** matrix_frame) {
	//模式判断
	//int output_matrix[height][width];
//	int** residual_matrix = (int**)malloc(sizeof(int*) * height);
//	for (int i = 0; i < height; i++)
//		residual_matrix[i] = (int*)malloc(sizeof(int) * width);





//	return residual_matrix;
//}



void intra_44_model_chose(int x, int y, int matrix_frame[][width], int reconstructed_frame[][width], int(*predicted_pixl4x4_buffer)[4], int *intraframe_model, int *Q2entropy) {


	//int** residual_matrix = (int**)malloc(sizeof(int*) * 4);
	//	for (int i = 0; i < 4; i++)
	//		residual_matrix[i] = (int*)malloc(sizeof(int) * 4);
	int residual_matrix[9][4][4];
	int ref_matrix[13];
	int residual2DCT[4][4];
	int predict_pixl[9][4][4];

	if (x == 0) {
		if (y == 0) {
			//左上角的block
			int ref[13] = { 128,128,128,128,128,128,128,128,128,128,128,128,128 }; //13个参考像素 A~M
			memcpy(ref_matrix, ref, sizeof(ref));

		}
		else {
			//顶部
			int ref[13] = { 128,128,128,128,128,128,128,128,reconstructed_frame[x][y - 1],reconstructed_frame[x - 1][y - 1],reconstructed_frame[x - 2][y - 1],reconstructed_frame[x - 3][y - 1],128 };
			memcpy(ref_matrix, ref, sizeof(ref));
		}
	}
	else {
		if (y == 0) {
			//左边
			int ref[13] = { reconstructed_frame[x - 1][y],reconstructed_frame[x - 1][y + 1], reconstructed_frame[x - 1][y + 2], reconstructed_frame[x - 1][y + 3], reconstructed_frame[x - 1][y + 4], reconstructed_frame[x - 1][y + 5], reconstructed_frame[x - 1][y + 6],reconstructed_frame[x - 1][y + 7],128,128,128,128,128 };
			memcpy(ref_matrix, ref, sizeof(ref));
		}
		else {
			//中间(两种情况，1）最右侧 2）中间)
			if (y == width - 4) {
				int ref[13] = { reconstructed_frame[x - 1][y],reconstructed_frame[x - 1][y + 1], reconstructed_frame[x - 1][y + 2], reconstructed_frame[x - 1][y + 3], reconstructed_frame[x - 1][y + 3],reconstructed_frame[x - 1][y + 3],reconstructed_frame[x - 1][y + 3],reconstructed_frame[x - 1][y + 3],reconstructed_frame[x][y - 1],reconstructed_frame[x + 1][y - 1] ,reconstructed_frame[x + 2][y - 1] ,reconstructed_frame[x + 3][y - 1],reconstructed_frame[x - 1][y - 1] };
				memcpy(ref_matrix, ref, sizeof(ref));
			}
			else {
				int ref[13] = { reconstructed_frame[x - 1][y],reconstructed_frame[x - 1][y + 1], reconstructed_frame[x - 1][y + 2], reconstructed_frame[x - 1][y + 3],reconstructed_frame[x - 1][y + 4], reconstructed_frame[x - 1][y + 5], reconstructed_frame[x - 1][y + 6],reconstructed_frame[x - 1][y + 7],reconstructed_frame[x][y - 1],reconstructed_frame[x + 1][y - 1] ,reconstructed_frame[x + 2][y - 1] ,reconstructed_frame[x + 3][y - 1],reconstructed_frame[x - 1][y - 1] };
				memcpy(ref_matrix, ref, sizeof(ref));
			}
		}
	}
	int min_cost = 99999999;


	//model有9种  0：vertical 1: horizontal 2: DC 3........

	intra_4x4_model0((int*)matrix_frame, ref_matrix, (int*)residual_matrix[0], (int*)predict_pixl[0], x, y);
	intra_4x4_model1((int*)matrix_frame, ref_matrix, (int*)residual_matrix[1], (int*)predict_pixl[1], x, y);
	intra_4x4_model2((int*)matrix_frame, ref_matrix, (int*)residual_matrix[2], (int*)predict_pixl[2], x, y);
	intra_4x4_model3((int*)matrix_frame, ref_matrix, (int*)residual_matrix[3], (int*)predict_pixl[3], x, y);
	intra_4x4_model4((int*)matrix_frame, ref_matrix, (int*)residual_matrix[4], (int*)predict_pixl[4], x, y);
	intra_4x4_model5((int*)matrix_frame, ref_matrix, (int*)residual_matrix[5], (int*)predict_pixl[5], x, y);
	intra_4x4_model6((int*)matrix_frame, ref_matrix, (int*)residual_matrix[6], (int*)predict_pixl[6], x, y);
	intra_4x4_model7((int*)matrix_frame, ref_matrix, (int*)residual_matrix[7], (int*)predict_pixl[7], x, y);
	intra_4x4_model8((int*)matrix_frame, ref_matrix, (int*)residual_matrix[8], (int*)predict_pixl[8], x, y);


	int min_num = 0;//用来存储cost最小的那个模式的编号
	for (int i = 0, min_cost = 99999999; i < 9; i++)
	{
		int satd = SATD4x4((int*)residual_matrix[i]);
		if (min_cost > satd) {
			min_cost = satd;
			min_num = i;
		}
	}

	//intraframe_model[(x*width/16+y/4)] = min_num;

	
	memcpy(predicted_pixl4x4_buffer, (int*)predict_pixl[min_num], sizeof(int)*16);
	memcpy(residual2DCT, (int*)residual_matrix[min_num], sizeof(int)*16); //residual2DCT中保存要进行DCT的残差矩阵

	//DCT
	int* DCT2Q = DCT4x4(residual2DCT); //dct[16]

	//对数组DCT2Q[16]进行量化

	int MF[6][16] =
	{ { 13107,8066,13107,8066,8066,5243,8066,5243,13107,8066,13107,8066,8066,5243,8066,5243 },
	{ 11916,7490,11916,7490,7490,4660,7490,4660,11916,7490,11916,7490,7490,4660,7490,4660 },
	{ 10082,6554,10082,6554,6554,4194,6554,4194,10082,6554,10082,6554,6554,4194,6554,4194 },
	{ 9362,5825,9362,5825,5825,3647,5825,3647,9362,5825,9362,5825,5825,3647,5825,3647 },
	{ 8192,5243,8192,5243,5243,3355,5243,3355,8192,5243,8192,5243,5243,3355,5243,3355 },
	{ 7282,4559,7282,4559,4559,2893,4559,2893,7282,4559,7282,4559,4559,2893,4559,2893 } };

	int qbits = 15 + QP / 6;

	quant4x4(DCT2Q, MF, pow(2,qbits) / 3, qbits); // 量化结果保存在DCT2Q[16]中了

	memcpy(Q2entropy, DCT2Q, sizeof(int)*16);
	


	free(DCT2Q);
}