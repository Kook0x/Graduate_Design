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



#include <stdlib.h>
#include <string.h>

#include "types.h"

#include "ecvcommon.h"
#include "ecvtypes.h"
#include "ecvutil.h"



/*
 * Using Sobel convolution kernels
 *
 *  -1  0  +1
 *  -2  0  +2
 *  -1  0  +1
 *
 * and
 *
 *  -1  -2  -1
 *   0   0   0
 *  +1  +2  +1
 *
 * The edges of the output image are ignored.
 *
 */
void SobelEdges (Image16_t      *outImgX,
                 Image16_t      *outImgY,
                 const Image8_t *inImg)
{
  const size_t height     = inImg->height;
  const size_t width      = inImg->width;
  const size_t twiceWidth = width << 1;

  int16_t *outDataX = (int16_t *)outImgX->data;
  int16_t *outDataY = (int16_t *)outImgY->data;

  memset(outDataX, 0, sizeof(uint16_t) * width * height);
  memset(outDataY, 0, sizeof(uint16_t) * width * height);

  size_t i, j;

  uint8_t value, twiceValue;

  const uint8_t *ptrIn         = inImg->data + width + 1;
  int16_t       *ptrUpLeftX    = outDataX;
  int16_t       *ptrUpLeftY    = outDataY;
  int16_t       *ptrUpY        = outDataY + 1;
  int16_t       *ptrUpRightX   = outDataX + 2;
  int16_t       *ptrUpRightY   = outDataY + 2;
  int16_t       *ptrLeftX      = outDataX + width;
  int16_t       *ptrRightX     = outDataX + width + 2;
  int16_t       *ptrDownLeftX  = outDataX + twiceWidth;
  int16_t       *ptrDownLeftY  = outDataY + twiceWidth;
  int16_t       *ptrDownY      = outDataY + twiceWidth + 1;
  int16_t       *ptrDownRightX = outDataX + twiceWidth + 2;
  int16_t       *ptrDownRightY = outDataY + twiceWidth + 2;

  for (i = height - 2; i; i--)
  {
    for (j = width - 2; j; j--)
    {
      value      = *ptrIn++;
      twiceValue = value << 1;

      /* horizontal edges */
      *ptrUpLeftX    += value;
      *ptrUpRightX   -= value;
      *ptrDownLeftX  += value;
      *ptrDownRightX -= value;

      *ptrLeftX++      += twiceValue;
      *ptrRightX++     -= twiceValue;

      /* vertical edges */
      *ptrUpLeftY++    += value;
      *ptrUpRightY++   += value;
      *ptrDownLeftY++  -= value;
      *ptrDownRightY++ -= value;

      *ptrUpY++        += twiceValue;
      *ptrDownY++      -= twiceValue;
    }

    ptrIn += 2;

    ptrUpLeftX += 2;
    ptrUpRightX += 2;
    ptrDownLeftX += 2;
    ptrDownRightX += 2;

    ptrLeftX += 2;
    ptrRightX += 2;

    ptrUpLeftY += 2;
    ptrUpRightY += 2;
    ptrDownLeftY += 2;
    ptrDownRightY += 2;

    ptrUpY += 2;
    ptrDownY += 2;
  }
}


/*
 * The one norm - sum of absolute values - of the X and Y component edge images
 *
 */
void EdgeImagesTo1Norm (Image8_t        *outImg,
                        const Image16_t *inImgEdgeX,
                        const Image16_t *inImgEdgeY,
                        const size_t     shift)
{
  const int16_t *ptrInX = (int16_t *)inImgEdgeX->data;
  const int16_t *ptrInY = (int16_t *)inImgEdgeY->data;

  const uint8_t *endOut = outImg->data + outImg->width * outImg->height;
  uint8_t       *ptrOut = outImg->data;

  while (ptrOut != endOut)
  {
    LOOP_UNROLL_MORE(
      *ptrOut++ = ( (uint16_t)(abs(*ptrInX++) + abs(*ptrInY++)) ) >> shift;
      )
  }
}


/*
 * The two norm - square root of the sum of squares 8 of the X and Y component
 * edge images
 *
 */
void EdgeImagesTo2Norm (Image8_t        *outImg,
                        const Image16_t *inImgEdgeX,
                        const Image16_t *inImgEdgeY,
                        const size_t     shift)
{
  const int16_t *ptrInX = (int16_t *)inImgEdgeX->data;
  const int16_t *ptrInY = (int16_t *)inImgEdgeY->data;

  const uint8_t *endOut = outImg->data + outImg->width * outImg->height;
  uint8_t       *ptrOut = outImg->data;

  int16_t xcomp, ycomp;

  while (ptrOut != endOut)
  {
    LOOP_UNROLL_MORE(
      xcomp = *ptrInX++;
      ycomp = *ptrInY++;
      *ptrOut++ = ( UintSqrt( xcomp * xcomp + ycomp * ycomp) ) >> shift;
      )
  }
}


/*
 * The sum of squares of the X and Y component edge images
 *
 */
