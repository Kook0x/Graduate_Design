/** vlCommon.h
 ** ABSTRACT: protos for basic VisLib data structure functions
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

#ifndef __COMMON_H__
#define __COMMON_H__

#include "vislib.h"

/* convenience macros for creating defaults 
 * (if only C had method overloading...)
 */
#define VL_POINT_CREATE() vlPointCreate(0,0)
#define VL_WINDOW_CREATE() vlWindowCreate(0,0,VL_COLS_DEFAULT,VL_ROWS_DEFAULT)
#define VL_WINDOW_CREATE_GRANDE() vlWindowCreate(0,0,VL_COLS_DEFAULT_GRANDE,VL_ROWS_DEFAULT_GRANDE)
#define VL_IMAGE_CREATE() vlImageCreate(NONE,0,0)
#define VL_IMAGE_CREATE_GRANDE() vlImageCreate(NONE,640,480)
#define VL_MASK_CREATE() vlMaskCreate(0,0,NULL)
#define VL_OBJECT_CREATE() vlObjectCreate(0,0,1,1)

/* create */
vlPoint *vlPointCreate (int x, int y);
vlWindow *vlWindowCreate (int x, int y, int width, int height);
vlImage *vlImageCreate (vlImageFormat format, int width, int height);
vlMask *vlMaskCreate (int width, int height, float *kernel);
vlObject *vlObjectCreate (int x, int y, int width, int height);

/* destroy */
void vlPointDestroy (vlPoint *point);
void vlWindowDestroy (vlWindow *window);
void vlImageDestroy (vlImage *image);
void vlMaskDestroy (vlMask *mask);
void vlObjectDestroy (vlObject *object);

/* convenience routines */
/* initialize existing (previously defined via vl...Create) */
int vlImageInit (vlImage *image, vlImageFormat format, int width, int height);
int vlObjectInit (vlObject *object, int width, int height);

/* copy from src->dest */
int vlImageCopy (vlImage *src, vlImage *dest);
int vlObjectCopy (vlObject *src, vlObject *dest);


#endif /* __COMMON_H__ */
