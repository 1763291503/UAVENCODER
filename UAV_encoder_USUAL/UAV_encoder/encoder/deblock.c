#include"deblock.h"
#include<stdio.h>
#include"DCT.h"
#include"mc.h"
#include"quant.h"
#include<stdlib.h>
#include"intra.h"
#include"main.h"

int clip(int value, int min, int max) {
    
    if (value < min) {
    
        return min;
    }
    else if (value > max) {
    
        return max;
    }
    else {
        return value;
    }
}



void deblock_44_intra(int (*reconstructed_frame)[width], int *alpha_beta) {

	//int loca_i, local_j;
	for (int i = 0; 4 * i < height; i++) {
		for (int j = 0; 4 * j < width; j++) {
			
			if (i == 0 && j != 0) {
				//顶部，只进行左边界滤波
                deblock_luma_intra_h(reconstructed_frame, 4*i, 4*j, alpha_beta[2*QP+0], alpha_beta[2*QP+1]);
			}
			else if (i != 0 && j == 0) {
				//左边，只进行上边界滤波
                deblock_luma_intra_v(reconstructed_frame, 4 * i, 4 * j, alpha_beta[2 * QP + 0], alpha_beta[2 * QP + 1]);
			}
			else if(i != 0 && j != 0){
				//中间
                deblock_luma_intra_h(reconstructed_frame, 4 * i, 4 * j, alpha_beta[2 * QP + 0], alpha_beta[2 * QP + 1]);
                deblock_luma_intra_v(reconstructed_frame, 4 * i, 4 * j, alpha_beta[2 * QP + 0], alpha_beta[2 * QP + 1]);
			}
		
		}
	}



}



//BS = 4 水平强滤波
void deblock_luma_intra_h(int(*reconstructed_frame)[width], int x, int y, int alpha, int beta) //!< 亮度分量
{
    for (int d = 0; d < 4; d++)
    {   
        int p2 = reconstructed_frame[x + d][y - 3];
        int p1 = reconstructed_frame[x + d][y - 2];
        int p0 = reconstructed_frame[x + d][y - 1];
        int q0 = reconstructed_frame[x + d][y];
        int q1 = reconstructed_frame[x + d][y + 1];
        int q2 = reconstructed_frame[x + d][y + 2];

        if (abs(p0 - q0) < alpha && abs(p1 - p0) < beta && abs(q1 - q0) < beta)
        {
            if (abs(p0 - q0) < ((alpha >> 2) + 2)) 
            {
                if (abs(p2 - p0) < beta) /* p0', p1', p2' */
                {
                    int p3 = reconstructed_frame[x + d][y - 4];
                    reconstructed_frame[x + d][y - 1] = (p2 + 2 * p1 + 2 * p0 + 2 * q0 + q1 + 4) >> 3; 
                    reconstructed_frame[x + d][y - 2] = (p2 + p1 + p0 + q0 + 2) >> 2; 
                    reconstructed_frame[x + d][y - 3] = (2 * p3 + 3 * p2 + p1 + p0 + q0 + 4) >> 3; 
                }
                else /* p0' */
                    reconstructed_frame[x + d][y - 1] = (2 * p1 + p0 + q1 + 2) >> 2; 
                if (abs(q2 - q0) < beta) /* q0', q1', q2' */
                {
                    int q3 = reconstructed_frame[x + d][y + 3];
                    reconstructed_frame[x + d][y] = (p1 + 2 * p0 + 2 * q0 + 2 * q1 + q2 + 4) >> 3; 
                    reconstructed_frame[x + d][y + 1] = (p0 + q0 + q1 + q2 + 2) >> 2; 
                    reconstructed_frame[x + d][y + 2] = (2 * q3 + 3 * q2 + q1 + q0 + p0 + 4) >> 3; 
                }
                else /* q0' */
                    reconstructed_frame[x + d][y] = (2 * q1 + q0 + p1 + 2) >> 2; 
            }
            else /* p0', q0' */
            {
                reconstructed_frame[x + d][y - 1] = (2 * p1 + p0 + q1 + 2) >> 2; //!< (8-488)
                reconstructed_frame[x + d][y] = (2 * q1 + q0 + p1 + 2) >> 2; //!< (8-495)
            }
        }       
    } 
}

