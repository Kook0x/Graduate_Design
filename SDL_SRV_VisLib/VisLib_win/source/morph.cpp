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
 * FILE:     morph.c
 *
 * ABSTRACT: morphologic functions
 *
 * REVISION HISTORY:
 *
 * $Id: morph.c,v 1.1.1.1 1999/07/06 13:43:18 terry Exp $
 *
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <malloc.h>

#include "vislib.h"

/* we want to take advantage of the fact that morphological filters
   using a flat, square support (i.e. Min and Max operators !!) are
   separable, and thus greatly reduce processing time ;
   it is then faster to implement an edge detector by substracting
   an eroded image from a dilated one, not to mention that we can
   use the basic morphological operators for other purposes such as :
   - noise reduction
   - segmentation
   - shape extraction
   - shape cleaning and simplification */


/* -----------------------------------------------------------
   MORPHOLOGIC TOOLS - erosion, dilation
   ----------------------------------------------------------- */

/* rgb erosion */
int
vlRgbErode(vlImage *src,int size,vlWindow *window,vlImage *dest)
{
  int i,j;
  int index;
  int width,height;
  int s1,s2;
  int x1,x2,y1,y2;
  int r_min,g_min,b_min;
  vlPixel *input;
  vlPixel *output;
  vlPixel *temp;
  int temp_index;
  vlPixel new1;

  /* verify parameters */
  if ((!src) || (size <= 0) || (!window) || (!dest)) {
    VL_ERROR ("vlRgbErode: error: illegal parameter\n");
    return (-1);		/* failure */
  }
  
  /* make sure we are given a RGB picture */
  if (src->format != RGB){
    VL_ERROR ("vlRgbErode: error: src image is not RGB\n");
    return (-1);		/* failure */
  }

  /* copy all the image so that not filtered area is uptodate */
  if (0 > vlImageCopy (src, dest)) {
    VL_ERROR ("vlRgbErode: unable to copy original image\n");
    return (-1);		/* failure */
  }

  /* extract useful data */
  width = src->width;
  height = src->height;
  input = src->pixel;
  output = dest->pixel;
  s2 = size/2;
  s1 = -(size-s2);
  x1 = VL_MAX(window->x-s1,-s1);
  x2 = VL_MIN(window->x+window->width,width-s2);
  y1 = VL_MAX(window->y-s1,-s1);
  y2 = VL_MIN(window->y+window->height,height-s2);

  /* create a temp array to store intermediary results */
  temp = (unsigned short *)malloc(VL_RGB_SIZE(width, height));
  memcpy (temp, input, VL_RGB_SIZE(width, height));


  /*
   * horizontal filter 
   */

  /* for each point within filtering window */
  for(j=y1;j<y2;j++){
    r_min = g_min = b_min = 255;
    for(i=x1;i<x2;i++){
      int ii;

      /* if we are at the beginning of a line */
      if(r_min == 255){
	
	index = VL_RGB_PIXEL*(j*width+i);
	for(ii=s1;ii<s2;ii++){
	  temp_index = index+VL_RGB_PIXEL*ii;
	  r_min = VL_MIN(input[temp_index],r_min);
	  g_min = VL_MIN(input[temp_index+1],g_min);
	  b_min = VL_MIN(input[temp_index+2],b_min);
	}
      } 
      else {
	index = VL_RGB_PIXEL*(j*width+i);
	
	/* get new red min */
	new1 = input[index+3*(s2-1)];
	if(new1 < r_min) r_min = new1;
	else {   
	  r_min = 255;
	  for(ii=s1;ii<s2;ii++) r_min = VL_MIN(input[index+3*ii],r_min);
	}
	
	/* get new green min */
	new1 = input[index+3*(s2-1)+1];
	if(new1 < g_min) g_min = new1;
	else {
	  g_min = 255;
	  for(ii=s1;ii<s2;ii++) g_min = VL_MIN(input[index+3*ii+1],g_min);
	}	

	/* get new blue min */
	new1 = input[index+3*(s2-1)+2];
	if(new1 < b_min) b_min = new1;
	else {
	  b_min = 255;
	  for(ii=s1;ii<s2;ii++) b_min = VL_MIN(input[index+3*ii+2],b_min);
	}	
      }
      
      /* set output */
      temp[index] = r_min;
      temp[index+1] = g_min;
      temp[index+2] = b_min;
    }
  }
  

  /* vertical filter */
  
  /* for each point within filtering window */
  for(i=x1;i<x2;i++){
    r_min = g_min = b_min = 255;
    for(j=y1;j<y2;j++){
      int ii;
      
      /* if we are at the beginning of a column */ 
      if(r_min == 255){ 
	r_min = g_min = b_min = 255;
	index = VL_RGB_PIXEL*(j*width+i);
	for(ii=s1;ii<s2;ii++){
	  temp_index = index+VL_RGB_PIXEL*width*ii;
	  r_min = VL_MIN(temp[temp_index],r_min);
	  g_min = VL_MIN(temp[temp_index+1],g_min);
	  b_min = VL_MIN(temp[temp_index+2],b_min);
	}
	
      } 
      else {
	index = VL_RGB_PIXEL*(j*width+i);
	
	/* get new red min */ 
	new1 = temp[index+3*width*(s2-1)];
	if(new1 < r_min) r_min = new1; 
	else { 
	  r_min = 255;
	  for(ii=s1;ii<s2;ii++) r_min = VL_MIN(temp[index+3*width*ii],r_min);
	}
	
	/* get new green min */
	new1 = temp[index+3*width*(s2-1)+1];
	if(new1 < g_min) g_min = new1;
	else { 
	  g_min = 255;
	  for(ii=s1;ii<s2;ii++) g_min = VL_MIN(temp[index+3*width*ii+1],g_min);
	}
	
	/* get new blue min */
	new1 = temp[index+3*width*(s2-1)+2];
	if(new1 < b_min) b_min = new1; 
	else { 
	  b_min = 255;
	  for(ii=s1;ii<s2;ii++) b_min = VL_MIN(temp[index+3*width*ii+2],b_min);
	}
      } 
      
      /* set output */
      output[index] = r_min;
      output[index+1] = g_min;
      output[index+2] = b_min;
    }
  }  
  
  /* get rid of temporary image */
  VL_FREE(temp);

  return (0);			/* success */
}


