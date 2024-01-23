/*
author: Zam（全都是cv上一届师兄的，只是加了点注释）
date: 2024/1/23
@brief：手动设置阈值进行二值化
time：720us（没什么用，不是我测的）
@param image0：原图像的起始地址
@param threshold：阈值
@return uint8*：返回二值化得到的图像的地址
*/
#include "camera.h"

uint8 *Simple_Binaryzation(uint8 *image0, uint8 threshold)
{
    uint16 temp = 0;
    static uint8 B_Picture[CAMERA_H][CAMERA_W];
    for(int i=0;i<CAMERA_H;i++)
    {
        for(int j=0;j<CAMERA_W;j++)
        {
            temp = *(image0 + i * CAMERA_W + j);//赋值
            if(temp > threshold)
            {
                B_Picture[i][j] = 0;
            }
            else
            {
                B_Picture[i][j] = 255;
            }
        }
    }
    return *Simple_Binaryzation;//返回二值化后的图像
}

/**
 * @brief 均值二值化
 * 用时：1100us
 * @param image1
 * @return uint8*
 */
uint8 *Mean_Binaryzation(uint8 *image1)
{
    static uint8 mean_bina[CAMERA_H][CAMERA_W]; // 均值二值化图像
    uint32 gray_sum = 0;                        // 灰度值总和
    uint16 temp1, temp2, meannum = 0;

    for (int i = 0; i < CAMERA_H; i++)
    {
        for (int j = 0; j < CAMERA_W; j++)
        {
            temp1 = *(image1 + i * CAMERA_W + j);
            gray_sum += temp1;
        }
    }
    meannum = gray_sum / (CAMERA_H * CAMERA_W); // 平均阈值
    for (int i = 0; i < CAMERA_H; i++)
    {
        for (int j = 0; j < CAMERA_W; j++)
        {
            temp2 = *(image1 + i * CAMERA_W + j);
            if (temp2 < meannum)
                mean_bina[i][j] = 0;
            else
                mean_bina[i][j] = 255;
        }
    }
    return *mean_bina;
}

/**
 * @brief 高斯模糊
 * 用时：5600us（数组索引表示）/4325us（指针表示）
 * @param image2
 * @return uint8*
 */
uint8 *Gaussain_Blur(uint8 *image2)
{
    static uint8 gauss_blur[CAMERA_H][CAMERA_W]; // 高斯模糊图像
    // 高斯模糊算子
    float Gaussain_Blur[9] = {0.0947416, 0.118318, 0.0947416, 0.118318, 0.147761, 0.118318, 0.0947416, 0.118318, 0.0947416};
    float fix_sum = 0; // 加权均值
    uint8 temp_fix[9]; // 3*3卷积核（感受野）

    for (int j = 0; j < CAMERA_W; ++j) // 第一行不做处理
    {
        gauss_blur[0][j] = *(image2);
        image2++;
    }
    for (int i = 1; i < CAMERA_H - 1; ++i)
    {
        gauss_blur[i][0] = *(image2); // 第一列不做处理
        image2++;

        for (int j = 1; j < CAMERA_W - 1; ++j)
        {
            temp_fix[0] = *(image2 - CAMERA_W - 1);
            temp_fix[1] = *(image2 - CAMERA_W);
            temp_fix[2] = *(image2 - CAMERA_W + 1);
            temp_fix[3] = *(image2 - 1);
            temp_fix[4] = *(image2);
            temp_fix[5] = *(image2 + 1);
            temp_fix[6] = *(image2 + CAMERA_W - 1);
            temp_fix[7] = *(image2 + CAMERA_W);
            temp_fix[8] = *(image2 + CAMERA_W + 1);
            for (int k = 0; k < 9; ++k)
            {
                fix_sum += (float)temp_fix[k] * Gaussain_Blur[k];
            }
            gauss_blur[i][j] = (int)fix_sum; //直接灰度值赋值，如果用指针传是不是快一点？？
            fix_sum = 0;
            image2++;
        }

        gauss_blur[i][CAMERA_W - 1] = *(image2); // 最后一列不做处理
        image2++;
    }

    for (int j = 0; j < CAMERA_W; ++j) // 最后一行不做处理
    {
        gauss_blur[CAMERA_H - 1][j] = *(image2);
        image2++;
    }
    return *gauss_blur;
}

/**
 * @brief Sobel算子边缘检测
 * 用时：4350us(InvSqrt,很稳定)/4200us(sqrt,波动大，最高4380，最低3890)
 * @param image4
 * @return uint8*
 */
