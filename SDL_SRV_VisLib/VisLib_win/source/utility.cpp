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

 * FILE:utility.c
 *
 * ABSTRACT: misc utilities implementation
 *
 * REVISION HISTORY:
 *
 * $Id: utility.c,v 1.2 1999/09/14 12:31:27 terry Exp $
 *
 *****************************************************************************/

#include <ctype.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>		/* strncmp */

//#include <unistd.h>

#include "vislib.h"

/* uncomment the following for debug messages */
/*#define DEBUG  */

/*********************************** defs ************************************/
#define _VL_MAXSTRLEN 1024


/* -----------------------------------------------------------
   DRAWING TOOLS - to superimpose on images
   ----------------------------------------------------------- */

/* draw a rectangle on an image */
int
vlDrawRect (vlImage *pic, vlWindow *rect, 
	    unsigned char r, unsigned char g, unsigned char b)
{
  int i,j;
  int x1,x2,y1,y2;
  int index1,index2,width;
  int dist;
  vlPixel *data;

  /* verify parameters */
  if ((!pic) || (!rect)) {
    VL_ERROR ("vlDrawRect: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  /* make sure we have a RGB picture */
  if (pic->format != RGB) {
    VL_ERROR ("vlDrawRect: error: image is not RGB\n");
    return (-1);		/* failure */
  }

  /* temp variables to speed up conversion */
  width = pic->width;
  x1 = rect->x;
  x2 = VL_MIN(x1+(rect->width),width-1);
  y1 = rect->y;
  y2 = VL_MIN(y1+(rect->height),(pic->height)-1);
  data = pic->pixel;
  
  /* draw top and bottom */
  index1 = VL_RGB_PIXEL * (y1*width+x1);
  index2 = VL_RGB_PIXEL * (y2*width+x1);
  
  for (i=x1; i<x2; i++){
    /* rectangle top */
    data[index1++] = r;
    data[index1++] = g;
    data[index1++] = b;
    
    /* rectangle bottom */
    data[index2++] = r;
    data[index2++] = g;
    data[index2++] = b;
  }

  /* draw left and right */
  dist = VL_RGB_PIXEL * (x2-x1);
  for (j=y1; j<y2; j++){
    index1 = VL_RGB_PIXEL * (j*width+x1);
    index2 = index1 + dist;
    
    /* one pixel to the left... */
    data[index1++] = r;
    data[index1++] = g;
    data[index1++] = b;
    
    /* ... and one pixel to the right ! */
    data[index2++] = r;
    data[index2++] = g;
    data[index2++] = b;
  }

  return (0);			/* success */
}


/* draw a cross within a rectangle */
int vlDrawCross(vlImage *pic, vlWindow *rect, 
		unsigned char r, unsigned char g, unsigned char b)
{
  int i,j;
  int x1,x2,y1,y2;
  int hor,vert,width;
  int dist;
  vlPixel *data;

  /* verify parameters */
  if ((!pic) || (!rect)) {
    VL_ERROR ("vlDrawCross: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  /* make sure we have a RGB picture */
  if (pic->format != RGB) {
    VL_ERROR ("vlDrawCross: error: image is not RGB\n");
    return (-1);		/* failure */
  }
  
  /* temp variables to speed up conversion */
  width = pic->width;
  x1 = rect->x;
  x2 = VL_MIN(x1+(rect->width),width-1);
  y1 = rect->y;
  y2 = VL_MIN(y1+(rect->height),(pic->height)-1);
  data = pic->pixel;
  
  hor = VL_RGB_PIXEL * ((y1+(y2-y1)/2)*width+x1);
  for (i=x1; i<x2; i++){
    data[hor++] = r;
    data[hor++] = g;
    data[hor++] = b;
  }
  
  dist = x1+(x2-x1)/2;
  for (j=y1; j<y2; j++){
    vert = VL_RGB_PIXEL * (j*width+dist);
    data[vert++] = r;
    data[vert++] = g;
    data[vert++] = b;
  }

  return (0);			/* success */
}

/* draw the given object in the image */
int vlDrawObject (vlImage *pic, vlObject *object, 
		  unsigned char r, unsigned char g, unsigned char b)
{
  /* image */
  int index, width, height, x, y;
  vlPixel *data;

  /* object */
  int row, col, lastRow, lastCol;
  int *x_offset;
  int *x_length;

  /* verify parameters */
  if ((!pic) || (!object)) {
    VL_ERROR ("vlDrawObj: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  /* make sure we have a RGB picture */
  if (pic->format != RGB) {
    VL_ERROR ("vlDrawObject: error: image is not RGB\n");
    return (-1);		/* failure */
  }
  
  /* image */
  data = pic->pixel;
  height = pic->height;
  width = pic->width;

  /* object */
  x_offset = object->x_offset;
  x_length = object->x_length;

  /* compute last object row to draw (i.e., clip at image bottom) */
  lastRow = VL_MIN (height - (object->y) - 1, object->height);

  /* draw each object row */
  for (row=0; row<lastRow; row++) {

    /* compute image coords */
    x = (object->x) + x_offset[row]; /* image x */
    y = (object->y) + row;	/* image y */
    index = VL_RGB_PIXEL * (y*width + x); /* index into image data */

    /* compute last object col to draw (i.e., clip at image right */
    lastCol = VL_MIN (width - x - 1, x_length[row]);

    for (col=0; col<lastCol; col++) {
      data[index++] = r;
      data[index++] = g;
      data[index++] = b;
    }
  } 
 
  return (0);			/* success */
}


/* superimpose the 1's in a binary image to a given picture
   with the desired color */
int 
vlDrawBinary (vlImage *pic,vlImage *bw_pic,vlWindow *window,
	      unsigned char r, unsigned char g, unsigned char b)
{
  int i,j;
  int x1,x2,y1,y2;
  int index,width;
  vlPixel *data;
  vlPixel *bw_data;

  /* verify parameters */
  if ((!pic) || (!bw_pic) || (!window)) {
    VL_ERROR ("vlDrawBinary: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  /* check image formats and compatibility */
  if( (pic->format != RGB) || (bw_pic->format != BINARY) ||
      (pic->width != bw_pic->width) || (pic->height != bw_pic->height) ){
    VL_ERROR ("vlDrawBinary: invalid image format or image size\n");
    return (-1);		/* failure */
  }

  /* shortcuts */
  width = pic->width;
  x1 = window->x;
  x2 = VL_MIN(x1 + (window->width),width);
  y1 = window->y;
  y2 = VL_MIN(y1 + (window->height),pic->height);
  data = pic->pixel;
  bw_data = bw_pic->pixel;

  /* merge !! */
  for (j=y1; j<y2; j++)
    for (i=x1; i<x2; i++){
      index = (j*width+i);
      if(bw_data[index] > 0){
	index *= VL_RGB_PIXEL;
	data[index++] = r;
	data[index++] = g;
	data[index] = b;
      }
    }

  return (0);			/* success */
}

/* draw a centered calbration grid on an image */
int vlDrawCenteredGrid (vlImage *pic)
{
  int i;
  int x1,y1;
  int index1,width,height;
  vlPixel *data;
  int red,green;

  /* verify parameters */
  if (!pic) {
    VL_ERROR ("vlDrawRect: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  /* make sure we have a RGB picture */
  if (pic->format != RGB) {
    VL_ERROR ("vlDrawRect: error: image is not RGB\n");
    return (-1);		/* failure */
  }

  /* temp variables to speed up conversion */
  width = pic->width;
  height = pic ->height;
  data = pic -> pixel;

  red=255; green=0;
  for (y1=height/2;y1<height;y1+=20)
  {
  	/* draw an horizontal line */
  	index1 = VL_RGB_PIXEL * (y1*width);
  
  	for (i=1; i<width-1; i++){
    		data[index1++] = red; 	/*red*/
    		data[index1++] = green;	/*green*/
    		data[index1++] = 0;	/*blue*/
	}
	red=0; green=255;
   }

  red=255; green=0;
  for (y1=height/2-1;y1>0;y1-=20)
  {
  	/* draw an horizontal line */
  	index1 = VL_RGB_PIXEL * (y1*width);
  
  	for (i=1; i<width-1; i++){
    		data[index1++] = red; 	/*red*/
    		data[index1++] = green;	/*green*/
    		data[index1++] = 0;		/*blue*/
	}
	red=0; green=255;
   }

  /* draw vertical lines */
  red=255; green=0;
 for (x1=width/2;x1<width;x1+=20)
 {
 	 for (i=1; i<height-1; i++){
    		index1= VL_RGB_PIXEL * (x1+i*width);
   		data[index1++] = red;			/*red*/
    		data[index1++] = green;	/*green*/
    		data[index1] = 0;		/*blue*/
  	 }
	red=0; green=255;
 }

  red=255; green=0;
 for (x1=width/2-1;x1>0;x1-=20)
 {
 	 for (i=1; i<height-1; i++){
    		index1= VL_RGB_PIXEL * (x1+i*width);
   		data[index1++] = red;			/*red*/
    		data[index1++] = green;	/*green*/
    		data[index1] = 0;		/*blue*/
  	 }
	red=0; green=255;
 }

 
   return (0);			/* success */
}



/* draw a  calbration grid on an image */
int vlDrawGrid (vlImage *pic)
{
  int i;
  int x1,y1;
  int index1,width,height;
  vlPixel *data;

  /* verify parameters */
  if (!pic) {
    VL_ERROR ("vlDrawRect: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  /* make sure we have a RGB picture */
  if (pic->format != RGB) {
    VL_ERROR ("vlDrawRect: error: image is not RGB\n");
    return (-1);		/* failure */
  }

  /* temp variables to speed up conversion */
  width = pic->width;
  height = pic ->height;

  printf ("Larghezza %d, altezza %d\n",width,height);
  data = pic -> pixel;

  for (y1=0;y1<height;y1+=10)
  {
  	/* draw an horizontal line */
  	index1 = VL_RGB_PIXEL * (y1*width);
  
  	for (i=1; i<width-1; i++){
    		data[index1++] = y1%100 ? 255 : 0; 	/*red*/
    		data[index1++] = y1%100 ? 0 : 255;	/*green*/
    		data[index1++] = 0;					/*blue*/
	}
   }

 for (x1=0;x1<width;x1+=10)
 {
 	 for (i=1; i<height-1; i++){
    		index1= VL_RGB_PIXEL * (x1+i*width);
   		data[index1++] = x1%100 ? 255 : 0;		/*red*/
    		data[index1++] = x1%100 ? 0 : 255;	/*green*/
    		data[index1] = 0;					/*blue*/
  	 }
 }

   return (0);			/* success */
}


/* -----------------------------------------------------------
   LITTLE TOOLS - proper utilities
   ----------------------------------------------------------- */


/* return the bytes per pixel value for a given image format */
int 
vlPixelSize (vlImageFormat format)
{
  switch (format) {
  case RGB:
    return (VL_RGB_PIXEL);
    break;

  case HSI:
    return (VL_HSI_PIXEL);
    break;

  case NRG: 
    return (VL_NRG_PIXEL);
    break;

  case GRAY:
    return (VL_GRAY_PIXEL);
    break;

  case BINARY:
    return (VL_BINARY_PIXEL);
    break;

  default:
    printf("vlPixelSize -> unknown image format\n");
    return (-1);
    break;
  }
}


/* qsort comparison function */
int 
vlQsortCompare (const void *x, const void *y)
{
  return (*((vlPixel *) x) < *((vlPixel *) y));
}


/* return the median of the 8 neighbors of a RGB pixel */
int 
vlMedianRgbValue(vlImage *pic,vlPoint *point,vlPoint *median)
{
  int i,j;
  int index;
  int x,y;
  int array_index = 0;
  int width,height;
  vlPixel r_array[9];
  vlPixel g_array[9];
  vlPixel b_array[9];
  vlPixel *data;

  /* verify parameters */
  if ((!pic) || (!point) || (!median)) {
    VL_ERROR ("vlMedianRgbValue: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  /* make sure we have a RGB picture */
  if (pic->format != RGB) {
    VL_ERROR ("vlMedianRgbValue: error: image is not RGB\n");
    return (-1);		/* failure */
  }

  /* shortcuts */
  width = pic->width;
  height = pic->height;
  data = pic->pixel;
  x = point->x;
  y = point->y;

  median->x=x;
  median->y=y;

  if( (x > 0) && (y > 0) &&
      (x < width-1) && (y < height-1) )
    
    /* regular case - away from borders */
    {
      for (j=y-1; j<y+2; j++)
	for (i=x-1; i<x+2; i++){
	  index = 3*(j*width+i);
	  r_array[array_index] = data[index];
	  g_array[array_index] = data[index+1];
	  b_array[array_index] = data[index+2];
	  array_index++;
	}
      
      /* sort array */
      index = 3*(y*width+x);
      
      /* red component */
      qsort(r_array, 9, sizeof(vlPixel), vlQsortCompare); 
      median->r = r_array[4];
      
      /* green component */
      qsort(g_array, 9, sizeof(vlPixel), vlQsortCompare); 
      median->g = g_array[4];

      /* blue component */
      qsort(b_array, 9, sizeof(vlPixel), vlQsortCompare); 
      median->b = b_array[4];
    }
  else
    /* all the exceptions 
       for now, just return the pixel itself */
    {
      median->r = point->r;
      median->g = point->g;
      median->b = point->b;
    }

  return (0);			/* success */
}


/* return the median of the 8 neighbors of a NRG pixel */
int 
vlMedianNrgValue(vlImage *pic,vlPoint *point,vlPoint *median)
{
  int i,j;
  int index;
  int x,y;
  int array_index = 0;
  int width,height;
  vlPixel nr_array[9];
  vlPixel ng_array[9];
  vlPixel *data;

  /* verify parameters */
  if ((!pic) || (!point) || (!median)) {
    VL_ERROR ("vlMedianNrgValue: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  /* make sure we have a normalized RG image */
  if (pic->format != NRG) {
    VL_ERROR ("vlMedianNrgValue: error: image is not NRG\n");
    return (-1);		/* failure */
  }

  /* shortcuts */
  width = pic->width;
  height = pic->height;
  data = pic->pixel;
  x = point->x;
  y = point->y;

  if( (x > 0) && (y > 0) &&
      (x < width-1) && (y < height-1) )
    
    /* regular case - away from borders */
    {
      for (j=y-1; j<y+2; j++)
	for (i=x-1; i<x+2; i++){
	  index = 2*(j*width+i);
	  nr_array[array_index] = data[index];
	  ng_array[array_index] = data[index+1];
	  array_index++;
	}
      
      /* sort array */
      index = 2*(y*width+x);
      
      /* red component */
      qsort(nr_array, 9, sizeof(vlPixel), vlQsortCompare); 
      median->nr = nr_array[4];
      
      /* green component */
      qsort(ng_array, 9, sizeof(vlPixel), vlQsortCompare); 
      median->ng = ng_array[4];

    }
  else
    /* all the exceptions 
       for now, just return the pixel itself */
    {
      index = 2*(y*width+x);
      median->nr = data[index];
      median->ng = data[index+1];
    }

  return (0);			/* success */
}


/* return the median of the 8 neighbors of a HSI pixel */
int 
vlMedianHsiValue(vlImage *pic,vlPoint *point,vlPoint *median)
{
  int i,j;
  int index;
  int x,y;
  int array_index = 0;
  int width,height;
  vlPixel h_array[9];
  vlPixel s_array[9];
  vlPixel i_array[9];
  vlPixel *data;

  /* verify parameters */
  if ((!pic) || (!point) || (!median)) {
    VL_ERROR ("vlMedianHsiValue: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  /* make sure we have a HSI picture */
  if (pic->format != HSI) {
    VL_ERROR ("vlMedianHsiValue: error: image is not HSI\n");
    return (-1);		/* failure */
  }

  /* shortcuts */
  width = pic->width;
  height = pic->height;
  data = pic->pixel;
  x = point->x;
  y = point->y;

  if( (x > 0) && (y > 0) &&
      (x < width-1) && (y < height-1) )
    
    /* regular case - away from borders */
    {
      for (j=y-1; j<y+2; j++)
	for (i=x-1; i<x+2; i++){
	  index = 3*(j*width+i);
	  h_array[array_index] = data[index];
	  s_array[array_index] = data[index+1];
	  i_array[array_index] = data[index+2];
	  array_index++;
	}
      
      /* sort array */
      index = 3*(y*width+x);
      
      /* red component */
      qsort(h_array, 9, sizeof(vlPixel), vlQsortCompare); 
      median->h = h_array[4];
      
      /* green component */
      qsort(s_array, 9, sizeof(vlPixel), vlQsortCompare); 
      median->s = s_array[4];

      /* blue component */
      qsort(i_array, 9, sizeof(vlPixel), vlQsortCompare); 
      median->i = i_array[4];
    }
  else
    /* all the exceptions 
       for now, just return the pixel itself */
    {
      median->h = point->h;
      median->s = point->s;
      median->i = point->i;
    }

  return (0);			/* success */
}



/* -----------------------------------------------------------
   FILE MANAGEMENT - ppm file format
   ----------------------------------------------------------- */

/* save image as a PPM (raw=binary) file. Currently only RGB & BINARY images
 * are supported */ 
int
vlImageSave (vlImage *pic, char *filename)
{
  int i,j;
  long int index=0;
  int width, height;
  unsigned char rgb[3];
  vlPixel *data;
  FILE *pFp;

  if (!pic) {
    VL_ERROR ("vlImageSave: error: NULL image\n");
    return (-1);		/* failure */
  }

  switch (pic->format) {
  case RGB:
  case BINARY:
  case GRAY:
    break;

  default:
    VL_ERROR ("vlImageSave: error: cannot save this format\n");
    return (-1);		/* failure */
  }

  printf ("Writing to %s\n", filename);
  if (NULL == (pFp = fopen (filename, "wb"))) {
    perror ("vlImageSave: unable to create file");
    return (-1);		/* failure */
  }

  /* shortcuts */
  width = pic->width;
  height = pic->height;
  data = pic->pixel;
  
  /* write PPM header */
  fprintf (pFp, "P6");
  unsigned char cr=0xa;
  fwrite ((void *) &cr, sizeof(unsigned char), 1, pFp);
  fprintf (pFp, "%d %d",width, height);
  fwrite ((void *) &cr, sizeof(unsigned char), 1, pFp);
  fprintf (pFp, "255");
  fwrite ((void *) &cr, sizeof(unsigned char), 1, pFp);



  /* write PPM data */
  index = 0;
  for (i=0; i<height; i++) {
    for (j=0; j<width; j++) {
      switch (pic->format) {
      case RGB:

	rgb[0] = (unsigned char) data[index++];  /* red */
	rgb[1] = (unsigned char) data[index++];  /* green */
	rgb[2] = (unsigned char) data[index++];  /* blue */
	
	break;

      case GRAY:
      case BINARY:
	rgb[0] = rgb[1] = rgb[2] = (unsigned char) data[index++];  /* red */
	break;

      default:
	break;
      }

	  
      /* not very efficient, write 3 bytes at a time */
      if (3 != fwrite ((void *) &rgb[0], sizeof(unsigned char), 3, pFp)) {
	perror ("vlImageSave: write error");
	fclose (pFp);
	return (-1);		/* failure */
      }    
    }
  }

  if (0 > fclose (pFp)) {
    perror ("vlImageSave: close error");
    return (-1);		/* failure */
  }    

  return (0);			/* success */
}


/* load a PPM (raw=binary) image file */
int
vlImageLoad (vlImage *pic, char *filename)
{
  int i,j;
  long int index;
  int width, height, maxval;
  unsigned char rgb[3];
  char header[3];
  vlPixel *data;
  FILE *pFp;

  if (!pic) {
    VL_ERROR ("vlImageLoad: error: NULL image\n");
    return (-1);
  }

  if (NULL == (pFp = fopen (filename, "rb"))) {
    perror ("vlImageLoad: unable to open file");
    return (-1);		/* failure */
  }

  /* read PPM header */
  if ((1 != fscanf (pFp, "%2s", &header[0])) ||
      (strcmp (header, "P6"))) {
    VL_ERROR ("vlImageLoad: error: cannot read as P6 format PPM\n");
    fclose (pFp);
    return (-1);		/* failure */
  }

  /* skip any lines not whitespace & comments */
  while (1) {
    /* read discard up to first non-whitespace in the line */
    do {
      i = fgetc (pFp);
    } while (isspace(i));
    
    if (isdigit (i)) {
      ungetc (i, pFp);
      break;
    }
    else {			/* discard the line */
      do {
	i = fgetc (pFp);
      } while (i != '\n');
    }
  }
  
  /* read image parameters */
  if (3 != fscanf (pFp, "%d %d %d ", &width, &height, &maxval)) {
    perror ("vlImageLoad: error: cannot read as P6 format PPM\n");
    fclose (pFp);
    return (-1);		/* failure */
  }

  if (maxval != 255) {
    VL_ERROR ("vlImageLoad: error: file does not have maxval=255\n");
    fclose (pFp);
    return (-1);		/* failure */
  } 


  /* write PPM data */
  index = 0;

  if(pic->format==GRAY||pic->format==BINARY)
  {
  	 if (0 > vlImageInit (pic, pic->format, width, height)) {
	    VL_ERROR ("vlImageLoad: error: could not initialize image\n");
	    return (-1);		/* failure */
	  }

	  /* shortcuts */
	  data = pic->pixel;

	for (i=0; i<height; i++) {
		    for (j=0; j<width; j++) {
		      /* not very efficient, read 3 bytes at a time */
		      if (3 != fread ((void *) &rgb[0], sizeof(unsigned char), 3, pFp)) {
			perror ("vlImageLoad: read error");
			fclose (pFp);	
			return (-1);		/* failure */
		      }    
		      data[index++] = (vlPixel) rgb[0]; /* red */
		  }
	}
  }
  else
  {
  	if (0 > vlImageInit (pic, RGB, width, height)) {
	    VL_ERROR ("vlImageLoad: error: could not initialize image\n");
	    return (-1);		/* failure */
	}

	  /* shortcuts */
	data = pic->pixel;
  	for (i=0; i<height; i++) {
		    for (j=0; j<width; j++) {
		      /* not very efficient, read 3 bytes at a time */
		      if (3 != fread ((void *) &rgb[0], sizeof(unsigned char), 3, pFp)) {
			perror ("vlImageLoad: read error");
			fclose (pFp);	
			return (-1);		/* failure */
		      }    
		      data[index++] = (vlPixel) rgb[0]; /* red */
		      data[index++] = (vlPixel) rgb[1]; /* green */
		      data[index++] = (vlPixel) rgb[2]; /* blue */
		  }
	}
  }
  	
  if (0 > fclose (pFp)) {
    perror ("vlImageLoad: close error");
    return (-1);		/* failure */
  }    

  return (0);			/* success */
}


/******************************************************************************
 *
 * vlSubtract --
 *	calculate the difference between two images:  dest = src1 - src2 
 *
 * INPUTS:
 *   src1	source image
 *   src2	source image
 *   window	area to difference
 *
 * OUTPUTS:
 *   dest	dest image (size & type will match src1 & src2)
 *
 * RETURNS:
 *   On success, 0 is returned. Otherwise -1.
 *
 * LIMITATIONS:
 *      src1 & src2 must be of the same type and size. If the difference
 *      is less than zero, the output pixel (in dest) will be set to zero.
 *
 *****************************************************************************/
int 
vlSubtract(vlImage *src1, vlImage *src2, vlWindow *window, vlImage *dest)
{
  int i, j, k;
  int index, width;
  int x1, x2, y1, y2;
  vlPixel *input1;
  vlPixel *input2;
  vlPixel *output;
  int pixel_size;

  /* verify parameters */
  if ((!src1) || (!src2) || (!window) || (!dest)) {
    VL_ERROR ("vlSubtract: error: one of the parameters is NULL\n");
    return (-1);		/* failure */
  }
  
  /* make sure we have matching image types */
  if (src1->format != src2->format) {
    VL_ERROR ("vlSubtract: error: src images are not the same type\n");
    return (-1);		/* failure */
  }

  /* make sure we have matching image sizes */
  if ((src1->width != src2->width) ||
      (src1->height != src2->height)) {
    VL_ERROR ("vlSubtract: error: src images are not the same size\n");
    return (-1);		/* failure */
  }

  /* initialize the new picture */
  if (0 > vlImageInit (dest, src1->format, src1->width, src1->height)) {
    VL_ERROR ("vlSubtract: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }
  
  /* temp variables to optimize memory access */
  width = src1->width;
  x1 = window->x;
  x2 = x1 + (window->width);
  y1 = window->y;
  y2 = y1 + (window->height);
  input1 = src1->pixel;
  input2 = src2->pixel;
  output = dest->pixel;
  
  /* proceed to conversion */
  pixel_size = vlPixelSize(src1->format);
  for (j=y1; j<y2; j++) {
    for (i=x1; i<x2; i++) {
      index = pixel_size * (j*width+i);

      for (k=0; k<pixel_size; k++) {
	output[index] = VL_MAX(0, input1[index] - input2[index]); 
	index++;
      }
    }
  }

  return (0);			/* success */
}


/******************************************************************************
 *
 * vlScale --
 *	scale an image
 *
 * INPUTS:
 *   src	source image
 *   scale	scaling factor (0.5=reduce to 1/4 size, 2=enlarge by 4x)
 *   dest	dest image
 *
 * RETURNS:
 *   On success, 0 is returned. Otherwise -1.
 *
 *****************************************************************************/
int
vlScale (vlImage *src, float scale, vlImage *dest)
{
  int row, col, i;
  int destWidth, destHeight, srcWidth, srcHeight;
  int destIndex, srcIndex;
  int pixel_size;
  int srcCol, rd, sr;
  vlPixel *input;
  vlPixel *output;
  
  /* verify parameters */
  if ((!src) || (scale < 0) || (!dest)) {
    VL_ERROR ("vlScale: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  srcWidth = src->width;
  srcHeight = src->height;
  destWidth = (int)(srcWidth*scale);
  destHeight = (int)(srcHeight*scale);

  /* initialize the new picture */
  if (0 > vlImageInit (dest, src->format, destWidth, destHeight)) {
    VL_ERROR ("vlScale: error: could not initialize dest image\n");
    return (-1);		/* failure */
  }

  input = src->pixel;
  output = dest->pixel;
  pixel_size = vlPixelSize(src->format);

  for (row=0; row<destHeight; row++) {/* dest */
    rd = row * destWidth;
    sr = (int) (row / scale) * srcWidth;

    for (col=0; col<destWidth; col++) { /* dest */
      destIndex = (rd + col) * pixel_size;
      srcCol = (int)(col / scale);
      srcIndex = (sr + srcCol) * pixel_size;

      /* copy the pixel */
      for (i=0; i<pixel_size; i++) {
	output[destIndex++] = input[srcIndex++];
      }
    }
  }

  return (0);			/* success */
}


/******************************************************************************
 *
 * vlObjectSave --
 *	save an object to a text file (which can be loaded using vlObjectLoad
 *
 * INPUTS:
 *   object	object to save
 *   filename	file to write
 *
 * RETURNS:
 *   On success, 0 is returned. Otherwise -1.
 *
 *****************************************************************************/
int
vlObjectSave (vlObject *object, char *filename)
{
  int i, j;
  int offset, length, fill;
  FILE *pFp;
  char *format;

  if (!object) {
    VL_ERROR ("vlObjectSave: error: NULL object\n");
    return (-1);		/* failure */
  }

  if (!(object->defined)) {
    VL_ERROR ("vlObjectSave: error: object is undefined\n");
    return (-1);		/* failure */
  }

  switch (object->format) {
  case BINARY:
    format = "binary";
    break;
  case HSI:
    format = "hsi";
    break;
  case NRG:
    format = "nrg";
    break;
  case RGB:
    format = "rgb";
    break;
  default:
    VL_ERROR ("vlObjectSave: error: unsupported object format\n");
    return (-1);		/* failure */
    break;
  }


  /*
   * create the file 
   */
  printf ("Writing to %s\n", filename);
  if (NULL == (pFp = fopen (filename, "w"))) {
    perror ("vlObjectSave: unable to create file");
    return (-1);		/* failure */
  }

  /*
   * write the file 
   */
  fprintf (pFp, "#\n");
  fprintf (pFp, "# %s -- VisLib object file\n", filename);
  fprintf (pFp, "#\n");

  fprintf (pFp, "\n# properties\n");
  fprintf (pFp, "width %d\n", object->width);
  fprintf (pFp, "height %d\n", object->height);
  fprintf (pFp, "x %d\n", object->x);
  fprintf (pFp, "y %d\n", object->y);
  fprintf (pFp, "format %s\n", format);

  fprintf (pFp, "\n# color boundaries\n");
  switch (object->format) {
  case HSI:
    fprintf (pFp, "h_min %3d # deg\n", HVAL_TO_HDEG(object->h_min));
    fprintf (pFp, "h_max %3d # deg\n", HVAL_TO_HDEG(object->h_max));
    fprintf (pFp, "s_min %3d # percent\n", SVAL_TO_SPCT(object->s_min));
    fprintf (pFp, "s_max %3d # percent\n", SVAL_TO_SPCT(object->s_max));
    fprintf (pFp, "i_min %3d # percent\n", IVAL_TO_IPCT(object->i_min));
    fprintf (pFp, "i_max %3d # percent\n", IVAL_TO_IPCT(object->i_max));
    break;
  case NRG:
    fprintf (pFp, "nr_min %3d # 0-255\n", object->nr_min);
    fprintf (pFp, "nr_max %3d # 0-255\n", object->nr_max);
    fprintf (pFp, "ng_min %3d # 0-255\n", object->ng_min);
    fprintf (pFp, "ng_max %3d # 0-255\n", object->ng_max);
    break;
  case RGB:
    fprintf (pFp, "r_min %3d # 0-255\n", object->r_min);
    fprintf (pFp, "r_max %3d # 0-255\n", object->r_max);
    fprintf (pFp, "g_min %3d # 0-255\n" ,object->g_min);
    fprintf (pFp, "g_max %3d # 0-255\n", object->g_max);
    fprintf (pFp, "b_min %3d # 0-255\n", object->b_min);
    fprintf (pFp, "b_max %3d # 0-255\n", object->b_max); 
    break;
  default:
    break;
  }

  fprintf (pFp, "\n# shape data\n");
  for (i=0; i<object->height; i++) {
    fprintf (pFp, "shape ");
    
    offset = (object->x_offset)[i];
    for (j=0; j<offset; j++) {
      fprintf (pFp, ".");
    }

    length = (object->x_length)[i];
    for (j=0; j<length; j++) {
      fprintf (pFp, "x");
    }

    fill = (object->width) - offset - length;
    for (j=0; j<fill; j++) {
      fprintf (pFp, ".");
    }

#ifdef DEBUG
    fprintf (pFp, " # %3d %3d", (object->x_offset)[i], (object->x_length)[i]);
#endif

    fprintf (pFp, "\n");
  }

  /* 
   * close the file
   */
  if (0 > fclose (pFp)) {
    perror ("vlObjectSave: close error");
    return (-1);		/* failure */
  }    

  return (0);			/* success */
}


/******************************************************************************
 *
 * vlObjectLoad --
 *	load an object from a text file
 *
 * INPUTS:
 *   object	object to load
 *   filename	file to write
 *
 * RETURNS:
 *   On success, 0 is returned. Otherwise -1.
 *
 *****************************************************************************/
int
vlObjectLoad (vlObject *object, char *filename)
{
  FILE *pFp;
  char buffer[_VL_MAXSTRLEN], key[_VL_MAXSTRLEN], value[_VL_MAXSTRLEN];
  char *bufptr;
  int i, width=-1, height=-1;
  int status=0;
  int x=0;
  int objInit = FALSE;

  /* open the file */
  if (NULL == (pFp = fopen (filename, "r"))) {
    perror ("vlObjectLoad: unable to open file");
    return (-1);		/* failure */
  }

  /* parse the file */
  while (fgets (buffer, _VL_MAXSTRLEN, pFp)) {

    /* skip leading whitespace */
    i=0;
    while (isspace (buffer[i]) && buffer[i] && (i < _VL_MAXSTRLEN)) {
      i++;
    }

    /* skip if at end of line or start of a comment */
    if ((!buffer[i]) ||	(buffer[i] == '#')) {
      continue;
    }

    /* terminate line at comment marker (if any) */
    if (NULL != (bufptr = strchr (buffer, '#'))) {
      *bufptr = (char) NULL;
    }

    /* process "key value" pairs */
    if (2 != sscanf (buffer, "%s %s", key, value)) {
      VL_ERROR ("vlObjectLoad: error: illegal parameter: \"%s\"\n", key);
      status = -1;		/* failure */
      goto close;
    }

#ifdef DEBUG
    printf ("<%s> <%s>\n", key, value);
#endif

    if (!strcmp (key, "width")) {
      width = atoi(value);
#ifdef DEBUG
      printf ("width = %d\n", width);
#endif
    }
    else if (!strcmp (key, "height")) {
      height = atoi(value);
#ifdef DEBUG
      printf ("height = %d\n", height);
#endif
    }
    else {
      /* verify that width & height have been specified */
      if ((width == -1) || (height == -1)) {
	VL_ERROR ("vlObjectLoad: error: width and height must be the first object parameters\n");
	status = -1;		/* failure */
	goto close;
      }

      /* initialize the object if needed */
      if (!objInit) {
	if (0 > vlObjectInit (object, width, height)) {
	  VL_ERROR ("vlObjectLoad: error: could not initialize object\n");
	  status = -1;		/* failure */
	  goto close;
	}
	objInit = TRUE;
      }

      if (!strcmp (key, "x")) {
	object->x = atoi (value);
#ifdef DEBUG
	printf ("x = %d\n", object->x);
#endif
      }
      else if (!strcmp (key, "y")) {
	object->y = atoi (value);
#ifdef DEBUG
	printf ("y = %d\n", object->y);
#endif
      }
      else if (!strcmp (key, "format")) {
	if (!strcmp (value, "binary")) {
	  object->format = BINARY;
	}
	else if (!strcmp (value, "hsi")) {
	  object->format = HSI;
	}
	else if (!strcmp (value, "nrg")) {
	  object->format = NRG;
	}
	else if (!strcmp (value, "rgb")) {
	  object->format = NRG;
	}
	else {
	  VL_ERROR ("vlObjectLoad: error: unsupported format: \"%s\"\n",
		    value);
	  status = -1;		/* failure */
	  goto close;
	}

#ifdef DEBUG
	printf ("format = %s\n", value);
#endif
      }
      else if (!strcmp (key, "h_min")) {
	object->h_min = HDEG_TO_HVAL(atoi (value));
#ifdef DEBUG
	printf ("h_min = %d\n", object->h_min);
#endif
      }
      else if (!strcmp (key, "h_max")) {
	object->h_max = HDEG_TO_HVAL(atoi (value));
#ifdef DEBUG
	printf ("h_max = %d\n", object->h_max);
#endif
      }
      else if (!strcmp (key, "s_min")) {
	object->s_min = SPCT_TO_SVAL(atoi (value));
#ifdef DEBUG
	printf ("s_min = %d\n", object->s_min);
#endif
      }
      else if (!strcmp (key, "s_max")) {
	object->s_max = SPCT_TO_SVAL(atoi (value));
#ifdef DEBUG
	printf ("s_max = %d\n", object->s_max);
#endif
      }
      else if (!strcmp (key, "i_min")) {
	object->i_min = IPCT_TO_IVAL(atoi (value));
#ifdef DEBUG
	printf ("i_min = %d\n", object->i_min);
#endif
      }
      else if (!strcmp (key, "i_max")) {
	object->i_max = IPCT_TO_IVAL(atoi (value));
#ifdef DEBUG
	printf ("i_max = %d\n", object->i_max);
#endif
      }
      else if (!strcmp (key, "nr_min")) {
	object->nr_min = atoi (value);
#ifdef DEBUG
	printf ("nr_min = %d\n", object->nr_min);
#endif
      }
      else if (!strcmp (key, "nr_max")) {
	object->nr_max = atoi (value);
#ifdef DEBUG
	printf ("nr_max = %d\n", object->nr_max);
#endif
      }
      else if (!strcmp (key, "ng_min")) {
	object->ng_min = atoi (value);
#ifdef DEBUG
	printf ("ng_min = %d\n", object->ng_min);
#endif
      }
      else if (!strcmp (key, "ng_max")) {
	object->ng_max = atoi (value);
#ifdef DEBUG
	printf ("ng_max = %d\n", object->ng_max);
#endif
      }
      else if (!strcmp (key, "r_min")) {
	object->r_min = atoi (value);
#ifdef DEBUG
	printf ("r_min = %d\n", object->r_min);
#endif
      }
      else if (!strcmp (key, "r_max")) {
	object->r_max = atoi (value);
#ifdef DEBUG
	printf ("r_max = %d\n", object->r_max);
#endif
      }
      else if (!strcmp (key, "g_min")) {
	object->g_min = atoi (value);
#ifdef DEBUG
	printf ("g_min = %d\n", object->g_min);
#endif
      }
      else if (!strcmp (key, "g_max")) {
	object->g_max = atoi (value);
#ifdef DEBUG
	printf ("g_max = %d\n", object->g_max);
#endif
      }
      else if (!strcmp (key, "b_min")) {
	object->b_min = atoi (value);
#ifdef DEBUG
	printf ("b_min = %d\n", object->b_min);
#endif
      }
      else if (!strcmp (key, "b_max")) {
	object->b_max = atoi (value);
#ifdef DEBUG
	printf ("b_max = %d\n", object->b_max);
#endif
      }
      else if (!strcmp (key, "shape")) {
	if (x >= height) {
	  VL_ERROR ("vlObjectLoad: error: too many shape data rows\n");
	  status = -1;		/* failure */
	  goto close;
	}

	if (strlen (value) != (unsigned) width) {
	  VL_ERROR ("vlObjectLoad: error: shape data row does not match width=%d:\n  %s\n", width, value);
	  status = -1;		/* failure */
	  goto close;
	}

	i=0;
	while ((value[i]) && (value[i] == '.')) {
	  i++;
	}
	object->x_offset[x] = i;

	while ((value[i]) && (value[i] == 'x')) {
	  i++;
	}
	object->x_length[x] = i - object->x_offset[x];

#ifdef DEBUG
	printf ("off=%d len=%d\n", object->x_offset[x], object->x_length[x]);
#endif
	x++;
      }
      else {
	VL_ERROR ("vlObjectLoad: error: unknown parameter: \"%s\"\n", key);
	status = -1;		/* failure */
	goto close;
      }
    }
  }

  object->defined = TRUE;

 close:
  /* 
   * close the file
   */
  if (0 > fclose (pFp)) {
    perror ("vlObjectLoad: close error");
    return (-1);		/* failure */
  }    

  return (status);		/* success */  
}

/******************************************************************************
october 2006
*******************************************************************************/

/******************************************************************************
 *
 * vlPixelColor --
 *	Return the colors of th pixel
 *
 * INPUTS:
 *   src image
 *   point where click
 *
 * RETURNS:
 *   vlPixel* pixel
 *
 *****************************************************************************/

vlColor vlPixelColor(vlImage *src, vlPoint *point){

  int x,y,w,index;
  vlPixel *pixel;
  vlColor color;
  pixel = src->pixel;	
  
  x=point->x;
  y=point->y;
  
  printf("X %d \n",x);
  printf("Y %d \n",y);
  
  w = src->width;

  index = VL_RGB_PIXEL * ( y * w + x);
  color.r = pixel [index++];
  color.g = pixel [index++];
  color.b = pixel [index++];
  
  return color;
	
}