void EdgeImagesToSS (Image8_t        *outImg,
                     const Image16_t *inImgEdgeX,
                     const Image16_t *inImgEdgeY,
                     const size_t     shift)
{
  const int16_t *ptrInX = (int16_t *)inImgEdgeX->data;
  const int16_t *ptrInY = (int16_t *)inImgEdgeY->data;

  const uint8_t *endOut = outImg->data + outImg->width * outImg->height;
  uint8_t       *ptrOut = outImg->data;

  int16_t xcomp, ycomp;

  while (ptrOut != endOut)
  {
    LOOP_UNROLL_MORE(
      xcomp = *ptrInX++;
      ycomp = *ptrInY++;
      *ptrOut++ = ( xcomp * xcomp + ycomp * ycomp ) >> shift;
      )
  }
}


/*
 * Morphological erosion with a structuring element 3 pixels across and 1 down
 *
 */
void RegionErode31 (Image8_t *inoutImg, const uint8_t mark)
{
  const size_t   width  = inoutImg->width;

  const uint8_t *endImg = inoutImg->data + width * inoutImg->height;
  uint8_t       *ptrImg = inoutImg->data;
  uint8_t       *endRow;

  uint8_t accum;  /* 3 pixel wide binary image window in lower 3 bits */

  while (ptrImg != endImg)
  {
    accum = 0;

    endRow = ptrImg + width;

    while (ptrImg != endRow)
    {
      LOOP_UNROLL_MORE(
        accum <<= 1;
        accum |= (*ptrImg != 0);

        if ( accum & 0x2 && ~accum & 0x7 )
        {
          *(ptrImg - 1) = mark;
        }

        ptrImg++;
        )
    }
  }
}


/*
 * Morphological erosion with a structuring element 5 pixels across and 1 down
 *
 */
void RegionErode51 (Image8_t *inoutImg, const uint8_t mark)
{
  const size_t   width  = inoutImg->width;

  const uint8_t *endImg = inoutImg->data + width * inoutImg->height;
  uint8_t       *ptrImg = inoutImg->data;
  uint8_t       *endRow;

  uint8_t accum;  /* 5 pixel wide binary image window in lower 5 bits */

  while (ptrImg != endImg)
  {
    accum = 0;

    endRow = ptrImg + width;

    while (ptrImg != endRow)
    {
      LOOP_UNROLL_MORE(
        accum <<= 1;
        accum |= (*ptrImg != 0);

        if ( accum & 0x4 && ~accum & 0x1f )
        {
          *(ptrImg - 2) = mark;
        }

        ptrImg++;
        )
    }
  }
}


/*
 * Morphological dilation with a structuring element 3 pixels across and 1 down
 *
 */
void RegionDilate31 (Image8_t *inoutImg, const uint8_t mark)
{
  const size_t   width  = inoutImg->width;

  const uint8_t *endImg = inoutImg->data + width * inoutImg->height;
  uint8_t       *ptrImg = inoutImg->data;
  uint8_t       *endRow;

  uint8_t accum;  /* 3 pixel wide binary image window in lower 3 bits */

  while (ptrImg != endImg)
  {
    accum = 0;

    endRow = ptrImg + width;

    /* must bounds check for head of the row */
    LOOP_UNROLL_MORE(
      accum <<= 1;
      accum |= (*ptrImg != 0);

      if ( ~accum & 0x2 && accum & 0x7 )
      {
        if ( (ptrImg - 1) >= inoutImg->data )
        {
          *(ptrImg - 1) = mark;
        }
      }

      ptrImg++;
      )

    /* rest of the row */
    while (ptrImg != endRow)
    {
      LOOP_UNROLL_MORE(
        accum <<= 1;
        accum |= (*ptrImg != 0);

        if ( ~accum & 0x2 && accum & 0x7 )
        {
          *(ptrImg - 1) = mark;
        }

        ptrImg++;
        )
    }
  }
}


/*
 * Morphological dilation with a structuring element 5 pixels across and 1 down
 *
 */
void RegionDilate51 (Image8_t *inoutImg, const uint8_t mark)
{
  const size_t   width  = inoutImg->width;

  const uint8_t *endImg = inoutImg->data + width * inoutImg->height;
  uint8_t       *ptrImg = inoutImg->data;
  uint8_t       *endRow;

  uint8_t accum;  /* 5 pixel wide binary image window in lower 3 bits */

  while (ptrImg != endImg)
  {
    accum = 0;

    endRow = ptrImg + width;

    /* must bounds check for head of the row */
    LOOP_UNROLL_MORE(
      accum <<= 1;
      accum |= (*ptrImg != 0);

      if ( ~accum & 0x4 && accum & 0x1f )
      {
        if ( (ptrImg - 2) > inoutImg->data )
        {
          *(ptrImg - 2) = mark;
        }
      }

      ptrImg++;
      )

    /* rest of the row */
    while (ptrImg != endRow)
    {
      LOOP_UNROLL_MORE(
        accum <<= 1;
        accum |= (*ptrImg != 0);

        if ( ~accum & 0x4 && accum & 0x1f )
        {
          *(ptrImg - 2) = mark;
        }

        ptrImg++;
        )
    }
  }
}


/*
 * Morphological erosion with a structuring element 1 pixel across and 3 down
 *
 */