uint8 *Sobel_Edge(uint8 *image4)
{
    static uint8 sobel_image[CAMERA_H][CAMERA_W]; // sobel边缘检测图像
    int Gx, Gy, G = 0;
    uint8 sobel_fix[9]; // 3*3卷积核（感受野）

    for (int j = 0; j < CAMERA_W; j++) //第一行不处理
    {
        sobel_image[0][j] = *(image4);
        image4++;
    }
    for (int i = 1; i < CAMERA_H - 1; i++)
    {
        sobel_image[i][0] = *(image4); //第一列不处理
        image4++;

        for (int j = 1; j < CAMERA_W - 1; j++)
        {
            sobel_fix[0] = *(image4 - CAMERA_W - 1);
            sobel_fix[1] = *(image4 - CAMERA_W);
            sobel_fix[2] = *(image4 - CAMERA_W + 1);
            sobel_fix[3] = *(image4 - 1);
            sobel_fix[4] = *(image4);
            sobel_fix[5] = *(image4 + 1);
            sobel_fix[6] = *(image4 + CAMERA_W - 1);
            sobel_fix[7] = *(image4 + CAMERA_W);
            sobel_fix[8] = *(image4 + CAMERA_W + 1);
            Gx = sobel_fix[2] - sobel_fix[0] + 2 * sobel_fix[5] - 2 * sobel_fix[3] + sobel_fix[8] - sobel_fix[6];//计算x方向上的梯度值
            Gy = sobel_fix[0] - sobel_fix[6] + 2 * sobel_fix[1] - 2 * sobel_fix[7] + sobel_fix[2] - sobel_fix[8];//计算y方向上的梯度值
            G = (int)sqrt(Gx * Gx + Gy * Gy); // InvSqrt or sqrt
            G = G > 255 ? 255 : G;            // 限幅
            sobel_image[i][j] = G;
            image4++;
        }

        sobel_image[i][CAMERA_W - 1] = *(image4); //最后一列不处理
        image4++;
    }

    for (int j = 0; j < CAMERA_W; j++) //最后一行不处理
    {
        sobel_image[CAMERA_H - 1][j] = *(image4);
        image4++;
    }

    return *sobel_image;
}

/**
 * @brief Scharr算子（Sobel算子改进版，边缘更明显）
 * 用时：3760us(InvSqrt,很稳定)/3400us(sqrt,波动大，最高3680，最低3300)
 * @param image5
 * @return uint8*
 */
uint8 *Scharr_Edge(uint8 *image5)
{
    static uint8 scharr_image[CAMERA_H][CAMERA_W]; // scharr边缘检测图像
    int Gx, Gy, G = 0;
    uint8 scharr_fix[9]; // 3*3卷积核（感受野）

    for (int j = 0; j < CAMERA_W; j++) //第一行不处理
    {
        scharr_image[0][j] = *(image5);
        image5++;
    }
    for (int i = 1; i < CAMERA_H - 1; i++)
    {
        scharr_image[i][0] = *(image5); //第一列不处理
        image5++;
        for (int j = 1; j < CAMERA_W - 1; j++)
        {
            scharr_fix[0] = *(image5 - CAMERA_W - 1);
            scharr_fix[1] = *(image5 - CAMERA_W);
            scharr_fix[2] = *(image5 - CAMERA_W + 1);
            scharr_fix[3] = *(image5 - 1);
            scharr_fix[4] = *(image5);
            scharr_fix[5] = *(image5 + 1);
            scharr_fix[6] = *(image5 + CAMERA_W - 1);
            scharr_fix[7] = *(image5 + CAMERA_W);
            scharr_fix[8] = *(image5 + CAMERA_W + 1);
            Gx = 3 * scharr_fix[2] - 3 * scharr_fix[0] + 10 * scharr_fix[5] - 10 * scharr_fix[3] + 3 * scharr_fix[8] - 3 * scharr_fix[6];
            Gy = 3 * scharr_fix[0] - 3 * scharr_fix[6] + 10 * scharr_fix[1] - 10 * scharr_fix[7] + 3 * scharr_fix[2] - 3 * scharr_fix[8];
            G = (int)sqrt(Gx * Gx + Gy * Gy); // InvSqrt or sqrt
            G = G > 255 ? 255 : G;
            scharr_image[i][j] = G;
            // dir[i][j] = Atan2(Gy, Gx); //用到了Atan2算法，dir表示像素梯度的方向，会在NMS()函数中用到
            image5++;
        }
        scharr_image[i][CAMERA_W - 1] = *(image5); //最后一列不处理
        image5++;
    }
    for (int j = 0; j < CAMERA_W; j++) //最后一行不处理
    {
        scharr_image[CAMERA_H - 1][j] = *(image5);
        image5++;
    }

    return *scharr_image;
}

