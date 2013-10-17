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
#include <stdlib.h>
#include <string.h>

#include "types.h"

#include "ecvtypes.h"



/*
 * Fixed font is 6 pixels wide by 10 pixels tall with unset pixels along the
 * rightmost column and bottom row (with a few exceptions). Store the alphabet
 * and digit characters as bitmaps here. There is a total of 60 bits for every
 * character.
 *
 * 64 bit integer values appear to cause compiler problems on some 32 bit
 * platforms. So to be safe, each bitmap is stored in two 32 bit unsigned
 * integers. The low 32 bits corresponds to the top half of the character.
 * The high 32 bits, which are read after the low, correspond to the bottom
 * half.
 *
 * The LSB corresponds to the upper left bit of the font. As bits are right
 * shifted off, the font pixel moves left to right, top to bottom. This
 * effectively means that all of the binary numbers are reversed and read
 * backwards.
 *
 */
#define FIXED_LOW_A 0xD1451284
#define FIXED_LOW_B 0x8E49248F
#define FIXED_LOW_C 0x4104144E
#define FIXED_LOW_D 0x9249248F
#define FIXED_LOW_E 0x4F04105F
#define FIXED_LOW_F 0x4F04105F
#define FIXED_LOW_G 0x4104144E
#define FIXED_LOW_H 0x5F451451
#define FIXED_LOW_I 0x0410410E
#define FIXED_LOW_J 0x0820821C
#define FIXED_LOW_K 0x43149451
#define FIXED_LOW_L 0x41041041
#define FIXED_LOW_M 0x5555B451
#define FIXED_LOW_N 0x555534D1
#define FIXED_LOW_O 0x5145144E
#define FIXED_LOW_P 0x4F45144F
#define FIXED_LOW_Q 0x5145144E
#define FIXED_LOW_R 0x4F45144F
#define FIXED_LOW_S 0x0E04144E
#define FIXED_LOW_T 0x0410411F
#define FIXED_LOW_U 0x51451451
#define FIXED_LOW_V 0x8A451451
#define FIXED_LOW_W 0x55451451
#define FIXED_LOW_X 0x8428A451
#define FIXED_LOW_Y 0x0428A451
#define FIXED_LOW_Z 0x8420841F
#define FIXED_LOW_0 0x51451284
#define FIXED_LOW_1 0x04105184
#define FIXED_LOW_2 0x0841144E
#define FIXED_LOW_3 0x0E10841F
#define FIXED_LOW_4 0x4A28C208
#define FIXED_LOW_5 0x1334105F
#define FIXED_LOW_6 0x4F04144E
#define FIXED_LOW_7 0x0420841F
#define FIXED_LOW_8 0x4E45144E
#define FIXED_LOW_9 0x1E45144E
#define FIXED_LOW_MINUS  0x1F000000
#define FIXED_LOW_PLUS   0x1F104000
#define FIXED_LOW_PERIOD 0x00000000

#define FIXED_HIGH_A 0x0114517
#define FIXED_HIGH_B 0x00F4924
#define FIXED_HIGH_C 0x00E4410
#define FIXED_HIGH_D 0x00F4924
#define FIXED_HIGH_E 0x01F0410
#define FIXED_HIGH_F 0x0010410
#define FIXED_HIGH_G 0x00E4516
#define FIXED_HIGH_H 0x0114514
#define FIXED_HIGH_I 0x00E1041
#define FIXED_HIGH_J 0x0062482
#define FIXED_HIGH_K 0x0114491
#define FIXED_HIGH_L 0x01F0410
#define FIXED_HIGH_M 0x0114514
#define FIXED_HIGH_N 0x0114596
#define FIXED_HIGH_O 0x00E4514
#define FIXED_HIGH_P 0x0010410
#define FIXED_HIGH_Q 0x40E5514
#define FIXED_HIGH_R 0x0114491
#define FIXED_HIGH_S 0x00E4504
#define FIXED_HIGH_T 0x0041041
#define FIXED_HIGH_U 0x00E4514
#define FIXED_HIGH_V 0x00410A2
#define FIXED_HIGH_W 0x00A5555
#define FIXED_HIGH_X 0x01144A2
#define FIXED_HIGH_Y 0x0041041
#define FIXED_HIGH_Z 0x01F0420
#define FIXED_HIGH_0 0x0042914
#define FIXED_HIGH_1 0x01F1041
#define FIXED_HIGH_2 0x01F0421
#define FIXED_HIGH_3 0x00E4504
#define FIXED_HIGH_4 0x00821F2
#define FIXED_HIGH_5 0x00E4504
#define FIXED_HIGH_6 0x00E4514
#define FIXED_HIGH_7 0x0020821
#define FIXED_HIGH_8 0x00E4514
#define FIXED_HIGH_9 0x00E4504
#define FIXED_HIGH_MINUS  0x0000000
#define FIXED_HIGH_PLUS   0x0000041
#define FIXED_HIGH_PERIOD 0x10E1000

