/** vlUtility.h
 ** ABSTRACT: protos for misc functions
 ** 
 * VisLib is protected by the GNU Public License.
 * VisLib is owned and distributed by ActivMedia Robotics, LLC
 *              ActivMedia Robotics, LLC
 *              44 Concord St
 *              Peterborough NH  03458
 *              (603)924-9100
 *              http://www.activrobots.com
 *              http://robots.activmedia.com
 *              support@activmedia.com
 * ActivMedia Robotics does not make any representation on the
 * suitability of this software for any purpose.
 * VisLib is released AS-IS, without implied or express warranty.
 * VisLib was developed by Terry Fong and S. Grange, EPFL.
 **/
    
    


#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <stdio.h>		/* printf */
#include <stdlib.h>

#include "vislib.h"

/* utility macros */
#define VL_MIN(a,b) ((a) < (b)) ? (a) : (b)
#define VL_MAX(a,b) ((a) < (b)) ? (b) : (a)
#define VL_ERROR    printf
#define VL_FREE(a)  if(a) { free(a); a=NULL; }

/*
 * HSI conversion macros 
 */

/* convert hue (degrees/value): [0,360] to [0,VL_PIXEL_MAXVAL] */
#define HDEG_TO_HVAL(a) (VL_PIXEL_MAXVAL * (a) / 360)
#define HVAL_TO_HDEG(a) (360 * (a) / VL_PIXEL_MAXVAL)
/* convert saturation (percent/value): [0,100] to [0,VL_PIXEL_MAXVAL] */
#define SPCT_TO_SVAL(a) (VL_PIXEL_MAXVAL * (a) / 100)
#define SVAL_TO_SPCT(a) (100 * (a) / VL_PIXEL_MAXVAL)
/* convert intensity (percent/value): [0,100] to [0,VL_PIXEL_MAXVAL] */
#define IPCT_TO_IVAL(a) (VL_PIXEL_MAXVAL * (a) / 100)
#define IVAL_TO_IPCT(a) (100 * (a) / VL_PIXEL_MAXVAL)

/* convenience defs for draw functions (r,g,b taken from X11's "rgb.txt") */
#define VL_COLOR_RED	255,0,0
#define VL_COLOR_GREEN  0,255,0
#define VL_COLOR_BLUE	0,0,255
#define VL_COLOR_WHITE  255,255,255
#define VL_COLOR_BLACK  0,0,0
#define VL_COLOR_ORANGE 255,165,0
#define VL_COLOR_GOLD   255,215,0
#define VL_COLOR_PINK	255,192,203
#define VL_COLOR_YELLOW 255,255,0

/* drawing tools prototypes */
int vlDrawRect (vlImage *image, vlWindow *rect, 
		unsigned char r, unsigned char g, unsigned char b);
int vlDrawCross(vlImage *image,vlWindow *rect,
		unsigned char r, unsigned char g, unsigned char b);
int vlDrawObject(vlImage *image,vlObject *object,
		 unsigned char r, unsigned char g, unsigned char b);
int vlDrawBinary(vlImage *image,vlImage *bwImage,vlWindow *window,
		 unsigned char r, unsigned char g, unsigned char b);
int vlDrawGrid(vlImage *image);
int vlDrawCenteredGrid(vlImage *image);

/* file management */
int vlImageSave(vlImage *image,char *file_name);
int vlImageLoad(vlImage *image,char *file_name);
int vlObjectSave (vlObject *object, char *filename);
int vlObjectLoad (vlObject *object, char *filename);


/* image manipulation */
int vlSubtract (vlImage *src1, vlImage *src2, vlWindow *window, vlImage *dest);
int vlScale (vlImage *src, float scale, vlImage *dest);

/* misc */
int vlQsortCompare (const void *x, const void *y);
int vlMedianRgbValue(vlImage *image,vlPoint *point,vlPoint *median);
int vlMedianNrgValue(vlImage *image,vlPoint *point,vlPoint *median);
int vlMedianHsiValue(vlImage *image,vlPoint *point,vlPoint *median);
int vlPixelSize (vlImageFormat format);

/*october 2006*/
typedef struct {
int r;
int g;
int b;

int h;
int s;
int i;
}vlColor;

vlColor vlPixelColor(vlImage *src, vlPoint *point);

#endif /* __UTILITY_H__ */
