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


#ifndef _EMBEDCV_ECVMANIP_H_
#define _EMBEDCV_ECVMANIP_H_


#include <stddef.h>

#include "types.h"
#include "ecvtypes.h"



/*
 * Crop out subimage
 *
 * The subimage is the full size of the output image. There is no bounds
 * checking of the column and row coordinates in the larger input image so be
 * careful.
 *
 * If a region of an image is determined to be of interest, then cropping it
 * out for more intensive processing makes sense.
 *
 */
void CropImage (Image8_t       *outImg,    /* smaller destination image */
                const Image8_t *inImg,     /* larger source image */
                const size_t    inColumn,  /* position in source image */
                const size_t    inRow);

/* 16 bit word version */
void CropImageW (Image16_t       *outImg,    /* smaller destination image */
                 const Image16_t *inImg,     /* larger source image */
                 const size_t     inColumn,  /* position in source image */
                 const size_t     inRow);


/*
 * Paste in subimage
 *
 * The subimage is placed on top of the larger output image. There is no bounds
 * checking of the column and row coordinates so be careful.
 *
 * This probably has no realistic use for computer vision per se. It is useful
 * for debugging and visualization during development. Multiple images that
 * indicate different aspects of what the vision algorithms are doing can be
 * pasted together and saved for later analysis.
 *
 */
void PasteImage (Image8_t       *outImg,    /* larger destination image */
                 const Image8_t *inImg,     /* smaller source image */
                 const size_t    outColumn, /* position in destination image */
                 const size_t    outRow);


/*
 * Downsample an image
 *
 * The hope is that statistical properties of the smaller downsampled image
 * are approximately the same as the full size image. Less computation is then
 * required for similar results. Note that this is not the same as pixel
 * binning. Downsampling is picking samples, not averaging neighbors together.
 *
 */
void DownsampleImage (Image8_t       *outImg,  /* smaller destination image */
                      const Image8_t *inImg);  /* larger source image */

/* 16 bit word version */
void DownsampleImageW (Image16_t       *outImg, /* smaller destination image */
                       const Image16_t *inImg); /* larger source image */


/*
 * Upsample an image
 *
 * Upsampling is used for some algorithms but probably should be avoided. It
 * doesn't do anything to alter the information in the image yet has some cost.
 * The main utility is for making small images large enough to be easily
 * viewed and analysed.
 *
 */
void UpsampleImage (Image8_t       *outImg,  /* larger destination image */
                    const Image8_t *inImg);  /* smaller source image */


/*
 * Flip an image up/down
 *
 * This can be useful as cameras may be mounted upside down or in other unusual
 * orientations. There is a cost associated with flipping the image. But it may
 * make the algorithms much simpler to visualize. It is also useful to rectify
 * images for later analysis.
 *
 */
void FlipImage (Image8_t *outImg);

/* 16 bit word version */
void FlipImageW (Image16_t *outImg);


/*
 * Flop an image left/right
 *
 * An inverted camera generates an image that is not only flipped but flopped
 * left to right. So correcting the orientation requires flopping. Note that
 * flopping is not done row-wise with memcpy so is much more expensive than
 * flipping.
 *
 */
void FlopImage (Image8_t *outImg);

/* 16 bit word version */
void FlopImageW (Image16_t *outImg);


/*
 * Convert a RGB image to YCbCr
 *
 */
void ConvertImageRGBtoYCbCr (Image8_t       *outYImg,
                             Image8_t       *outCbImg,
                             Image8_t       *outCrImg,
                             const Image8_t *inRedImg,
                             const Image8_t *inGreenImg,
                             const Image8_t *inBlueImg);

/* packed 16 bit CbCr pixel version */
void ConvertImageRGBtoYCbCrPacked (Image8_t       *outYImg,
                                   Image16_t      *outCbCrImg,
                                   const Image8_t *inRedImg,
                                   const Image8_t *inGreenImg,
                                   const Image8_t *inBlueImg);


/*
 * Convert the integral image to an 8 bit image
 *
 * The integral image is typically a different size than the desired 8 bit
 * output image. It must be shifted and (usually) rescaled to fit.
 *
 */
void ConvertIntegralImage (Image8_t        *outImg,
                           const Image32_t *inImg,   /* feature image */
                           const size_t     shift);  /* right shift rescale */



#endif
