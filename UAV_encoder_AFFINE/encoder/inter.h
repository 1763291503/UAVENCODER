

void inter_4x4_affine(int matrix_frame[][width], int reconstructed_frame[][width], int(*reconstructed_frame_tmp)[width], int dequant_mf[6][16], int* alpha_beta, int* BS, double a, double b, double c, double d, double e, double f, struct Queue* p);

//int MF[6][16] =
//{ { 13107,8066,13107,8066,8066,5243,8066,5243,13107,8066,13107,8066,8066,5243,8066,5243 },
//{ 11916,7490,11916,7490,7490,4660,7490,4660,11916,7490,11916,7490,7490,4660,7490,4660 },
//{ 10082,6554,10082,6554,6554,4194,6554,4194,10082,6554,10082,6554,6554,4194,6554,4194 },
//{ 9362,5825,9362,5825,5825,3647,5825,3647,9362,5825,9362,5825,5825,3647,5825,3647 },
//{ 8192,5243,8192,5243,5243,3355,5243,3355,8192,5243,8192,5243,5243,3355,5243,3355 },
//{ 7282,4559,7282,4559,4559,2893,4559,2893,7282,4559,7282,4559,4559,2893,4559,2893 } };