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


#ifndef _EMBEDCV_ECVHIST_H_
#define _EMBEDCV_ECVHIST_H_

#include <stddef.h>

#include "types.h"
#include "ecvtypes.h"



/*
 * Count the pixel values into histogram bins
 *
 * The histogram represents the probabilities of the pixel values. It is used
 * for some image processing techniques like equalization. But the main thing
 * of interest for computer vision is image segmentation. The histogram
 * indicates if pixel values are clumped. Presumably these clumps of pixel
 * values correspond to objects that can be segmented out of the image.
 *
 * Note that this function does not set the bins to zero before starting to
 * count image pixel values. This is deliberate. Multiple image histograms may
 * be added together and then averaged. I'm not sure how useful this is.
 *
 */
void ImageHistogram (Histogram_t    *outHistogram,  /* 256 bins */
                     const Image8_t *inImg);

/* create histogram of pixel distances from supplied reference pixel */
void ImageHistogramDist (Histogram_t    *outHistogram,  /* 256 bins */
                         const Image8_t *inImg,
                         const uint8_t   value);

/* 16 bit packed CbCr pixel version - useful special case */
void ImageHistogramCbCr (Histogram_t     *outCbHistogram,  /* 256 bins */
                         Histogram_t     *outCrHistogram,  /* 256 bins */
                         const Image16_t *inImg);


/*
 * create histogram of pixel distances from supplied reference pixel in CbCr
 *
 * Warning: This computes a square root for every pixel - beware.
 *          The histogram should only be computed periodically for calibration
 *          of the image segmentation map. That's the only reason why it is
 *          acceptable to use the Euclidean norm (which requires a square
 *          root). Otherwise, the one norm (sum of absolute values) would be
 *          used. However, I'm concerned about segmentation accuracy so am
 *          using the Euclidean notion of distance here as well as in the
 *          image segmentation map.
 *
 */
void ImageHistogramCbCrDist (Histogram_t     *outHistogram,  /* 361 bins */
                             const Image16_t *inImg,
                             const uint16_t   value);


/*
 * Compute histogram statistics: minimum
 *                               maximum
 *                               mean
 *                               variance
 *
 * Remember that the output values are for histogram indexes.
 *
 */
void HistogramStats (size_t *outMinIdx,        /* index of minimum value */
                     size_t *outMaxIdx,        /* index of maximum value */
                     size_t *outMeanIdx,       /* index of mean value */
                     size_t *outVariance,      /* variance */
                     const Histogram_t *inHistogram);


/*
 * Compute the median index value of the histogram
 *
 */
size_t HistogramMedian (const Histogram_t *inHistogram);


/*
 * Histogram equalize an image
 *
 * Histogram equalization tries to balance the dynamic range of pixels in an
 * image so it looks better when viewed. This is also useful to some algorithms
 * by "normalizing" differences.
 *
 */
void EqualizeImage (Image8_t          *outImg,
                    const Histogram_t *inHistogram);

/* convenient macro for equalizing an image */
#define EQUALIZEIMG( IMGNAME ) \
HISTOGRAM8( IMGNAME ## Histogram ) \
ImageHistogram( & IMGNAME ## Histogram, & IMGNAME ); \
EqualizeImage( & IMGNAME , & IMGNAME ## Histogram );


/*
 * Compute Otsu's image segmentation threshold
 *
 * When applied to an image histogram, this method tries to segment foreground
 * from background pixels. More generally, it maximizes the interclass variance
 * in a histogram. Presumably, this is optimal. In practice, it is often not.
 * Hand tuned thresholds and dual threshold approaches produce better results.
 * The advantage of Otsu's method is that it is automatic and performs
 * reasonably well. It is also relatively cheap to compute once the image
 * histogram is available.
 *
 * In general, the Otsu threshold tends to "saturate" which is to be expected
 * given that interclass variance is maximized. This can mean that the
 * generated threshold is large, causing "over segmentation".
 *
 */
size_t OtsuThreshold (const Histogram_t *inHistogram);


/*
 * Add to image segmentation map
 *
 * The map is a lookup table that indexes image pixel values to set membership
 * as a foreground or background pixel. If the pixel value is deemed to be in
 * the foreground, then it is set to the specified value. Otherwise, the pixel
 * value is set to zero.
 *
 * Note that the output map is not initialized and elements set to zero before
 * the foreground elements are set. This allows multiple clusters of pixel
 * values to correspond with an image segment.
 *
 */
void SegmentMap (uint8_t       *outMap,          /* length is 256 */
                 const uint8_t  center,
                 const size_t   threshold,
                 const uint8_t  value);

/* 16 bit packed CbCr pixel version - useful special case */
void SegmentMapCbCr (uint8_t        *outMap,     /* length is 65536 */
                     const uint16_t  center,
                     const size_t    threshold,
                     const uint8_t   value);


/*
 * Convenience macros for image segmentation map arrays
 *
 */
#define SEGMAP( NAME, LENGTH ) \
  uint8_t NAME [ LENGTH ]; \
  memset( NAME , 0, sizeof(uint8_t) * LENGTH );

#define SEGMENTMAP( NAME ) SEGMAP( NAME, 256 )
#define SEGMENTMAPCBCR( NAME ) SEGMAP( NAME, 65536 )
#define SEGMENTMAP565( NAME ) SEGMAP( NAME, 65536 )


/*
 * Segment an image
 *
 */
void SegmentImage (Image8_t       *outImg,
                   const Image8_t *inImg,
                   const uint8_t  *inMap);

/* 16 bit packed CbCr pixel version - useful special case */
void SegmentImageW (Image8_t        *outImg,
                    const Image16_t *inImg,
                    const uint8_t   *inMap);


/*
 * Split individual image segments out from a single image after segmentation
 *
 */
void SplitImageSegmentation (Image8_t       **outImg,
                             const Image8_t  *inImg);



#endif