/*
 * Every platform except for EBCDIC mainframes uses ASCII 7 so it is safe to
 * use a simple map from characters to the font bitmaps.
 *
 */
static const uint64_t font_low_map[128] =
{
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, /* 0 to 9 */
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, /* 10 to 19 */
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, /* 20 to 29 */
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, /* 30 to 39 */

  /* 40 to 47 */
  0x0, 0x0, 0x0, FIXED_LOW_PLUS, 0x0, FIXED_LOW_MINUS, FIXED_LOW_PERIOD, 0x0,

  FIXED_LOW_0, /* 48 decimal */
  FIXED_LOW_1,
  FIXED_LOW_2,
  FIXED_LOW_3,
  FIXED_LOW_4,
  FIXED_LOW_5,
  FIXED_LOW_6,
  FIXED_LOW_7,
  FIXED_LOW_8,
  FIXED_LOW_9, /* 57 decimal */

  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                /* 58 to 64 */

  FIXED_LOW_A, /* 65 decimal */
  FIXED_LOW_B,
  FIXED_LOW_C,
  FIXED_LOW_D,
  FIXED_LOW_E,
  FIXED_LOW_F,
  FIXED_LOW_G,
  FIXED_LOW_H,
  FIXED_LOW_I,
  FIXED_LOW_J,
  FIXED_LOW_K,
  FIXED_LOW_L,
  FIXED_LOW_M,
  FIXED_LOW_N,
  FIXED_LOW_O,
  FIXED_LOW_P,
  FIXED_LOW_Q,
  FIXED_LOW_R,
  FIXED_LOW_S,
  FIXED_LOW_T,
  FIXED_LOW_U,
  FIXED_LOW_V,
  FIXED_LOW_W,
  FIXED_LOW_X,
  FIXED_LOW_Y,
  FIXED_LOW_Z, /* 90 decimal */

  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,      /* 91 to 99 */
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, /* 100 to 109 */
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, /* 110 to 119 */
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0            /* 120 to 127 */
};

static const uint64_t font_high_map[128] =
{
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, /* 0 to 9 */
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, /* 10 to 19 */
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, /* 20 to 29 */
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, /* 30 to 39 */

  /* 40 to 47 */
  0x0, 0x0, 0x0, FIXED_HIGH_PLUS, 0x0, FIXED_HIGH_MINUS, FIXED_HIGH_PERIOD, 0x0,

  FIXED_HIGH_0, /* 48 decimal */
  FIXED_HIGH_1,
  FIXED_HIGH_2,
  FIXED_HIGH_3,
  FIXED_HIGH_4,
  FIXED_HIGH_5,
  FIXED_HIGH_6,
  FIXED_HIGH_7,
  FIXED_HIGH_8,
  FIXED_HIGH_9, /* 57 decimal */

  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,                /* 58 to 64 */

  FIXED_HIGH_A, /* 65 decimal */
  FIXED_HIGH_B,
  FIXED_HIGH_C,
  FIXED_HIGH_D,
  FIXED_HIGH_E,
  FIXED_HIGH_F,
  FIXED_HIGH_G,
  FIXED_HIGH_H,
  FIXED_HIGH_I,
  FIXED_HIGH_J,
  FIXED_HIGH_K,
  FIXED_HIGH_L,
  FIXED_HIGH_M,
  FIXED_HIGH_N,
  FIXED_HIGH_O,
  FIXED_HIGH_P,
  FIXED_HIGH_Q,
  FIXED_HIGH_R,
  FIXED_HIGH_S,
  FIXED_HIGH_T,
  FIXED_HIGH_U,
  FIXED_HIGH_V,
  FIXED_HIGH_W,
  FIXED_HIGH_X,
  FIXED_HIGH_Y,
  FIXED_HIGH_Z, /* 90 decimal */

  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,      /* 91 to 99 */
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, /* 100 to 109 */
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, /* 110 to 119 */
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0            /* 120 to 127 */
};


