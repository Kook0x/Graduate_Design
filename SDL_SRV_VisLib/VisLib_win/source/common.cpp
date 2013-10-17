
#include <stdio.h>
#include <memory.h>
#include <malloc.h>

#include "vislib.h"


/******************************************************************************
 *
 * vlPointCreate --
 *	create a point data structure
 *
 * INPUTS:
 *   x	x coordinate
 *   y	y coordinate
 *
 * RETURNS:
 *   On success, a newly allocated point is returned. Otherwise NULL. 
 *   When finished, destroy using use vlPointDestroy();
 *
 *****************************************************************************/
vlPoint *
vlPointCreate (int x, int y)
{
  vlPoint *point;

  /* create the pointer and initializes its instances */
  if ((point = (vlPoint*)malloc(sizeof(vlPoint)))) {
    point->x = x;
    point->y = y;
  }
  else {
    VL_ERROR ("vlPointCreate: malloc failed");
  }

  return (point);
}


/******************************************************************************
 *
 * vlWindowCreate --
 *	create a window (rectangle) data structure
 *
 * INPUTS:
 *   x		x coordinate
 *   y		y coordinate
 *   width	window width
 *   height	window height
 *
 * RETURNS:
 *   On success, a newly allocated window is returned. Otherwise, NULL.
 *   When finished, destroy using use vlWindowDestroy();
 *
 *****************************************************************************/
vlWindow *
vlWindowCreate (int x, int y, int width, int height)
{
  vlWindow *window;

  /* create the pointer */
  if ((window =(vlWindow *) malloc(sizeof(vlWindow)))) {
    window->x = x;
    window->y = y;
    window->width = width;
    window->height = height;
  }
  else {
    VL_ERROR ("vlWindowCreate: malloc failed");
  }

  return (window);
}


/******************************************************************************
 *
 * vlImageCreate --
 *	create an image data structure. If format is NONE, only a
 *      single pixel will be allocated. For all other format, the appropriate
 *      amount of storage will be allocated.
 *
 * INPUTS:
 *   format	NONE, RGB, HSI, NRG, GRAY, BINARY
 *   width	image width
 *   height	image height
 * 
 * RETURNS:
 *   On success, a newly allocated window is returned. Otherwise, NULL.
 *   When finished, destroy using use vlImageDestroy().
 *
 *****************************************************************************/
vlImage *
vlImageCreate (vlImageFormat format, int width, int height)
{
  vlImage *image = NULL;
  int size;

  switch (format) {
  case NONE:
    size = VL_NONE_SIZE (width, height);
    break;

  case RGB:
    size = VL_RGB_SIZE (width, height);
    break;

  case HSI:
    size = VL_HSI_SIZE (width, height);
    break;

  case NRG:
    size = VL_NRG_SIZE (width, height);
    break;

  case GRAY:
    size = VL_GRAY_SIZE (width, height);
    break;

  case BINARY:
    size = VL_BINARY_SIZE (width, height);
    break;

  default:
    VL_ERROR ("vlImageCreate: unsupported image format\n");
    return (image);
  }
  
  /* initialize instances */
  if ((image = (vlImage *)malloc(sizeof(vlImage))) &&
      (image->pixel = (unsigned short *)malloc(size))) {
    image->format = format;
    image->width = width;
    image->height = height;
  }
  else {
    VL_ERROR ("vlImageCreate: malloc failed");
  }
  
  return (image);
}


/* initialize an existing image */
int
vlImageInit (vlImage *image, vlImageFormat format, int width, int height)
{
  int size;

  if (!image) {
    VL_ERROR ("vlImageInit: error: NULL image\n");
    return (-1);		/* failure */
  }

  switch (format) {
  case NONE:
    size = VL_NONE_SIZE (width, height);
    break;

  case RGB:
    size = VL_RGB_SIZE (width, height);
    break;

  case HSI:
    size = VL_HSI_SIZE (width, height);
    break;

  case NRG:
    size = VL_NRG_SIZE (width, height);
    break;

  case GRAY:
    size = VL_GRAY_SIZE (width, height);
    break;

  case BINARY:
    size = VL_BINARY_SIZE (width, height);
    break;

  default:
    VL_ERROR ("vlImageCreate: unsupported image format\n");
    return (-1);		/* failure */
  }

  image->format = format;
  image->width = width;
  image->height = height;

  VL_FREE (image->pixel);
  if (!(image->pixel = (unsigned short *)malloc(size))) {
    VL_ERROR ("vlImageInit: malloc failed");
    return (-1);		/* failure */
  }
  
  return (0);			/* success */
}


/******************************************************************************
 *
 * vlImageCopy --
 *	copy an image (from src to dest)
 *
 * INPUTS:
 *   src	source image (copy from)
 *   dest	destination image (copy to)
 *
 * RETURNS:
 *   On success, 0 is returned. Otherwise, -1.
 *
 *****************************************************************************/
int
vlImageCopy (vlImage *src, vlImage *dest)
{
  int size, width, height;

  if ((!src) || (!dest)) {
    VL_ERROR ("vlImageCopy: error: NULL image\n");
    return (-1);		/* failure */
  }

  /* free existing malloc'd data */
  VL_FREE (dest->pixel);

  /* copy */
  *dest = *src;
  
  /* duplicate malloc'd data */
  width = src->width;
  height = src->height;
  switch (src->format) {
  case NONE:
    size = VL_NONE_SIZE (width, height);
    break;

  case RGB:
    size = VL_RGB_SIZE (width, height);
    break;

  case HSI:
    size = VL_HSI_SIZE (width, height);
    break;

  case NRG:
    size = VL_NRG_SIZE (width, height);
    break;

  case GRAY:
    size = VL_GRAY_SIZE (width, height);
    break;

  case BINARY:
    size = VL_BINARY_SIZE (width, height);
    break;

  default:
    VL_ERROR ("vlImageCopy: unsupported image format\n");
    return (-1);		/* failure */
  }

  if (NULL == (dest->pixel = (unsigned short *)malloc (size))) {
    VL_ERROR ("vlImageCopy: malloc failed\n");
    return (-1);		/* failure */
  }
  memcpy(dest->pixel, src->pixel, size);

  return (0);			/* success */
}