void RegionErode13 (Image8_t *inoutImg, const uint8_t mark)
{
  const size_t   width  = inoutImg->width;

  //uint8_t accum[width];  /* 3 pixel tall binary image window for entire row */
  uint8_t *accum=(uint8_t *)malloc(sizeof(uint8_t)*width);
  memset(accum, 0, sizeof(uint8_t) * width);

  const uint8_t *endImg = inoutImg->data + width * inoutImg->height;
  uint8_t       *ptrImg = inoutImg->data;
  uint8_t       *endRow;

  uint8_t *ptrAccum;

  /* main loop */
  while (ptrImg != endImg)
  {
    ptrAccum = accum;

    endRow = ptrImg + width;

    while (ptrImg != endRow)
    {
      LOOP_UNROLL_MORE(
        *ptrAccum <<= 1;
        *ptrAccum |= (*ptrImg != 0);

        if ( *ptrAccum & 0x2 && ~(*ptrAccum) & 0x7 )
        {
          *(ptrImg - width) = mark;
        }

        ptrImg++;
        ptrAccum++;
        )
    }
  }
}


/*
 * Morphological erosion with a structuring element 1 pixel across and 5 down
 *
 */
void RegionErode15 (Image8_t *inoutImg, const uint8_t mark)
{
  const size_t width      = inoutImg->width;
  const size_t twiceWidth = width << 1;

  //uint8_t accum[width];  /* 5 pixel tall binary image window for entire row */
  uint8_t *accum=(uint8_t *)malloc(width);
  memset(accum, 0, sizeof(uint8_t) * width);

  const uint8_t *endImg = inoutImg->data + width * inoutImg->height;
  uint8_t       *ptrImg = inoutImg->data;
  uint8_t       *endRow;

  uint8_t *ptrAccum;

  /* main loop */
  while (ptrImg != endImg)
  {
    ptrAccum = accum;

    endRow = ptrImg + width;

    while (ptrImg != endRow)
    {
      LOOP_UNROLL_MORE(
        *ptrAccum <<= 1;
        *ptrAccum |= (*ptrImg != 0);

        if ( *ptrAccum & 0x4 && ~(*ptrAccum) & 0x1f )
        {
          *(ptrImg - twiceWidth) = mark;
        }

        ptrImg++;
        ptrAccum++;
        )
    }
  }
}


/*
 * Morphological dilation with a structuring element 1 pixel across and 3 down
 *
 */
void RegionDilate13 (Image8_t *inoutImg, const uint8_t mark)
{
  const size_t   width  = inoutImg->width;

  //uint8_t accum[width];  /* 3 pixel tall binary image window for entire row */
  uint8_t *accum=(uint8_t *)malloc(width);
  memset(accum, 0, sizeof(uint8_t) * width);

  const uint8_t *endImg = inoutImg->data + width * inoutImg->height;
  uint8_t       *ptrImg = inoutImg->data;
  uint8_t       *endRow;

  uint8_t *ptrAccum;

  /* handle first row with bounds check on output pointer */
  ptrAccum = accum;
  endRow = ptrImg + width;
  while (ptrImg != endRow)
  {
    LOOP_UNROLL_MORE(
      *ptrAccum <<= 1;
      *ptrAccum |= (*ptrImg != 0);

      if ( ~(*ptrAccum) & 0x2 && *ptrAccum & 0x7 )
      {
        if ( (ptrImg - width) >= inoutImg->data )
        {
          *(ptrImg - width) = mark;
        }
      }

      ptrImg++;
      ptrAccum++;
      )
  }

  /* main loop */
  while (ptrImg != endImg)
  {
    ptrAccum = accum;

    endRow = ptrImg + width;

    while (ptrImg != endRow)
    {
      LOOP_UNROLL_MORE(
        *ptrAccum <<= 1;
        *ptrAccum |= (*ptrImg != 0);

        if ( ~(*ptrAccum) & 0x2 && *ptrAccum & 0x7 )
        {
          *(ptrImg - width) = mark;
        }

        ptrImg++;
        ptrAccum++;
        )
    }
  }
}


/*
 * Morphological dilation with a structuring element 1 pixel across and 5 down
 *
 */
void RegionDilate15 (Image8_t *inoutImg, const uint8_t mark)
{
  const size_t width      = inoutImg->width;
  const size_t twiceWidth = width << 1;

  uint8_t *accum=(uint8_t *)malloc(width);  /* 5 pixel tall binary image window for entire row */
  memset(accum, 0, sizeof(uint8_t) * width);

  const uint8_t *endImg = inoutImg->data + width * inoutImg->height;
  uint8_t       *ptrImg = inoutImg->data;
  uint8_t       *endRow;

  uint8_t *ptrAccum;

  /* handle first two rows with bounds check on output pointer */
  size_t i;
  for (i = 2; i; i--)
  { 
    ptrAccum = accum;
    endRow = ptrImg + width;
    while (ptrImg != endRow)
    {
      LOOP_UNROLL_MORE(
        *ptrAccum <<= 1;
        *ptrAccum |= (*ptrImg != 0);

        if ( ~(*ptrAccum) & 0x4 && *ptrAccum & 0x1f )
        {
          if ( (ptrImg - twiceWidth) >= inoutImg->data )
          {
            *(ptrImg - twiceWidth) = mark;
          }
        }

        ptrImg++;
        ptrAccum++;
        )
    }
  }

  /* main loop */
  while (ptrImg != endImg)
  {
    ptrAccum = accum;

    endRow = ptrImg + width;

    while (ptrImg != endRow)
    {
      LOOP_UNROLL_MORE(
        *ptrAccum <<= 1;
        *ptrAccum |= (*ptrImg != 0);

        if ( ~(*ptrAccum) & 0x4 && *ptrAccum & 0x1f )
        {
          *(ptrImg - twiceWidth) = mark;
        }

        ptrImg++;
        ptrAccum++;
        )
    }
  }
}


