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
 * FILE:     convolution.c
 *
 * ABSTRACT: image convolution functions
 *
 * REVISION HISTORY:
 *
 * $Id: convolution.c,v 1.1.1.1 1999/07/06 13:43:19 terry Exp $
 *
 *
 *****************************************************************************/

#include <stdio.h>
#include <memory.h>		/* malloc */

#include "vislib.h"

/****************************** private functions ****************************/
static int _vlRGBconvolution (vlImage *src, vlMask *mask, vlWindow *window,
			       vlImage *dest);


/* filter image formats  */
int
vlConvolve (vlImage *src, vlMask *mask, vlWindow *window, vlImage *dest)
{
  /* verify parameters */
  if ((!src) || (!mask) || (!window) || (!dest)) {
    VL_ERROR ("vlConvolve: error: one of the parameters is NULL\n");
    return (-1);		/* failure */
  }

  switch(src->format){
  case RGB:
    return (_vlRGBconvolution(src, mask, window, dest));
    break;
  default:
    printf ("vlConvolve: convolution format not implemented yet\n");
    return (-1);		/* failure */
    break;
  }

  return (0);			/* success */
}


/* 2D RGB image convolution */
static int
_vlRGBconvolution (vlImage *src, vlMask *mask, vlWindow *window,
		   vlImage *dest)
{
  int x, y, r, c, r1, r2, c1, c2, row, col;
  int index, d_index, k_index;
  int width, height, k_width, k_height, k_width2, k_height2;
  float *kernel, sum_r, sum_g, sum_b;
  vlPixel *new_data, *data;

  width = src->width;
  height = src->height;

  /* verify window coords */
  if ((window->width > width) ||
      (window->width < 0) ||
      (window->height > height) ||
      (window->height < 0) ||
      (window->x > width) ||
      (window->x < 0) ||
      (window->y > height) ||
      (window->y < 0)) {
    VL_ERROR ("_vlRGBconvolution: illegal window coordinates\n");
    return (-1);		/* failure */
  }

  /* duplicate the old image so that the borders are correct */
  if (0 > vlImageCopy (src, dest)) {
    VL_ERROR ("_vlRGBconvolution: unable to copy original image\n");
    return (-1);		/* failure */
  }

  r1 = window->y + (mask->height / 2);
  r2 = window->height - (mask->height / 2);
  c1 = window->x + (mask->width / 2);
  c2 = window->width - (mask->width / 2);

  data = src->pixel;
  new_data = dest->pixel;
  kernel = mask->kernel;
  k_width = mask->width;
  k_height = mask->height;
  k_width2 = k_width / 2;
  k_height2 = k_height / 2;

  for (r=r1; r<r2; r++) {
    for (c=c1; c<c2; c++) {

      sum_r = sum_g = sum_b = 0;
      for (y=0; y<k_height; y++) {
	int yk = y*k_width;
	int ck2 = c + k_width2;
	row = r - y + k_height2;

	for (x=0; x<k_width; x++) {
	  k_index = yk + x;

	  /* compute data indexes */
	  col = ck2 - x;
	  d_index = (row*width + col) * VL_RGB_PIXEL;

	  sum_r += data[d_index]   * kernel[k_index];
	  sum_g += data[d_index++] * kernel[k_index];
	  sum_b += data[d_index++] * kernel[k_index];
	}
      }

      index = (r*width + c) * VL_RGB_PIXEL; /* current pixel */
      new_data[index] = (vlPixel) sum_r;
      new_data[index++] = (vlPixel) sum_g;
      new_data[index++] = (vlPixel) sum_b;
    }
  }

  return (0);			/* success */
}



/* -----------------------------------------------------------
   FAST SMOOTHER - separable averaging window
   ----------------------------------------------------------- */
