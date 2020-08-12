#include<stdio.h>
#include"DCT.h"
#include"deblock.h"
#include"mc.h"
#include"quant.h"
#include<stdlib.h>
#include"intra.h"
#include"inter.h"
#include"main.h"
#include<string.h>
#include"entroy.h"



void entroy_CAVLC_block4x4(int *dct, int *intraframe_model) {
	
	//做帧内模式预测，编码（真实模式-预测模式）写入头信息

	//对dct[16]进行CAVLC编码



	//将编码的码流输出到txt文档中（txt相当于buffer）


}