/*
 * Morphological erosion with a structuring element 3 pixels square
 *
 */
void RegionErode33 (Image8_t *inoutImg, const uint8_t mark)
{
  const size_t   width  = inoutImg->width;
  const size_t   offset = width + 1;

  //uint8_t accum[width];  /* 3 pixel tall binary image window for entire row */
  uint8_t *accum=(uint8_t *)malloc(width);
  memset(accum, 0, sizeof(uint8_t) * width);

  const uint8_t *endImg = inoutImg->data + width * inoutImg->height;
  uint8_t       *ptrImg = inoutImg->data;
  uint8_t       *endRow;

  uint8_t *ptrAccum0, *ptrAccum1, *ptrAccum2;

  /* main loop */
  while (ptrImg != endImg)
  {
    endRow = ptrImg + width;

    ptrAccum0 = accum;
    ptrAccum1 = accum + 1;
    ptrAccum2 = accum + 2;

    /* 0th column in row */
    *ptrAccum0 <<= 1;
    *ptrAccum0 |= (*ptrImg++ != 0);

    /* 1st column in row */
    *ptrAccum1 <<= 1;
    *ptrAccum1 |= (*ptrImg++ != 0);

    /* 2nd through 7th columns in row */
    LOOP_UNROLL_MORE2(
      *ptrAccum2 <<= 1;
      *ptrAccum2 |= (*ptrImg != 0);

      if ( *ptrAccum1 & 0x2
           && (~(*ptrAccum1) & 0x7
                   || ~(*ptrAccum0) & 0x7
                   || ~(*ptrAccum2) & 0x7) )
      {
        *(ptrImg - offset) = mark;
      }

      ptrImg++;
      ptrAccum0++;
      ptrAccum1++;
      ptrAccum2++;
      )

    while (ptrImg != endRow)
    {
      LOOP_UNROLL_MORE(
        *ptrAccum2 <<= 1;
        *ptrAccum2 |= (*ptrImg != 0);

        if ( *ptrAccum1 & 0x2
             && (~(*ptrAccum1) & 0x7
                     || ~(*ptrAccum0) & 0x7
                     || ~(*ptrAccum2) & 0x7) )
        {
          *(ptrImg - offset) = mark;
        }

        ptrImg++;
        ptrAccum0++;
        ptrAccum1++;
        ptrAccum2++;
        )
    }
  }
}


/*
 * Morphological erosion with a structuring element 5 pixels square
 *
 */
void RegionErode55 (Image8_t *inoutImg, const uint8_t mark)
{
  const size_t   width  = inoutImg->width;
  const size_t   offset = (width + 1) << 1;


  //uint8_t accum[width];  /* 5 pixel tall binary image window for entire row */
  uint8_t *accum=(uint8_t *)malloc(width);
  memset(accum, 0, sizeof(uint8_t) * width);

  const uint8_t *endImg = inoutImg->data + width * inoutImg->height;
  uint8_t       *ptrImg = inoutImg->data;
  uint8_t       *endRow;

  uint8_t *ptrAccum0, *ptrAccum1, *ptrAccum2, *ptrAccum3, *ptrAccum4;

  /* main loop */
  while (ptrImg != endImg)
  {
    endRow = ptrImg + width;

    ptrAccum0 = accum;
    ptrAccum1 = accum + 1;
    ptrAccum2 = accum + 2;
    ptrAccum3 = accum + 3;
    ptrAccum4 = accum + 4;

    /* 0th column in row */
    *ptrAccum0 <<= 1;
    *ptrAccum0 |= (*ptrImg++ != 0);

    /* 1st column in row */
    *ptrAccum1 <<= 1;
    *ptrAccum1 |= (*ptrImg++ != 0);

    /* 2nd column in row */
    *ptrAccum2 <<= 1;
    *ptrAccum2 |= (*ptrImg++ != 0);

    /* 3rd column in row */
    *ptrAccum3 <<= 1;
    *ptrAccum3 |= (*ptrImg++ != 0);

    /* 4th through 7th columns in row */
    LOOP_UNROLL_MORE4(
      *ptrAccum4 <<= 1;
      *ptrAccum4 |= (*ptrImg != 0);

      if ( *ptrAccum2 & 0x4
           && (~(*ptrAccum2) & 0x1f
                   || ~(*ptrAccum0) & 0x1f
                   || ~(*ptrAccum1) & 0x1f
                   || ~(*ptrAccum3) & 0x1f
                   || ~(*ptrAccum4) & 0x1f) )
      {
        *(ptrImg - offset) = mark;
      }

      ptrImg++;
      ptrAccum0++;
      ptrAccum1++;
      ptrAccum2++;
      ptrAccum3++;
      ptrAccum4++;
      )

    while (ptrImg != endRow)
    {
      LOOP_UNROLL_MORE(
        *ptrAccum4 <<= 1;
        *ptrAccum4 |= (*ptrImg != 0);

        if ( *ptrAccum2 & 0x4
             && (~(*ptrAccum2) & 0x1f
                     || ~(*ptrAccum0) & 0x1f
                     || ~(*ptrAccum1) & 0x1f
                     || ~(*ptrAccum3) & 0x1f
                     || ~(*ptrAccum4) & 0x1f) )
        {
          *(ptrImg - offset) = mark;
        }

        ptrImg++;
        ptrAccum0++;
        ptrAccum1++;
        ptrAccum2++;
        ptrAccum3++;
        ptrAccum4++;
        )
    }
  }
}


