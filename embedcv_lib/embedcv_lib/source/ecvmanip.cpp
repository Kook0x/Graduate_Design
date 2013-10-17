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




#include <stddef.h>
#include <string.h>

#include "types.h"

#include "ecvcommon.h"
#include "ecvtypes.h"
#include "ecvutil.h"



/*
 * Crop an 8 bit image from another image
 *
 * The output image is presumed to be smaller than the input image and fit
 * inside. If the specified column and row combined with the dimensions of
 * the two images do not allow this, then the memory copy will cause a
 * segmentation fault. There is no bounds checking.
 *
 */
void CropImage (Image8_t       *outImg,
                const Image8_t *inImg,
                const size_t    inColumn,
                const size_t    inRow)
{
  const size_t outWidth = outImg->width;
  const size_t  inWidth = inImg->width;

  const size_t cpylen = sizeof(uint8_t) * outWidth;

  const uint8_t *ptrInData  = inImg->data + inRow * inWidth + inColumn;
  const uint8_t *endOutData = outImg->data + outWidth * outImg->height;
  uint8_t       *ptrOutData = outImg->data;

  while (ptrOutData != endOutData)
  {
    LOOP_UNROLL_LESS(
      memcpy(ptrOutData, ptrInData, cpylen);
      ptrOutData += outWidth;
      ptrInData  += inWidth;
      )
  }
}


/*
 * Crop a 16 bit image from another image
 *
 * The output image is presumed to be smaller than the input image and fit
 * inside. If the specified column and row combined with the dimensions of
 * the two images do not allow this, then the memory copy will cause a
 * segmentation fault. There is no bounds checking.
 *
 */
void CropImageW (Image16_t       *outImg,
                 const Image16_t *inImg,
                 const size_t     inColumn,
                 const size_t     inRow)
{
  const size_t outWidth = outImg->width;
  const size_t  inWidth = inImg->width;

  const size_t cpylen = sizeof(uint16_t) * outWidth;

  const uint16_t *ptrInData  = inImg->data + inRow * inWidth + inColumn;
  const uint16_t *endOutData = outImg->data + outWidth * outImg->height;
  uint16_t       *ptrOutData = outImg->data;

  while (ptrOutData != endOutData)
  {
    LOOP_UNROLL_LESS(
      memcpy(ptrOutData, ptrInData, cpylen);
      ptrOutData += outWidth;
      ptrInData  += inWidth;
      )
  }
}


/*
 * Paste an 8 bit image into another image
 *
 * The smaller input image is pasted into a larger output image. There is no
 * bounds checking so if the input image does not fit inside the output image,
 * the memory copy will cause a segmentation fault.
 *
 */
void PasteImage (Image8_t       *outImg,
                 const Image8_t *inImg,
                 const size_t    outColumn,
                 const size_t    outRow)
{
  const size_t outWidth = outImg->width;
  const size_t  inWidth = inImg->width;

  const size_t cpylen = sizeof(uint8_t) * inWidth;

  const uint8_t *ptrInData  = inImg->data;
  const uint8_t *endInData  = inImg->data + inWidth * inImg->height;
  uint8_t       *ptrOutData = outImg->data + outRow * outWidth + outColumn;

  while (ptrInData != endInData)
  {
    LOOP_UNROLL_LESS(
      memcpy(ptrOutData, ptrInData, cpylen);
      ptrOutData += outWidth;
      ptrInData  += inWidth;
      )
  }
}


/*
 * Downsample an 8 bit image (subsample)
 *
 * The input image is presumed to be dimensionally larger than the output
 * image. The smaller output dimensions should divide evenly into the larger
 * input dimensions.
 *
 */
void DownsampleImage (Image8_t       *outImg,
                      const Image8_t *inImg)
{
  const size_t outWidth  = outImg->width;
  const size_t outHeight = outImg->height;
  const size_t inWidth   = inImg->width;
  const size_t inHeight  = inImg->height;
  const size_t innerStep = inWidth / outWidth;
  const size_t outerStep = inWidth * (inHeight / outHeight - 1);

  const uint8_t *ptrIn   = inImg->data;

  const uint8_t *endOut  = outImg->data + outWidth * outHeight;
  const uint8_t *endRow  = outImg->data + outWidth;

  uint8_t       *ptrOut  = outImg->data;

  while (ptrOut != endOut)
  {
    while (ptrOut != endRow)
    {
      LOOP_UNROLL_LESS(
        *ptrOut++ = *ptrIn;
        ptrIn += innerStep;
        )
    }
    ptrIn += outerStep;
    endRow += outWidth;
  }
}


/*
 * Downsample a 16 bit image (subsample)
 *
 * The input image is presumed to be dimensionally larger than the output
 * image. The smaller output dimensions should divide evenly into the larger
 * input dimensions.
 *
 *
 */
