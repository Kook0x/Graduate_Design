/** vlMotion.h
 ** ABSTRACT: defs & function protos
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
    
    


#ifndef __MOTION_H__
#define __MOTION_H__

#include "vislib.h"

/* initialization routines */
int vlMotionInit (void);
int vlMotionInitGrande (void);
int vlMotionShutdown (void);
int vlMotionThresholdSet (int threshold);

/* motion detection prototypes */
int vlMotionUpdate (vlImage *pic);
int vlMotionUpdate2 (vlImage *pic);

/* motion extraction prototypes */
int vlMotionRgbImage (vlImage *src, vlWindow *window, vlImage *dest);
int vlMotionRgbArea (vlImage *src, vlWindow *window, vlImage *dest);
int vlMotionBinaryImage (vlImage *src, vlWindow *window, vlImage *dest);
int vlMotionBinaryArea (vlImage *src, vlWindow *window, vlImage *dest);

#endif /* __MOTION_H__ */