/*
 * Morphological dilation with a structuring element 3 pixels square
 *
 */
void RegionDilate33 (Image8_t *inoutImg, const uint8_t mark)
{
  const size_t   width  = inoutImg->width;
  const size_t   offset = width + 1;

  //uint8_t accum[width];  /* 3 pixel tall binary image window for entire row */
  uint8_t *accum=(uint8_t *)malloc(width);
  memset(accum, 0, sizeof(uint8_t) * width);

  const uint8_t *endImg = inoutImg->data + width * inoutImg->height;
  uint8_t       *ptrImg = inoutImg->data;
  uint8_t       *endRow;

  uint8_t *ptrAccum0, *ptrAccum1, *ptrAccum2;

  /* bounds check for first row */
  endRow = ptrImg + width;
  ptrAccum0 = accum;
  ptrAccum1 = accum + 1;
  ptrAccum2 = accum + 2;
  /* 0th column in row */
  *ptrAccum0 <<= 1;
  *ptrAccum0 |= (*ptrImg++ != 0);
  /* 1st column in row */
  *ptrAccum1 <<= 1;
  *ptrAccum1 |= (*ptrImg++ != 0);
  /* 2nd through 7th columns in row */
  LOOP_UNROLL_MORE2(
    *ptrAccum2 <<= 1;
    *ptrAccum2 |= (*ptrImg != 0);

    if ( ~(*ptrAccum1) & 0x2
         && (*ptrAccum1 & 0x7 || *ptrAccum0 & 0x7 || *ptrAccum2 & 0x7) )
    {
      if ( (ptrImg - offset) >= inoutImg->data )
      {
        *(ptrImg - offset) = mark;
      }
    }

    ptrImg++;
    ptrAccum0++;
    ptrAccum1++;
    ptrAccum2++;
    )
  /* rest of the first row */
  while (ptrImg != endRow)
  {
    LOOP_UNROLL_MORE(
      *ptrAccum2 <<= 1;
      *ptrAccum2 |= (*ptrImg != 0);

      if ( ~(*ptrAccum1) & 0x2
           && (*ptrAccum1 & 0x7 || *ptrAccum0 & 0x7 || *ptrAccum2 & 0x7) )
      {
        if ( (ptrImg - offset) >= inoutImg->data )
        {
          *(ptrImg - offset) = mark;
        }
      }

      ptrImg++;
      ptrAccum0++;
      ptrAccum1++;
      ptrAccum2++;
      )
  }

  /* main loop */
  while (ptrImg != endImg)
  {
    endRow = ptrImg + width;

    ptrAccum0 = accum;
    ptrAccum1 = accum + 1;
    ptrAccum2 = accum + 2;

    /* 0th column in row */
    *ptrAccum0 <<= 1;
    *ptrAccum0 |= (*ptrImg++ != 0);

    /* 1st column in row */
    *ptrAccum1 <<= 1;
    *ptrAccum1 |= (*ptrImg++ != 0);

    /* 2nd through 7th columns in row */
    LOOP_UNROLL_MORE2(
      *ptrAccum2 <<= 1;
      *ptrAccum2 |= (*ptrImg != 0);

      if ( ~(*ptrAccum1) & 0x2
           && (*ptrAccum1 & 0x7 || *ptrAccum0 & 0x7 || *ptrAccum2 & 0x7) )
      {
        *(ptrImg - offset) = mark;
      }

      ptrImg++;
      ptrAccum0++;
      ptrAccum1++;
      ptrAccum2++;
      )

    while (ptrImg != endRow)
    {
      LOOP_UNROLL_MORE(
        *ptrAccum2 <<= 1;
        *ptrAccum2 |= (*ptrImg != 0);

        if ( ~(*ptrAccum1) & 0x2
             && (*ptrAccum1 & 0x7 || *ptrAccum0 & 0x7 || *ptrAccum2 & 0x7) )
        {
          *(ptrImg - offset) = mark;
        }

        ptrImg++;
        ptrAccum0++;
        ptrAccum1++;
        ptrAccum2++;
        )
    }
  }
}


/*
 * Morphological dilation with a structuring element 5 pixels square
 *
 */
