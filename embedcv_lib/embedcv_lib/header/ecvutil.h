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


#ifndef _EMBEDCV_ECVUTIL_H_
#define _EMBEDCV_ECVUTIL_H_


#include <stddef.h>

#include "types.h"
#include "ecvtypes.h"



/*
 * Often need the absolute difference between two unsigned integers
 *
 */
#define UINTDIFF( v1, v2 ) ( (v1 < v2) ? (v2 - v1) : (v1 - v2) )


/*
 * Integer square roots using Newton's method
 *
 */
#ifdef USE_INLINE
static inline
#endif
size_t UintSqrt(size_t value)
#ifdef USE_INLINE
{
  size_t last, current;

  if (value == 0)
  {
    return 0;
  }
  else if (value == 1)
  {
    return 1;
  }
  else
  {
    /* initial guess is half of the squared value */
    current = value >> 1;

    do
    {
      last = current;

      /* next guess */
      current = ( last + value / last ) >> 1;
    }
    while ( last != current && UINTDIFF( last, current ) != 1 );

    return current;
  }
}
#else
;
#endif


/*
 * The sum norm (one norm) distance between two packed 16 bit CbCr pixels.
 * This is the sum of the absolute differences for Cb and Cr taken
 * individually.
 *
 */
#define CBCR1DIST( v1, v2 ) \
( UINTDIFF( ((PackedCbCr_t)v1).data[0], ((PackedCbCr_t)v2).data[0] ) \
  + UINTDIFF( ((PackedCbCr_t)v1).data[1], ((PackedCbCr_t)v2).data[1] ) )


/*
 * Sum of squared differences between two packed 16 bit CbCr pixels.
 * This is the square of the Euclidean (two norm) distance. This is more
 * convenient sometimes as it avoids a square root.
 *
 */
#ifdef USE_INLINE
static inline
#endif
size_t CbCrSSD(const uint16_t v1, const uint16_t v2)
#ifdef USE_INLINE
{
  const uint16_t diff1 = UINTDIFF( ((PackedCbCr_t)v1).data[0],
                                   ((PackedCbCr_t)v2).data[0] );
  const uint16_t diff2 = UINTDIFF( ((PackedCbCr_t)v1).data[1],
                                   ((PackedCbCr_t)v2).data[1] );
  return diff1 * diff1 + diff2 * diff2;
}
#else
;
#endif


/*
 * Euclidean (two norm) distance between two packed 16 bit CbCr pixels.
 *
 */
#define CBCR2DIST( v1, v2 ) UintSqrt( CbCrSSD( v1, v2 ) )


/*
 * Euclidean (two norm) measure of a packed 16 bit CbCr pixel
 *
 */
#define CBCR2NORM( v ) CBCR2DIST( 0, v )


/*
 * Convert 8 bit RGB to YCbCr
 *
 * Typically the color image source is either 16 bits or 24 bits per pixel. In
 * the 16 bit case, likely RGB is represented by 5/6/5 bits. In the 24 bit
 * case, RGB is represented by three bytes. Either way, YCbCr is often more
 * useful for computer vision so conversion from RGB should be done as each
 * pixel is read in.
 *
 * This is a small function used in inner loops so benefits from inlining, if
 * available. The default is not to inline so it is sure to build for everyone.
 * But any C99 compiler should be ok with inlining. Some older C89 compilers
 * may also support inlining.
 *
 */
#ifdef USE_INLINE
static inline
#endif
void YCbCrFromRGB (uint8_t *outY,          /* luminance */
                   uint8_t *outCb,         /* chroma blue */
                   uint8_t *outCr,         /* chroma red */
                   const uint8_t inRed,
                   const uint8_t inGreen,
                   const uint8_t inBlue)
#ifdef USE_INLINE
{
  size_t tmpY, tmpCb, tmpCr;

  tmpY   = inRed * 299;
  tmpCb  = 128000000 - 168736 * inRed;
  tmpCr  = 128000000 + 500000 * inRed;

  tmpY  += 587 * inGreen;
  tmpCb -= 331264 * inGreen;
  tmpCr -= 418688 * inGreen;

  tmpY  += 114 * inBlue;
  tmpCb += 500000 * inBlue;
  tmpCr -= 81312 * inBlue;

  *outY  = tmpY / 1000;
  *outCb = tmpCb / 1000000;
  *outCr = tmpCr / 1000000;
}
#else
;
#endif



#endif