/**
 * @brief 非极大值抑制
 * 用时：8100us(包括Sobel)/8200us(包括Scharr)
 * @param image6
 * @param choose_edge,0为Sobel,1为Scharr
 * @return uint8*
 */
uint8 *NMS(uint8 *image6, uint8 choose_edge)
{
    static uint8 NMS_image[CAMERA_H][CAMERA_W]; // 非极大值抑制图像
    uint8 edge_image[CAMERA_H][CAMERA_W];       // 边缘处理图像
    uint8 edge_fix[9];                          // 3*3卷积核（感受野）
    int Gx, Gy, G = 0;                          // 梯度
    uint8 dir[CAMERA_H][CAMERA_W] = {0};        // 各像素点方向梯度
    const uint8 NMS_LOSS = 2;                   // 非极大值抑制补偿，根据实际效果来确定，初始值为0；

    // 边缘算法
    for (int j = 0; j < CAMERA_W; j++) //第一行不处理
    {
        edge_image[0][j] = *(image6);
        image6++;
    }
    for (int i = 1; i < CAMERA_H - 1; i++)
    {
        edge_image[i][0] = *(image6); //第一列不处理
        image6++;
        for (int j = 1; j < CAMERA_W - 1; j++)
        {
            edge_fix[0] = *(image6 - CAMERA_W - 1);
            edge_fix[1] = *(image6 - CAMERA_W);
            edge_fix[2] = *(image6 - CAMERA_W + 1);
            edge_fix[3] = *(image6 - 1);
            edge_fix[4] = *(image6);
            edge_fix[5] = *(image6 + 1);
            edge_fix[6] = *(image6 + CAMERA_W - 1);
            edge_fix[7] = *(image6 + CAMERA_W);
            edge_fix[8] = *(image6 + CAMERA_W + 1);

            if (choose_edge == 0) // Sobel
            {
                Gx = edge_fix[2] - edge_fix[0] + 2 * edge_fix[5] - 2 * edge_fix[3] + edge_fix[8] - edge_fix[6];
                Gy = edge_fix[0] - edge_fix[6] + 2 * edge_fix[1] - 2 * edge_fix[7] + edge_fix[2] - edge_fix[8];
            }
            else if (choose_edge == 1) // Scharr
            {
                Gx = 3 * edge_fix[2] - 3 * edge_fix[0] + 10 * edge_fix[5] - 10 * edge_fix[3] + 3 * edge_fix[8] - 3 * edge_fix[6];
                Gy = 3 * edge_fix[0] - 3 * edge_fix[6] + 10 * edge_fix[1] - 10 * edge_fix[7] + 3 * edge_fix[2] - 3 * edge_fix[8];
            }

            G = (int)sqrt(Gx * Gx + Gy * Gy); //用到卡马尔开方
            G = G > 255 ? 255 : G;
            edge_image[i][j] = G;
            dir[i][j] = Atan2(Gy, Gx); //用到了Atan2算法，dir表示像素梯度的方向，会在NMS()函数中用到
            image6++;
        }
        edge_image[i][CAMERA_W - 1] = *(image6); //最后一列不处理
        image6++;
    }
    for (int j = 0; j < CAMERA_W; j++) //最后一行不处理
    {
        edge_image[CAMERA_H - 1][j] = *(image6);
        image6++;
    }

    // 非极大值抑制
    for (int j = 0; j < CAMERA_W; j++) // 第一行和最后一行不处理
    {
        NMS_image[0][j] = edge_image[0][j];
        NMS_image[CAMERA_H - 1][j] = edge_image[CAMERA_H - 1][j];
    }
    for (int i = 1; i < CAMERA_H - 1; i++) // 第一列和最后一列不处理
    {
        NMS_image[i][0] = edge_image[i][0];
        NMS_image[i][CAMERA_W - 1] = edge_image[i][CAMERA_W - 1];
    }

    for (int i = 1; i < CAMERA_H - 1; i++)
    {
        for (int j = 1; j < CAMERA_W - 1; j++)
        {
            switch (dir[i][j])
            {
            case 0:
            {
                if ((edge_image[i][j] > edge_image[i][j + 1] - NMS_LOSS) && (edge_image[i][j] > edge_image[i][j - 1] - NMS_LOSS))
                    NMS_image[i][j] = edge_image[i][j];
                else
                    NMS_image[i][j] = 0;
                break;
            }
            case 1:
            {
                if ((edge_image[i][j] > edge_image[i + 1][j - 1] - NMS_LOSS) && (edge_image[i][j] > edge_image[i - 1][j + 1] - NMS_LOSS))
                    NMS_image[i][j] = edge_image[i][j];
                else
                    NMS_image[i][j] = 0;
                break;
            }
            case 2:
            {
                if ((edge_image[i][j] > edge_image[i - 1][j - 1] - NMS_LOSS) && (edge_image[i][j] > edge_image[i + 1][j + 1] - NMS_LOSS))
                    NMS_image[i][j] = edge_image[i][j];
                else
                    NMS_image[i][j] = 0;
                break;
            }
            case 3:
            {
                if ((edge_image[i][j] > edge_image[i + 1][j] - NMS_LOSS) && (edge_image[i][j] > edge_image[i - 1][j] - NMS_LOSS))
                    NMS_image[i][j] = edge_image[i][j];
                else
                    NMS_image[i][j] = 0;
                break;
            }
            default:
                break;
            }
        }
    }

    return *NMS_image;
}