/* perform a fast 2D smoothing using separable averaging window */
int
vlSmooth (vlImage *src, int size, vlWindow *window, vlImage *dest)
{
  int i,j;
  int index;
  int width,height;
  int s1,s2;
  int x1,x2,y1,y2;
  int r_sum,g_sum,b_sum;
  vlPixel *input;
  vlPixel *output;

  /* verify parameters */
  if ((!src) || (size <= 0) || (!window) || (!dest)) {
    VL_ERROR ("vlSmooth: error: illegal parameter\n");
    return (-1);		/* failure */
  }
  
  /* RGB only... */
  if(src->format != RGB){
    VL_ERROR ("vlSmooth: error: src image is not RGB\n");
    return (-1);		/* failure */
  }

  /* extract useful data */
  width = src->width;
  height = src->height;
  input = src->pixel;
  s2 = size/2;
  s1 = -(size-s2);
  x1 = VL_MAX(window->x-s1,-s1);
  x2 = VL_MIN(window->x+window->width,width-s2);
  y1 = VL_MAX(window->y-s1,-s1);
  y2 = VL_MIN(window->y+window->height,height-s2);

  /* initialize new pic */
  if (0 > vlImageInit (dest, RGB, width, height)) {
    VL_ERROR ("vlSmooth: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }

  output = dest->pixel;
  
  /* copy all the image so that not filtered area is uptodate */
  memcpy(output,input,3*width*height*sizeof(vlPixel));

  /* horizontal filter */

  /* for each point within filtering window */
  for(j=y1;j<y2;j++){
    r_sum = g_sum = b_sum = -1;
    for(i=x1;i<x2;i++){
      
      /* if we are at the beginning of a line */
      if(r_sum == -1){
	int ii;
	int temp_index;
	
	index = 3*(j*width+i);
	r_sum = g_sum = b_sum = 0;
	for(ii=s1;ii<s2;ii++){
	  temp_index = index+3*ii;
	  r_sum += input[temp_index];
	  g_sum += input[temp_index+1];
	  b_sum += input[temp_index+2];
	}
      } 
      else {
	index = 3*(j*width+i);
	r_sum = r_sum + input[index+3*(s2-1)] - input[index+3*(s1-1)];
	g_sum = g_sum + input[index+3*(s2-1)+1] - input[index+3*(s1-1)+1];
	b_sum = b_sum + input[index+3*(s2-1)+2] - input[index+3*(s1-1)+2];
      }

      /* set output */
      output[index] = r_sum/size;
      output[index+1] = g_sum/size;
      output[index+2] = b_sum/size;
    }
  }

  return (0);			/* success */
}


/* -----------------------------------------------------------
   NON LINEAR SMOOTHER - median smoother
   ----------------------------------------------------------- */

/* this filter is quite efficient to deal with random noise,
   and has the advantage to preserve edges,
   even though it's computationnaly expensive ;
   performance degrade with high noise density */
int
vlSmoothMedian(vlImage *src,int size,vlWindow *window,vlImage *dest)
{
  int i,j;
  int index;
  int width,height;
  int s1,s2;
  int x1,x2,y1,y2;
  int sort_size = size*size;
  int array_index;
  vlPixel *input;
  vlPixel *output;
  vlPixel *r_array;
  vlPixel *g_array;
  vlPixel *b_array;

  /* verify parameters */
  if ((!src) || (size <= 0) || (!window) || (!dest)) {
    VL_ERROR ("vlSmoothMedian: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  /* RGB only... */
  if(src->format != RGB){
    VL_ERROR ("vlSmoothMedian: error: src image is not RGB\n");
    return (-1);		/* failure */
  }

  r_array = (vlPixel *) malloc (sort_size * sizeof(vlPixel));
  g_array = (vlPixel *) malloc (sort_size * sizeof(vlPixel));
  b_array = (vlPixel *) malloc (sort_size * sizeof(vlPixel));

  /* extract useful data */
  width = src->width;
  height = src->height;
  input = src->pixel;
  s2 = size/2;
  s1 = -(size-s2);
  x1 = VL_MAX(window->x-s1,-s1);
  x2 = VL_MIN(window->x+window->width,width-s2);
  y1 = VL_MAX(window->y-s1,-s1);
  y2 = VL_MIN(window->y+window->height,height-s2);

  /* initialize new pic */
  if (0 > vlImageInit (dest, RGB, width, height)) {
    VL_ERROR ("vlSmoothMedian: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }

  output = dest->pixel;
  
  /* copy all the image so that not filtered area is uptodate */
  memcpy(output,input,3*width*height*sizeof(vlPixel));

  /* filter */
  /* for each point within filtering window */
  for(j=y1;j<y2;j++)
    for(i=x1;i<x2;i++){
      int ii,jj;
      
      /* build component arrays */
      /* else padd the slider area around current pixel in image */
      array_index = 0;
      for(ii=s1;ii<s2;ii++)
	for(jj=s1;jj<s2;jj++){
	  int x = i + ii, y = j + jj;
	  
	  /* if in image
	     if ( (x>=x1) && (x<x2) && (y>=y1) && (y<y2) ){ */

	  index = 3*(y*width+x);
	  r_array[array_index] = input[index];
	  g_array[array_index] = input[index+1];
	  b_array[array_index] = input[index+2];
	  array_index++;
	}
      
      /* current pixel index */
      index = 3*(j*width+i);
      
      /* red component 
	 sort array, get median element and put it in output pixel */
      qsort(r_array, sort_size, sizeof(vlPixel), vlQsortCompare); 
      output[index] = r_array[sort_size/2];

      /* green component 
	 sort array, get median element and put it in output pixel */
      qsort(g_array, sort_size, sizeof(vlPixel), vlQsortCompare); 
      output[index+1] = g_array[sort_size/2];

      /* blue component 
	 sort array, get median element and put it in output pixel */
      qsort(b_array, sort_size, sizeof(vlPixel), vlQsortCompare); 
      output[index+2] = b_array[sort_size/2];
    }

  VL_FREE (r_array);
  VL_FREE (g_array);
  VL_FREE (b_array);

  return (0);			/* success */
}

