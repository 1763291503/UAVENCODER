

/*

//此部分用于将二维矩阵按元素写入"C:/Users/Administrator/Desktop/1.txt"的每一行。一行一个像素
			FILE* fp;
			if (fopen_s(&fp, "C:/Users/Administrator/Desktop/inter20.txt", "wb"))
			{
				printf("\nopen file Fail,close!");
				getchar();
				exit(1);
			}
			for (int i = 0; i < 720; i++) {
				for (int j = 0; j < 1280; j++) {
					fprintf(fp, "%d\n", reconstructed_frame[i][j]);
				}
			}
			fclose(fp);



//求矩阵的逆
int  matrix_inv(double* a_matrix, int ndimen)
////////////////////////////////////////////////////////////////////////////
//	a_matrix:矩阵
//	ndimen :维数
////////////////////////////////////////////////////////////////////////////
{
	double tmp, tmp2, b_tmp[20], c_tmp[20];
	int k, k1, k2, k3, j, i, j2, i2, kme[20], kmf[20];
	i2 = j2 = 0;

	for (k = 0; k < ndimen; k++)
	{
		tmp2 = 0.0;
		for (i = k; i < ndimen; i++)
		{
			for (j = k; j < ndimen; j++)
			{
				if (fabs(a_matrix[i*ndimen+j]) <= fabs(tmp2))
					continue;
				tmp2 = a_matrix[i * ndimen + j];
				i2 = i;
				j2 = j;
			}
		}
		if (i2 != k)
		{
			for (j = 0; j < ndimen; j++)
			{
				tmp = a_matrix[i2 * ndimen + j];
				a_matrix[i2 * ndimen + j] = a_matrix[k * ndimen + j];
				a_matrix[k * ndimen + j] = tmp;
			}
		}
		if (j2 != k)
		{
			for (i = 0; i < ndimen; i++)
			{
				tmp = a_matrix[i * ndimen + j2];
				a_matrix[i * ndimen + j2] = a_matrix[i * ndimen + k];
				a_matrix[i * ndimen + k] = tmp;
			}
		}
		kme[k] = i2;
		kmf[k] = j2;
		for (j = 0; j < ndimen; j++)
		{
			if (j == k)
			{
				b_tmp[j] = 1.0 / tmp2;
				c_tmp[j] = 1.0;
			}
			else
			{
				b_tmp[j] = -a_matrix[k * ndimen + j] / tmp2;
				c_tmp[j] = a_matrix[j * ndimen + k];
			}
			a_matrix[k * ndimen + j] = 0.0;
			a_matrix[j * ndimen + k] = 0.0;
		}
		for (i = 0; i < ndimen; i++)
		{
			for (j = 0; j < ndimen; j++)
			{
				a_matrix[i * ndimen + j] = a_matrix[i * ndimen + j] + c_tmp[i] * b_tmp[j];
			}
		}
	}
	for (k3 = 0; k3 < ndimen; k3++)
	{
		k = ndimen - k3 - 1;
		k1 = kme[k];
		k2 = kmf[k];
		if (k1 != k)
		{
			for (i = 0; i < ndimen; i++)
			{
				tmp = a_matrix[i * ndimen + k1];
				a_matrix[i * ndimen + k1] = a_matrix[i * ndimen + k];
				a_matrix[i * ndimen + k] = tmp;
			}
		}
		if (k2 != k)
		{
			for (j = 0; j < ndimen; j++)
			{
				tmp = a_matrix[k2 * ndimen + j];
				a_matrix[k2 * ndimen + j] = a_matrix[k * ndimen + j];
				a_matrix[k * ndimen + j] = tmp;
			}
		}
	}
	return (0);
}








*/
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

//求矩阵的逆

int  matrix_inv(double* a_matrix, int ndimen)
////////////////////////////////////////////////////////////////////////////
//	a_matrix:矩阵
//	ndimen :维数
////////////////////////////////////////////////////////////////////////////
{
	double tmp, tmp2, b_tmp[20], c_tmp[20];
	int k, k1, k2, k3, j, i, j2, i2, kme[20], kmf[20];
	i2 = j2 = 0;

	for (k = 0; k < ndimen; k++)
	{
		tmp2 = 0.0;
		for (i = k; i < ndimen; i++)
		{
			for (j = k; j < ndimen; j++)
			{
				if (fabs(a_matrix[i * ndimen + j]) <= fabs(tmp2))
					continue;
				tmp2 = a_matrix[i * ndimen + j];
				i2 = i;
				j2 = j;
			}
		}
		if (i2 != k)
		{
			for (j = 0; j < ndimen; j++)
			{
				tmp = a_matrix[i2 * ndimen + j];
				a_matrix[i2 * ndimen + j] = a_matrix[k * ndimen + j];
				a_matrix[k * ndimen + j] = tmp;
			}
		}
		if (j2 != k)
		{
			for (i = 0; i < ndimen; i++)
			{
				tmp = a_matrix[i * ndimen + j2];
				a_matrix[i * ndimen + j2] = a_matrix[i * ndimen + k];
				a_matrix[i * ndimen + k] = tmp;
			}
		}
		kme[k] = i2;
		kmf[k] = j2;
		for (j = 0; j < ndimen; j++)
		{
			if (j == k)
			{	
				b_tmp[j] = 1.0 / tmp2;
				//printf("tmp2:%f", b_tmp[j]);
				c_tmp[j] = 1.0;
			}
			else
			{
				b_tmp[j] = -a_matrix[k * ndimen + j] / tmp2;
				c_tmp[j] = a_matrix[j * ndimen + k];
			}
			a_matrix[k * ndimen + j] = 0.0;
			a_matrix[j * ndimen + k] = 0.0;
		}
		for (i = 0; i < ndimen; i++)
		{
			for (j = 0; j < ndimen; j++)
			{
				a_matrix[i * ndimen + j] = a_matrix[i * ndimen + j] + c_tmp[i] * b_tmp[j];
			}
		}
	}
	for (k3 = 0; k3 < ndimen; k3++)
	{
		k = ndimen - k3 - 1;
		k1 = kme[k];
		k2 = kmf[k];
		if (k1 != k)
		{
			for (i = 0; i < ndimen; i++)
			{
				tmp = a_matrix[i * ndimen + k1];
				a_matrix[i * ndimen + k1] = a_matrix[i * ndimen + k];
				a_matrix[i * ndimen + k] = tmp;
			}
		}
		if (k2 != k)
		{
			for (j = 0; j < ndimen; j++)
			{
				tmp = a_matrix[k2 * ndimen + j];
				a_matrix[k2 * ndimen + j] = a_matrix[k * ndimen + j];
				a_matrix[k * ndimen + j] = tmp;
			}
		}
	}
	//for (int i = 0; i < 9; i++)
		//printf("ma%d,", a_matrix[i]);
	return (0);
}