/* rgb dilation */
int
vlRgbDilate(vlImage *src,int size,vlWindow *window,vlImage *dest)
{
  int i,j;
  int index;
  int width,height;
  int s1,s2;
  int x1,x2,y1,y2;
  int r_max,g_max,b_max;
  vlPixel *input;
  vlPixel *output;
  vlPixel *temp;

  /* verify parameters */
  if ((!src) || (size <= 0) || (!window) || (!dest)) {
    VL_ERROR ("vlRgbDilate: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  /* make sure we are given a RGB picture */
  if (src->format != RGB){
    VL_ERROR ("vlRgbDilate: error: src image is not RGB\n");
    return (-1);		/* failure */
  }

  /* copy all the image so that not filtered area is uptodate */
  if (0 > vlImageCopy (src, dest)) {
    VL_ERROR ("vlRgbDilate: unable to copy original image\n");
    return (-1);		/* failure */
  }

  /* extract useful data */
  width = src->width;
  height = src->height;
  input = src->pixel;
  output = dest->pixel;
  s2 = size/2;
  s1 = -(size-s2);
  x1 = VL_MAX(window->x-s1,-s1);
  x2 = VL_MIN(window->x+window->width,width-s2);
  y1 = VL_MAX(window->y-s1,-s1);
  y2 = VL_MIN(window->y+window->height,height-s2);

  /* create a temp array to store intermediary results */
  temp = (unsigned short *)malloc(VL_RGB_SIZE(width, height));
  memcpy (temp, input, VL_RGB_SIZE(width, height));

  /*
   * horizontal filter 
   */

  /* for each point within filtering window */
  for(j=y1;j<y2;j++){
    r_max = g_max = b_max = -1;
    for(i=x1;i<x2;i++){
      int ii;

      /* if we are at the beginning of a line */
      if(r_max == -1){
	int temp_index;
	
	index = VL_RGB_PIXEL*(j*width+i);
	for(ii=s1;ii<s2;ii++){
	  temp_index = index+VL_RGB_PIXEL*ii;
	  r_max = VL_MAX(input[temp_index],r_max);
	  g_max = VL_MAX(input[temp_index+1],g_max);
	  b_max = VL_MAX(input[temp_index+2],b_max);
	}
      } 
      else {
	vlPixel new1;
	index = VL_RGB_PIXEL*(j*width+i);
	
	/* get new red max */
	new1 = input[index+3*(s2-1)];
	if(new1 > r_max) r_max = new1;
	else {   
	  r_max = 0;
	  for(ii=s1;ii<s2;ii++) r_max = VL_MAX(input[index+3*ii],r_max);
	}
	
	/* get new green max */
	new1 = input[index+3*(s2-1)+1];
	if(new1 > g_max) g_max = new1;
	else {
	  g_max = 0;
	  for(ii=s1;ii<s2;ii++) g_max = VL_MAX(input[index+3*ii+1],g_max);
	}	

	/* get new blue max */
	new1 = input[index+3*(s2-1)+2];
	if(new1 > b_max) b_max = new1;
	else {
	  b_max = 0;
	  for(ii=s1;ii<s2;ii++) b_max = VL_MAX(input[index+3*ii+2],b_max);
	}	
      }
      
      /* set output */
      temp[index] = r_max;
      temp[index+1] = g_max;
      temp[index+2] = b_max;
    }
  }
  

  /* vertical filter */
  
  /* for each point within filtering window */
  for(i=x1;i<x2;i++){
    r_max = g_max = b_max = 0;
    for(j=y1;j<y2;j++){
      int ii;
      
      /* if we are at the beginning of a column */ 
      if(r_max == 255){ 
	int temp_index;
	r_max = g_max = b_max = 0;
	index = VL_RGB_PIXEL*(j*width+i);
	for(ii=s1;ii<s2;ii++){
	  temp_index = index+VL_RGB_PIXEL*width*ii;
	  r_max = VL_MAX(temp[temp_index],r_max);
	  g_max = VL_MAX(temp[temp_index+1],g_max);
	  b_max = VL_MAX(temp[temp_index+2],b_max);
	}
	
      } 
      else {
	vlPixel new1;
	index = VL_RGB_PIXEL*(j*width+i);
	
	/* get new red max */ 
	new1 = temp[index+3*width*(s2-1)];
	if(new1 > r_max) r_max = new1; 
	else { 
	  r_max = 0;
	  for(ii=s1;ii<s2;ii++) r_max = VL_MAX(temp[index+3*width*ii],r_max);
	}
	
	/* get new green max */
	new1 = temp[index+3*width*(s2-1)+1];
	if(new1 > g_max) g_max = new1;
	else { 
	  g_max = 0;
	  for(ii=s1;ii<s2;ii++) g_max = VL_MAX(temp[index+3*width*ii+1],g_max);
	}
	
	/* get new blue max */
	new1 = temp[index+3*width*(s2-1)+2];
	if(new1 > b_max) b_max = new1; 
	else { 
	  b_max = 0;
	  for(ii=s1;ii<s2;ii++) b_max = VL_MAX(temp[index+3*width*ii+2],b_max);
	}
      } 
      
      /* set output */
      output[index] = r_max;
      output[index+1] = g_max;
      output[index+2] = b_max;
    }
  }  

  /* get rid of temporary image */
  VL_FREE(temp);

  return (0);			/* success */  
}


/* -----------------------------------------------------------
   EDGE DETECTION - based on morphological tools
   ----------------------------------------------------------- */

int
vlEdgeDetect(vlImage *src,int size,vlWindow *window,vlImage *dest)
{
  vlImage *src1;
  vlImage *src2;

  /* verify parameters */
  if ((!src) || (size <= 0) || (!window) || (!dest)) {
    VL_ERROR ("vlEdgeDetect: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  /* make sure we are given a RGB picture */
  if(src->format != RGB){
    VL_ERROR ("vlRgbDetect: error: src image is not RGB\n");
    return (-1);		/* failure */
  }

  src1 = VL_IMAGE_CREATE();
  src2 = VL_IMAGE_CREATE();
  
  vlRgbErode(src,size,window,src1);
  vlRgbDilate(src,size,window,src2);
  
  vlSubtract(src2,src1,window,dest);

  vlImageDestroy(src1);
  vlImageDestroy(src2);

  return (0);			/* success */
}



/* -----------------------------------------------------------
   BINARY IMAGES - erode and dilate
   ----------------------------------------------------------- */

int
vlBinaryErode(vlImage *src,int size,vlWindow *window,vlImage *dest)
{
  int i,j;
  int x1,x2,y1,y2;
  int s1,s2;
  int width,height;
  vlPixel *input;
  vlPixel *output;

  /* verify parameters */
  if ((!src) || (size <= 0) || (!window) || (!dest)) {
    VL_ERROR ("vlBinaryErode: error: illegal parameter\n");
    return (-1);		/* failure */
  }
  
  /* make sure we're given a BINARY image */
  if(src->format != BINARY){
    VL_ERROR ("vlBinaryErode: error: src image is not BINARY\n");
    return (-1);		/* failure */
  }

  /* copy all the image so that not filtered area is uptodate */
  if (0 > vlImageCopy (src, dest)) {
    VL_ERROR ("vlBinaryErode: unable to copy original image\n");
    return (-1);		/* failure */
  }

  /* extract useful data */
  width = src->width;
  height = src->height;
  input = src->pixel;
  output = dest->pixel;
  s2 = size/2;
  s1 = -(size-s2);
  x1 = VL_MAX(window->x-s1,-s1);
  x2 = VL_MIN(window->x+window->width,width-s2);
  y1 = VL_MAX(window->y-s1,-s1);
  y2 = VL_MIN(window->y+window->height,height-s2);

  for(j=y1;j<y2;j++) {
    for(i=x1;i<x2;i++){
      int ii,jj;
      
      /* if original is set, ignore what follows */
      if(input[j*width+i] != 0)	continue;
      
      /* else padd the slider area around current pixel in image */
      for(jj=s1;jj<s2;jj++){
	int y = j + jj;
	int index = y*width;
	for(ii=s1;ii<s2;ii++){
	  int x = i + ii;
	  output[index+x] = 0;
	}
      }
    }
  }
  
  return (0);			/* success */
}

int
vlBinaryDilate(vlImage *src,int size,vlWindow *window,vlImage *dest)
{
  int i,j;
  int x1,x2,y1,y2;
  int s1,s2;
  int width,height;
  vlPixel *input;
  vlPixel *output;

  /* verify parameters */
  if ((!src) || (size <= 0) || (!window) || (!dest)) {
    VL_ERROR ("vlBinaryDilate: error: illegal parameter\n");
    return (-1);		/* failure */
  }
  
  /* make sure we're given a BW image */
  if(src->format != BINARY){
    VL_ERROR ("vlBinaryDilate: error: src image is not BINARY\n");
    return (-1);		/* failure */
  }

  /* copy all the image so that not filtered area is uptodate */
  if (0 > vlImageCopy (src, dest)) {
    VL_ERROR ("_vlBinaryDilate: unable to copy original image\n");
    return (-1);		/* failure */
  }
  
  /* temp variables to speed up conversion */
  width = src->width;
  height = src->height;
  input = src->pixel;
  output = dest->pixel;
  s2 = size/2;
  s1 = -(size-s2);
  x1 = VL_MAX(window->x-s1,-s1);
  x2 = VL_MIN(window->x+window->width,width-s2);
  y1 = VL_MAX(window->y-s1,-s1);
  y2 = VL_MIN(window->y+window->height,height-s2);
 
  for(j=y1;j<y2;j++) {
    for(i=x1;i<x2;i++){
      int ii,jj;
      
      /* if original is set, ignore what follows */
      if(input[j*width+i] == 0)	continue;
      
      /* else padd the slider area around current pixel in image */
      for(jj=s1;jj<s2;jj++){
	int y = j + jj;
	int index = y*width;
	for(ii=s1;ii<s2;ii++){
	  int x = i + ii;
	  output[index+x] = 255;
	}
      }
    }
  }

  return (0);			/* success */
}


int
vlBinaryOpen(vlImage *src,int size,int num,vlWindow *window)
{
vlImage *dest1;
int i,j;

dest1 = VL_IMAGE_CREATE ();

if (src->format != BINARY) {
    VL_ERROR ("vlBinaryOpen: src image is not BINARY\n");
    return (-1);		/* failure */
  }
  /* initialize the new picture */
  if (0 > vlImageInit (dest1, BINARY, src->width, src->height)) {
    VL_ERROR ("vlRgb2Binary: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }
for(i=0;i<num;i++){
	vlBinaryErode(src,size,window,dest1);
	vlImageCopy(dest1, src);
}
for(j=0;j<num;j++){
	vlBinaryDilate(src,size,window,dest1);
	vlImageCopy(dest1, src);
}
vlImageDestroy(dest1);

return (0);
}

int vlBinaryClose(vlImage *src,int size,int t,vlWindow *window)
{
	int i,j;
	vlImage *dest1;
	dest1 = VL_IMAGE_CREATE ();
	if (src->format != BINARY) {
		VL_ERROR ("vlBinaryOpen: src image is not BINARY\n");
		return (-1);		/* failure */
	  }
	  /* initialize the new picture */

	if (0 > vlImageInit (dest1, BINARY, src->width, src->height)) {
		VL_ERROR ("vlRgb2Binary: error: could not initialize dest image\n");
		return (-1);		/* failure */
	  }
	for(i=0;i<t;i++){
		vlBinaryDilate(src,size,window,dest1);
		vlImageCopy(dest1, src);
	}
	for(j=0;j<t;j++){
		vlBinaryErode(src,size,window,dest1);
		vlImageCopy(dest1, src);
	}

	vlImageDestroy(dest1);
	return (0);
}