void RegionDilate55 (Image8_t *inoutImg, const uint8_t mark)
{
  const size_t   width  = inoutImg->width;
  const size_t   offset = (width + 1) << 1;

  uint8_t *accum= (uint8_t *)malloc(width);  /* 5 pixel tall binary image window for entire row */

  memset(accum, 0, sizeof(uint8_t) * width);

  const uint8_t *endImg = inoutImg->data + width * inoutImg->height;
  uint8_t       *ptrImg = inoutImg->data;
  uint8_t       *endRow;

  uint8_t *ptrAccum0, *ptrAccum1, *ptrAccum2, *ptrAccum3, *ptrAccum4;

  /* handle first two rows with bounds check on output pointer */
  size_t i;
  for (i = 2; i; i--)
  {
    endRow = ptrImg + width;

    ptrAccum0 = accum;
    ptrAccum1 = accum + 1;
    ptrAccum2 = accum + 2;
    ptrAccum3 = accum + 3;
    ptrAccum4 = accum + 4;

    /* 0th column in row */
    *ptrAccum0 <<= 1;
    *ptrAccum0 |= (*ptrImg++ != 0);

    /* 1st column in row */
    *ptrAccum1 <<= 1;
    *ptrAccum1 |= (*ptrImg++ != 0);

    /* 2nd column in row */
    *ptrAccum2 <<= 1;
    *ptrAccum2 |= (*ptrImg++ != 0);

    /* 3rd column in row */
    *ptrAccum3 <<= 1;
    *ptrAccum3 |= (*ptrImg++ != 0);

    /* 4th through 7th columns in row */
    LOOP_UNROLL_MORE4(
      *ptrAccum4 <<= 1;
      *ptrAccum4 |= (*ptrImg != 0);

      if ( ~(*ptrAccum2) & 0x4
           && (*ptrAccum2 & 0x1f
                   || *ptrAccum0 & 0x1f
                   || *ptrAccum1 & 0x1f
                   || *ptrAccum3 & 0x1f
                   || *ptrAccum4 & 0x1f) )
      {
        if ( (ptrImg - offset) >= inoutImg->data )
        {
          *(ptrImg - offset) = mark;
        }
      }

      ptrImg++;
      ptrAccum0++;
      ptrAccum1++;
      ptrAccum2++;
      ptrAccum3++;
      ptrAccum4++;
      )

    while (ptrImg != endRow)
    {
      LOOP_UNROLL_MORE(
        *ptrAccum4 <<= 1;
        *ptrAccum4 |= (*ptrImg != 0);

        if ( ~(*ptrAccum2) & 0x4
             && (*ptrAccum2 & 0x1f
                     || *ptrAccum0 & 0x1f
                     || *ptrAccum1 & 0x1f
                     || *ptrAccum3 & 0x1f
                     || *ptrAccum4 & 0x1f) )
        {
          if ( (ptrImg - offset) >= inoutImg->data )
          {
            *(ptrImg - offset) = mark;
          }
        }

        ptrImg++;
        ptrAccum0++;
        ptrAccum1++;
        ptrAccum2++;
        ptrAccum3++;
        ptrAccum4++;
        )
    }
  }

  /* main loop */
  while (ptrImg != endImg)
  {
    endRow = ptrImg + width;

    ptrAccum0 = accum;
    ptrAccum1 = accum + 1;
    ptrAccum2 = accum + 2;
    ptrAccum3 = accum + 3;
    ptrAccum4 = accum + 4;

    /* 0th column in row */
    *ptrAccum0 <<= 1;
    *ptrAccum0 |= (*ptrImg++ != 0);

    /* 1st column in row */
    *ptrAccum1 <<= 1;
    *ptrAccum1 |= (*ptrImg++ != 0);

    /* 2nd column in row */
    *ptrAccum2 <<= 1;
    *ptrAccum2 |= (*ptrImg++ != 0);

    /* 3rd column in row */
    *ptrAccum3 <<= 1;
    *ptrAccum3 |= (*ptrImg++ != 0);

    /* 4th through 7th columns in row */
    LOOP_UNROLL_MORE4(
      *ptrAccum4 <<= 1;
      *ptrAccum4 |= (*ptrImg != 0);

      if ( ~(*ptrAccum2) & 0x4
           && (*ptrAccum2 & 0x1f
                   || *ptrAccum0 & 0x1f
                   || *ptrAccum1 & 0x1f
                   || *ptrAccum3 & 0x1f
                   || *ptrAccum4 & 0x1f) )
      {
        *(ptrImg - offset) = mark;
      }

      ptrImg++;
      ptrAccum0++;
      ptrAccum1++;
      ptrAccum2++;
      ptrAccum3++;
      ptrAccum4++;
      )

    while (ptrImg != endRow)
    {
      LOOP_UNROLL_MORE(
        *ptrAccum4 <<= 1;
        *ptrAccum4 |= (*ptrImg != 0);

        if ( ~(*ptrAccum2) & 0x4
             && (*ptrAccum2 & 0x1f
                     || *ptrAccum0 & 0x1f
                     || *ptrAccum1 & 0x1f
                     || *ptrAccum3 & 0x1f
                     || *ptrAccum4 & 0x1f) )
        {
          *(ptrImg - offset) = mark;
        }

        ptrImg++;
        ptrAccum0++;
        ptrAccum1++;
        ptrAccum2++;
        ptrAccum3++;
        ptrAccum4++;
        )
    }
  }
}


/*
 * If the images are denoted by
 *
 * img_0  - most recent image, passed in as inImg
 * img_1  - last image
 * img_2  - image before last
 * ...
 *
 * Then the binomial average is
 *
 * (img_0 / 2) + (img_1 / 4) + (img_2 / 8) + ...
 *
 * This is easy to compute as a running average.
 *
 */
