/*****************************************************************************
 *
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
 * FILE:     filter.c
 *
 * ABSTRACT: image filtering module implementation
 *
 * REVISION HISTORY:
 *
 * $Id: filter.c,v 1.2 1999/09/14 12:34:27 terry Exp $
 *
 *
 *****************************************************************************/

#include <stdio.h>

#include "vislib.h"

/* filter an image with RGB attributes */
int
vlRgbFilter (vlImage *src, vlObject *object, vlWindow *window, vlImage *dest)
{
  int i,j;
  int x1,x2,y1,y2;
  int r_min,g_min,b_min;
  int r_max,g_max,b_max;
  int index,width;
  vlPixel *input;
  vlPixel *output;

  /* verify parameters */
  if ((!src) || (!object) || (!window) || (!dest)) {
    VL_ERROR ("vlRgbFilter: error: one of the parameters is NULL\n");
    return (-1);		/* failure */
  }
  
  /* make sure we are given a RGB picture */
  if(src->format != RGB){
    VL_ERROR ("vlRgbFilter: error: src image is not RGB\n");
    return (-1);		/* failure */
  }
  
  /* initialize output picture */
  if (0 > vlImageInit (dest, BINARY, src->width, src->height)) {
    VL_ERROR ("vlRgbFilter: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }

  /* temp variables to optimize memory access */
  width = src->width;
  x1 = window->x;
  x2 = x1 + (window->width);
  y1 = window->y;
  y2 = y1 + (window->height);
  r_min = object->r_min; r_max = object->r_max;
  g_min = object->g_min; g_max = object->g_max;
  b_min = object->b_min; b_max = object->b_max;
  input = src->pixel;
  output = dest->pixel;

  /* filter ! */
  for (j=y1; j<y2; j++) {
    for (i=x1; i<x2; i++){
      index = VL_RGB_PIXEL*(j*width+i);
      if ((input[index] < r_min) || (input[index] > r_max) ||
	  (input[index+1] < g_min) || (input[index+1] > g_max) || 
	  (input[index+2] < b_min) || (input[index+2] > b_max)) {
	output[index/VL_RGB_PIXEL] = 0;
      }
      else {
	output[index/VL_RGB_PIXEL] = 255;
      }
    }
  }

  return (0);			/* success */
}


/* filter an image with NRG attributes */
int
vlNrgFilter (vlImage *src, vlObject *object, vlWindow *window, vlImage *dest)
{
  int i,j;
  int x1,x2,y1,y2;
  int nr_min,ng_min;
  int nr_max,ng_max;
  int index,width;
  vlPixel *input;
  vlPixel *output;

  /* verify parameters */
  if ((!src) || (!object) || (!window) || (!dest)) {
    VL_ERROR ("vlNrgFilter: error: one of the parameters is NULL\n");
    return (-1);		/* failure */
  }
  
  /* make sure we are given a normalized RGB picture */
  if(src->format != NRG){
    VL_ERROR ("vlNrgFilter: error: src image is not NRG\n");
    return (-1);		/* failure */
  }

  /* initialize output picture */
  if (0 > vlImageInit (dest, BINARY, src->width, src->height)) {
    VL_ERROR ("vlRgbFilter: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }
  
  /* temp variables to optimize memory access */
  width = src->width;
  x1 = window->x;
  x2 = x1 + (window->width);
  y1 = window->y;
  y2 = y1 + (window->height);
  nr_min = object->nr_min; nr_max = object->nr_max;
  ng_min = object->ng_min; ng_max = object->ng_max;
  input = src->pixel;
  output = dest->pixel;

  /* filter ! */
  for (j=y1; j<y2; j++) {
    for (i=x1; i<x2; i++){
      index = VL_NRG_PIXEL*(j*width+i);
      if ((input[index] < nr_min) || (input[index] > nr_max) ||
	  (input[index+1] < ng_min) || (input[index+1] > ng_max)) {
	output[index/VL_NRG_PIXEL] = 0;
      }
      else {
	output[index/VL_NRG_PIXEL] = 255;
      }
    }
  }

  return (0);			/* success */
}

/* filter an image with HSI attributes */
int
vlHsiFilter (vlImage *src, vlObject *object, vlWindow *window, vlImage *dest)
{
  int i,j;
  int x1,x2,y1,y2;
  int h_min,s_min,i_min;
  int h_max,s_max,i_max;
  int index,width;
  vlPixel *input;
  vlPixel *output;

  /* verify parameters */
  if ((!src) || (!object) || (!window) || (!dest)) {
    VL_ERROR ("vlHsiFilter: error: one of the parameters is NULL\n");
    return (-1);		/* failure */
  }

  /* make sure we are given a HSI picture */
  if(src->format != HSI){
    VL_ERROR ("vlHsiFilter: error: src image is not HSI\n");
    return (-1);		/* failure */
  }

  /* initialize output picture */
  if (0 > vlImageInit (dest, BINARY, src->width, src->height)) {
    VL_ERROR ("vlHsiFilter: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }
  
  /* temp variables to optimize memory access */
  width = src->width;
  x1 = window->x;
  x2 = x1 + (window->width);
  y1 = window->y;
  y2 = y1 + (window->height);
  h_min = object->h_min; 
  h_max = object->h_max;
  s_min = object->s_min; 
  s_max = object->s_max;
  i_min = object->i_min; 
  i_max = object->i_max;
  input = src->pixel;
  output = dest->pixel;

  /* filter ! */
  for (j=y1; j<y2; j++) {
    for (i=x1; i<x2; i++){
      index = VL_HSI_PIXEL*(j*width+i);
      if ((input[index] < h_min) || (input[index] > h_max) ||
	  (input[index+1] < s_min) || (input[index+1] > s_max) || 
	  (input[index+2] < i_min) || (input[index+2] > i_max)) {
	output[index/VL_HSI_PIXEL] = 0;
      }
      else {
	output[index/VL_HSI_PIXEL] = 255;
      }
    }
  }

  return (0);			/* success */
}

