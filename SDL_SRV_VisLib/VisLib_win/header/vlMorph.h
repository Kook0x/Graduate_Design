/** vlMorph.h
 ** ABSTRACT: protos for morphological functions
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
    
    


#ifndef __MORPH_H__
#define __MORPH_H__

#include "vislib.h"

/* morphological RGB operators */
int vlRgbErode (vlImage *src, int size, vlWindow *window, vlImage *dest);
int vlRgbDilate (vlImage *src, int size, vlWindow *window, vlImage *dest);
int vlEdgeDetect (vlImage *src, int size, vlWindow *window, vlImage *dest);

/* morphological B&W operators */
int vlBinaryErode (vlImage *src, int size, vlWindow *window, vlImage *dest);
int vlBinaryDilate (vlImage *src, int size, vlWindow *window, vlImage *dest);
int vlBinaryOpen(vlImage *src,int size,int t,vlWindow *window);
int vlBinaryClose(vlImage *src,int size,int t,vlWindow *window);
//int vlDealBinPic(vlImage *src,vlHSI_carl_tol_t *para,vlImage *dest);

#endif /* __MORPH_H__ */