void BinAvgImageSeq (Image8_t       *inoutImg,
                     const Image8_t *inImg)
{
  const uint8_t *ptrIn    = inImg->data;
  const uint8_t *endIn    = inImg->data + inImg->width * inImg->height;

  uint8_t       *ptrInout = inoutImg->data;

  uint16_t tmp;

  while (ptrIn != endIn)
  {
    LOOP_UNROLL_MORE(
      tmp = *ptrInout;
      tmp += *ptrIn++;
      *ptrInout++ = tmp >> 1;
      )
  }
}


/*
 * Compute the unsigned difference between two images
 *
 * This is combined with a table look up for segmenting the output image.
 *
 */
void DiffImages (Image8_t       *outImg,
                 const Image8_t *inImg1,
                 const Image8_t *inImg2,
                 const uint8_t  *inMap)
{
  const uint8_t *ptrIn1 = inImg1->data;
  const uint8_t *ptrIn2 = inImg2->data;

  const uint8_t *endOut = outImg->data + outImg->width * outImg->height;
  uint8_t       *ptrOut = outImg->data;

  while (ptrOut != endOut)
  {
    LOOP_UNROLL_MORE( *ptrOut++ = inMap[ UINTDIFF( *ptrIn1++, *ptrIn2++ ) ]; )
  }
}


/*
 * Blur an image using a 3x3 box window average
 *
 * Note this is not isotropic as a box is not a Gaussian convolution kernel.
 * Artifacts are introduced. A box has a definite hard edge and is not the same
 * in all directions. The advantage of the sliding box is faster implementation
 * in software.
 *
 */
void BlurImage33 (Image8_t       *outImg,
                  const Image8_t *inImg)
{
  const size_t   width     = inImg->width;

  const uint8_t *ptrInUp   = inImg->data;
  const uint8_t *ptrInMid  = inImg->data + width;
  const uint8_t *ptrInDown = inImg->data + (width << 1);

  uint16_t accum0, accum1, accum2;

  uint8_t *ptrOut    = outImg->data + width + 1;

  size_t i, j;
  for (i = inImg->height - 2; i; i--)
  {
    /* initialize the sliding box window */
    accum0 = *ptrInUp++;     /* col 0, row 0 */
    accum1 = *ptrInUp++;     /* col 1, row 0 */
    accum0 += *ptrInMid++;   /* col 0, row 1 */
    accum1 += *ptrInMid++;   /* col 1, row 1 */
    accum0 += *ptrInDown++;  /* col 0, row 2 */
    accum1 += *ptrInDown++;  /* col 1, row 2 */

    /* process one row */
    for (j = width - 2; j; j--)
    {
      accum2 = *ptrInUp++;
      accum2 += *ptrInMid++;
      accum2 += *ptrInDown++;

      *ptrOut++ = (accum0 + accum1 + accum2) / 9;

      accum0 = accum1;
      accum1 = accum2;
    }

    ptrOut += 2;
  }
}


/*
 * Blur an image using a modified 3x3 box window average
 *
 * Note this is not isotropic as a box is not a Gaussian convolution kernel.
 * Artifacts are introduced. A box has a definite hard edge and is not the same
 * in all directions. The advantage of the sliding box is faster implementation
 * in software.
 *
 */
void BlurImage33Fast (Image8_t       *outImg,
                      const Image8_t *inImg)
{
  const size_t   width     = inImg->width;

  const uint8_t *ptrInUp   = inImg->data;
  const uint8_t *ptrInMid  = inImg->data + width;
  const uint8_t *ptrInDown = inImg->data + (width << 1);

  uint16_t accum0, accum1, accum2;

  const uint8_t *ptrIn  = inImg->data + width + 1;
  uint8_t       *ptrOut = outImg->data + width + 1;

  size_t i, j;
  for (i = inImg->height - 2; i; i--)
  {
    /* initialize the sliding box window */
    accum0 = *ptrInUp++;     /* col 0, row 0 */
    accum1 = *ptrInUp++;     /* col 1, row 0 */
    accum0 += *ptrInMid++;   /* col 0, row 1 */
    accum1 += *ptrInMid++;   /* col 1, row 1 */
    accum0 += *ptrInDown++;  /* col 0, row 2 */
    accum1 += *ptrInDown++;  /* col 1, row 2 */

    /* process one row */
    for (j = width - 2; j; j--)
    {
      accum2 = *ptrInUp++;
      accum2 += *ptrInMid++;
      accum2 += *ptrInDown++;

      *ptrOut++ = (accum0 + accum1 + accum2 - *ptrIn++) >> 3;  /* div by 8 */

      accum0 = accum1;
      accum1 = accum2;
    }

    ptrIn  += 2;
    ptrOut += 2;
  }
}


/*
 * Integral image transform (as used by Viola and Jones)
 *
 */
void IntegralImage (Image32_t      *outImg,
                    const Image8_t *inImg)
{
  const size_t width  = inImg->width;
  const size_t height = inImg->height;

  size_t  *ptrOut = outImg->data;
  uint8_t  *ptrIn = inImg->data;

  /* first row */
  const uint8_t *endIn = ptrIn + width;
  size_t accum = 0;
  while (ptrIn != endIn)
  {
    LOOP_UNROLL_LESS(
      accum += *ptrIn++;
      *ptrOut++ = accum;
      )
  }

  /* subsequent rows */
  const size_t *ptrLast;
  size_t rowIdx;
  for (rowIdx = height - 1; rowIdx; rowIdx--)
  {
    ptrLast = ptrOut - width;
    endIn   = ptrIn + width;
    accum   = 0;

    while (ptrIn != endIn)
    {
      LOOP_UNROLL_LESS(
        accum += *ptrIn++;
        *ptrOut++ = accum + *ptrLast++;
        )
    }
  }
}