void DownsampleImageW (Image16_t *outImg,
                       const Image16_t *inImg)
{
  const size_t outWidth  = outImg->width;
  const size_t outHeight = outImg->height;
  const size_t inWidth   = inImg->width;
  const size_t inHeight  = inImg->height;
  const size_t innerStep = inWidth / outWidth;
  const size_t outerStep = inWidth * (inHeight / outHeight - 1);

  const uint16_t *ptrIn  = inImg->data;

  const uint16_t *endOut = outImg->data + outWidth * outHeight;
  const uint16_t *endRow = outImg->data + outWidth;

  uint16_t       *ptrOut = outImg->data;

  while (ptrOut != endOut)
  {
    while (ptrOut != endRow)
    {
      LOOP_UNROLL_LESS(
        *ptrOut++ = *ptrIn;
        ptrIn += innerStep;
        )
    }
    ptrIn += outerStep;
    endRow += outWidth;
  }
}


/*
 * Upsample an 8 bit image (make it larger)
 *
 */
void UpsampleImage (Image8_t *outImg,
                    const Image8_t *inImg)
{
  const size_t outWidth  = outImg->width;
  const size_t outHeight = outImg->height;

  const size_t inWidth   = inImg->width;
  const size_t inHeight  = inImg->height;

  const size_t widthRatio  = outWidth / inWidth;
  const size_t heightRatio = outHeight / inHeight;

  const size_t cpylen = sizeof(uint8_t) * outWidth;

  size_t col, i;

  const uint8_t *ptrLast;

  const uint8_t *ptrIn  = inImg->data;
  const uint8_t *endIn  = inImg->data + inWidth * inHeight;

  uint8_t       *ptrOut = outImg->data;

  while (ptrIn != endIn)
  {
    ptrLast = ptrOut;

    for (col = 0; col < inWidth; col++)
    {
      for (i = 0; i < widthRatio; i++)
      {
        *ptrOut++ = *ptrIn;
      }
      ptrIn++;
    }

    for (i = 1; i < heightRatio; i++)
    {
      memcpy(ptrOut, ptrLast, cpylen);
      ptrOut += outWidth;
    }
  }
}


/*
 * Flip an 8 bit image up / down
 *
 */
void FlipImage (Image8_t *outImg)
{
  const size_t outWidth = outImg->width;
  uint8_t *rowbuf= (uint8_t *)malloc(sizeof(uint8_t)*outWidth);

  const size_t   cpylen     = sizeof(uint8_t) * outWidth;
  const size_t   numPixels  = outWidth * outImg->height;

  const uint8_t *ptrHalfway = outImg->data + (numPixels >> 1);
  uint8_t       *ptrTop     = outImg->data;
  uint8_t       *ptrBottom  = outImg->data + numPixels - outWidth;

  while (ptrTop < ptrHalfway)
  {
    LOOP_UNROLL_LESS(
      memcpy(rowbuf, ptrTop, cpylen);
      memcpy(ptrTop, ptrBottom, cpylen);
      memcpy(ptrBottom, rowbuf, cpylen);

      ptrTop    += outWidth;
      ptrBottom -= outWidth;
      )
  }
}


/*
 * Flip an 16 bit image up / down
 *
 */
void FlipImageW (Image16_t *outImg)
{
  const size_t outWidth = outImg->width;
  uint16_t *rowbuf= (uint16_t *)malloc(sizeof(uint16_t)*outWidth);


  const size_t   cpylen     = sizeof(uint16_t) * outWidth;
  const size_t   numPixels  = outWidth * outImg->height;

  const uint16_t *ptrHalfway = outImg->data + (numPixels >> 1);
  uint16_t       *ptrTop     = outImg->data;
  uint16_t       *ptrBottom  = outImg->data + numPixels - outWidth;

  while (ptrTop < ptrHalfway)
  {
    LOOP_UNROLL_LESS(
      memcpy(rowbuf, ptrTop, cpylen);
      memcpy(ptrTop, ptrBottom, cpylen);
      memcpy(ptrBottom, rowbuf, cpylen);

      ptrTop    += outWidth;
      ptrBottom -= outWidth;
      )
  }
}


/*
 * Flop an 8 bit image left / right
 *
 */
void FlopImage (Image8_t *outImg)
{
  const size_t outHeight    = outImg->height;
  const size_t outWidth     = outImg->width;
  const size_t halfWidth    = outWidth >> 1;
  const size_t widthLessOne = outWidth - 1;

  const uint8_t *endRow = outImg->data + outWidth * outHeight;
  uint8_t       *ptrRow = outImg->data;

  const uint8_t *ptrCenter;
  uint8_t       *ptrLeft, *ptrRight, tmp;

  while (ptrRow != endRow)
  {
    ptrCenter = ptrRow + halfWidth;

    ptrLeft   = ptrRow;
    ptrRight  = ptrRow + widthLessOne;

    while (ptrLeft < ptrCenter)
    {
      LOOP_UNROLL_LESS(
        tmp         = *ptrLeft;
        *ptrLeft++  = *ptrRight;
        *ptrRight-- = tmp;
        )
    }

    ptrRow += outWidth;
  }
}


