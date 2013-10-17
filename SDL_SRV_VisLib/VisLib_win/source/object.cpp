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
 * FILE:     object.c
 *
 * ABSTRACT: object extraction
 *
 * REVISION HISTORY:
 *
 * $Id: object.c,v 1.3 1999/09/14 12:32:57 terry Exp $
 *
 *
 *****************************************************************************/

#include <stdio.h>
#include <malloc.h>

#include "vislib.h"

/* uncomment the following for debug messages */
/*#define DEBUG */

/****************************** private functions ****************************/
static void
_vlCheckObjectPoint (vlPixel *edge_pixel, vlPixel *nrg_pixel, 
		     int width, int height, 
		     int x, int y, vlPoint *ref, vlPoint *var, char **result);
static void
_vlCheckNrgPoint (vlPixel *pixel, int width, int height, 
		 int x, int y, vlPoint *ref, vlPoint *var, char **result);
static void
_vlCheckHsiPoint (vlPixel *pixel, int width, int height, 
		  int x, int y, vlPoint *ref, vlPoint *var, char **result);
static void
_vlCheckEdgePoint (vlPixel *pixel, int width, int height, 
		  int x, int y, char **result);

static void
_vlCheckRgbPoint (vlPixel *pixel, int width, int height, 
		  int x, int y, vlPoint *ref, vlPoint *var, char **result);


/****************************** module globals *******************************/
static int _vlObjectSizeLimit = VL_OBJECT_SIZE_LIMIT_DEFAULT;
static int _vlObjectColorUpdateStep = VL_OBJECT_COLOR_UPDATE_STEP_DEFAULT;


/******************************************************************************
 *
 * vlObjectSizeLimitSet --
 *	set the minimum size an object must be in order to be defined
 *
 * INPUTS:
 *   limit	# of pixels
 *
 * RETURNS:
 *   On success, 0 is returned. Otherwise, -1.
 *
 *****************************************************************************/
