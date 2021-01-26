#include<stdio.h>
// Conversion from RGB24 to YUV420
void InitLookupTable();
int ConvertRGB2YUV(int w,int h,unsigned char *rgbdata,unsigned char *yuv);
// Conversion from YUV420 to RGB24
void InitConvertTable();
void ConvertYUV2RGB(unsigned char *src0,unsigned char *src1,unsigned char *src2,unsigned char *dst_ori,
int width,int height);