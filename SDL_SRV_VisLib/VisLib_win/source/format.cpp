/*****************************************************************************
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
 *
 * FILE:     format.c
 *
 * ABSTRACT: image format and size conversion implementation
 *
 * REVISION HISTORY:
 *
 * $Id: format.c,v 1.3 1999/09/14 12:34:09 terry Exp $
 *
 *
 *****************************************************************************/

#include <stdio.h>
#include <math.h>

#include "vislib.h"

/* uncomment the following for debug messages */
/*#define DEBUG*/

/* uncomment the following to use integer math */
#define USE_INT_MATH


/* ---------------------------------------------------------
   image format tools
   --------------------------------------------------------- */

/* given a RGB picture, return the R frame only */
int
vlRgb2R(vlImage *src, vlWindow *window, vlImage *dest)
{
  int i, j;
  int index, width;
  int x1, x2, y1, y2;
  vlPixel *input;
  vlPixel *output;

  /* verify parameters */
  if ((!src) || (!window) || (!dest)) {
    VL_ERROR ("vlRgb2R: error: one of the parameters is NULL\n");
    return (-1);		/* failure */
  }

  /* make sure we have a RGB picture */
  if (src->format != RGB) {
    VL_ERROR ("vlRgb2R: error: src image is not RGB\n");
    return (-1);		/* failure */
  }
  
  /* initialize the new picture */
  if (0 > vlImageInit (dest, GRAY, src->width, src->height)) {
    VL_ERROR ("vlRgb2R: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }

  /* temp variables to optimize memory access */
  width = src->width;
  x1 = window->x;
  x2 = x1 + (window->width);
  y1 = window->y;
  y2 = y1 + (window->height);
  input = src->pixel;
  output = dest->pixel;
 
  /* proceed to conversion */
  for (j=y1; j<y2; j++) {
    for (i=x1; i<x2; i++) {
      index = VL_GRAY_PIXEL * (j*width + i);
      output[index] = input[VL_RGB_PIXEL*index]; /* R */
    }
  }
  
  return (0);			/* success */
}


/* given a RGB picture, return the G frame only */
int
vlRgb2G(vlImage *src, vlWindow *window, vlImage *dest)
{
  int i, j;
  int index, width;
  int x1, x2, y1, y2;
  vlPixel *input;
  vlPixel *output;

  /* verify parameters */
  if ((!src) || (!window) || (!dest)) {
    VL_ERROR ("vlRgb2G: error: one of the parameters is NULL\n");
    return (-1);		/* failure */
  }

  /* make sure we have a RGB picture */
  if (src->format != RGB) {
    VL_ERROR ("vlRgb2G: src image is not RGB\n");
    return (-1);		/* failure */
  }
  
  /* initialize the new picture */
  if (0 > vlImageInit (dest, GRAY, src->width, src->height)) {
    VL_ERROR ("vlRgb2G: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }

  /* temp variables to optimize memory access */
  width = src->width;
  x1 = window->x;
  x2 = x1 + (window->width);
  y1 = window->y;
  y2 = y1 + (window->height);
  input = src->pixel;
  output = dest->pixel;
 
  /* proceed to conversion */
  for (j=y1; j<y2; j++) {
    for (i=x1; i<x2; i++) {
      index = VL_GRAY_PIXEL * (j*width + i);
      output[index] = input[VL_RGB_PIXEL*index+1]; /* G */
    }
  }

  return (0);			/* success */
}


/* given a RGB picture, return the B frame only */
int 
vlRgb2B(vlImage *src, vlWindow *window, vlImage *dest)
{
  int i, j;
  int index, width;
  int x1, x2, y1, y2;
  vlPixel *input;
  vlPixel *output;

  /* verify parameters */
  if ((!src) || (!window) || (!dest)) {
    VL_ERROR ("vlRgb2B: error: one of the parameters is NULL\n");
    return (-1);		/* failure */
  }

  /* make sure we have a RGB picture */
  if (src->format != RGB) {
    VL_ERROR ("vlRgb2B: src image is not RGB\n");
    return (-1);		/* failure */
  }

  /* initialize the new picture */
  if (0 > vlImageInit (dest, GRAY, src->width, src->height)) {
    VL_ERROR ("vlRgb2B: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }

  /* temp variables to optimize memory access */
  width = src->width;
  x1 = window->x;
  x2 = x1 + (window->width);
  y1 = window->y;
  y2 = y1 + (window->height);
  input = src->pixel;
  output = dest->pixel;
 
  /* proceed to conversion */
  for (j=y1; j<y2; j++) {
    for (i=x1; i<x2; i++){
      index = VL_GRAY_PIXEL * (j*width + i);
      output[index] = input[VL_RGB_PIXEL*index+2]; /* B */
    }
  }

  return (0);			/* success */
} 



/* given a RGB picture, return a gray level one */
int 
vlRgb2Gray(vlImage *src, vlWindow *window, vlImage *dest)
{
  int i, j;
  int index, width;
  int x1, x2, y1, y2;
  vlPixel *input;
  vlPixel *output;

  /* verify parameters */
  if ((!src) || (!window) || (!dest)) {
    VL_ERROR ("vlRgb2Gray: error: one of the parameters is NULL\n");
    return (-1);		/* failure */
  }

  /* make sure we have a RGB picture */
  if (src->format != RGB) {
    VL_ERROR ("vlRgb2Gray: src image is not RGB\n");
    return (-1);		/* failure */
  }

  /* initialize the new picture */
  if (0 > vlImageInit (dest, GRAY, src->width, src->height)) {
    VL_ERROR ("vlRgb2Gray: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }

  /* temp variables to optimize memory access */
  width = src->width;
  x1 = window->x;
  x2 = x1 + (window->width);
  y1 = window->y;
  y2 = y1 + (window->height);
  input = src->pixel;
  output = dest->pixel;
 
  /* proceed to conversion */
  for (j=y1; j<y2; j++) {
    for (i=x1; i<x2; i++) {
      index = VL_RGB_PIXEL * (j*width + i);

      /* average RGB into graylevel */
      output[index/VL_RGB_PIXEL] = 
	(input[index] + input[index+1] + input[index+2]) / 3;
    }
  }

  return (0);			/* success */
} 


/* given a RGB picture, return a binary level one */
int 
vlRgb2Binary(vlImage *src, int threshold, vlWindow *window, vlImage *dest)
{
  int i, j;
  int index, width;
  int x1, x2, y1, y2;
  vlPixel *input;
  vlPixel *output;

  /* verify parameters */
  if ((!src) || (threshold < 0) || (!window) || (!dest)) {
    VL_ERROR ("vlRgb2Binary: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  /* make sure we have a RGB picture */
  if (src->format != RGB) {
    VL_ERROR ("vlRgb2Binary: src image is not RGB\n");
    return (-1);		/* failure */
  }

  /* initialize the new picture */
  if (0 > vlImageInit (dest, BINARY, src->width, src->height)) {
    VL_ERROR ("vlRgb2Binary: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }

  /* temp variables to optimize memory access */
  width = src->width;
  x1 = window->x;
  x2 = x1 + (window->width);
  y1 = window->y;
  y2 = y1 + (window->height);
  input = src->pixel;
  output = dest->pixel;
 
  /* proceed to conversion */
  for (j=y1; j<y2; j++) {
    for (i=x1; i<x2; i++) {
      index = VL_RGB_PIXEL * (j*width + i);

      if (((input[index]+input[index+1]+input[index+2])/VL_RGB_PIXEL) >
	  threshold) {
	output[index/VL_RGB_PIXEL] = 255;
      }
      else {
	output[index/VL_RGB_PIXEL] = 0;
      }
    }
  }

  return (0);			/* success */
} 


/* given a RGB picture with RGB AND TOL, return a binary level one */
int 
vlRgb2Binary_tol(vlImage *src, vlHSI_carl_tol_t *para, vlWindow *window, vlImage *dest)
{
  int i, j;
  int index, width;
  int x1, x2, y1, y2;
  vlPixel *input;
  vlPixel *output;

  /* verify parameters */
  if ((!src) || (!para) || (!window) || (!dest)) {
    VL_ERROR ("vlRgb2Binary: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  /* make sure we have a RGB picture */
  if (src->format != RGB) {
    VL_ERROR ("vlRgb2Binary: src image is not RGB\n");
    return (-1);		/* failure */
  }

  /* initialize the new picture */
  if (0 > vlImageInit (dest, BINARY, src->width, src->height)) {
    VL_ERROR ("vlRgb2Binary: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }

  /* temp variables to optimize memory access */
  width = src->width;
  x1 = window->x;
  x2 = x1 + (window->width);
  y1 = window->y;
  y2 = y1 + (window->height);
  input = src->pixel;
  output = dest->pixel;


   vlPixel r_max = CLAMP(para->h+para->h_tol, 0, 255);
   vlPixel r_min = CLAMP(para->h-para->h_tol, 0, 255);
   vlPixel g_max = CLAMP(para->s+para->s_tol, 0, 255);
   vlPixel g_min = CLAMP(para->s-para->s_tol, 0, 255);
   vlPixel b_max = CLAMP(para->i+para->i_tol, 0, 255);
   vlPixel b_min = CLAMP(para->i-para->i_tol, 0, 255);


  /* proceed to conversion */
  for (j=y1; j<y2; j++) {
    for (i=x1; i<x2; i++) {
      index = VL_RGB_PIXEL * (j*width + i);

      if ((input[index]>r_max)
	  	||(input[index]<r_min) 
	  	||(input[index+1]>g_max)
	  	||(input[index+1]<g_min)
	  	||(input[index+2]>b_max)
	  	||(input[index+2]<b_min)
	  	) {
	output[index/VL_RGB_PIXEL] = 255;
      }
      else {
	output[index/VL_RGB_PIXEL] = 0;
      }
    }
  }

  return (0);			/* success */
} 


/* given a normalized rgb picture, return a gray level one */
int
vlNrg2Gray(vlImage *src, vlWindow *window, vlImage *dest)
{
  int i, j;
  int index, width;
  int x1, x2, y1, y2;
  vlPixel *input;
  vlPixel *output;
  
  /* verify parameters */
  if ((!src) || (!window) || (!dest)) {
    VL_ERROR ("vlNrg2Gray: error: one of the parameters is NULL\n");
    return (-1);		/* failure */
  }

  /* make sure we have a NRG picture */
  if (src->format != NRG) {
    VL_ERROR ("vlNrg2Gray: src image is not NRG\n");
    return (-1);		/* failure */
  }

  /* initialize the new picture */
  if (0 > vlImageInit (dest, GRAY, src->width, src->height)) {
    VL_ERROR ("vlNrg2Gray: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }
  
  /* temp variables to optimize memory access */
  width = src->width;
  x1 = window->x;
  x2 = x1 + (window->width);
  y1 = window->y;
  y2 = y1 + (window->height);
  input = src->pixel;
  output = dest->pixel;
  
  /* proceed to conversion */
  for (j=y1; j<y2; j++) {
    for (i=x1; i<x2; i++) {
      index = j*width + i;
      output[index] = 
	input[VL_NRG_PIXEL*index] + /* normalized R */
	input[VL_NRG_PIXEL*index+1]; /* normalized G */
    }
  }

  return (0);			/* success */
}


/******************************************************************************
 *
 * vlRgb2Hsi --
 *	given a RGB picture, return a HSI one. Truth in advertising: this
 *      function actually converts from RGB to HLS (Hue, Lightness, Saturation)
 *      [Foley & VanDam, 90]. HLS is supposed to "correlate" well with
 *      human notions of visual perception (i.e., what artists think 
 *      of as tint, shade, and tone). However, HLS (and all related spaces
 *      such as HSV, HSI, and HCI) are simple linear transforms from RGB and
 *      share the same shortcomings. The only advantage of the HLS spaces is
 *      that they allow more "intuitive" color specification. For more
 *      information, see Charles Poynton's Color Space FAQ, currently at:
 *      http://www.inforamp.net/~poynton/ColorFAQ.html
 *
 *****************************************************************************/
int 
vlRgb2Hsi(vlImage *src, vlWindow *window, vlImage *dest)
#ifdef USE_INT_MATH
/* the following uses integer math */
{
  int i, j;
  int r, g, b;
  int index, width;
  int x1, x2, y1, y2;
  vlPixel *input;
  vlPixel *output;
  int min, max, delta, sum;
  int vl43 = VL_PIXEL_MAXVAL * 4 / 3;
  int vl3 = VL_PIXEL_MAXVAL / 3;
  int vl53 = VL_PIXEL_MAXVAL * 5 / 3;
  int vl23 = VL_PIXEL_MAXVAL * 2 / 3;

#ifdef DEBUG
  printf ("vlRgb2Hsi: integer version\n");
#endif
  
  /* verify parameters */
  if ((!src) || (!window) || (!dest)) {
    VL_ERROR ("vlRgb2Hsi: error: one of the parameters is NULL\n");
    return (-1);		/* failure */
  }

  /* make sure we have a RGB picture */
  if (src->format != RGB) {
    VL_ERROR ("vlRgb2Hsi: src image is not RGB\n");
    return (-1);		/* failure */
  }
  
  /* initialize the new picture */
  if (0 > vlImageInit (dest, HSI, src->width, src->height)) {
    VL_ERROR ("vlRgb2Hsi: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }
  
  /* temporary variable to optimize memory access */
  width = src->width;
  x1 = window->x;
  x2 = x1 + (window->width);
  y1 = window->y;
  y2 = y1 + (window->height);
  output = dest->pixel;
  input = src->pixel;

  /* proceed to conversion */
  for (j=y1; j<y2; j++) {
    for (i=x1; i<x2; i++){

      /* From Foley & vanDam, "Computer Graphics: Principles and Practice",
       *      Addison-Wesley, 2nd edition, 1990, p. 595 (Fig 13.36)
       */

      /* get a point */
      index = VL_RGB_PIXEL * (j*width + i);

      r = input[index++];	/* 0-255 */
      g = input[index++];	/* 0-255 */
      b = input[index];		/* 0-255 */

      max = VL_MAX(r, g);
      max = VL_MAX(max, b);
      min = VL_MIN(r, g);
      min = VL_MIN(min, b);

      sum = min+max;

      /*
       * LIGHTNESS
       * scale lightness [0,255] to [0,VL_PIXEL_MAXVAL] and output
       */
      output[index--] = (vlPixel) (VL_PIXEL_MAXVAL * sum / 510);


      /* achromatic case (color is a grey) */      
      if (max == min){
	output[index--] = 0;	/* saturation */
	output[index] = 0;	/* hue is "undefined", but set to 0 */
      }

      /* chromatic case */      
      else {
	delta = max - min;

	/*
	 * SATURATION
	 * scale saturation [0,255] to [0,VL_PIXEL_MAXVAL] and output
	 */
	if (sum < 255) {	/* bottom cone (dark) */
	  output[index--] = (vlPixel) 
	    (VL_PIXEL_MAXVAL * delta / sum);
	}
	else {			/* top cone (bright) */
	  output[index--] = (vlPixel) 
	    (VL_PIXEL_MAXVAL * delta / (510-sum));
	}
	
	/*
	 * HUE
	 * scale hue [0,360] to [0,VL_PIXEL_MAXVAL] and output 
	 * note: rounding error if VL_PIXEL_MAXVAL < 360 (i.e., vlPixel=8-bit)
	 */
	if (r == max) {		/* color is between yellow and magenta */
	  if (g < b) {
	    output[index] = (vlPixel) 
	      (VL_PIXEL_MAXVAL * (g-b) / (delta * 6) + VL_PIXEL_MAXVAL);
	  }
	  else {
	    output[index] = (vlPixel) 
	      (VL_PIXEL_MAXVAL * (g-b) / (delta * 6));
	  }
	}
	else if (g == max) {	/* color is between cyan and yellow */
	  if ((2*delta) < (b-r)) {
	    output[index] = (vlPixel) 
	      (VL_PIXEL_MAXVAL * (b-r) / (delta * 6) + vl43);
	  }
	  else {
	    output[index] = (vlPixel) 
	      (VL_PIXEL_MAXVAL * (b-r) / (delta * 6) + vl3);
	  }
	}
	else if (b == max) {	/* color is between magenta and cyan */
	  if ((4*delta) < (r-g)) {
	    output[index] = (vlPixel) 
	      (VL_PIXEL_MAXVAL * (r-g) / (delta * 6) + vl53);
	  }
	  else {
	    output[index] = (vlPixel) 
	      (VL_PIXEL_MAXVAL * (r-g) / (delta * 6) + vl23);
	  }
	}
	else {
	  VL_ERROR ("vlRgb2Hsi: Problem with hue\n");
	  output[index] = 0;
	}
      }	/* end chromatic */
    }
  }

  return (0);			/* success */
}
#else
/* the following uses floating point math */
{
  int i, j;
  float r, g, b;
  int index, width;
  int x1, x2, y1, y2;
  float hue, saturation, lightness;
  vlPixel *input;
  vlPixel *output;
  float min, max, delta, sum;

#ifdef DEBUG
  printf ("vlRgb2Hsi: floating point version\n");
#endif
  
  /* verify parameters */
  if ((!src) || (!window) || (!dest)) {
    VL_ERROR ("vlRgb2Hsi: error: one of the parameters is NULL\n");
    return (-1);		/* failure */
  }

  /* make sure we have a RGB picture */
  if (src->format != RGB) {
    VL_ERROR ("vlRgb2Hsi: src image is not RGB\n");
    return (-1);		/* failure */
  }
  
  /* initialize the new picture */
  if (0 > vlImageInit (dest, HSI, src->width, src->height)) {
    VL_ERROR ("vlRgb2Hsi: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }
  
  /* temporary variable to optimize memory access */
  width = src->width;
  x1 = window->x;
  x2 = x1 + (window->width);
  y1 = window->y;
  y2 = y1 + (window->height);
  output = dest->pixel;
  input = src->pixel;

  /* proceed to conversion */
  for (j=y1; j<y2; j++) {
    for (i=x1; i<x2; i++){

      /* From Foley & vanDam, "Computer Graphics: Principles and Practice",
       *      Addison-Wesley, 2nd edition, 1990, p. 595 (Fig 13.36)
       */

      /* get a point */
      index = VL_RGB_PIXEL * (j*width + i);
      r = (float) input[index] / 255.0f; /* 0-1 */
      g = (float) input[index+1] / 255.0f; /* 0-1 */
      b = (float) input[index+2] / 255.0f; /* 0-1 */

      max = VL_MAX(r, g);
      max = VL_MAX(max, b);
      min = VL_MIN(r, g);
      min = VL_MIN(min, b);

      /*
       * LIGHTNESS
       */
      lightness = (min+max) / 2.0f;

      /* achromatic case (because r=g=b) */
      if (max == min) {
	saturation = 0;
	hue = 0;		/* hue is undefined */
      }

      /* chromatic case */
      else {
	delta = max - min;
	sum = max + min;

	/*
	 * SATURATION
	 */
	if (lightness < 0.5f) { /* bottom cone (dark) */
	  saturation = delta / sum;
	}
	else {			/* top cone (bright) */
	  saturation = delta / (2.0f - sum);
	}
	
	/*
	 * HUE
	 */
	if (r == max) {		/* color is between yellow and magenta */
	  hue = (g-b) / delta;
	}
	else if (g == max) {	/* color is between cyan and yellow */
	  hue = 2 + (b-r) / delta;
	}
	else if (b == max) {	/* color is between magenta and cyan */
	  hue = 4 + (r-g) / delta;
	}
	else {
	  VL_ERROR ("vlRgb2Hsi: Problem with hue\n");
	  hue = 0;
	}

	hue *= 60.0;

	if (hue < 0) {
	  hue += 360;      
	}
      }	/* end chromatic */

      /* scale hue [0,360] to [0,VL_PIXEL_MAXVAL] and output 
       * note: rounding error if VL_PIXEL_MAXVAL < 360 (i.e., vlPixel=8-bit)
       */
      output[index] = (vlPixel) (VL_PIXEL_MAXVAL * hue / 360.0f + 0.5f);

      /* scale saturation [0,1] to [0,VL_PIXEL_MAXVAL] and output */
      output[index+1] = (vlPixel) (VL_PIXEL_MAXVAL * saturation + 0.5f);

      /* scale lightness [0,1] to [0,VL_PIXEL_MAXVAL] and output */
      output[index+2] = (vlPixel) (VL_PIXEL_MAXVAL * lightness + 0.5f);

#ifdef DEBUG
      if (!(index % 10000)) {
	printf ("%d: %d, %d, %d -> %.2f, %.2f, %.2f (%d, %d, %d)\n", 
		index,
		(int) input[index], (int) input[index+1], (int) input[index+2],
		hue, saturation, lightness,
		(int) output[index], (int) output[index+1], (int) output[index+2]);
      }
#endif

    }
  }

  return (0);			/* success */
}
#endif


/* helper function for vlHsi2Rgb 
 *  n1 in [-1,1]
 *  n2 in [0,1]
 *  hue    in [0,360]
 */
float
_vlValue (float n1, float n2, float hue) 
{
  float value;

  if (hue > 360.0f) {
    hue -= 360.0f;
  }
  else if (hue < 0) {
    hue += 360.0f;
  }

  if (hue < 60.0f) {
    value = (n1 + (n2-n1) * hue / 60.0f);
  }
  else if (hue < 180.0f) {
    value = n2;
  }
  else if (hue < 240.0f) {
    value = (n1 + (n2-n1) * (240.0f-hue) / 60.0f);
  }
  else {
    value = n1;
  }

  return (value);
}


/******************************************************************************
 *
 * vlHsi2Rgb --
 *	given a HSI picture, return a RGB one. Truth in advertising: this
 *      function actually converts from HLS to RGB [Foley & VanDam, 90]
 *
 *****************************************************************************/
int 
vlHsi2Rgb (vlImage *src, vlWindow *window, vlImage *dest)
{
  int i, j;
  float r, g, b;
  int index, width;
  int x1, x2, y1, y2;
  float hue, saturation, lightness;
  float m1, m2;
  vlPixel *input;
  vlPixel *output;
  
  /* verify parameters */
  if ((!src) || (!window) || (!dest)) {
    VL_ERROR ("vlHsi2Rgb: error: one of the parameters is NULL\n");
    return (-1);		/* failure */
  }

  /* make sure we have a HSI picture */
  if (src->format != HSI) {
    VL_ERROR ("vlHsi2Rgb: src image is not HSI\n");
    return (-1);		/* failure */
  }
  
  /* initialize the new picture */
  if (0 > vlImageInit (dest, RGB, src->width, src->height)) {
    VL_ERROR ("vlHsi2Rgb: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }
  
  /* temporary variable to optimize memory access */
  width = src->width;
  x1 = window->x;
  x2 = x1 + (window->width);
  y1 = window->y;
  y2 = y1 + (window->height);
  output = dest->pixel;
  input = src->pixel;

  /* proceed to conversion */
  for (j=y1; j<y2; j++) {
    for (i=x1; i<x2; i++){

      /* From Foley & vanDam, "Computer Graphics: Principles and Practice",
       *      Addison-Wesley, 2nd edition, 1990, p. 596 (Fig 13.37)
       */

      /* get a point */
      index = VL_HSI_PIXEL * (j*width + i);

      /* scale input [0,VL_PIXEL_MAXVAL] to hue [0,360] */
      hue = input[index] * 360.0f / (float) VL_PIXEL_MAXVAL;

      /* scale input [0,VL_PIXEL_MAXVAL] to saturation [0,1] */
      saturation = input[index+1] / (float) VL_PIXEL_MAXVAL;

      /* scale input [0,VL_PIXEL_MAXVAL] to lightness [0,1] */
      lightness = input[index+2] / (float) VL_PIXEL_MAXVAL;

      if (lightness < 0.5f) {
	m2 = lightness * (1.0f + saturation);
      }
      else {
	m2 = lightness + saturation - lightness * saturation;
      }

      m1 = 2.0f * lightness - m2;

      /* 
       * achromatic case (no hue) 
       */
      if (saturation == 0) {
	if (hue == 0) {
	  r = g = b = lightness;
	}
	else {
	  VL_ERROR ("vlHsi2Rgb: Problem with hue (input = %d, %d, %d)\n",
		    (int) input[index], (int) input[index+1], (int) input[index+2]);
	  r = g = b = 0;
	}
      }
      else {
	r = _vlValue (m1, m2, hue + 120.0f);
	g = _vlValue (m1, m2, hue);
	b = _vlValue (m1, m2, hue - 120.0f);
      }	  

      output[index] = (vlPixel) (255.0f * r + 0.5f);
      output[index+1] = (vlPixel) (255.0f * g + 0.5f);
      output[index+2] = (vlPixel) (255.0f * b + 0.5f);

#ifdef DEBUG
      if (!(index % 10000)) {
	printf ("%d: %d, %d, %d <- %.2f, %.2f, %.2f (%d, %d, %d)\n", 
		index,
		(int) output[index], (int) output[index+1], (int) output[index+2],
		hue, saturation, lightness,
		(int) input[index], (int) input[index+1], (int) input[index+2]);
      }
#endif

    }
  }

  return (0);			/* success */
}


/* given a gray picture, return a bin one */
int 
vlGray2Binary(vlImage *src, int threshold, vlWindow *window, vlImage *dest)
{
  int i, j;
  int index, width;
  int x1, x2, y1, y2;
  vlPixel *input;
  vlPixel *output;
  
  /* verify parameters */
  if ((!src) || (threshold < 0) || (!window) || (!dest)) {
    VL_ERROR ("vlGray2Binary: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  /* make sure we have a RGB picture */
  if (src->format != GRAY) {
    VL_ERROR ("vlGray2Binary: src image is not GRAY\n");
    return (-1);		/* failure */
  }
  
  /* initialize the new picture */
  if (0 > vlImageInit (dest, BINARY, src->width, src->height)) {
    VL_ERROR ("vlGray2Binary: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }
  
  /* temp variables to optimize memory access */
  width = src->width;
  x1 = window->x;
  x2 = x1 + (window->width);
  y1 = window->y;
  y2 = y1 + (window->height);
  input = src->pixel;
  output = dest->pixel;
  
  /* proceed to conversion */
  for (j=y1; j<y2; j++) {
    for (i=x1; i<x2; i++){
      index = VL_GRAY_PIXEL * (j*width + i);
      if (input[index] > threshold) {
	output[index] = 255;
      }
      else {
	output[index] = 0;
      }
    }
  }
  
  return (0);			/* success */
}


/* given a RGB picture, return a normalized red/green one */
int 
vlRgb2Nrg(vlImage *src, vlWindow *window, vlImage *dest)
{
  int i, j;
  int index, width;
  int x1, x2, y1, y2;
  int sum;
  vlPixel *input;
  vlPixel *output;

  /* verify parameters */
  if ((!src) || (!window) || (!dest)) {
    VL_ERROR ("vlRgb2Nrg: error: one of the parameters is NULL\n");
    return (-1);		/* failure */
  }

  /* make sure we have a RGB picture */
  if (src->format != RGB) {
    VL_ERROR ("vlRgb2Nrg: src image is not RGB\n");
    return (-1);		/* failure */
  }
  
  /* initialize the new picture */
  if (0 > vlImageInit (dest, NRG, src->width, src->height)) {
    VL_ERROR ("vlRgb2Nrg: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }
  
  /* temp variables to optimize memory access */
  width = src->width;
  x1 = window->x;
  x2 = x1 + (window->width);
  y1 = window->y;
  y2 = y1 + (window->height);
  input = src->pixel;
  output = dest->pixel;

  /* proceed to conversion */
  for (j=y1; j<y2; j++) {
    for (i=x1; i<x2; i++) {
      index = j*width + i;
      sum = 
	input[VL_RGB_PIXEL*index] + /* R */
	input[VL_RGB_PIXEL*index+1] + /* G */
	input[VL_RGB_PIXEL*index+2]; /* B */

      /* normalized R = 255 * R / (R+G+B) */
      output[VL_NRG_PIXEL*index] = (vlPixel)
	(255.0f * input[VL_RGB_PIXEL*index] / sum + 0.5f);

      /* normalized G = 255 * G / (R+G+B) */
      output[VL_NRG_PIXEL*index+1] = (vlPixel)
	(255.0f * input[VL_RGB_PIXEL*index+1] / sum + 0.5f);
    }
  }

  return (0);			/* success */
}


/* given a BIN picture, return a RGB equivalent one */
int 
vlBinary2Rgb(vlImage *src, vlWindow *window, vlImage *dest)
{
  int i, j;
  int index, width;
  int x1, x2, y1, y2;
  vlPixel *input;
  vlPixel *output;

  /* verify parameters */
  if ((!src) || (!window) || (!dest)) {
    VL_ERROR ("vlBinary2Rgb: error: one of the parameters is NULL\n");
    return (-1);		/* failure */
  }

  /* make sure we have a BINARY picture */
  if (src->format != BINARY) {
    VL_ERROR ("vlBinary2Rgb: src image is not BINARY\n");
    return (-1);		/* failure */
  }
  
  /* initialize the new picture */
  if (0 > vlImageInit (dest, RGB, src->width, src->height)) {
    VL_ERROR ("vlBinary2Rgb: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }
  
  /* temp variables to optimize memory access */
  width = src->width;
  x1 = window->x;
  x2 = x1 + (window->width);
  y1 = window->y;
  y2 = y1 + (window->height);
  input = src->pixel;
  output = dest->pixel;

  /* proceed to conversion */
  for (j=y1; j<y2; j++) {
    for (i=x1; i<x2; i++){
      index = VL_RGB_PIXEL * (j*width + i);
      output[index] = 
	output[index+1] = 
	output[index+2] = 
	input[index/VL_RGB_PIXEL];
    }
  }

  return (0);			/* success */
}


/* ---------------------------------------------------------
   image format tools
   --------------------------------------------------------- */

/* given a HSI picture, return the H frame only */
int
vlHsi2H(vlImage *src, vlWindow *window, vlImage *dest)
{
  int i, j;
  int index, width;
  int x1, x2, y1, y2;
  vlPixel *input;
  vlPixel *output;
  
  /* verify parameters */
  if ((!src) || (!window) || (!dest)) {
    VL_ERROR ("vlHsi2H: error: one of the parameters is NULL\n");
    return (-1);		/* failure */
  }

  /* make sure we have a RGB picture */
  if (src->format != HSI) {
    VL_ERROR ("vlHsi2H: error: src image is not HSI\n");
    return (-1);		/* failure */
  }
  
  /* initialize the new picture */
  if (0 > vlImageInit (dest, GRAY, src->width, src->height)) {
    VL_ERROR ("vlHsi2H: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }

  /* temp variables to optimize memory access */
  width = src->width;
  x1 = window->x;
  x2 = x1 + (window->width);
  y1 = window->y;
  y2 = y1 + (window->height);
  input = src->pixel;
  output = dest->pixel;
 
  /* proceed to conversion */
  for (j=y1; j<y2; j++) {
    for (i=x1; i<x2; i++) {
      index = VL_GRAY_PIXEL * (j*width + i);
      output[index] = input[VL_HSI_PIXEL*index]; /*H */
    }
  }
  dest->format=H;///////////////////////////////////////////////////////
  return (0);			/* success */
}

/* ---------------------------------------------------------
   image format tools
   --------------------------------------------------------- */

/* given a HSI picture, return the S frame only */
int
vlHsi2S(vlImage *src, vlWindow *window, vlImage *dest)
{
  int i, j;
  int index, width;
  int x1, x2, y1, y2;
  vlPixel *input;
  vlPixel *output;

  /* verify parameters */
  if ((!src) || (!window) || (!dest)) {
    VL_ERROR ("vlHsi2S: error: one of the parameters is NULL\n");
    return (-1);		/* failure */
  }

  /* make sure we have a RGB picture */
  if (src->format != HSI) {
    VL_ERROR ("vlHsi2S: error: src image is not HSI\n");
    return (-1);		/* failure */
  }
  
  /* initialize the new picture */
  if (0 > vlImageInit (dest, GRAY, src->width, src->height)) {
    VL_ERROR ("vlHsi2S: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }

  /* temp variables to optimize memory access */
  width = src->width;
  x1 = window->x;
  x2 = x1 + (window->width);
  y1 = window->y;
  y2 = y1 + (window->height);
  input = src->pixel;
  output = dest->pixel;
 
  /* proceed to conversion */
  for (j=y1; j<y2; j++) {
    for (i=x1; i<x2; i++) {
      index = VL_GRAY_PIXEL * (j*width + i);
      output[index] = input[VL_HSI_PIXEL*index]; /*S */
    }
  }
  dest->format=S;
  
  return (0);			/* success */
}
/* ---------------------------------------------------------
   image format tools
   --------------------------------------------------------- */

/* given a HSI picture, return the I frame only */
int
vlHsi2I(vlImage *src, vlWindow *window, vlImage *dest)
{
  int i, j;
  int index, width;
  int x1, x2, y1, y2;
  vlPixel *input;
  vlPixel *output;

  /* verify parameters */
  if ((!src) || (!window) || (!dest)) {
    VL_ERROR ("vlHsi2I: error: one of the parameters is NULL\n");
    return (-1);		/* failure */
  }

  /* make sure we have a RGB picture */
  if (src->format != HSI) {
    VL_ERROR ("vlHsi2I: error: src image is not HSI\n");
    return (-1);		/* failure */
  }
  
  /* initialize the new picture */
  if (0 > vlImageInit (dest, GRAY, src->width, src->height)) {
    VL_ERROR ("vlHsi2I: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }

  /* temp variables to optimize memory access */
  width = src->width;
  x1 = window->x;
  x2 = x1 + (window->width);
  y1 = window->y;
  y2 = y1 + (window->height);
  input = src->pixel;
  output = dest->pixel;
 
  /* proceed to conversion */
  for (j=y1; j<y2; j++) {
    for (i=x1; i<x2; i++) {
      index = VL_GRAY_PIXEL * (j*width + i);
      output[index] = input[VL_HSI_PIXEL*index]; /*I */
    }
  }
  dest->format=I;
  return (0);			/* success */
}

