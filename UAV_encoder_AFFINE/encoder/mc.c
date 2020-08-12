#include"mc.h"

#include <stdlib.h>
# include <string.h>

int* MCsearch(int* r_frame, int* c_frame, int w, int h, int window_size)
{
    //for (int i = 0; i < 921600; i++)
    //    if (c_frame[i] > 255)
    //        printf("errorpixl%d\n", c_frame[i]);
    int* MVP;
    int MVP_num = 0;
    //判断长宽能否满足搜索窗口大小

    if (window_size > w || window_size > h || window_size % 2 == 0)
        return NULL;

    MVP = (int*)malloc(w * h / 16 * 2 * sizeof(int));
    memset(MVP, 0, w * h / 16 * 2 * sizeof(int));

    for (int j = 0; j < h; j += 4)
        for (int i = 0; i < w; i += 4, MVP_num++)
        {
            //i,j为搜索窗口中点坐标
            int last = 0;
            int x = i, y = j;//存放每一轮最小SAD的坐标
            int temp = window_size / 4;

            int sum = 9999999999;
            while (last != 1)
            {
                int SAD[8][3] = { 0 };//x,y,SAD
                //初始化8个坐标
                SAD[0][0] = x - temp;
                SAD[0][1] = y - temp;
                SAD[1][0] = x;
                SAD[1][1] = y - temp;
                SAD[2][0] = x + temp;
                SAD[2][1] = y - temp;

                SAD[3][0] = x - temp;
                SAD[3][1] = y;
                SAD[4][0] = x + temp;
                SAD[4][1] = y;

                SAD[5][0] = x - temp;
                SAD[5][1] = y + temp;
                SAD[6][0] = x;
                SAD[6][1] = y + temp;
                SAD[7][0] = x + temp;
                SAD[7][1] = y + temp;

                if (x - temp < 0) {//左边空间不够
                    SAD[0][0] = -1;
                    SAD[0][1] = -1;
                    SAD[3][0] = -1;
                    SAD[3][1] = -1;
                    SAD[5][0] = -1;
                    SAD[5][1] = -1;
                }
                if (y - temp < 0) {//上空间不够
                    SAD[0][0] = -1;
                    SAD[0][1] = -1;
                    SAD[1][0] = -1;
                    SAD[1][1] = -1;
                    SAD[2][0] = -1;
                    SAD[2][1] = -1;
                }
                if (y + temp + 4 > h - 1) {//下空间不够
                    SAD[5][0] = -1;
                    SAD[5][1] = -1;
                    SAD[6][0] = -1;
                    SAD[7][1] = -1;
                    SAD[7][0] = -1;
                    SAD[7][1] = -1;
                }
                if (x + temp + 4 > w - 1) {//右空间不够
                    SAD[2][0] = -1;
                    SAD[2][1] = -1;
                    SAD[4][0] = -1;
                    SAD[4][1] = -1;
                    SAD[7][0] = -1;
                    SAD[7][1] = -1;
                }
                //算8个SAD
                for (int k = 0; k < 8; k++)
                {
                    if (SAD[k][0] != -1)
                        for (int m = 0; m < 4; m++)
                            for (int n = 0; n < 4; n++) {
                                SAD[k][2] += abs(c_frame[(SAD[k][1] + m) * w + SAD[k][0] + n] - r_frame[(SAD[k][1] + m) * w + SAD[k][0] + n]);
                                //if(SAD[k][2] > 3000)
                                //    printf("SAD:%d\n", SAD[k][2]);
                            
                            }
                }

                //找出8个SAD的最小值
                for (int m = 0; m < 8; m++) {
                    if (SAD[m][0] != -1 && SAD[m][2] < sum) {
                        sum = SAD[m][2];
                        x = SAD[m][0];
                        y = SAD[m][1];
                    }
                }
                if (temp == 1)
                    last = 1;
                temp /= 2;
            }
            //存坐标
            MVP[MVP_num * 2] = x;
            MVP[MVP_num * 2 + 1] = y;
        }

    //printf("%d,%d\n", MVP[0], MVP[1]);
    //printf("MVP:%d", MVP_num);
    return MVP;//需要free(MVP)
}
