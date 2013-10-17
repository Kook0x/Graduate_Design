#include "global.h"
#include "yuv2rgb.h"

/* lookup tables */
unsigned int  ng_yuv_gray[256];
unsigned int  ng_yuv_red[256];
unsigned int  ng_yuv_blue[256];
unsigned int  ng_yuv_g1[256];
unsigned int  ng_yuv_g2[256];
unsigned int  ng_clip[256 + 2 * CLIP];


int width=WIDTH;
int height=HEIGHT;


void yuv2rgb_init(void)
{
	int i;
  
	/* init Lookup tables */
	for (i = 0; i < 256; i++) {
		ng_yuv_gray[i] = i * LUN_MUL >> 8;
		ng_yuv_red[i]  = (RED_ADD    + i * RED_MUL)    >> 8;
		ng_yuv_blue[i] = (BLUE_ADD   + i * BLUE_MUL)   >> 8;
		ng_yuv_g1[i]   = (GREEN1_ADD + i * GREEN1_MUL) >> 8;
		ng_yuv_g2[i]   = (GREEN2_ADD + i * GREEN2_MUL) >> 8;
  	}
  	for (i = 0; i < CLIP; i++)
    		ng_clip[i] = 0;
  	for (; i < CLIP + 256; i++)
    		ng_clip[i] = i - CLIP;
  	for (; i < 2 * CLIP + 256; i++)
    		ng_clip[i] = 255;
}

void yuv2rgb (unsigned char* in_addr, vlPixel* out_addr)
{
	unsigned char *y,*u,*v;
	unsigned char *us,*vs;
	vlPixel *dp, *d;
	int i,j,gray;
  
  	dp = out_addr;
 	y  = (unsigned char *)in_addr;
	u  = y + width * height;
	v  = u + width * height / 4;
	for (i = 0; i < height; i++) {
		d = dp;
		us = u; vs = v;
		for (j = 0; j < width; j+=2) {
      		gray   = Gray(*y);
      		*(d++) = Red(gray,*v);
			*(d++) = Green(gray,*v,*u);
			*(d++) = Blue(gray,*u);

			y++;
			gray   = Gray(*y);
			*(d++) = Red(gray,*v);
			*(d++) = Green(gray,*v,*u);
			*(d++) = Blue(gray,*u);

			y++; u++; v++;
		}
		if (0 == (i % 2)) {
			u = us; v = vs;
		}
		dp += (width*3);
	}

}

