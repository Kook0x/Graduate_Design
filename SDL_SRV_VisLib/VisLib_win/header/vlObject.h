/** vlObject.h
 ** ABSTRACT: protos for object functions
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
    
    


#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "vislib.h"


/* set processing parameters */
int vlObjectSizeLimitSet (int limit);
int vlObjectColorUpdateStepSet (int step);

/* shape based correlation */
int vlMatchShape (vlImage *image, vlObject *object, vlWindow *window);

/*
 * object properties update 
 */
int vlObjectColorUpdate (vlImage *nrgImage, vlImage *binImage, 
			 vlObject *object);
int vlObjectShapeUpdate (vlImage *image, vlImage *binImage, vlWindow *window,
			 vlObject *object);
int vlObjectMotionUpdate (vlObject *object);
int vlCorrelation(vlImage *pic, vlImage *pic2);

/*
 * object extraction routines
 */

/* region growth (combined edge detection and normalized color blob) */
int vlObjectExtract (vlImage *image, vlPoint *point, 
		     int toleranceNR, int toleranceNG, 
		     vlObject *object);

/* region growth (normalized color blob) */
int vlObjectExtractNrg (vlImage *image, vlPoint *point, 
			int toleranceNR, int toleranceNG,
			vlObject *object);

/* region growth (hsi blob) */
int vlObjectExtractHsi (vlImage *pic, vlPoint *point, 
			int toleranceHue, int toleranceSat, int toleranceInt,
			vlObject *object);

/* region growth (edge detection) */
int vlObjectExtractEdge (vlImage *image, vlPoint *point, vlObject *object);

/* region growth (rgb blob) */
int vlObjectExtractRgb (vlImage *pic, vlPoint *point, 
			int toleranceR, int toleranceG, int toleranceB,
			vlObject *object);

int getobjectcolorhsi (vlHSI_carl_tol_t *objectcolor);

int vlBinary(vlImage *src, vlHSI_carl_tol_t *para,vlImage * dest);


#endif /* __OBJECT_H__ */







