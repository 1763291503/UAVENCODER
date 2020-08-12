#include"main.h"

void deblock_44_intra(int(*reconstructed_frame)[width], int* alpha_beta);

void deblock_luma_intra_h(int(*reconstructed_frame)[width], int x, int y, int alpha, int beta);

void deblock_luma_intra_v(int(*reconstructed_frame)[width], int x, int y, int alpha, int beta);

int clip(int value, int min, int max);

void deblock_44_inter(int(*reconstructed_frame_tmp)[width], int* alpha_beta, int* BS);

void deblock_luma_inter_h(int(*reconstructed_frame_tmp)[width], int x, int y, int alpha, int beta, int tc0);

void deblock_luma_inter_v(int(*reconstructed_frame_tmp)[width], int x, int y, int alpha, int beta, int tc0);