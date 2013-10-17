/** vlFormat.h          
 ** ABSTRACT: protos for image format functions
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
    
    


#ifndef __FORMAT_H__
#define __FORMAT_H__

#include "vislib.h"

int vlRgb2R (vlImage *src, vlWindow *window, vlImage *dest);
int vlRgb2G (vlImage *src, vlWindow *window, vlImage *dest);
int vlRgb2B (vlImage *src, vlWindow *window, vlImage *dest);
int vlRgb2Gray (vlImage *src, vlWindow *window, vlImage *dest);
int vlRgb2Binary (vlImage *src, int threshold, vlWindow *window, vlImage *dest);
int vlRgb2Hsi (vlImage *src, vlWindow *window, vlImage *dest);

int vlRgb2Nrg (vlImage *src, vlWindow *window, vlImage *dest);

int vlNrg2Gray (vlImage *src, vlWindow *window, vlImage *dest);

int vlGray2Binary (vlImage *src, int threshold, vlWindow *window, vlImage *dest);

int vlBinary2Rgb (vlImage *src, vlWindow *window, vlImage *dest);

int vlRgb2Binary_tol(vlImage *src, vlHSI_carl_tol_t *para, vlWindow *window, vlImage *dest);

int vlHsi2Rgb (vlImage *src, vlWindow *window, vlImage *dest);

int vlHsi2H(vlImage *src, vlWindow *window, vlImage *dest);
int vlHsi2S(vlImage *src, vlWindow *window, vlImage *dest);
int vlHsi2I(vlImage *src, vlWindow *window, vlImage *dest);



#endif /* __FORMAT_H__ */

