/*
 * EmbedCV - an embeddable computer vision library
 *
 * Copyright (C) 2006  Chris Jang
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 *
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 *
 * Email the author: cjang@ix.netcom.com
 *
 */


#ifndef _EMBEDCV_ECVOPS_H_
#define _EMBEDCV_ECVOPS_H_


#include <stddef.h>

#include "types.h"
#include "ecvtypes.h"



/*
 * Arithmetic macros for images
 */
#define OPIMGTOIMG( OP, A, B, C ) \
{ \
  const size_t count = A .width * A .height; \
  size_t i; \
  for (i = 0; i < count; i++) \
  { \
    A .data[i] = B .data[i] OP C .data[i]; \
  } \
}

#define OPIMGTOSCALAR( OP, A, B, C ) \
{ \
  const size_t count = A .width * A .height; \
  size_t i; \
  for (i = 0; i < count; i++) \
  { \
    A .data[i] = B .data[i] OP C; \
  } \
}

#define ADDIMAGES( A, B, C ) OPIMGTOIMG( +, A, B, C )
#define SUBIMAGES( A, B, C ) OPIMGTOIMG( -, A, B, C )
#define MULIMAGES( A, B, C ) OPIMGTOIMG( *, A, B, C )
#define DIVIMAGES( A, B, C ) OPIMGTOIMG( /, A, B, C )

#define ADDIMGSCALAR( A, B, C ) OPIMGTOSCALAR( +, A, B, C )
#define SUBIMGSCALAR( A, B, C ) OPIMGTOSCALAR( -, A, B, C )
#define MULIMGSCALAR( A, B, C ) OPIMGTOSCALAR( *, A, B, C )
#define DIVIMGSCALAR( A, B, C ) OPIMGTOSCALAR( /, A, B, C )


/*
 * Sobel edge detection
 *
 */
void SobelEdges (Image16_t      *outImgX,  /* same dimensions as inImg */
                 Image16_t      *outImgY,  /* same dimensions as inImg */
                 const Image8_t *inImg);


/*
 * Compute magnitude image of gradient from two edge images
 *
 */
void EdgeImagesTo1Norm (Image8_t        *outImg,
                        const Image16_t *inImgEdgeX,
                        const Image16_t *inImgEdgeY,
                        const size_t     shift);  /* right shift rescale */


/* Euclidean norm - warning, uses square root for every pixel */
void EdgeImagesTo2Norm (Image8_t        *outImg,
                        const Image16_t *inImgEdgeX,
                        const Image16_t *inImgEdgeY,
                        const size_t     shift);  /* right shift rescale */

/* Square of the Euclidean norm */
void EdgeImagesToSS (Image8_t        *outImg,
                     const Image16_t *inImgEdgeX,
                     const Image16_t *inImgEdgeY,
                     const size_t     shift);     /* right shift rescale */


/*
 * Binary image morphology operations (erosion and dilation)
 *
 * This is region shrinking (erosion) and growing (dilation). When used in
 * combination, small regions of noise can be removed with opening (erosion
 * followed by dilation). Holes in a region can be filled with closing
 * (dilation followed by erosion).
 *
 */

/* horizontal region change */
void RegionErode31 (Image8_t  *inoutImg, const uint8_t mark);
void RegionErode51 (Image8_t  *inoutImg, const uint8_t mark);
void RegionDilate31 (Image8_t *inoutImg, const uint8_t mark);
void RegionDilate51 (Image8_t *inoutImg, const uint8_t mark);
/* vertical region change */
void RegionErode13 (Image8_t  *inoutImg, const uint8_t mark);
void RegionErode15 (Image8_t  *inoutImg, const uint8_t mark);
void RegionDilate13 (Image8_t *inoutImg, const uint8_t mark);
void RegionDilate15 (Image8_t *inoutImg, const uint8_t mark);
/* square structuring element */
void RegionErode33 (Image8_t  *inoutImg, const uint8_t mark);
void RegionErode55 (Image8_t  *inoutImg, const uint8_t mark);
void RegionDilate33 (Image8_t *inoutImg, const uint8_t mark);
void RegionDilate55 (Image8_t *inoutImg, const uint8_t mark);


/*
 * Binomial averaged image sequence
 *
 */
void BinAvgImageSeq (Image8_t       *inoutImg,
                     const Image8_t *inImg);


/*
 * Difference between two images
 *
 */
void DiffImages (Image8_t       *outImg,
                 const Image8_t *inImg1,
                 const Image8_t *inImg2,
                 const uint8_t  *inMap);  /* segmentation map */


/*
 * Blur an image using a 3x3 box window average
 *
 */
void BlurImage33 (Image8_t       *outImg,
                  const Image8_t *inImg);


/*
 * Blur an image using a modified 3x3 box window average
 *
 * The center pixel is not counted which allows for a faster average
 * calculation with bit shifting.
 *
 */
void BlurImage33Fast (Image8_t       *outImg,
                      const Image8_t *inImg);


/*
 * Integral image transform (as used by Viola and Jones)
 *
 */
void IntegralImage (Image32_t      *outImg,
                    const Image8_t *inImg);


/*
 * Integral image feature of two boxes, one above the other
 * This detects vertically oriented features.
 *
 */
void IntegralFeatureUpDown (Image32_t       *outImg,
                            const Image32_t *inImg,    /* the integral image */
                            const size_t     boxWidth,
                            const size_t     boxHeight,
                            const size_t     colStep,
                            const size_t     rowStep);


/*
 * Integral image feature of two boxes, one left and the other right
 * This detects horizontally oriented features.
 *
 */
void IntegralFeatureLeftRight (Image32_t       *outImg,
                               const Image32_t *inImg, /* the integral image */
                               const size_t     boxWidth,
                               const size_t     boxHeight,
                               const size_t     colStep,
                               const size_t     rowStep);


#endif