/**
 * @brief 双边缘阈值链接
 * 用时：1000us
 * image_debug = TwoThreshold(NMS(*mt9v03x_image, 1));9100us
 * @param image7
 * @return uint8*
 */
uint8 *TwoThreshold(uint8 *image7)
{
    static uint8 Link_image[CAMERA_H][CAMERA_W] = {0}; // 双边缘链接图像
    uint8 Link_fix[8];                                 // 只读取八邻域的值
    uint8 lowThr, highThr;                             // 双边缘阈值

    // highThr = Camera_GetOSTU(image7); // 该大津法阈值不适用
    highThr = 230;
    lowThr = highThr / 2;

    image7 = image7 + CAMERA_W; // 从第二行首列开始

    for (int i = 1; i < CAMERA_H - 1; i++)
    {
        image7++; // 跳过第一列
        for (int j = 1; j < CAMERA_W - 1; j++)
        {
            if (*image7 < lowThr)
                Link_image[i][j] = 0;
            else if (*image7 > highThr)
                Link_image[i][j] = 255;
            else
            {
                Link_fix[0] = *(image7 - CAMERA_W - 1);
                Link_fix[1] = *(image7 - CAMERA_W);
                Link_fix[2] = *(image7 - CAMERA_W + 1);
                Link_fix[3] = *(image7 - 1);
                Link_fix[4] = *(image7 + 1);
                Link_fix[5] = *(image7 + CAMERA_W - 1);
                Link_fix[6] = *(image7 + CAMERA_W);
                Link_fix[7] = *(image7 + CAMERA_W + 1);

                quick_sort(Link_fix, 0, 8); //快速排序算法1，对八领域的灰度值进行排序;7最大，0最小
                // qsort(Link_fix, 8, sizeof(uint8), compare); //快速排序算法2，对八领域的灰度值进行排序;7最大，0最小
                if (Link_fix[4] > highThr)
                    Link_image[i][j] = 255; //满足条件，链接
                else
                    Link_image[i][j] = 0; //不满足条件，灰度值置为0
            }
            image7++;
        }
        image7++; // 跳过最后一列
    }

    return *Link_image;
}

/**
 * @brief OSTU大津法求最佳阈值
 * 用时：2000us(不稳定)
 * @param tmImage
 * @return uint8
 */
