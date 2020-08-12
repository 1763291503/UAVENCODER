#include"intra_44_model.h"
#include"main.h"

//哈达玛变换
int SATD4x4(int* residual_matrix) {
	int m[16];
	int d[16];
	int satd = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			//printf("res%d\n", residual_matrix[4*i + j]);
			d[4 * i + j] = residual_matrix[4 * i + j];

		}
	}

	m[0] = d[0] + d[12];
	m[4] = d[4] + d[8];
	m[8] = d[4] - d[8];
	m[12] = d[0] - d[12];
	m[1] = d[1] + d[13];
	m[5] = d[5] + d[9];
	m[9] = d[5] - d[9];
	m[13] = d[1] - d[13];
	m[2] = d[2] + d[14];
	m[6] = d[6] + d[10];
	m[10] = d[6] - d[10];
	m[14] = d[2] - d[14];
	m[3] = d[3] + d[15];
	m[7] = d[7] + d[11];
	m[11] = d[7] - d[11];
	m[15] = d[3] - d[15];

	d[0] = m[0] + m[4];
	d[8] = m[0] - m[4];
	d[4] = m[8] + m[12];
	d[12] = m[12] - m[8];
	d[1] = m[1] + m[5];
	d[9] = m[1] - m[5];
	d[5] = m[9] + m[13];
	d[13] = m[13] - m[9];
	d[2] = m[2] + m[6];
	d[10] = m[2] - m[6];
	d[6] = m[10] + m[14];
	d[14] = m[14] - m[10];
	d[3] = m[3] + m[7];
	d[11] = m[3] - m[7];
	d[7] = m[11] + m[15];
	d[15] = m[15] - m[11];

	m[0] = d[0] + d[3];
	m[1] = d[1] + d[2];
	m[2] = d[1] - d[2];
	m[3] = d[0] - d[3];
	m[4] = d[4] + d[7];
	m[5] = d[5] + d[6];
	m[6] = d[5] - d[6];
	m[7] = d[4] - d[7];
	m[8] = d[8] + d[11];
	m[9] = d[9] + d[10];
	m[10] = d[9] - d[10];
	m[11] = d[8] - d[11];
	m[12] = d[12] + d[15];
	m[13] = d[13] + d[14];
	m[14] = d[13] - d[14];
	m[15] = d[12] - d[15];

	d[0] = m[0] + m[1];
	d[1] = m[0] - m[1];
	d[2] = m[2] + m[3];
	d[3] = m[3] - m[2];
	d[4] = m[4] + m[5];
	d[5] = m[4] - m[5];
	d[6] = m[6] + m[7];
	d[7] = m[7] - m[6];
	d[8] = m[8] + m[9];
	d[9] = m[8] - m[9];
	d[10] = m[10] + m[11];
	d[11] = m[11] - m[10];
	d[12] = m[12] + m[13];
	d[13] = m[12] - m[13];
	d[14] = m[14] + m[15];
	d[15] = m[15] - m[14];

	for (int k = 0; k < 16; k++) {
		satd += (d[k] < 0 ? -d[k] : d[k]);
	}

	return satd;
}

