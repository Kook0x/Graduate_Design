/** vislib.h
 ** ABSTRACT: VisLib defs & protos
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
 **
 **/

#ifndef __VISLIB_H__
#define __VISLIB_H__

/* VisLib version */
#define VL_VERSION			"beta 0.1"


/*
 * defs
 */

/* image properties */
#define VL_ROWS_DEFAULT			240/* 1/2 NTSC */
#define VL_COLS_DEFAULT			320/* 1/2 NTSC */

/* Immagini a 640 x 480 */
#define VL_ROWS_DEFAULT_GRANDE			480/* 1/2 NTSC */
#define VL_COLS_DEFAULT_GRANDE			640/* 1/2 NTSC */

/* framegrabber default device */
#define VL_GRAB_DEVICE_DEFAULT		"/dev/video0"

/* framgrabber INPUT select */
#define IN_TV			0
#define IN_COMPOSITE	1
#define IN_COMPOSITE2	2
#define IN_SVIDEO		3
#define IN_DEFAULT		8

/* framegrabber color SYSTEM select */
#define NORM_PAL		0
#define NORM_NTSC		1
#define NORM_SECAM		2
#define NORM_DEFAULT	0

/* recommended for use with vlEdgeDetect */
#define VL_EDGE_DETECT_DEFAULT		3

/* recommended for use with vlRgb2Binary */
#define VL_EDGE_THRESHOLD_DEFAULT	25

/* minimum pixel change for motion detection (vlMotionThresholdSet) */
#define VL_MOTION_THRESHOLD_DEFAULT	50

/* length of array used for object speed estimation */
#define VL_OBJECT_NPOSITIONS			10

/* minimal object size [pixel] for at least 1-D (vlObjectSizeLimitSet) */
#define VL_OBJECT_SIZE_LIMIT_DEFAULT	10	

/* step used to widen color boundaries (vlObjectColorUpdateStepSet) */
#define VL_OBJECT_COLOR_UPDATE_STEP_DEFAULT 1

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/*
 * types
 */

/* basic pixel */
typedef unsigned short int vlPixel;/* 16-bit */
#define VL_PIXEL_MAXVAL 65535

/* different image formats */
typedef enum { NONE, RGB, HSI, NRG, GRAY, BINARY,H,S,I} vlImageFormat;////////////

/* image pixel size (# of basic pixels per image pixel)*/
#define VL_NONE_PIXEL	1
#define VL_RGB_PIXEL    3
#define VL_HSI_PIXEL	3
#define VL_NRG_PIXEL    2
#define VL_GRAY_PIXEL   1
#define VL_BINARY_PIXEL 1

/* image data size */
#define VL_NONE_SIZE(width, height)   (VL_NONE_PIXEL)
#define VL_RGB_SIZE(width, height)    (VL_RGB_PIXEL*width*height*sizeof(vlPixel))
#define VL_HSI_SIZE(width, height)    (VL_HSI_PIXEL*width*height*sizeof(vlPixel))
#define VL_NRG_SIZE(width, height)    (VL_NRG_PIXEL*width*height*sizeof(vlPixel))
#define VL_GRAY_SIZE(width, height)   (VL_GRAY_PIXEL*width*height*sizeof(vlPixel))
#define VL_BINARY_SIZE(width, height) (VL_BINARY_PIXEL*width*height*sizeof(vlPixel))


#define CLAMP(val, low, high) ((val<low) ? low : ((val>high) ? high : val))
#define MAX(A,B)        ((A) > (B) ? (A) : (B))
#define MIN(A,B)        ((A) < (B) ? (A) : (B))



/* image data structure */
typedef struct {
  vlImageFormat format;		/* image type */
  int width;			/* # of columns */
  int height;			/* # of rows */
  vlPixel *pixel;		/* pixel array */
} vlImage;

/* convolution mask data structure */
typedef struct {
  int width, height;
  float *kernel;
} vlMask;

/* window (within image) data structure */
typedef struct {
  int x, y;			/* top left corner */
  int width, height;		/* columns, rows */
} vlWindow;  

/* simple point */
typedef struct {
  int x, y;			/* position */
  vlPixel r, g, b;		/* red, green, blue (0-255 each) */
  vlPixel h, s, i;		/* hue, saturation, intensity 
				   (0-VL_PIXEL_MAXVAL each) */
  vlPixel nr, ng;		/* normalized red/green (0-255 each) */
} vlPoint;

typedef struct
{
	vlPixel h, s, i;
	vlPixel h_tol, s_tol, i_tol;
}vlHSI_carl_tol_t;


/* tracking object */
typedef struct {
  int defined;

  /* 
   * geometric properties
   */
  int width;			/* object width (rectangular area) */
  int height;			/* object height (rectangular area) */
  int x, y;			/* image coords */

  /*
   * color boundaries
   */
  vlImageFormat format;
  
  /* RGB domain */
  vlPixel r_max, r_min;
  vlPixel g_max, g_min;
  vlPixel b_max, b_min;
  
  /* normalized color domain */
  vlPixel nr_max, nr_min;
  vlPixel ng_max, ng_min; 
  
  /* HSI domain */
  vlPixel h_max, h_min;
  vlPixel s_max, s_min;
  vlPixel i_max, i_min;

  /*
   * shape data. The image area is containted within a rectangle with
   * pixels on line i (starting at x_offset[i], x_length[i] long). Same for
   * columns. NOTE : patterns stored can only be convex 
   */
  int *x_offset;
  int *x_length;

  /*
   * motion parameters 
   */

  /* estimated velocity */
  int x_vel;
  int y_vel;
  
  /* last position storing arrays */
  int last_x[VL_OBJECT_NPOSITIONS];
  int last_y[VL_OBJECT_NPOSITIONS];

  /* expected next position */
  int next_x;
  int next_y;
  
} vlObject;

/* signal handler */
typedef void (*VL_SIGHND)(int signalNum, ...);


/*
 * function protos 
*/
#include "vlCommon.h"
#include "vlFormat.h"

#include "vlConvolution.h"
#include "vlFilter.h"
#include "vlObject.h"
#include "vlMotion.h"
#include "vlMorph.h"
#include "vlUtility.h"
#include "vlBlob.h"

#include "a_hsi_carl.h"
#include  "myhist.h"
#include "yuv2rgb.h"


#endif /* __VISLIB_H__ */