//BS = 4 垂直强滤波
void deblock_luma_intra_v(int(*reconstructed_frame)[width], int x, int y, int alpha, int beta) //!< 亮度分量
{
    for (int d = 0; d < 4; d++)
    {
        int p2 = reconstructed_frame[x - 3][y + d];
        int p1 = reconstructed_frame[x - 2][y + d];
        int p0 = reconstructed_frame[x - 1][y + d];
        int q0 = reconstructed_frame[x][y + d];
        int q1 = reconstructed_frame[x + 1][y + d];
        int q2 = reconstructed_frame[x + 2][y + d];

        if (abs(p0 - q0) < alpha && abs(p1 - p0) < beta && abs(q1 - q0) < beta)
        {
            if (abs(p0 - q0) < ((alpha >> 2) + 2)) //!< (8-484)
            {
                if (abs(p2 - p0) < beta) /* p0', p1', p2' */
                {
                    int p3 = reconstructed_frame[x - 4][y + d];
                    reconstructed_frame[x - 1][y + d] = (p2 + 2 * p1 + 2 * p0 + 2 * q0 + q1 + 4) >> 3; 
                    reconstructed_frame[x - 2][y + d] = (p2 + p1 + p0 + q0 + 2) >> 2; 
                    reconstructed_frame[x - 3][y + d] = (2 * p3 + 3 * p2 + p1 + p0 + q0 + 4) >> 3; 
                }
                else /* p0' */
                    reconstructed_frame[x - 1][y + d] = (2 * p1 + p0 + q1 + 2) >> 2; 
                if (abs(q2 - q0) < beta) /* q0', q1', q2' */
                {
                    int q3 = reconstructed_frame[x + 3][y + d];
                    reconstructed_frame[x][y + d] = (p1 + 2 * p0 + 2 * q0 + 2 * q1 + q2 + 4) >> 3; 
                    reconstructed_frame[x + 1][y + d] = (p0 + q0 + q1 + q2 + 2) >> 2; 
                    reconstructed_frame[x + 2][y + d] = (2 * q3 + 3 * q2 + q1 + q0 + p0 + 4) >> 3; 
                }
                else /* q0' */
                    reconstructed_frame[x][y + d] = (2 * q1 + q0 + p1 + 2) >> 2; 
            }
            else /* p0', q0' */
            {
                reconstructed_frame[x - 1][y + d] = (2 * p1 + p0 + q1 + 2) >> 2; 
                reconstructed_frame[x][y + d] = (2 * q1 + q0 + p1 + 2) >> 2; 
            }
        }
    }
}


//BS = 1,2,3
void deblock_44_inter(int(*reconstructed_frame_tmp)[width], int* alpha_beta, int* BS) {



    //int loca_i, local_j;
    for (int i = 0; 4 * i < height; i++) {
        for (int j = 0; 4 * j < width; j++) {

            if (i == 0 && j != 0) {
                //顶部，只进行左边界滤波
                deblock_luma_inter_h(reconstructed_frame_tmp, 4 * i, 4 * j, alpha_beta[2 * QP + 0], alpha_beta[2 * QP + 1], BS[QP]);
            }
            else if (i != 0 && j == 0) {
                //左边，只进行上边界滤波
                deblock_luma_inter_v(reconstructed_frame_tmp, 4 * i, 4 * j, alpha_beta[2 * QP + 0], alpha_beta[2 * QP + 1], BS[QP]);
            }
            else if (i != 0 && j != 0) {
                //中间
                deblock_luma_inter_h(reconstructed_frame_tmp, 4 * i, 4 * j, alpha_beta[2 * QP + 0], alpha_beta[2 * QP + 1], BS[QP]);
                deblock_luma_inter_v(reconstructed_frame_tmp, 4 * i, 4 * j, alpha_beta[2 * QP + 0], alpha_beta[2 * QP + 1], BS[QP]);
            }

        }
    }



}