/*
 * Integral image feature of two boxes, one above the other
 *
 * The output image is smaller than the input image for two reasons. One is
 * that the dimensions of the box feature limits travel to the borders of the
 * input image. The box feature window can not move all the way over to the
 * edge without hanging over it. The other reason is the specified step size
 * as the box feature scans over the input image. The window skips positions
 * in the input image. So it consequently is more efficient to store the
 * output image in a packed representation.
 *
 */
void IntegralFeatureUpDown (Image32_t       *outImg,
                            const Image32_t *inImg,
                            const size_t     boxWidth,
                            const size_t     boxHeight,
                            const size_t     colStep,
                            const size_t     rowStep)
{
  const size_t *ptrInUpperLeft   = inImg->data;
  const size_t *ptrInCenterLeft  = inImg->data + boxHeight * inImg->width;
  const size_t *ptrInLowerLeft   = inImg->data
                                       + ((boxHeight * inImg->width) << 1);
  const size_t *ptrInUpperRight  = ptrInUpperLeft + boxWidth;
  const size_t *ptrInCenterRight = ptrInCenterLeft + boxWidth;
  const size_t *ptrInLowerRight  = ptrInLowerLeft + boxWidth;

  const size_t numColSteps = (inImg->width - boxWidth) / colStep;
  const size_t rowOffset   = inImg->width * rowStep - numColSteps * colStep;

  size_t *ptrOut = outImg->data;

  size_t upperBoxSum, lowerBoxSum, colIdx, rowIdx;

  for (rowIdx = (inImg->height - (boxHeight << 1)) / rowStep;
       rowIdx;
       rowIdx--)
  {
    for (colIdx = numColSteps;
         colIdx;
         colIdx--)
    {
      upperBoxSum = *ptrInUpperLeft + *ptrInCenterRight
                        - (*ptrInUpperRight + *ptrInCenterLeft);

      lowerBoxSum = *ptrInCenterLeft + *ptrInLowerRight
                        - (*ptrInCenterRight + *ptrInLowerLeft);

      *ptrOut++ = UINTDIFF(upperBoxSum, lowerBoxSum);

      ptrInUpperLeft   += colStep;
      ptrInCenterLeft  += colStep;
      ptrInLowerLeft   += colStep;
      ptrInUpperRight  += colStep;
      ptrInCenterRight += colStep;
      ptrInLowerRight  += colStep;
    }

    ptrInUpperLeft   += rowOffset;
    ptrInCenterLeft  += rowOffset;
    ptrInLowerLeft   += rowOffset;
    ptrInUpperRight  += rowOffset;
    ptrInCenterRight += rowOffset;
    ptrInLowerRight  += rowOffset;
  }
}


/*
 * Integral image feature of two boxes, one left and the other right
 *
 * This is very much like the IntegralFeatureUpDown() function except that
 * the box window looks for differences horizontally instead of vertically.
 *
 */
void IntegralFeatureLeftRight (Image32_t       *outImg,
                               const Image32_t *inImg,
                               const size_t     boxWidth,
                               const size_t     boxHeight,
                               const size_t     colStep,
                               const size_t     rowStep)
{
  const size_t *ptrInUpperLeft   = inImg->data;
  const size_t *ptrInUpperCenter = inImg->data + boxWidth;
  const size_t *ptrInUpperRight  = inImg->data + (boxWidth << 1);

  const size_t *ptrInLowerLeft   = inImg->data + (boxHeight * inImg->width);
  const size_t *ptrInLowerCenter = ptrInLowerLeft + boxWidth;
  const size_t *ptrInLowerRight  = ptrInLowerLeft + (boxWidth << 1);

  const int numColSteps = (inImg->width - (boxWidth << 1)) / colStep;
  const int rowOffset   = inImg->width * rowStep - numColSteps * colStep;

  size_t *ptrOut = outImg->data;

  size_t leftBoxSum, rightBoxSum, colIdx, rowIdx;

  for (rowIdx = (inImg->height - boxHeight) / rowStep;
       rowIdx;
       rowIdx--)
  {
    for (colIdx = numColSteps;
         colIdx;
         colIdx--)
    {
      leftBoxSum  = *ptrInUpperLeft + *ptrInLowerCenter
                        - (*ptrInUpperCenter + *ptrInLowerLeft);

      rightBoxSum = *ptrInUpperCenter + *ptrInLowerRight
                        - (*ptrInUpperRight + *ptrInLowerCenter);

      *ptrOut++ = UINTDIFF(leftBoxSum, rightBoxSum);

      ptrInUpperLeft   += colStep;
      ptrInUpperCenter += colStep;
      ptrInUpperRight  += colStep;
      ptrInLowerLeft   += colStep;
      ptrInLowerCenter += colStep;
      ptrInLowerRight  += colStep;
    }

    ptrInUpperLeft   += rowOffset;
    ptrInUpperCenter += rowOffset;
    ptrInUpperRight  += rowOffset;
    ptrInLowerLeft   += rowOffset;
    ptrInLowerCenter += rowOffset;
    ptrInLowerRight  += rowOffset;
  }
}


