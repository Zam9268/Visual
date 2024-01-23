#ifndef __CAMERA_H_
#define __CAMERA_H_
//修改定义
typedef unsigned char (uint8);
typedef unsigned short (uint16);
typedef unsigned int (uint32);

#define CAMERA_H (100)
#define CAMERA_W (120)//定义原图像的宽和高


extern uint8 *Simple_Binaryzation(uint8 *image0, uint8 threshold);   // 简单二值化
extern uint8 *Mean_Binaryzation(uint8 *image1);                      // 均值二值化
extern uint8 *Gaussain_Blur(uint8 *image2);                          // 高斯模糊
extern uint8 *Sobel_Edge(uint8 *image4);                             // Sobel边缘算子
extern uint8 *Scharr_Edge(uint8 *image5);                            // Scharr边缘算子
extern uint8 *NMS(uint8 *image6, uint8 choose_edge);                 // 非极大值抑制
extern uint8 *TwoThreshold(uint8 *image7);                           // 双边缘阈值链接
extern uint8 Camera_GetOSTU(uint8 *tmImage);                         // 全局大津法
extern uint8 *CutImageH(uint8 *image8, uint16 height, uint16 width); // 图像height方向裁剪
extern uint8 *CutImageW(uint8 *image9, uint16 height, uint16 width); // 图像width方向裁剪

#endif 
/*
author: Zam
date: 2024/1/23
*/