//水平滤波
void deblock_luma_inter_h(int(*reconstructed_frame_tmp)[width], int x, int y, int alpha, int beta, int tc0)
{
    for (int d = 0; d < 4; d++)
    {
        int p2 = reconstructed_frame_tmp[x + d][y - 3];
        int p1 = reconstructed_frame_tmp[x + d][y - 2];
        int p0 = reconstructed_frame_tmp[x + d][y - 1];
        int q0 = reconstructed_frame_tmp[x + d][y];
        int q1 = reconstructed_frame_tmp[x + d][y + 1];
        int q2 = reconstructed_frame_tmp[x + d][y + 2];


        if (abs(p0 - q0) < alpha && abs(p1 - p0) < beta && abs(q1 - q0) < beta)
        {
            int tc = tc0;
            int delta;
            //上面2个点（p0，p2）满足条件的时候，滤波p1
            //int x264_clip3( int v, int i_min, int i_max )用于限幅
            if (abs(p2 - p0) < beta)
            {
                if (tc0) 
                    reconstructed_frame_tmp[x + d][y - 2] = p1 + clip(((p2 + ((p0 + q0 + 1) >> 1)) >> 1) - p1, -tc0, tc0);
                tc++;
            }
            //下面2个点（q0，q2）满足条件的时候，滤波q1
            if (abs(q2 - q0) < beta)
            {
                if (tc0)
                    reconstructed_frame_tmp[x + d][y + 1] = q1 + clip(((q2 + ((p0 + q0 + 1) >> 1)) >> 1) - q1, -tc0, tc0);
                tc++;
            }

            delta = clip((((q0 - p0) << 2) + (p1 - q1) + 4) >> 3, -tc, tc);
            //p0
            reconstructed_frame_tmp[x + d][y - 1] = clip(p0 + delta, 0, 255);    /* p0' */
            //q0
            reconstructed_frame_tmp[x + d][y] = clip(q0 - delta, 0, 255);    /* q0' */
        }

    }

}



//垂直滤波
void deblock_luma_inter_v(int(*reconstructed_frame_tmp)[width], int x, int y, int alpha, int beta, int tc0)
{
    for (int d = 0; d < 4; d++)
    {
        int p2 = reconstructed_frame_tmp[x - 3][y + d];
        int p1 = reconstructed_frame_tmp[x - 2][y + d];
        int p0 = reconstructed_frame_tmp[x - 1][y + d];
        int q0 = reconstructed_frame_tmp[x][y + d];
        int q1 = reconstructed_frame_tmp[x + 1][y + d];
        int q2 = reconstructed_frame_tmp[x + 2][y + d];


        if (abs(p0 - q0) < alpha && abs(p1 - p0) < beta && abs(q1 - q0) < beta)
        {
            int tc = tc0;
            int delta;
            //上面2个点（p0，p2）满足条件的时候，滤波p1
            //int x264_clip3( int v, int i_min, int i_max )用于限幅
            if (abs(p2 - p0) < beta)
            {
                if (tc0)
                    reconstructed_frame_tmp[x - 2][y + d] = p1 + clip(((p2 + ((p0 + q0 + 1) >> 1)) >> 1) - p1, -tc0, tc0);
                tc++;
            }
            //下面2个点（q0，q2）满足条件的时候，滤波q1
            if (abs(q2 - q0) < beta)
            {
                if (tc0)
                    reconstructed_frame_tmp[x + 1][y + d] = q1 + clip(((q2 + ((p0 + q0 + 1) >> 1)) >> 1) - q1, -tc0, tc0);
                tc++;
            }

            delta = clip((((q0 - p0) << 2) + (p1 - q1) + 4) >> 3, -tc, tc);
            //p0
            reconstructed_frame_tmp[x - 1][y + d] = clip(p0 + delta, 0, 255);    /* p0' */
            //q0
            reconstructed_frame_tmp[x][y + d] = clip(q0 - delta, 0, 255);    /* q0' */
        }

    }

}