/*
 * Flop an 16 bit image left / right
 *
 */
void FlopImageW (Image16_t *outImg)
{
  const size_t outHeight    = outImg->height;
  const size_t outWidth     = outImg->width;
  const size_t halfWidth    = outWidth >> 1;
  const size_t widthLessOne = outWidth - 1;

  const uint16_t *endRow = outImg->data + outWidth * outHeight;
  uint16_t       *ptrRow = outImg->data;

  const uint16_t *ptrCenter;
  uint16_t       *ptrLeft, *ptrRight, tmp;

  while (ptrRow != endRow)
  {
    ptrCenter = ptrRow + halfWidth;

    ptrLeft   = ptrRow;
    ptrRight  = ptrRow + widthLessOne;

    while (ptrLeft < ptrCenter)
    {
      LOOP_UNROLL_LESS(
        tmp         = *ptrLeft;
        *ptrLeft++  = *ptrRight;
        *ptrRight-- = tmp;
        )
    }

    ptrRow += outWidth;
  }
}


/*
 * Convert a RGB image to YCbCr
 *
 */
void ConvertImageRGBtoYCbCr (Image8_t       *outYImg,
                             Image8_t       *outCbImg,
                             Image8_t       *outCrImg,
                             const Image8_t *inRedImg,
                             const Image8_t *inGreenImg,
                             const Image8_t *inBlueImg)
{
  const uint8_t *ptrRed   = inRedImg->data;
  const uint8_t *ptrGreen = inGreenImg->data;
  const uint8_t *ptrBlue  = inBlueImg->data;

  const uint8_t *endLuma  = outYImg->data + outYImg->width * outYImg->height;
  uint8_t       *ptrLuma  = outYImg->data;

  uint8_t       *ptrCb    = outCbImg->data;
  uint8_t       *ptrCr    = outCrImg->data;

  while (ptrLuma != endLuma)
  {
    LOOP_UNROLL_LESS(
      YCbCrFromRGB(ptrLuma++,
                   ptrCb++,
                   ptrCr++,
                   *ptrRed++,
                   *ptrGreen++,
                   *ptrBlue++);
      )
  }

}


/*
 * Convert a RGB image to YCbCr in packed 16 bit pixels
 *
 */
void ConvertImageRGBtoYCbCrPacked (Image8_t       *outYImg,
                                   Image16_t      *outCbCrImg,
                                   const Image8_t *inRedImg,
                                   const Image8_t *inGreenImg,
                                   const Image8_t *inBlueImg)
{
  const uint8_t *ptrRed   = inRedImg->data;
  const uint8_t *ptrGreen = inGreenImg->data;
  const uint8_t *ptrBlue  = inBlueImg->data;

  const uint8_t *endLuma  = outYImg->data + outYImg->width * outYImg->height;
  uint8_t       *ptrLuma  = outYImg->data;

  PackedCbCr_t  *ptrCbCr = (PackedCbCr_t *) outCbCrImg->data;

  while (ptrLuma != endLuma)
  {
    LOOP_UNROLL_LESS(
      YCbCrFromRGB(ptrLuma++,
                   &ptrCbCr->data[0],
                   &ptrCbCr->data[1],
                   *ptrRed++,
                   *ptrGreen++,
                   *ptrBlue++);
      ptrCbCr++;
      )
  }

}


/*
 * Convert the integral image to an 8 bit image
 *
 */
void ConvertIntegralImage (Image8_t        *outImg,
                           const Image32_t *inImg,
                           const size_t     shift)
{
  const size_t colStep   = outImg->width / inImg->width;
  const size_t rowStep   = outImg->height / inImg->height;
  const size_t rowOffset = outImg->width * rowStep - inImg->width * colStep;

  const size_t *ptrIn = inImg->data;

  uint8_t *ptrOut = outImg->data
                        + ((outImg->width - inImg->width * colStep) >> 1)
                        + ((outImg->height - inImg->height * rowStep) >> 1)
                              * outImg->width;

  size_t rowIdx, colIdx;

  for (rowIdx = inImg->height; rowIdx; rowIdx--)
  {
    for (colIdx = inImg->width; colIdx; colIdx--)
    {
      *ptrOut = (*ptrIn++) >> shift;

      ptrOut += colStep;
    }

    ptrOut += rowOffset;
  }
}