void intra_4x4_model0(int* matrix_frame, int* ref_matrix, int* residual_matrix, int* predict_pixl, int x, int y)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			predict_pixl[4 * i + j] = ref_matrix[j]; //i行j列，预测值，用于重建图像
			residual_matrix[4 * i + j] = matrix_frame[width * (x + i) + y + j] - predict_pixl[4 * i + j];//残差=原始值-预测值
		}
	}
}
void intra_4x4_model1(int* matrix_frame, int* ref_matrix, int* residual_matrix, int* predict_pixl, int x, int y)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			predict_pixl[4 * i + j] = ref_matrix[i + 8];
			residual_matrix[4 * i + j] = matrix_frame[width * (x + i) + y + j] - predict_pixl[4 * i + j];
		}
	}
}
void intra_4x4_model2(int* matrix_frame, int* ref_matrix, int* residual_matrix, int* predict_pixl, int x, int y)
{
	int sum = (ref_matrix[0] + ref_matrix[1] + ref_matrix[2] + ref_matrix[3] + ref_matrix[8] + ref_matrix[9] + ref_matrix[10] + ref_matrix[11] + 4) >> 3;//求平均
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			predict_pixl[4 * i + j] = sum; //预测值
			residual_matrix[4 * i + j] = matrix_frame[width * (x + i) + y + j] - predict_pixl[4 * i + j];
		}
	}
}
void intra_4x4_model3(int* matrix_frame, int* ref_matrix, int* residual_matrix, int* predict_pixl, int x, int y)
{

	predict_pixl[4 * 0 + 0] = (ref_matrix[0] + 2 * ref_matrix[1] + ref_matrix[2] + 2) >> 2;//a
	predict_pixl[4 * 0 + 1] =
		predict_pixl[4 * 1 + 0] = (ref_matrix[1] + 2 * ref_matrix[2] + ref_matrix[3] + 2) >> 2;//be
	predict_pixl[4 * 0 + 2] =
		predict_pixl[4 * 1 + 1] =
		predict_pixl[4 * 2 + 0] = (ref_matrix[2] + 2 * ref_matrix[3] + ref_matrix[4] + 2) >> 2;//cfi
	predict_pixl[4 * 0 + 3] =
		predict_pixl[4 * 1 + 2] =
		predict_pixl[4 * 2 + 1] =
		predict_pixl[4 * 3 + 0] = (ref_matrix[3] + 2 * ref_matrix[4] + ref_matrix[5] + 2) >> 2;//dgjm
	predict_pixl[4 * 1 + 3] =
		predict_pixl[4 * 2 + 2] =
		predict_pixl[4 * 3 + 1] = (ref_matrix[4] + 2 * ref_matrix[5] + ref_matrix[6] + 2) >> 2;//hkn
	predict_pixl[4 * 2 + 3] =
		predict_pixl[4 * 3 + 2] = (ref_matrix[5] + 2 * ref_matrix[6] + ref_matrix[7] + 2) >> 2;//lo
	predict_pixl[4 * 3 + 3] = (ref_matrix[6] + 3 * ref_matrix[7] + 2) >> 2;//p

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			residual_matrix[4 * i + j] = matrix_frame[width * (x + i) + y + j] - predict_pixl[4 * i + j];
		}
	}

}
void intra_4x4_model4(int* matrix_frame, int* ref_matrix, int* residual_matrix, int* predict_pixl, int x, int y)
{
	predict_pixl[4 * 0 + 3] = (ref_matrix[1] + 2 * ref_matrix[2] + ref_matrix[3] + 2) >> 2;//d
	predict_pixl[4 * 1 + 3] =
		predict_pixl[4 * 0 + 2] = (ref_matrix[0] + 2 * ref_matrix[1] + ref_matrix[2] + 2) >> 2;//ch
	predict_pixl[4 * 0 + 1] =
		predict_pixl[4 * 1 + 2] =
		predict_pixl[4 * 2 + 3] = (ref_matrix[12] + 2 * ref_matrix[0] + ref_matrix[1] + 2) >> 2;//bgl
	predict_pixl[4 * 0 + 0] =
		predict_pixl[4 * 1 + 1] =
		predict_pixl[4 * 2 + 2] =
		predict_pixl[4 * 3 + 3] = (ref_matrix[8] + 2 * ref_matrix[12] + ref_matrix[1] + 2) >> 2;//afkp
	predict_pixl[4 * 1 + 0] =
		predict_pixl[4 * 2 + 1] =
		predict_pixl[4 * 3 + 2] = (ref_matrix[9] + 2 * ref_matrix[8] + ref_matrix[12] + 2) >> 2;//ejo
	predict_pixl[4 * 2 + 0] =
		predict_pixl[4 * 3 + 1] = (ref_matrix[10] + 2 * ref_matrix[9] + ref_matrix[8] + 2) >> 2;//in
	predict_pixl[4 * 3 + 0] = (ref_matrix[11] + 2 * ref_matrix[10] + ref_matrix[9] + 2) >> 2;//m

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			residual_matrix[4 * i + j] = matrix_frame[width * (x + i) + y + j] - predict_pixl[4 * i + j];
		}
	}
}
void intra_4x4_model5(int* matrix_frame, int* ref_matrix, int* residual_matrix, int* predict_pixl, int x, int y)
{
	predict_pixl[4 * 0 + 0] =
		predict_pixl[4 * 2 + 1] = (ref_matrix[12] + ref_matrix[0] + 1) >> 1;//aj
	predict_pixl[4 * 2 + 2] =
		predict_pixl[4 * 0 + 1] = (ref_matrix[0] + ref_matrix[1] + 1) >> 1;//bk
	predict_pixl[4 * 2 + 3] =
		predict_pixl[4 * 0 + 2] = (ref_matrix[1] + ref_matrix[2] + 1) >> 1;//cl
	predict_pixl[4 * 0 + 3] = (ref_matrix[2] + ref_matrix[3] + 1) >> 1;//d
	predict_pixl[4 * 2 + 0] = (ref_matrix[9] + ref_matrix[10] + 1) >> 1;//i
	predict_pixl[4 * 1 + 0] =
		predict_pixl[4 * 3 + 1] = (ref_matrix[8] + 2 * ref_matrix[12] + ref_matrix[0] + 2) >> 2;//en
	predict_pixl[4 * 1 + 1] =
		predict_pixl[4 * 3 + 2] = (ref_matrix[12] + 2 * ref_matrix[0] + ref_matrix[1] + 2) >> 2;//fo
	predict_pixl[4 * 1 + 2] =
		predict_pixl[4 * 3 + 3] = (ref_matrix[0] + 2 * ref_matrix[1] + ref_matrix[2] + 2) >> 2;//gp
	predict_pixl[4 * 1 + 3] = (ref_matrix[1] + 2 * ref_matrix[2] + ref_matrix[3] + 2) >> 2;//h
	predict_pixl[4 * 3 + 0] = (ref_matrix[9] + 2 * ref_matrix[10] + ref_matrix[11] + 2) >> 2;//m

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			residual_matrix[4 * i + j] = matrix_frame[width * (x + i) + y + j] - predict_pixl[4 * i + j];
		}
	}

}
void intra_4x4_model6(int* matrix_frame, int* ref_matrix, int* residual_matrix, int* predict_pixl, int x, int y)
{
	predict_pixl[4 * 0 + 0] =
		predict_pixl[4 * 1 + 2] = (ref_matrix[8] + ref_matrix[12] + 1) >> 1;//ag
	predict_pixl[4 * 1 + 0] =
		predict_pixl[4 * 2 + 2] = (ref_matrix[8] + ref_matrix[9] + 1) >> 1;//ek
	predict_pixl[4 * 2 + 0] =
		predict_pixl[4 * 3 + 2] = (ref_matrix[9] + ref_matrix[10] + 1) >> 1;//io
	predict_pixl[4 * 3 + 0] = (ref_matrix[10] + ref_matrix[11] + 1) >> 1;//m
	predict_pixl[4 * 0 + 2] = (ref_matrix[1] + ref_matrix[2] + 1) >> 1;//c
	predict_pixl[4 * 0 + 1] =
		predict_pixl[4 * 1 + 3] = (ref_matrix[8] + 2 * ref_matrix[12] + ref_matrix[0] + 2) >> 2;//bh
	predict_pixl[4 * 1 + 1] =
		predict_pixl[4 * 2 + 3] = (ref_matrix[9] + 2 * ref_matrix[8] + ref_matrix[12] + 2) >> 2;//fl
	predict_pixl[4 * 2 + 1] =
		predict_pixl[4 * 3 + 3] = (ref_matrix[10] + 2 * ref_matrix[9] + ref_matrix[8] + 2) >> 2;//jp
	predict_pixl[4 * 3 + 1] = (ref_matrix[11] + 2 * ref_matrix[10] + ref_matrix[9] + 2) >> 2;//n
	predict_pixl[4 * 0 + 3] = (ref_matrix[1] + 2 * ref_matrix[2] + ref_matrix[3] + 2) >> 2;//d

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			residual_matrix[4 * i + j] = matrix_frame[width * (x + i) + y + j] - predict_pixl[4 * i + j];
		}
	}
}
void intra_4x4_model7(int* matrix_frame, int* ref_matrix, int* residual_matrix, int* predict_pixl, int x, int y)
{

	predict_pixl[4 * 0 + 3] =
		predict_pixl[4 * 2 + 2] = (ref_matrix[3] + ref_matrix[4] + 1) >> 1;//dk
	predict_pixl[4 * 0 + 2] =
		predict_pixl[4 * 2 + 1] = (ref_matrix[2] + ref_matrix[3] + 1) >> 1;//cj
	predict_pixl[4 * 2 + 0] =
		predict_pixl[4 * 0 + 1] = (ref_matrix[1] + ref_matrix[2] + 1) >> 1;//bi
	predict_pixl[4 * 0 + 0] = (ref_matrix[0] + ref_matrix[1] + 1) >> 1;//a
	predict_pixl[4 * 2 + 3] = (ref_matrix[4] + ref_matrix[5] + 1) >> 1;//l
	predict_pixl[4 * 1 + 3] =
		predict_pixl[4 * 3 + 2] = (ref_matrix[3] + 2 * ref_matrix[4] + ref_matrix[5] + 2) >> 2;//ho
	predict_pixl[4 * 1 + 2] =
		predict_pixl[4 * 3 + 1] = (ref_matrix[2] + 2 * ref_matrix[3] + ref_matrix[4] + 2) >> 2;//gn
	predict_pixl[4 * 1 + 1] =
		predict_pixl[4 * 3 + 0] = (ref_matrix[1] + 2 * ref_matrix[2] + ref_matrix[3] + 2) >> 2;//fm
	predict_pixl[4 * 1 + 0] = (ref_matrix[0] + 2 * ref_matrix[1] + ref_matrix[2] + 2) >> 2;//e
	predict_pixl[4 * 3 + 3] = (ref_matrix[4] + 2 * ref_matrix[5] + ref_matrix[6] + 2) >> 2;//p

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			residual_matrix[4 * i + j] = matrix_frame[width * (x + i) + y + j] - predict_pixl[4 * i + j];
		}
	}

}
void intra_4x4_model8(int* matrix_frame, int* ref_matrix, int* residual_matrix, int* predict_pixl, int x, int y)
{
	predict_pixl[4 * 0 + 2] =
		predict_pixl[4 * 1 + 0] = (ref_matrix[9] + ref_matrix[10] + 1) >> 1;//ec
	predict_pixl[4 * 1 + 2] =
		predict_pixl[4 * 2 + 0] = (ref_matrix[11] + ref_matrix[10] + 1) >> 1;//ig
	predict_pixl[4 * 0 + 0] = (ref_matrix[8] + ref_matrix[9] + 1) >> 1;//a
	predict_pixl[4 * 1 + 1] =
		predict_pixl[4 * 0 + 3] = (ref_matrix[10] + 2 * ref_matrix[9] + ref_matrix[8] + 2) >> 2;//fd
	predict_pixl[4 * 2 + 1] =
		predict_pixl[4 * 1 + 3] = (ref_matrix[11] + 2 * ref_matrix[10] + ref_matrix[9] + 2) >> 2;//jh
	predict_pixl[4 * 0 + 1] = (ref_matrix[8] + 2 * ref_matrix[9] + ref_matrix[12] + 2) >> 2;//b
	predict_pixl[4 * 3 + 0] =
		predict_pixl[4 * 3 + 1] =
		predict_pixl[4 * 3 + 2] =
		predict_pixl[4 * 3 + 3] =
		predict_pixl[4 * 2 + 3] =
		predict_pixl[4 * 2 + 2] = ref_matrix[11];//mnoplk

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			residual_matrix[4 * i + j] = matrix_frame[width * (x + i) + y + j] - predict_pixl[4 * i + j];
		}
	}
}