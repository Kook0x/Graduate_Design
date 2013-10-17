/** vlConvolution.h
 ** ABSTRACT: protos for convolution functions
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
    
    



#ifndef __CONVOLUTION_H__
#define __CONVOLUTION_H__

#include "vislib.h"

/* general 2D kernel convolution */
int vlConvolve (vlImage *src, vlMask *mask, vlWindow *window, vlImage *dest);

/* fast separable 2D averaging window smoothing filter  */
int vlSmooth (vlImage *src, int size, vlWindow *window, vlImage *dest);

/* slow, non-linear (moving windows like) filter */
int vlSmoothMedian (vlImage *src, int size, vlWindow *window, vlImage *dest);

#endif /* __CONVOLUTION_H__ */












