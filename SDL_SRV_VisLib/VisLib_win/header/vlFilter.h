/** vlFilter.h
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
    
    

#ifndef __FILTER_H__
#define __FILTER_H__

#include "vislib.h"

/* image filtering with various attributes */
int vlRgbFilter (vlImage *src, vlObject *object, vlWindow *window, 
		 vlImage *dest);
int vlNrgFilter (vlImage *src, vlObject *object, vlWindow *window, 
		 vlImage *dest);
int vlHsiFilter (vlImage *src, vlObject *object, vlWindow *window,
		 vlImage *dest);

#endif /* __FILTER_H__ */