int
vlObjectSizeLimitSet (int limit)
{
  if (limit < 0) {
    VL_ERROR ("vlObjectSizeLimitSet: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  _vlObjectSizeLimit = limit;

  return (0);			/* success */
}


/******************************************************************************
 *
 * vlObjectColorUpdateStepSet --
 *	set step used to widen color boundaries (vlObjectColorUpdateStepSet)
 *
 * INPUTS:
 *   step	nrg level
 *
 * RETURNS:
 *   On success, 0 is returned. Otherwise, -1.
 *
 *****************************************************************************/int
vlObjectColorUpdateStepSet (int step)
{
  if ((step < 0) || (step > 255)) {
    VL_ERROR ("vlObjectColorUpdateStepSet: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  _vlObjectColorUpdateStep = step;

  return (0);			/* success */
}


/*****************************************************************************
 *
 * vlMatchShape --
 *	correlate based on the shape of the given object
 *      and in the given window using a binary search.
 *
 * INPUTS:
 *   pic	binary image to search
 *   object	object to search for
 *   window	region of pic to search
 *
 * RETURNS:
 *   The percentage (0-100) of confidence in the match (0 is returned if
 *   the window is empty). Otherwise, -1 is returned on error.
 *
 * LIMITATIONS:
 *   This function assumes that the object is completely contained within the
 *   search window.
 *
 *****************************************************************************/
int 
vlMatchShape (vlImage *pic, vlObject *object, vlWindow *window)
{
  /* image */
  int height, width;

  /* object */
  int objWidth, objHeight;
  int *x_offset;
  int *x_length;

  int winWidth, winHeight;	/* window of pic */
  int rectWidth, rectHeight;	/* search area in window */

  int row, col;
  int i, j, k;
  int X1, X2, Y1, Y2;
  int colStart, colEnd;
  int count, sum;
  int maxSum, maxX, maxY;

  vlPixel *pixel;


  /* verify parameters */
  if ((!pic) || (!object) || (!window)) {
    VL_ERROR ("vlMatchShape: error: illegal parameter\n");
    return (-1);		/* failure */
  }
  
  /* make sure we are given a binary image */
  if (pic->format != BINARY) {
    VL_ERROR ("vlMatchShape: error: src image is not BINARY\n");
    return (-2);		/* failure */
  }

  /* convenience references */
  height = pic->height;
  width = pic->width;

  objHeight = object->height;
  objWidth = object->width;
  x_offset = object->x_offset;
  x_length = object->x_length;

  winWidth = window->width;
  winHeight = window->height;

  /* compute correlation area */
  rectWidth = winWidth - objWidth;	
  rectHeight = winHeight - objHeight;
 
  /* make sure we are not at the edge of the image */
  if ((rectWidth <= 0) || (rectHeight <= 0)) {
    VL_ERROR ("vlMatchShape: error: at edge of image\n");
    return (-3);		/* failure */
  }

  X1 = window->x;
  Y1 = window->y;
  X2 = X1 + rectWidth;
  Y2 = Y1 + rectHeight;

  /*
   * correlation search
   */
#ifdef DEBUG
  printf ("searching: rows=%d to %d, cols=%d to %d\n", Y1, Y2, X1, X2);
  printf ("objHeight=%d objWidth=%d\n", objHeight, objWidth);
#endif

  /* search in the correlation area */
  maxSum = maxX = maxY = 0;
  for (row=Y1; row<Y2; row++) {	/* rows */
    for (col=X1; col<X2; col++) { /* cols */
      sum = 0;

      for (j=row; j<(row+objHeight); j++) {
	k = j-row;
	colStart = col + x_offset[k];
	colEnd = colStart + x_length[k];

	/* the following is only valid for binary (one component) images */
	pixel = pic->pixel + j*width + colStart;

	for (i=colStart; i<colEnd; i++) {
	  if (*pixel++) {
	    sum++;
	  }
	}
      }
      
      if (sum > maxSum) {
	maxSum = sum;
	maxX = col;
	maxY = row;
      }
    }
  }

  /* update coordinates */
  object->x = maxX;
  object->y = maxY;

  /* compute max possible count (i.e., # of pixels in object) */
  count = 0;
  for (i=0; i<objHeight; i++) {
    count += x_length[i];
  }


  if(count==0)
  {
	  vlImageSave(pic, "zero.ppm");
	  return 0;
  }

#ifdef DEBUG
  printf ("maxY=%d maxX=%d sum=%d count=%d\n", maxY, maxX, maxSum, count);
#endif
  if (maxSum > count) {
    maxSum = count;
  }
  return (100 * maxSum / count);
}



/*****************************************************************************
 *
 * vlCorrelation
 *	Calcola la correlazione di 2 immagini e ne restituisce la percentuale 
 *
 * INPUTS:
 *	picture_1
 *	picture_2
 *
 * RETURNS:
 *	The percentage (0-100) of confidence in the match (0 is returned if
 * 	the window is empty). Otherwise, -1 is returned on error.
 *****************************************************************************/

int vlCorrelation(vlImage *pic, vlImage *pic2){

 /* image */
 int height, width;

  int row, col,sum, sumMax;
  vlPixel *pixel, *pixel2;
 
/* verify parameters */
  if ((!pic) || (!pic2)) {
    VL_ERROR ("vlCorrelation: error: illegal parameter\n");
    return (-1);	/* failure */
  }

  pixel = pic->pixel;
  pixel2 = pic2->pixel;

  height = pic->height;
  width = pic->width;

 
  sum = 0;
  sumMax = 0;
  	
 
  for (row = 0; row<height; row++){
  	/* ciclo sulle righe */

    for (col = 0; col<width; col++){

    	/* ciclo sulle colonne */
	sumMax = sumMax + *pixel * *pixel; /* correlazione massima */
	sum = sum + *pixel * *pixel2;
	pixel++;
	pixel2++;
	/*if (row == 0 && col == 0){
	  	printf("sum = %d \n",sum);
	 	printf("sumMax = %d \n",sumMax);
	}*/
    }
  }
  printf("\n");

  return (int)(sum/(sumMax/100.0));
}

/*****************************************************************************
 *
 * vlObjectColorUpdate --
 *	dynamically update/adapt the color boundaries of an object
 *
 * INPUTS:
 *   nrg_pic
 *   bin_pic
 *   object
 *
 * RETURNS:
 *   On success, 0 is returned. Otherwise, -1.
 *
 *****************************************************************************/
int
vlObjectColorUpdate (vlImage *pic, vlImage *bin_pic, vlObject *object)
{
  int i, j, k;
  int colStart, colEnd;
  int width;
  int x1, x2, y1, y2;
  vlPixel *pixel;
  vlPixel *bin_pixel;
  int *x_offset;
  int *x_length;

  /* verify parameters */
  if ((!pic) || (!bin_pic) || (!object)) {
    VL_ERROR ("vlObjectColorUpdate: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  if (bin_pic->format != BINARY) {
    VL_ERROR ("vlObjectColorUpdate: invalid image format (BINARY expected)\n");
    return (-1);		/* failure */
  }

  if ((pic->width != bin_pic->width) ||
      (pic->height != bin_pic->height)) {
    VL_ERROR ("vlObjectColorUpdate: incompatible input images size\n");
    return (-1);		/* failure */
  }

  if (object->format != pic->format) {
    VL_ERROR ("vlObjectColorUpdate: image & object format do not match\n");
    return (-1);		/* failure */
  }

  /* shortcuts */
  width = pic->width;
  x1 = object->x;
  x2 = x1 + (object->width);
  y1 = object->y;
  y2 = y1 + (object->height);
  x_offset = object->x_offset;
  x_length = object->x_length;
  pixel = pic->pixel;

  switch (object->format) {
  case NRG:
    {
      vlPixel nr_min = object->nr_min;
      vlPixel nr_max = object->nr_max;
      vlPixel ng_min = object->ng_min;
      vlPixel ng_max = object->ng_max;

      vlPixel nr_max_actual=0, nr_min_actual=255;
      vlPixel ng_max_actual=0, ng_min_actual=255;
  
      /* for each line */
      for (j=y1; j<y2; j++) {
	k = j-y1;
	colStart = x1+x_offset[k];
	colEnd = colStart + x_length[k];
	
	/* the following is only valid for binary (one component) images */
	bin_pixel = bin_pic->pixel + (j*width) + colStart;
	
	for (i=colStart; i<colEnd; i++) {
	  if (*bin_pixel++) {
	    int index = VL_NRG_PIXEL * (j*width+i);
	    int nr = pixel[index];
	    int ng = pixel[index+1];
	    
	    /* update thresholds */
	    nr_max_actual = VL_MAX (nr, nr_max_actual);
	    nr_min_actual = VL_MIN (nr, nr_min_actual);
	    ng_max_actual = VL_MAX (ng, ng_max_actual);
	    ng_min_actual = VL_MIN (ng, ng_min_actual);
	  }
	}
      }
  
      /*
       * update object color boundaries
       */
      /* normalized red upper threshold */
      if (nr_max == nr_max_actual) 
	object->nr_max += _vlObjectColorUpdateStep;
      else 
	object->nr_max = nr_max_actual - _vlObjectColorUpdateStep;
      
      /* normalized green upper threshold */
      if (ng_max == ng_max_actual) 
	object->ng_max += _vlObjectColorUpdateStep;
      else 
	object->ng_max = ng_max_actual - _vlObjectColorUpdateStep;
      
      /* normalized red lower threshold */
      if (nr_min == nr_min_actual) 
	object->nr_min -= _vlObjectColorUpdateStep;
      else 
	object->nr_min = nr_min_actual + _vlObjectColorUpdateStep;
      
      /* normalized green lower threshold */
      if (ng_min == ng_min_actual) 
	object->ng_min -= _vlObjectColorUpdateStep;
      else 
	object->ng_min = ng_min_actual + _vlObjectColorUpdateStep;
    }
    break;

  case HSI:
    {
      vlPixel h_min = object->h_min;
      vlPixel h_max = object->h_max;
      vlPixel s_min = object->s_min;
      vlPixel s_max = object->s_max;
      vlPixel i_min = object->i_min;
      vlPixel i_max = object->i_max;

      vlPixel h_max_actual=0, h_min_actual=255;
      vlPixel s_max_actual=0, s_min_actual=255;
      vlPixel i_max_actual=0, i_min_actual=255;
  
      /* for each line */
      for (j=y1; j<y2; j++) {
	k = j-y1;
	colStart = x1+x_offset[k];
	colEnd = colStart + x_length[k];
	
	/* the following is only valid for binary (one component) images */
	bin_pixel = bin_pic->pixel + (j*width) + colStart;
	
	for (i=colStart; i<colEnd; i++) {
	  if (*bin_pixel++) {
	    int index = VL_HSI_PIXEL * (j*width+i);
	    int h = pixel[index];
	    int s = pixel[index+1];
	    int i = pixel[index+2];
	    
	    /* update thresholds */
	    h_max_actual = VL_MAX (h, h_max_actual);
	    h_min_actual = VL_MIN (h, h_min_actual);
	    s_max_actual = VL_MAX (s, s_max_actual);
	    s_min_actual = VL_MIN (s, s_min_actual);
	    i_max_actual = VL_MAX (i, i_max_actual);
	    i_min_actual = VL_MIN (i, i_min_actual);
	  }
	}
      }
  
      /*
       * update object color boundaries
       */
      /* hue upper threshold */
      if (h_max == h_max_actual) 
	object->h_max += HDEG_TO_HVAL(_vlObjectColorUpdateStep);
      else 
	object->h_max = h_max_actual - HDEG_TO_HVAL(_vlObjectColorUpdateStep);

      /* hue lower threshold */
      if (h_min == h_min_actual) 
	object->h_min -= HDEG_TO_HVAL(_vlObjectColorUpdateStep);
      else 
	object->h_min = h_min_actual + HDEG_TO_HVAL(_vlObjectColorUpdateStep);
      
      /* saturation upper threshold */
      if (s_max == s_max_actual) 
	object->s_max += SPCT_TO_SVAL(_vlObjectColorUpdateStep);
      else 
	object->s_max = s_max_actual - SPCT_TO_SVAL(_vlObjectColorUpdateStep);

      /* saturation lower threshold */
      if (s_min == s_min_actual) 
	object->s_min -= SPCT_TO_SVAL(_vlObjectColorUpdateStep);
      else 
	object->s_min = s_min_actual + SPCT_TO_SVAL(_vlObjectColorUpdateStep);

      /* intensity upper threshold */
      if (i_max == i_max_actual) 
	object->i_max += IPCT_TO_IVAL(_vlObjectColorUpdateStep);
      else 
	object->i_max = i_max_actual - IPCT_TO_IVAL(_vlObjectColorUpdateStep);

      /* intensity lower threshold */
      if (i_min == i_min_actual) 
	object->i_min -= IPCT_TO_IVAL(_vlObjectColorUpdateStep);
      else 
	object->i_min = i_min_actual + IPCT_TO_IVAL(_vlObjectColorUpdateStep);
    }
    break;

  default:
    VL_ERROR ("vlObjectColorUpdate: not implemented\n");
    return (-1);		/* failure */
  }
  
  return (0);			/* success */
}


/*****************************************************************************
 *
 * vlObjectShapeUpdate --
 *	dynamically update/adapt the shape of an object
 *
 * INPUTS:
 *   pic
 *   bin_pic
 *   window
 *   object
 *
 * RETURNS:
 *   On success, 0 is returned. Otherwise, -1 if object is lost or error.
 *
 *****************************************************************************/
int 
vlObjectShapeUpdate (vlImage *pic, vlImage *bin_pic, 
		     vlWindow *window, vlObject *object)
{
  int i, j, k=0;
  int x1, x2, y1, y2;
  int new_x1, new_x2, new_y1, new_y2, new_height, new_width;
  int first, last;
  int width;
  vlPixel *bin_pixel;

  /* verify parameters */
  if ((!pic) || (!bin_pic) || (!window) || (!object)) {
    VL_ERROR ("vlObjectShapeUpdate: error: illegal parameter\n");
    return (-1);		/* failure */
  }


  /* check formats */
  if (bin_pic->format != BINARY) {
    VL_ERROR ("vlObjectShapeUpdate: invalid image format (BINARY expected)\n");
    return (-2);		/* failure */
  }

  /* shortcuts */
  width = bin_pic->width;
  x1 = window->x;
  x2 = x1 + (window->width);
  y1 = window->y;
  y2 = y1 + (window->height);
  bin_pixel = bin_pic->pixel;

  /* process binary array */
  new_x1 = new_y1 = 1000;	/* arbitrary big nummber... */
  new_x2 = new_y2 = 0;
  //在window 内检测object，给出其范围
  for (j=y1; j<y2; j++) {
    for (i=x1; i<x2; i++) {
      if (bin_pixel[j*width+i] > 0) {
	if (new_x1 > i) new_x1 = i;
	if (new_y1 > j) new_y1 = j;
	if (new_x2 < i) new_x2 = i;
	if (new_y2 < j) new_y2 = j;
      }
    }
  }
  //新object 的width 和height
  new_height = new_y2 - new_y1;
  new_width = new_x2 - new_x1;

  /* check whether what we found is good enough to be used */
  if ((new_width < _vlObjectSizeLimit) && (new_height < _vlObjectSizeLimit)) {
    /* object too small, refuse ! */
    return -3;
  }
  
  /* update object */
  object->x = new_x1;
  object->y = new_y1;
  object->width = new_width;
  object->height = new_height;

  //重新分配形状数据
  VL_FREE (object->x_offset); 
  object->x_offset = (int *)malloc (new_height * sizeof(int));
  VL_FREE (object->x_length); 
  object->x_length = (int *)malloc (new_height * sizeof(int));

  /* extract exact object shape in the X direction */
  /*for (j=new_y1; j<new_y2; j++) {
   	i=new_x1;
	while(bin_pixel[j*width+i]==0)
		i++;
	object->x_offset[j-new_y1]= i;
	k=new_width-i-1;
	while(bin_pixel[j*width+k]==0)
		k--;
	object->x_length[j-new_y1]= k-i+1;
  }*/
    /* extract exact object shape in the X direction */
  for (j=new_y1; j<new_y2; j++) {
    first = last = -1;
	
    for (i=new_x1; i<new_x2; i++) {
		if (bin_pixel[j*width+i] > 0) {	
			/* update object boundaries */
			if (first == -1) {
			  first = i;
			}
			else {
			  last = i;
			}
		}
    }   
	object->x_offset[j-new_y1] = first - new_x1;
    object->x_length[j-new_y1] = last - first;

	  if(first - new_x1<0||last - first<0)
		  k++;

  }

  printf ("redefined object %dx%d at (x=%d, y=%d)\n", 
	  object->width, object->height, object->x, object->y);
  
  return (0);			/* success */
}


/*****************************************************************************
 *
 * vlObjectMotionUpdate --
 *	dynamically update/adapt the object speed estimate 
 *      using previous positions to estimate next position.
 *
 * INPUTS:
 *   object
 *
 * RETURNS:
 *   On success, 0 is returned. Otherwise, -1 on error.
 *
 *****************************************************************************/
int
vlObjectMotionUpdate (vlObject *object)
{
  int i;
  int *last_x;
  int *last_y;
  int dx, dy;

  /* verify parameters */
  if ((!object)) {
    VL_ERROR ("vlObjectMotionUpdate: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  /* shortcuts */
  last_x = object->last_x;
  last_y = object->last_y;

  /* initialization case */
  if (last_x[0] == -1) {
    object->next_x = last_x[2] = last_x[1] = last_x[0] = object->x;
    object->next_y = last_y[2] = last_y[1] = last_y[0] = object->y;
    return (0);			/* success */
  }

  /* update position array */
  for (i=VL_OBJECT_NPOSITIONS-1; i>0; i--) {
    last_x[i] = last_x[i-1];
    last_y[i] = last_y[i-1];
  }
  last_x[0] = object->x;	/* current x */
  last_y[0] = object->y;	/* current y */

  /* compute dx, dy */
  dx = last_x[0] - last_x[1];
  dy = last_y[0] - last_y[1];

  /* finally, compute next position estimate */
  object->next_x = last_x[0] + dx/2;
  object->next_y = last_y[0] + dy/2;

  return (0);			/* success */
}


/******************************************************************************
 *
 * vlObjectExtract --
 *	define an object via region growth limited by normalized color limits
 *      and edge detection
 *
 * INPUTS:
 *   toleranceNR	+/- nr value
 *   toleranceNG	+/- ng value
 *
 * RETURNS:
 *   On success (object found and defined), 0 is returned. 
 *   Otherwise, -1 (no growth achieved).
 *
 *****************************************************************************/
int 
vlObjectExtract (vlImage *pic, vlPoint *point, 
		 int toleranceNR, int toleranceNG,
		 vlObject *object)
{
  int i, j;
  int index;
  int width, height;
  int x, y;
  int x1, x2, y1, y2;
  int first, last;
  vlPixel nr_max, nr_min;
  vlPixel ng_max, ng_min; 
  vlPoint *ref;
  vlPoint *var;
  char **result;
  vlImage *edge_pic;
  vlImage *nrg_pic;
  vlPixel *edge_pixel;
  vlPixel *nrg_pixel;
  vlWindow *window;

  /* verify parameters */
  if ((!pic) || (!point) || 
      (toleranceNR < 0) || (toleranceNG < 0) ||
      (!object)) {
    VL_ERROR ("vlObjectExtract: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  /* make sure we receive a RGB image */
  if (pic->format != RGB) {
    VL_ERROR ("vlObjectExtract: invalid image format (NRG expected)\n");
    return (-1);		/* failure */
  }
  
  /* extract useful data */
  x = point->x;
  y = point->y;
  width = pic->width;
  height = pic->height;
  
  /* create the edge and nrg images */
  window = vlWindowCreate (0, 0, width, height);
  edge_pic = VL_IMAGE_CREATE();
  nrg_pic = VL_IMAGE_CREATE();
  vlEdgeDetect (pic, VL_EDGE_DETECT_DEFAULT, window, nrg_pic);
  vlRgb2Binary (nrg_pic, VL_EDGE_THRESHOLD_DEFAULT, window, edge_pic);
  edge_pixel = edge_pic->pixel;
  vlRgb2Nrg (pic, window, nrg_pic);
  nrg_pixel = nrg_pic->pixel;

  /* create a result array to store accepted pixels 
     1 stands for accepted, 0 for refused and -1 for unchecked */
  result = (char **)malloc (width*sizeof (char *));
  for (i=0; i<width; i++) {
    result[i] =(char *) malloc (height);
    for (j=0; j<height; j++) {
      result[i][j] = -1;
    }
  }  

  /* create initial reference value */
  ref = vlPointCreate (0, 0);
  vlMedianNrgValue (nrg_pic, point, ref);

#ifdef DEBUG
  printf ("nominal value : %d, %d\n", 
	  nrg_pixel[2* (y*width+x)], nrg_pixel[2* (y*width+x)+1]);
  printf ("median  value : %d, %d\n", ref->nr, ref->ng);
#endif

  /* create NRG variation tolerance */
  var = vlPointCreate (0, 0);
  var->nr = toleranceNR;
  var->ng = toleranceNG;

  /* launch the recursive call */
  _vlCheckObjectPoint (edge_pixel, nrg_pixel, width, height, x, y, 
		       ref, var, result);

  /* process result array */
  x1 = y1 = 1000;
  x2 = y2 = 0;
  for (j=0; j<height; j++) {
    for (i=0; i<width; i++) {
      if (result[i][j] > 0) {
	if (x1 > i) x1 = i;
	if (y1 > j) y1 = j;
	if (x2 < i) x2 = i;
	if (y2 < j) y2 = j;
      }
    }
  }

  /* initialize object */
  if (0 > vlObjectInit (object, x2-x1, y2-y1)) {
    VL_ERROR ("vlObjectExtract: error: could not initialize object\n");
  }
  object->x = x1;
  object->y = y1;
  object->format = NRG;

#ifdef DEBUG
  printf ("found object %dx%d at (x=%d, y=%d)\n", 
	  object->width, object->height, object->x, object->y);
#endif

  /* extract exact object shape in the X direction 
     and actual color boundaries */
  nr_max = ng_max = 0;
  nr_min = ng_min = 255;
  for (j=y1; j<y2; j++) {
    first = last = -1;
    for (i=x1; i<x2; i++) {
      if (result[i][j] > 0) {
	/* update actual color boundaries */
	index = VL_NRG_PIXEL * (j*width+i);
	nr_max = VL_MAX (nrg_pixel[index], nr_max);
	nr_min = VL_MIN (nrg_pixel[index], nr_min);
	ng_max = VL_MAX (nrg_pixel[index+1], ng_max);
	ng_min = VL_MIN (nrg_pixel[index+1], ng_min);
	
	/* update object boundaries */
	if (first == -1) {
	  first = i;
	}
	else {
	  last = i;
	}
      }
      object->x_offset[j-y1] = first - x1;
      object->x_length[j-y1] = last - first;
    }
  }
  
  /* update object color boundaries */
  object->nr_max = nr_max;
  object->nr_min = nr_min;
  object->ng_max = ng_max;
  object->ng_min = ng_min; 
#ifdef DEBUG
  printf ("color boundaries (min, max): norm_red (%d, %d), norm_green (%d, %d)\n",
	  object->nr_min, object->nr_max, object->ng_min, object->ng_max);
#endif  

  /* get rid of temp images */
  vlImageDestroy (edge_pic);
  vlImageDestroy (nrg_pic);
  
  /* get rid of result array */
  for (i=0; i<width; i++) {
    VL_FREE (result[i]);
  }
  VL_FREE (result);

  vlWindowDestroy (window);
  vlPointDestroy (ref);
  vlPointDestroy (var);

  /* check whether the object is large enough */
  if ((object->width < _vlObjectSizeLimit) && 
      (object->height < _vlObjectSizeLimit)) {
    return (-1);		/* failure */
  }

  object->defined = TRUE;
  return (0);			/* success */
}

/* check if pixel (x, y) is within the var interval around the ref values 
   called recursively by each pixel on its neighbours */
static void
_vlCheckObjectPoint (vlPixel *edge_pixel, vlPixel *nrg_pixel, 
		     int width, int height, 
		     int x, int y, vlPoint *ref, vlPoint *var, char **result)
{
  int index;
  int nr, ng;
  int nr_ref, ng_ref;
  int nr_max, nr_min;
  int ng_max, ng_min;

  /* first make sure point hasn't been checked already 
     and is within the image size */
  if ((x < 0) || (x >= width-1) ||
      (y < 0) || (y >= height-1) || (result[x][y] > (char) -1)) {
    return;
  }

  /* then check if we reached an edge */
  if (edge_pixel[y*width+x] > 0) {
    result[x][y] = 0;		/* mark pixels */
    return;
  }

  /* if not, extract useful data to test NRG values */
  index = 2* (y*width+x);
  nr = nrg_pixel[index];
  ng = nrg_pixel[index+1];
  nr_ref = ref->nr;
  ng_ref = ref->ng;
  nr_max = nr_ref + (var->nr);
  nr_min = nr_ref - (var->nr);
  ng_max = ng_ref + (var->ng);
  ng_min = ng_ref - (var->ng);

  /* check if point is valid */
  if ((nr < nr_min) || (nr > nr_max) ||
      (ng < ng_min) || (ng > ng_max)) {
    result[x][y] = 0;		/* mark pixels */
    return;
  }
  else {   /* mark pixels in result and check surrounding points */
    result[x][y] = 1;

    /* check surrounding pixels */
    _vlCheckObjectPoint (edge_pixel, nrg_pixel, width, height, 
			 x-1, y, ref, var, result);
    _vlCheckObjectPoint (edge_pixel, nrg_pixel, width, height, 
			 x+1, y, ref, var, result);
    _vlCheckObjectPoint (edge_pixel, nrg_pixel, width, height, 
			 x, y-1, ref, var, result);
    _vlCheckObjectPoint (edge_pixel, nrg_pixel, width, height, 
			 x, y+1, ref, var, result);
  }
}


/******************************************************************************
 *
 * vlObjectExtractNrg --
 *	define an object via region growth limited by normalized color limits
 *
 * INPUTS:
 *   toleranceNR	+/- nr value
 *   toleranceNG	+/- ng value
 *
 * RETURNS:
 *   On success (object found and defined), 0 is returned. 
 *   Otherwise, -1 (no growth achieved).
 *
 *****************************************************************************/
int 
vlObjectExtractNrg (vlImage *pic, vlPoint *point, 
		    int toleranceNR, int toleranceNG, 
		    vlObject *object)
{
  int i, j;
  int width, height;
  int x, y;
  int x1, x2, y1, y2;
  int first, last;
  vlPoint *ref;
  vlPoint *var;
  char **result;
  vlPixel *data;

  if ((!pic) || (!point) || 
      (toleranceNR < 0) || (toleranceNG < 0) ||
      (!object)) {
    VL_ERROR ("vlObjectExtractNrg: error: illegal parameter\n");
    return (-1);		/* failure */
  }
  
  /* make sure we receive a normalized RG image */
  if (pic->format != NRG) {
    VL_ERROR ("vlObjectExtractNrg: invalid image format (NRG expected)\n");
    return (-1);		/* failure */
  }
  
  /* extract useful data */
  x = point->x;
  y = point->y;
  width = pic->width;
  height = pic->height;
  data = pic->pixel;

  /* create a result array to store accepted pixels 
     1 stands for accepted, 0 for refused and -1 for unchecked */
  result = (char **)malloc (width*sizeof (char *));
  for (i=0; i<width; i++) {
    result[i] = (char *)malloc (height*sizeof (char));
    for (j=0; j<height; j++)
      result[i][j] = -1;
  }  

  /* create initial reference value */
  ref = vlPointCreate (0, 0);
  vlMedianNrgValue (pic, point, ref);

  /* create variation tolerance */
  var = vlPointCreate (0, 0);
  var->nr = toleranceNR;
  var->ng = toleranceNG;

  /* launch the recursive call */
  _vlCheckNrgPoint (data, width, height, x, y, ref, var, result);

  /* process result array */
  x1 = y1 = 1000;
  x2 = y2 = 0;
  for (j=0; j<height; j++) {
    for (i=0; i<width; i++) {
      if (result[i][j] > 0) {
	if (x1 > i) x1 = i;	/* left */
	if (y1 > j) y1 = j;	/* top */
	if (x2 < i) x2 = i;	/* right */
	if (y2 < j) y2 = j;	/* bottom */
      }
    }
  }
  
  /* check result size */
  if (x1 == 1000) x1 = 0;
  if (y1 == 1000) y1 = 0;

  /* initialize object */
  if (0 > vlObjectInit (object, x2-x1, y2-y1)) {
    VL_ERROR ("vlObjectExtractNrg: error: could not initialize object\n");
  }
  object->x = x1;
  object->y = y1;
  object->format = NRG;

#ifdef DEBUG
  printf ("found object %dx%d at (x=%d, y=%d)\n", 
	  object->width, object->height, object->x, object->y);
#endif

  /* update object color boundaries */
  object->nr_max = VL_MIN ((ref->nr) + (var->nr), 255);
  object->nr_min = VL_MAX ((ref->nr) - (var->nr), 0);
  object->ng_max = VL_MIN ((ref->ng) + (var->ng), 255);
  object->ng_min = VL_MAX ((ref->ng) - (var->ng), 0);
#ifdef DEBUG
  printf ("color boundaries (min, max): norm_red (%d, %d), norm_green (%d, %d)\n",
	  object->nr_min, object->nr_max, object->ng_min, object->ng_max);
#endif  

  /* extract exact object shape in the X direction */
  for (j=y1; j<y2; j++) {
    first = last = -1;
    for (i=x1; i<=x2; i++) {
      if (result[i][j] > 0) {
	if (first == -1) {
	  first = i;
	}
	else {
	  last = i;
	}
      }
    }
    object->x_offset[j-y1] = first - x1;
    object->x_length[j-y1] = last - first;
  }

  /* get rid of result array */
  for (i=0; i<width; i++)
    VL_FREE (result[i]);
  VL_FREE (result);

  vlPointDestroy (ref);
  vlPointDestroy (var);

  /* check whether the object is large enough */
  if ((object->width < _vlObjectSizeLimit) && 
      (object->height < _vlObjectSizeLimit)) {
    return (-1);		/* failure */
  }

  object->defined = TRUE;  
  return (0);			/* success */
}

/* check if pixel (x, y) is within the var interval around the ref values 
   called recursively by each pixel on its neighbours */
static void
_vlCheckNrgPoint (vlPixel *pixel, int width, int height, 
		 int x, int y, vlPoint *ref, vlPoint *var, char **result)
{
  int index;
  int nr, ng;
  int nr_ref, ng_ref;
  int nr_max, nr_min;
  int ng_max, ng_min;
  
  /* first make sure point hasn't been checked already 
     and is within the image size */
  if ((x < 0) || (x >= width) ||
      (y < 0) || (y >= height) || (result[x][y] > (char) -1)) {
    return;
  }

  /* if not, extract useful data */
  index = VL_NRG_PIXEL * (y*width+x);
  nr = pixel[index];
  ng = pixel[index+1];
  nr_ref = ref->nr;
  ng_ref = ref->ng;
  nr_max = nr_ref + (var->nr);
  nr_min = nr_ref - (var->nr);
  ng_max = ng_ref + (var->ng);
  ng_min = ng_ref - (var->ng);

  /* check if point is valid */
  if ((nr < nr_min) || (nr > nr_max) ||
      (ng < ng_min) || (ng > ng_max)) {
      result[x][y] = 0;		/* mark pixels */
      return;
  }

  /* if yes, mark pixels in result and check surrounding points */
  else {
    result[x][y] = 1;
      
    /* check surrounding pixels */
    _vlCheckNrgPoint (pixel, width, height, x-1, y, ref, var, result);
    _vlCheckNrgPoint (pixel, width, height, x+1, y, ref, var, result);
    _vlCheckNrgPoint (pixel, width, height, x, y-1, ref, var, result);
    _vlCheckNrgPoint (pixel, width, height, x, y+1, ref, var, result);
  }
}


/******************************************************************************
 *
 * vlObjectExtractHsi --
 *	define an object via region growth limited by hsi tolerance
 *
 * INPUTS:
 *   toleranceHue	+/- deg
 *   toleranceSat	+/- percent
 *   toleranceInt	+/- percent
 *
 * RETURNS:
 *   On success (object found and defined), 0 is returned. 
 *   Otherwise, -1 (no growth achieved).
 *
 *****************************************************************************/
int 
vlObjectExtractHsi (vlImage *pic, vlPoint *point, 
		    int toleranceHue, int toleranceSat, int toleranceInt,
		    vlObject *object)
{
  int i, j;
  int width, height;
  int x, y;
  int first, last;
  int x1, x2, y1, y2;
  vlPoint *ref;
  vlPoint *var;
  char **result;
  vlPixel *data;

  /* verify parameters */
  if ((!pic) || (!point) || 
      (toleranceHue < 0) || (toleranceHue > 180) || 
      (toleranceSat < 0) || (toleranceSat > 50) || 
      (toleranceInt < 0) || (toleranceInt > 50) || 
      (!object)) {
    VL_ERROR ("vlObjectExtractHsi: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  /* make sure we receive a HSI image */  
  if (pic->format != HSI) {
    VL_ERROR ("vlObjectExtractHsi: invalid image format (HSI expected)\n");
    return (-1);		/* failure */
  }
  
  /* extract useful data */
  x = point->x;
  y = point->y;
  width = pic->width;
  height = pic->height;
  data = pic->pixel;


  /* create a result array to store accepted pixels 
     1 stands for accepted, 0 for refused and -1 for unchecked */
  result = (char **)malloc (width*sizeof (char *));
  for (i=0; i<width; i++) {
    result[i] = (char *)malloc (height);
    for (j=0; j<height; j++) {
      result[i][j] = -1;
    }
  }  

  /* create initial reference value */
  ref = vlPointCreate (0, 0);
  vlMedianHsiValue (pic, point, ref);

  /* create variation tolerance */
  var = vlPointCreate (0, 0);
  var->h = HDEG_TO_HVAL(toleranceHue);
  var->s = SPCT_TO_SVAL(toleranceSat);
  var->i = IPCT_TO_IVAL(toleranceInt);

  /* launch the recursive call */
  _vlCheckHsiPoint (data, width, height, x, y, ref, var, result);

  /* process result array */
  x1 = y1 = 1000;
  x2 = y2 = 0;
  for (j=0; j<height; j++)
    for (i=0; i<width; i++) {
      if (result[i][j] > 0) {
	if (x1 > i) x1 = i;	/* left */
	if (y1 > j) y1 = j;	/* top */
	if (x2 < i) x2 = i;	/* right */
	if (y2 < j) y2 = j;	/* bottom */
      }
    }

  /* check result size */
  if (x1 == 1000) x1 = 0;
  if (y1 == 1000) y1 = 0;

#ifdef DEBUG
  printf ("x1=%d x2=%d y1=%d y2=%d\n", x1, x2, y1, y2);
#endif
  
  /* initialize object */
  if (0 > vlObjectInit (object, x2-x1, y2-y1)) {
    VL_ERROR ("vlObjectExtractHsi: error: could not initialize object\n");
  }
  object->x = x1;
  object->y = y1;
  object->format = HSI;

#ifdef DEBUG
  printf ("found object %dx%d at (x=%d, y=%d)\n", 
	  object->width, object->height, object->x, object->y);
#endif

  /* update object color boundaries */
  object->h_max = VL_MIN ((ref->h) + (var->h), VL_PIXEL_MAXVAL);
  object->h_min = VL_MAX ((ref->h) - (var->h), 0);
  object->s_max = VL_MIN ((ref->s) + (var->s), VL_PIXEL_MAXVAL);
  object->s_min = VL_MAX ((ref->s) - (var->s), 0);
  object->i_max = VL_MIN ((ref->i) + (var->i), VL_PIXEL_MAXVAL);
  object->i_min = VL_MAX ((ref->i) - (var->i), 0);
#ifdef DEBUG
  printf ("color boundaries (min, max): H (%d, %d), S (%d, %d), I (%d, %d)\n",
	  HVAL_TO_HDEG(object->h_min), HVAL_TO_HDEG(object->h_max), 
	  SVAL_TO_SPCT(object->s_min), SVAL_TO_SPCT(object->s_max), 
	  IVAL_TO_IPCT(object->i_min), IVAL_TO_IPCT(object->i_max));
#endif

  /* extract exact object shape in the X direction */
  for (j=y1; j<y2; j++) {
    first = last = -1;
    for (i=x1; i<=x2; i++) {
      if (result[i][j] > 0) {
	if (first == -1) {
	  first = i;
	}
	else {
	  last = i;
	}
      }
    }
    object->x_offset[j-y1] = first - x1;
    object->x_length[j-y1] = last - first;
  }

  /* get rid of result array */
  for (i=0; i<width; i++)
    VL_FREE (result[i]);
  VL_FREE (result);

  vlPointDestroy (ref);
  vlPointDestroy (var);

  /* check whether the object is large enough */
  if ((object->width < _vlObjectSizeLimit) && 
      (object->height < _vlObjectSizeLimit)) {
    return (-1);		/* failure */
  }

  object->defined = TRUE;  
  return (0);			/* success */
}

/* check if pixel (x, y) is within the var interval around the ref values */
static void
_vlCheckHsiPoint (vlPixel *pixel, int width, int height, 
		  int x, int y, vlPoint *ref, vlPoint *var, char **result)
{
  int index;
  int h, s, i;
  int h_ref, s_ref, i_ref;
  int h_max, h_min;
  int s_max, s_min;
  int i_max, i_min;
  
  /* first make sure point hasn't been checked already 
     and is within the image size */
  if ((x < 0) || (x >= width) ||
      (y < 0) || (y >= height) || (result[x][y] > (char) -1)) {
    return;
  }

  /* if not, extract useful data */
  index = 3* (y*width+x);
  h = pixel[index];
  s = pixel[index+1];
  i = pixel[index+2];
  h_ref = ref->h;
  s_ref = ref->s;
  i_ref = ref->i;
  h_max = h_ref + (var->h);
  h_min = h_ref - (var->h);
  s_max = s_ref + (var->s);
  s_min = s_ref - (var->s);
  i_max = i_ref + (var->i);
  i_min = i_ref - (var->i);

  /* check if point is valid */
  if ((h < h_min) || (h > h_max) ||
      (s < s_min) || (s > s_max) ||
      (i < i_min) || (i > i_max)
      ) {
    /* if not, mark pixels and return -1 */
    result[x][y] = 0;
    return;
  }
  /* if yes, mark pixels in result and check surrounding points */
  else {
    result[x][y] = 1;
    
    /* check surrounding pixels */
    _vlCheckHsiPoint (pixel, width, height, x-1, y, ref, var, result);
    _vlCheckHsiPoint (pixel, width, height, x+1, y, ref, var, result);
    _vlCheckHsiPoint (pixel, width, height, x, y-1, ref, var, result);
    _vlCheckHsiPoint (pixel, width, height, x, y+1, ref, var, result);
  }
}


/******************************************************************************
 *
 * vlObjectExtractEdge --
 *	define an object via region growth limited by edge detection
 *
 * RETURNS:
 *   On success (object found and defined), 0 is returned. 
 *   Otherwise, -1 (no growth achieved).
 *
 *****************************************************************************/
/* initiate a region growth based on a recursive call */
int 
vlObjectExtractEdge (vlImage *pic, vlPoint *point, vlObject *object)
{
  int i, j;
  int width, height;
  int x, y, first, last;
  int x1, x2, y1, y2;
  char **result;
  vlPixel *data;

  /* verify parameters */
  if ((!pic) || (!point) || (!object)) {
    VL_ERROR ("vlObjectExtractEdge: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  /* make sure we receive a binary image */
  if (pic->format != BINARY) {
    VL_ERROR ("vlObjectExtractEdge: invalid image format (BINARY expected)\n");
    return (-1);		/* failure */
  }
  
  /* extract useful data and create edge map */
  x = point->x;
  y = point->y;
  width = pic->width;
  height = pic->height;
  data = pic->pixel;

  /* create a result array to store accepted pixels 
     1 stands for accepted, 0 for refused and -1 for unchecked */
  result = (char **)malloc (width*sizeof (char *));
  for (i=0; i<width; i++) {
    result[i] = (char *)malloc (height);
    for (j=0; j<height; j++)
      result[i][j] = -1;
  }  

  /* launch the recursive call */
  _vlCheckEdgePoint (data, width, height, x, y, result);

  /* process result array */
  x1 = y1 = 1000;
  x2 = y2 = 0;
  for (j=0; j<height; j++)
    for (i=0; i<width; i++) {
      if (result[i][j] > 0) {
	if (x1 > i) x1 = i;	/* left */
	if (y1 > j) y1 = j;	/* top */
	if (x2 < i) x2 = i;	/* right */
	if (y2 < j) y2 = j;	/* bottom */
      }
    }

  /* check result size */
  if (x1 == 1000) x1 = 0;
  if (y1 == 1000) y1 = 0;

  /* initialize object */
  if (0 > vlObjectInit (object, x2-x1, y2-y1)) {
    VL_ERROR ("vlObjectExtractEdge: error: could not initialize object\n");
  }
  object->x = x1;
  object->y = y1;
  object->format = BINARY;

#ifdef DEBUG
  printf ("found object %dx%d at (x=%d, y=%d)\n", 
	  object->width, object->height, object->x, object->y);
#endif
  
  /* extract exact object shape in the X direction */
  for (j=y1; j<y2; j++) {
    first = last = -1;
    for (i=x1; i<=x2; i++) {
      if (result[i][j] > 0) {
	if (first == -1) {
	  first = i;
	}
	else {
	  last = i;
	}
      }
    }
    object->x_offset[j-y1] = first - x1;
    object->x_length[j-y1] = last - first;
  }

  /* get rid of result array */
  VL_FREE (result);

  /* check whether the object is large enough */
  if ((object->width < _vlObjectSizeLimit) && 
      (object->height < _vlObjectSizeLimit)) {
    return (-1);		/* failure */
  }

  object->defined = TRUE;  
  return (0);			/* success */
}


/* check if pixel (x, y) is within the var interval around the ref values */
static void
_vlCheckEdgePoint (vlPixel *pixel, int width, int height, 
		  int x, int y, char **result)
{
  int index;
  
  /* first make sure point hasn't been checked already 
     and is within the image size */
  if ((x < 2) || (x >= width-2) ||
      (y < 2) || (y >= height-2) || (result[x][y] > (char) -1)) {
    return;
  }
  
  /* if not, extract useful data */
  index = (y*width+x);
  
  /* check if point is valid */
  if (pixel[index] > 0) {

    /* if not, mark pixels and return -1 */
    result[x][y] = 0;
    return;
  }
  /* if yes, mark pixels in result and check surrounding points */
  else {
    result[x][y] = 1;
    
    /* check surrounding pixels */
    _vlCheckEdgePoint (pixel, width, height, x-1, y, result);
    _vlCheckEdgePoint (pixel, width, height, x+1, y, result);
    _vlCheckEdgePoint (pixel, width, height, x, y-1, result);
    _vlCheckEdgePoint (pixel, width, height, x, y+1, result);
  }
}

/* blah */


/******************************************************************************
 *
 * vlObjectExtractRgb --
 *	define an object via region growth limited by rgb tolerance
 *
 * INPUTS:
 *   toleranceR		+/- R val
 *   toleranceG		+/- G val
 *   toleranceB		+/- Bval
 *
 * RETURNS:
 *   On success (object found and defined), 0 is returned. 
 *   Otherwise, -1 (no growth achieved).
 *
 *****************************************************************************/
int 
vlObjectExtractRgb (vlImage *pic, vlPoint *point, 
		    int toleranceR, int toleranceG, int toleranceB,
		    vlObject *object)
{
  int i, j;
  int width, height;
  int x, y;
  int first, last;
  int x1, x2, y1, y2;
  vlPoint *ref;
  vlPoint *var;
  char **result;
  vlPixel *data;

  /* verify parameters */
  if ((!pic) || (!point) || 
      (toleranceR < 0) || (toleranceR > 127) || 
      (toleranceG < 0) || (toleranceG > 127) || 
      (toleranceB < 0) || (toleranceB > 127) || 
      (!object)) {
    VL_ERROR ("vlObjectExtractRgb: error: illegal parameter\n");
    return (-1);		/* failure */
  }

  /* make sure we receive a RGB image */  
  if (pic->format != RGB) {
    VL_ERROR ("vlObjectExtractRgb: invalid image format (RGB expected)\n");
    return (-1);		/* failure */
  }
  
  /* extract useful data */
  x = point->x;
  y = point->y;
  width = pic->width;
  height = pic->height;
  data = pic->pixel;


  /* create a result array to store accepted pixels 
     1 stands for accepted, 0 for refused and -1 for unchecked */
  result = (char **)malloc (width*sizeof (char *));
  for (i=0; i<width; i++) {
    result[i] = (char *)malloc (height);
    for (j=0; j<height; j++) {
      result[i][j] = -1;
    }
  }  

  /* create initial reference value */
  ref = vlPointCreate (0, 0);
  vlMedianRgbValue (pic, point, ref);

  /* create variation tolerance */
  var = vlPointCreate (0, 0);
  var->r = toleranceR;
  var->g = toleranceG;
  var->b = toleranceB;

  /* launch the recursive call */
  _vlCheckRgbPoint (data, width, height, x, y, ref, var, result);

  /* process result array */
  x1 = y1 = 1000;
  x2 = y2 = 0;
  for (j=0; j<height; j++)
    for (i=0; i<width; i++) {
      if (result[i][j] > 0) {
	if (x1 > i) x1 = i;	/* left */
	if (y1 > j) y1 = j;	/* top */
	if (x2 < i) x2 = i;	/* right */
	if (y2 < j) y2 = j;	/* bottom */
      }
    }

  /* check result size */
  if (x1 == 1000) x1 = 0;
  if (y1 == 1000) y1 = 0;

#ifdef DEBUG
  printf ("x1=%d x2=%d y1=%d y2=%d\n", x1, x2, y1, y2);
#endif
  
  /* initialize object */
  if (0 > vlObjectInit (object, x2-x1, y2-y1)) {
    VL_ERROR ("vlObjectExtractRgb: error: could not initialize object\n");
  }
  object->x = x1;
  object->y = y1;
  object->format = RGB;

#ifdef DEBUG
  printf ("found object %dx%d at (x=%d, y=%d)\n", 
	  object->width, object->height, object->x, object->y);
#endif

  /* update object color boundaries */
  object->r_max = VL_MIN ((ref->r) + (var->r), 255);
  object->r_min = VL_MAX ((ref->r) - (var->r), 0);
  object->g_max = VL_MIN ((ref->g) + (var->g), 255);
  object->g_min = VL_MAX ((ref->g) - (var->g), 0);
  object->b_max = VL_MIN ((ref->b) + (var->b), 255);
  object->b_min = VL_MAX ((ref->b) - (var->b), 0);
#ifdef DEBUG
  printf ("color boundaries (min, max): R (%d, %d), G (%d, %d), B (%d, %d)\n",
	  object->r_min, object->r_max, 
	  object->g_min, object->g_max, 
	  object->b_min, object->b_max);
#endif

  /* extract exact object shape in the X direction */
  for (j=y1; j<y2; j++) {
    first = last = -1;
    for (i=x1; i<=x2; i++) {
      if (result[i][j] > 0) {
	if (first == -1) {
	  first = i;
	}
	else {
	  last = i;
	}
      }
    }
    object->x_offset[j-y1] = first - x1;
    object->x_length[j-y1] = last - first;
  }

  /* get rid of result array */
  for (i=0; i<width; i++)
    VL_FREE (result[i]);
  VL_FREE (result);

  vlPointDestroy (ref);
  vlPointDestroy (var);

  /* check whether the object is large enough */
  if ((object->width < _vlObjectSizeLimit) && 
      (object->height < _vlObjectSizeLimit)) {
    return (-1);		/* failure */
  }

  object->defined = TRUE;  
  return (0);			/* success */
}

/* check if pixel (x, y) is within the var interval around the ref values */
static void
_vlCheckRgbPoint (vlPixel *pixel, int width, int height, 
		  int x, int y, vlPoint *ref, vlPoint *var, char **result)
{
  int index;
  int r, g, b;
  int r_ref, g_ref, b_ref;
  int r_max, r_min;
  int g_max, g_min;
  int b_max, b_min;
  
  /* first make sure point hasn't been checked already 
     and is within the image size */
  if ((x < 0) || (x >= width) ||
      (y < 0) || (y >= height) || (result[x][y] > (char) -1)) {
    return;
  }

  /* if not, extract useful data */
  index = 3* (y*width+x);
  r = pixel[index];
  g = pixel[index+1];
  b = pixel[index+2];
  r_ref = ref->r;
  g_ref = ref->g;
  b_ref = ref->b;
  r_max = r_ref + (var->r);
  r_min = r_ref - (var->r);
  g_max = g_ref + (var->g);
  g_min = g_ref - (var->g);
  b_max = b_ref + (var->b);
  b_min = b_ref - (var->b);

  /* check if point is valid */
  if ((r < r_min) || (r > r_max) ||
      (g < g_min) || (g > g_max) ||
      (b < b_min) || (b > b_max)
      ) {
    /* if not, mark pixels and return -1 */
    result[x][y] = 0;
    return;
  }
  /* if yes, mark pixels in result and check surrounding points */
  else {
    result[x][y] = 1;
    
    /* check surrounding pixels */
    _vlCheckRgbPoint (pixel, width, height, x-1, y, ref, var, result);
    _vlCheckRgbPoint (pixel, width, height, x+1, y, ref, var, result);
    _vlCheckRgbPoint (pixel, width, height, x, y-1, ref, var, result);
    _vlCheckRgbPoint (pixel, width, height, x, y+1, ref, var, result);
  }
}


int getobjectcolorhsi (vlHSI_carl_tol_t *objectcolor)
{
  return 0;
 }


int
vlBinary(vlImage *src, vlHSI_carl_tol_t *para,vlImage * dest)
{
  int i, j;
  int index, width;
  int x1, x2, y1, y2;
  int h_max, h_min;
  int s_max, s_min;
  int i_max, i_min;
  vlImage *src1;
  vlPixel *input;
  vlPixel *output;
  vlPoint *point1;
  vlWindow *window;
  
  src1 = VL_IMAGE_CREATE ();
  point1 = vlPointCreate (0, 0);
  point1->h=para->h;
  point1->s=para->s;
  point1->i=para->i;
  
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
    window = vlWindowCreate (0, 0, src->width, src->height);
    vlRgb2Hsi(src, window, src1);

    if (src1->format != HSI) {
      VL_ERROR ("vlHsi2Binary: src1 image is not HSI\n");
      return (-1);		/* failure */
    }

   /*printf("width %d height %d",src1->width, src1->height);*/
   width = src1->width;
   x1 = window->x;
   x2 = x1 + (window->width);
   y1 = window->y;
   y2 = y1 + (window->height);  
   input = src1->pixel;
   output = dest->pixel;
   h_max = point1->h+HDEG_TO_HVAL(para->h_tol);
   h_min = point1->h-HDEG_TO_HVAL(para->h_tol);
   s_max = point1->s+SPCT_TO_SVAL(para->s_tol);
   s_min = point1->s-SPCT_TO_SVAL(para->s_tol);
   i_max = point1->i+IPCT_TO_IVAL(para->i_tol);
   i_min = point1->i-IPCT_TO_IVAL(para->i_tol);

   /*
   printf("\n h_max%d,  h_min %d",h_max,h_min);
   printf("\n s_max%d,  s_min %d",s_max,s_min);
   printf("\n i_max%d,  i_min %d",i_max,i_min);
   */

   /* proceed to conversion */
   for (j=y1; j<y2; j++) {
     for (i=x1; i<x2; i++) {
       index = VL_HSI_PIXEL * (j*width + i);
       if ((input[index]>h_max)
	  	||(input[index]<h_min) 
	  	||(input[index+1]>s_max)
	  	||(input[index+1]<s_min)
	  	||(input[index+2]>i_max)
	  	||(input[index+2]<i_min))
	  	{
	          output[index/VL_HSI_PIXEL] = 255;
             }
       else {
	   	output[index/VL_HSI_PIXEL] = 0;
              }
      }
   }

   vlImageDestroy(src1);
   vlPointDestroy(point1);
   vlWindowDestroy(window);
   
   return (0);	
 }


