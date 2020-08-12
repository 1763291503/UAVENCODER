#include"DCT.h"
#include<stdio.h>
#include"deblock.h"
#include"mc.h"
#include"quant.h"
#include"main.h"
#include<stdlib.h>
#include<string.h>

int* DCT4x4(int residual2DCT[][4]) {
	int tmp[16];
	//int d[16];
	int* d = (int*)malloc(sizeof(int) * 16);

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			d[4 * i + j] = residual2DCT[i][j];
		}
	}
	//蝶形算法：横向4个像素
	for (int i = 0; i < 4; i++)
	{
		int s03 = d[i * 4 + 0] + d[i * 4 + 3];
		int s12 = d[i * 4 + 1] + d[i * 4 + 2];
		int d03 = d[i * 4 + 0] - d[i * 4 + 3];
		int d12 = d[i * 4 + 1] - d[i * 4 + 2];

		tmp[0 * 4 + i] = s03 + s12;
		tmp[1 * 4 + i] = 2 * d03 + d12;
		tmp[2 * 4 + i] = s03 - s12;
		tmp[3 * 4 + i] = d03 - 2 * d12;
	}
	//蝶形算法：纵向
	for (int i = 0; i < 4; i++)
	{
		int s03 = tmp[i * 4 + 0] + tmp[i * 4 + 3];
		int s12 = tmp[i * 4 + 1] + tmp[i * 4 + 2];
		int d03 = tmp[i * 4 + 0] - tmp[i * 4 + 3];
		int d12 = tmp[i * 4 + 1] - tmp[i * 4 + 2];

		d[i * 4 + 0] = s03 + s12;
		d[i * 4 + 1] = 2 * d03 + d12;
		d[i * 4 + 2] = s03 - s12;
		d[i * 4 + 3] = d03 - 2 * d12;
	}
	return d; //d[16] ,用完需要free()

}



//4x4DCT反变换（“add”代表叠加到已有的像素上）
void add_IDCT44(int* p_dst, int* dct) //p_dst 输出
{
	int d[16];
	int tmp[16];

	for (int i = 0; i < 4; i++)
	{
		int s02 = dct[0 * 4 + i] + dct[2 * 4 + i];
		int d02 = dct[0 * 4 + i] - dct[2 * 4 + i];
		int s13 = dct[1 * 4 + i] + (dct[3 * 4 + i] >> 1);
		int d13 = (dct[1 * 4 + i] >> 1) - dct[3 * 4 + i];

		tmp[i * 4 + 0] = s02 + s13;
		tmp[i * 4 + 1] = d02 + d13;
		tmp[i * 4 + 2] = d02 - d13;
		tmp[i * 4 + 3] = s02 - s13;
	}

	for (int i = 0; i < 4; i++)
	{
		int s02 = tmp[0 * 4 + i] + tmp[2 * 4 + i];
		int d02 = tmp[0 * 4 + i] - tmp[2 * 4 + i];
		int s13 = tmp[1 * 4 + i] + (tmp[3 * 4 + i] >> 1);
		int d13 = (tmp[1 * 4 + i] >> 1) - tmp[3 * 4 + i];

		d[0 * 4 + i] = (s02 + s13 + 32) >> 6;
		d[1 * 4 + i] = (d02 + d13 + 32) >> 6;
		d[2 * 4 + i] = (d02 - d13 + 32) >> 6;
		d[3 * 4 + i] = (s02 - s13 + 32) >> 6;
	}


	for (int i = 0; i < 16; i++) {
		p_dst[i] = p_dst[i] + d[i];
		if (p_dst[i] > 255)
			p_dst[i] = 255;
		else if(p_dst < 0)
			p_dst[i] = 0;
	}

	//for (int y = 0; y < 4; y++)
	//{
	//	for (int x = 0; x < 4; x++)
	//		p_dst[x] = x264_clip_pixel(p_dst[x] + d[y * 4 + x]);
	//	p_dst += FDEC_STRIDE;
	//}
}