uint8 Camera_GetOSTU(uint8 *tmImage)
{
    signed short i, j;
    unsigned long Amount = 0;
    unsigned long PixelBack = 0;
    unsigned long PixelIntegralBack = 0;
    unsigned long PixelIntegral = 0;
    signed long PixelIntegralFore = 0;
    signed long PixelFore = 0;
    double OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // 类间方差;
    signed short MinValue, MaxValue;
    uint8 Threshold = 0;
    unsigned short HistoGram[256]; //

    for (j = 0; j < 256; j++)
        HistoGram[j] = 0; //初始化灰度直方图

    for (j = 0; j < CAMERA_H; j++)
    {
        for (i = 0; i < CAMERA_W; i++)
        {
            HistoGram[*(tmImage + j * CAMERA_H + i)]++; //统计灰度级中每个像素在整幅图像中的个数
        }
    }

    for (MinValue = 0; MinValue < 256 && HistoGram[MinValue] == 0; MinValue++)
        ; //获取最小灰度的值
    for (MaxValue = 255; MaxValue > MinValue && HistoGram[MaxValue] == 0; MaxValue--)
        ; //获取最大灰度的值

    if (MaxValue == MinValue)
        return MaxValue; // 图像中只有一个颜色
    if (MinValue + 1 == MaxValue)
        return MinValue; // 图像中只有二个颜色

    for (j = MinValue; j <= MaxValue; j++)
        Amount += HistoGram[j]; //  像素总数

    PixelIntegral = 0;
    for (j = MinValue; j <= MaxValue; j++)
    {
        PixelIntegral += HistoGram[j] * j; //灰度值总数
    }
    SigmaB = -1;
    for (j = MinValue; j < MaxValue; j++)
    {
        PixelBack = PixelBack + HistoGram[j];                                              //前景像素点数
        PixelFore = Amount - PixelBack;                                                    //背景像素点数
        OmegaBack = (double)PixelBack / Amount;                                            //前景像素百分比
        OmegaFore = (double)PixelFore / Amount;                                            //背景像素百分比
        PixelIntegralBack += HistoGram[j] * j;                                             //前景灰度值
        PixelIntegralFore = PixelIntegral - PixelIntegralBack;                             //背景灰度值
        MicroBack = (double)PixelIntegralBack / PixelBack;                                 //前景灰度百分比
        MicroFore = (double)PixelIntegralFore / PixelFore;                                 //背景灰度百分比
        Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore); //计算类间方差
        if (Sigma > SigmaB)                                                                //遍历最大的类间方差g //找出最大类间方差以及对应的阈值
        {
            SigmaB = Sigma;
            Threshold = j;
        }
    }
    return Threshold; //返回最佳阈值;
}

/**
 * @brief 图像裁剪-Height(待改进)
 *
 * @param image8
 * @param height
 * @param width
 * @return uint8*
 */
uint8 *CutImageH(uint8 *image8, uint16 height, uint16 width)
{
    static uint8 cuth_image[CAMERA_H][CAMERA_W]; // 裁剪h后的图像
    uint16 error_h = 0;                          // 目标图像与实际图像的偏差的绝对值

    if (CAMERA_H > height) // 目标高度大于实际高度
    {
        error_h = CAMERA_H - height;
        for (int temp = 0, i = 0; i < CAMERA_H; i++, temp++)
        {
            for (int j = 0; j < width; j++)
            {
                cuth_image[i][j] = *(image8);
                image8++;
            }

            if ((temp + 1) % (height / error_h) == 0)
            {
                i += 1;
                for (int j = 0; j < width; j++)
                {
                    cuth_image[i][j] = cuth_image[i - 1][j];
                }
            }
        }
    }

    else if (CAMERA_H < height) // 目标高度小于实际高度
    {
        error_h = height - CAMERA_H;
        for (int temp = 0, i = 0; i < CAMERA_H; i++, temp++)
        {
            if ((temp + 1) % (height / error_h) == 0)
            {
                temp += 1;
                for (int j = 0; j < width; j++)
                {
                    image8++;
                }
            }
            for (int j = 0; j < width; j++)
            {
                cuth_image[i][j] = *(image8);
                image8++;
            }
        }
    }

    else if (CAMERA_H == height) // 目标高度等于实际高度
    {
        return image8;
    }

    return *cuth_image;
}

/**
 * @brief 图像裁剪-Width(待改进)
 *
 * @param image9
 * @param height
 * @param width
 * @return uint8*
 */
uint8 *CutImageW(uint8 *image9, uint16 height, uint16 width)
{
    static uint8 cutw_image[CAMERA_H][CAMERA_W]; // 裁剪h后的图像
    uint16 error_w = 0;                          // 目标图像与实际图像的偏差的绝对值

    if (CAMERA_W > width) // 目标宽度大于实际宽度
    {
        error_w = CAMERA_W - width;
        for (int i = 0; i < CAMERA_H; i++)
        {
            for (int temp = 0, j = 0; j < CAMERA_W; j++, temp++)
            {
                cutw_image[i][j] = *(image9);
                image9++;
                if ((temp + 1) % (width / error_w) == 0)
                {
                    j++;
                    cutw_image[i][j] = cutw_image[i][j - 1];
                }
                // printf(" %3d ",cutw_image[i][j]);
            }
        }
    }

    else if (CAMERA_W < width) // 目标宽度小于实际宽度
    {
        error_w = width - CAMERA_W;
        for (int i = 0; i < CAMERA_H; i++)
        {
            for (int temp = 0, j = 0; j < CAMERA_W; j++, temp++)
            {
                if ((temp + 1) % (width / error_w) == 0)
                {
                    temp += 1;
                    image9++;
                }
                cutw_image[i][j] = *(image9);
                image9++;
                // printf(" %3d ",cutw_image[i][j]);
            }
        }
    }

    else if (CAMERA_W == width)
    {
        return image9;
    }

    return *cutw_image;
}