vlMask *
vlMaskCreate (int width, int height, float *kernel)
{
  vlMask *mask;
  int r, c, index;

  if ((mask = (vlMask *)malloc (sizeof(vlMask))) &&
      (mask->kernel = (float*)malloc (width * height * sizeof(float)))) {
    mask->width = width;
    mask->height = height;

    if (kernel) {
      for (r=0; r<height; r++) {
	for (c=0; c<width; c++) {
	  index = r*width + c;
	  mask->kernel[index] = kernel[index];
	}
      }
    }
  }
  else {
    VL_ERROR ("vlMaskCreate: malloc failed");
  }
  
  return (mask);
}


/******************************************************************************
 *
 * vlObjectCreate --
 *	create an object data structure
 *
 * INPUTS:
 *   x		x coordinate
 *   y		y coordinate
 *   width	window width
 *   height	window height
 *
 * RETURNS:
 *   On success, a newly allocated object is returned. Otherwise, NULL.
 *   When finished, destroy using use vlObjectDestroy().
 *
 *****************************************************************************/
vlObject *
vlObjectCreate (int x, int y, int width, int height)
{
  int i;
  vlObject *object;
  
  /* make sure all elements are initialized */
  if ((object = (vlObject *)malloc(sizeof(vlObject))) &&
      (object->x_offset = (int *)malloc(height*sizeof(int))) &&
      (object->x_length = (int *)malloc(height*sizeof(int)))) {
    object->defined = FALSE;
    object->x = x;
    object->y = y;
    object->width = width;
    object->height = height;
    object->format = NONE;

    /* initialize last positions array */
    for(i=0; i<VL_OBJECT_NPOSITIONS; i++) {
      object->last_x[i] = -1;
      object->last_y[i] = -1;
    }
  }
  else {
    VL_ERROR ("vlObjectCreate: malloc failed");
  }

  return (object);
}


/* initialize an existing object */
int
vlObjectInit (vlObject *object, int width, int height)
{
  int size, i;

  if (!object) {
    VL_ERROR ("vlObjectInit: error: NULL object\n");
    return (-1);		/* failure */
  }

  object->defined = FALSE;
  object->x = 0;
  object->y = 0;
  object->width = width;
  object->height = height;
  object->format = NONE;
    
  size = (object->height) * sizeof(int);
  VL_FREE (object->x_offset);
  object->x_offset = (int *)malloc (size);
  VL_FREE (object->x_length);
  object->x_length = (int *)malloc (size);

  /* initialize last positions array */
  for(i=0; i<VL_OBJECT_NPOSITIONS; i++) {
    object->last_x[i] = -1;
    object->last_y[i] = -1;
  }

  return (0);			/* success */
}


/******************************************************************************
 *
 * vlObjectCopy --
 *	copy an object (from src to dest)
 *
 * INPUTS:
 *   src	source object (copy from)
 *   dest	destination object (copy to)
 *
 * RETURNS:
 *   On success, 0 is returned. Otherwise, -1.
 *
 *****************************************************************************/
int
vlObjectCopy (vlObject *src, vlObject *dest)
{
  int size;

  if ((!src) || (!dest)) {
    VL_ERROR ("vlObjectCopy: error: NULL object\n");
    return (-1);		/* failure */
  }

  /* free existing malloc'd data */
  VL_FREE (dest->x_offset);
  VL_FREE (dest->x_length);

  /* copy */
  *dest = *src;

  /* duplicate malloc'd data */
  size = (dest->height) * sizeof(int);

  dest->x_offset = (int *)malloc (size);
  memcpy ((void *) dest->x_offset, (void *) src->x_offset, size);

  dest->x_length = (int *)malloc (size);
  memcpy ((void *) dest->x_length, (void *) src->x_length, size);

  return (0);			/* success */
}


/******************************************************************************
 *
 * vlPointDestroy --
 *	free a point data structure
 *
 * INPUTS:
 *   point	to be freed
 *
 *****************************************************************************/
void 
vlPointDestroy(vlPoint *point)
{
  if (point) {
    VL_FREE (point);
  }
}


/******************************************************************************
 *
 * vlWindowDestroy  --
 *	free a window data structure
 *
 * INPUTS:
 *   window	to be freed
 *
 *****************************************************************************/
void 
vlWindowDestroy (vlWindow *window)
{
  if (window) {
    VL_FREE (window);
  }
}


/******************************************************************************
 *
 * vlImageDestroy  --
 *	free an image data structure
 *
 * INPUTS:
 *   image	to be freed
 *
 *****************************************************************************/
void 
vlImageDestroy (vlImage *image)
{
  if (image) {
    VL_FREE (image->pixel);
    VL_FREE (image);
  }
}


void 
vlMaskDestroy (vlMask *mask)
{
  if (mask) {
    VL_FREE (mask->kernel);
    VL_FREE (mask);
  }
}


/******************************************************************************
 *
 * vlObjectDestroy --
 *	free an object data structure
 *
 * INPUTS:
 *   object	to be freed
 *
 *****************************************************************************/
void 
vlObjectDestroy(vlObject *object)
{
  if (object) {
    VL_FREE(object->x_length);
    VL_FREE(object->x_offset);
    VL_FREE(object);
  }
}



