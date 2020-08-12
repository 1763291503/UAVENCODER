#include"quant.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"main.h"


//量化
#define QUANT_ONE( coef, mf, f , qbits) \
{ \
    if( (coef) > 0 ) \
        (coef) = (f + (coef) * (mf)) >> qbits; \
    else \
        (coef) = - ((f - (coef) * (mf)) >> qbits); \
    nz |= (coef); \
}


int quant4x4(int *dct, int mf[6][16], int bias, int qbits)
{   
    int k = QP % 6;
    int nz = 0;
    for (int i = 0; i < 16; i++) {

        QUANT_ONE(dct[i], mf[k][i], bias, qbits);
        //printf("DCTDCTDCT%d\n", dct[i]);
    }
    return !!nz;
}


//反量化
#define DEQUANT_SHL( x ) \
    dct[x] = ( dct[x] * dequant_mf[i_mf][x] ) << i_qbits

#define DEQUANT_SHR( x ) \
    dct[x] = ( dct[x] * dequant_mf[i_mf][x] + f ) >> (-i_qbits)


void dequant_4x4(int* dct, int dequant_mf[6][16], int i_qp)
{
    int i_mf = i_qp % 6;
    int i_qbits = i_qp / 6;
    //int i_qbits = i_qp / 6 -4;

    if (i_qbits >= 0)
    {
        for (int i = 0; i < 16; i++)
            DEQUANT_SHL(i);
            //dct[i] = (dct[i] * dequant_mf[i_mf][i]) << i_qbits;
    }
    else
    {
        int f = 1 << (-i_qbits - 1);
        for (int i = 0; i < 16; i++)
            DEQUANT_SHR(i);
            //dct[i] = (dct[i] * dequant_mf[i_mf][i] + f) >> (-i_qbits);
    }
}


















