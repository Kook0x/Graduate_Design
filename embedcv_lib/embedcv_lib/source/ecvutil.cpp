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

#include "types.h"
#include "ecvutil.h"



/*
 *
 */
#ifndef USE_INLINE
size_t UintSqrt(size_t value)
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
#endif


/*
 *
 */
#ifndef USE_INLINE
size_t CbCrSSD(const uint16_t v1, const uint16_t v2)
{
  PackedCbCr_t t1, t2;
  t1.CbCr=v1;
  t2.CbCr=v2;
  
  const uint16_t diff1 = UINTDIFF( t1.data[0],
                                   t2.data[0] );
  const uint16_t diff2 = UINTDIFF( t1.data[1],
                                   t2.data[1] );
  return diff1 * diff1 + diff2 * diff2;
}
#endif


/*
 * From: http://en.wikipedia.org/wiki/YCbCr
 *
JPEG-YCbCr (601) from "digital 8-bit R'G'B'  "
========================================================================
Y' =       + 0.299    * R'd + 0.587    * G'd + 0.114    * B'd
Cb = 128   - 0.168736 * R'd - 0.331264 * G'd + 0.5      * B'd
Cr = 128   + 0.5      * R'd - 0.418688 * G'd - 0.081312 * B'd
........................................................................
R'd, G'd, B'd   in {0, 1, 2, ..., 255}
Y', Cb, Cr      in {0, 1, 2, ..., 255}
 *
 */
#ifndef USE_INLINE
void YCbCrFromRGB (uint8_t *outY,
                   uint8_t *outCb,
                   uint8_t *outCr,
                   const uint8_t inRed,
                   const uint8_t inGreen,
                   const uint8_t inBlue)
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
#endif