/*
 * Draw a character into an image
 *
 * This is not very efficiently done. However, it is probably good enough as
 * drawing characters is only used for annotating images.
 *
 */
void DrawImageChar (Image8_t     *outImg,
                    const char    character,  /* must be ASCII 7 */
                    const size_t  column,
                    const size_t  row,
                    const uint8_t foreground,
                    const int     shiftBack)
{
  const size_t numShifts     = abs(shiftBack);
  const int    dimBackground = shiftBack < 0;

  const size_t   width       = outImg->width;
  const size_t   rowOffset   = outImg->width - 6;
  uint8_t       *ptrOut      = outImg->data + row * outImg->width + column;
  const uint8_t *endRow      = ptrOut + 6;

  size_t counter;

  /* upper half of character is in low 32 bits */
  uint32_t charLowBits  = font_low_map[(size_t) character];
  for (counter = 32; counter; counter--)
  {
    if (charLowBits & 0x1)
    {
      *ptrOut = foreground;
    }
    else if (numShifts)
    {
      if (dimBackground)
      {
        /* dim the background, shift to the right */
        *ptrOut >>= numShifts;
      }
      else
      {
        /* brighten the background, shift to the left */
        *ptrOut <<= numShifts;
      }
    }

    if (++ptrOut == endRow)
    {
      ptrOut += rowOffset;
      endRow += width;
    }

    charLowBits >>= 1;
  }

  /* lower half of character is in high 32 bits */
  uint32_t charHighBits = font_high_map[(size_t) character];
  for (counter = 28; counter; counter--) /* only 28 bits remaining out of 60 */
  {
    if (charHighBits & 0x1)
    {
      *ptrOut = foreground;
    }
    else if (numShifts)
    {
      if (dimBackground)
      {
        /* dim the background, shift to the right */
        *ptrOut >>= numShifts;
      }
      else
      {
        /* brighten the background, shift to the left */
        *ptrOut <<= numShifts;
      }
    }

    if (++ptrOut == endRow)
    {
      ptrOut += rowOffset;
      endRow += width;
    }

    charHighBits >>= 1;
  }
}


/*
 * Draw a string into an image
 *
 */
void DrawImageString (Image8_t     *outImg,
                      const char   *text,
                      const size_t  column,
                      const size_t  row,
                      const uint8_t foreground,
                      const int     shiftBack)
{
  const char *endChar = text + strlen(text);
  const char *ptrChar = text;

  size_t col = column;

  while (ptrChar != endChar)
  {
    DrawImageChar( outImg, *ptrChar++, col, row, foreground, shiftBack );
    col += 6;
  }
}


/*
 * Draw a bounding box (no fill) into an image
 *
 */
void DrawImageBoundingBox (Image8_t     *outImg,
                           const size_t  column,
                           const size_t  row,
                           const size_t  boxWidth,
                           const size_t  boxHeight,
                           const uint8_t foreground)
{
  const size_t width     = outImg->width;
  const size_t rowOffset = outImg->width - boxWidth;

  uint8_t *ptrOut = outImg->data + row * outImg->width + column;

  size_t idx;

  /* top line */
  for (idx = boxWidth; idx; idx--)
  {
    *ptrOut++ = foreground;
  }

  /* left and right lines */
  ptrOut += rowOffset;
  uint8_t *ptrRight = ptrOut + boxWidth - 1;
  for (idx = boxHeight - 2; idx; idx--)
  {
    *ptrOut = *ptrRight = foreground;
    ptrOut   += width;
    ptrRight += width;
  }

  /* bottom line */
  for (idx = boxWidth; idx; idx--)
  {
    *ptrOut++ = foreground;
  }
}


