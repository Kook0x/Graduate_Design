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



#include <stdio.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

#include "ecvcommon.h"
#include "ecvio.h"



/*
 * Buffered reading of bytes from a stream (safe version)
 *
 */
#ifndef USE_INLINE
int ReadByte (uint8_t  *outValue,
              FILE     *inStream,
              Buffer_t *inoutBuffer)
{
  if ( (inoutBuffer->position == inoutBuffer->size) && (! feof(inStream)) )
  {
    inoutBuffer->position = inoutBuffer->head;
    inoutBuffer->size = inoutBuffer->head +
                        fread(inoutBuffer->head,
                              sizeof(uint8_t),
                              inoutBuffer->tail - inoutBuffer->head,
                              inStream);
  }

  if (inoutBuffer->position < inoutBuffer->size)
  {
    *outValue = *inoutBuffer->position++;
    return 1;
  }
  else
  {
    return 0;
  }
}
#endif


/*
 * Buffered reading of bytes from a stream (unsafe version)
 *
 */
#ifndef USE_INLINE
uint8_t ReadByteFast (FILE     *inStream,
                      Buffer_t *inoutBuffer)
{
  if ( (inoutBuffer->position == inoutBuffer->size) && (! feof(inStream)) )
  {
    inoutBuffer->position = inoutBuffer->head;
    inoutBuffer->size = inoutBuffer->head +
                        fread(inoutBuffer->head,
                              sizeof(uint8_t),
                              inoutBuffer->tail - inoutBuffer->head,
                              inStream);
  }

  if (inoutBuffer->position < inoutBuffer->size)
  {
    return *inoutBuffer->position++;
  }
  else
  {
    return 0;
  }
}
#endif


/*
 * Read from a stream until a marker string is encountered
 *
 */
void SeekMarker (uint8_t       *markBuffer,    /* one element smaller */
                 const size_t   markLength,
                 const uint8_t *markString,
                 Buffer_t      *outBuffer,     /* optional - may be null */
                 FILE          *stream,
                 Buffer_t      *streamBuffer)
{
  const uint8_t  tailMarkChar = *(markString + markLength - 1);

  const uint8_t *endMarkBuffer = markBuffer + markLength - 2;

  uint8_t       *ptrMarkBuffer,
                *nextMarkBuffer,
                *ptrSize;

  uint8_t value;  /* most recently read byte */

  /* previously read bytes go in the markBuffer */
  memset(markBuffer, 0, sizeof(uint8_t) * (markLength - 1));

  if (outBuffer)
  {
    ptrSize = (uint8_t *) outBuffer->size;  /* cast away constness */

    while (1)
    {
      if (! ReadByte(&value, stream, streamBuffer))
      {
        break;
      }

      /* expect output buffer is large enough to contain all bytes read */
      if (ptrSize != outBuffer->tail)
      {
        *ptrSize++ = value;
      }

      if ( (value == tailMarkChar) &&
           (! memcmp(markBuffer, markString, markLength - 1)) )
      {
        break;
      }

      ptrMarkBuffer  = markBuffer;
      nextMarkBuffer = markBuffer + 1;
      while (ptrMarkBuffer != endMarkBuffer)
      {
        *ptrMarkBuffer++ = *nextMarkBuffer++;
      }
      *ptrMarkBuffer = value;
    }

    outBuffer->size = ptrSize;
  }
  else
  {
    while (1)
    {
      if (! ReadByte(&value, stream, streamBuffer))
      {
        break;
      }

      if ( (value == tailMarkChar) &&
           (! memcmp(markBuffer, markString, markLength - 1)) )
      {
        break;
      }

      ptrMarkBuffer  = markBuffer;
      nextMarkBuffer = markBuffer + 1;
      while (ptrMarkBuffer != endMarkBuffer)
      {
        *ptrMarkBuffer++ = *nextMarkBuffer++;
      }
      *ptrMarkBuffer = value;
    }
  }
}


/*
 * Read in a RGB PPM image header from a string.
 *
 */
void ReadPPMHead (size_t *outWidth,
                  size_t *outHeight,
                  FILE     *stream,
                  Buffer_t *streamBuffer)
{
  const uint8_t PPM_BEGIN [] = { '2', '5', '5', '\n' };
  uint8_t markBuffer[sizeof(PPM_BEGIN) / sizeof(uint8_t) - 1];

  /* 64 bytes should be adequate */
  READBUFFER( headBuffer, 64 )

  SeekMarker(markBuffer,
             sizeof(PPM_BEGIN) / sizeof(uint8_t),
             PPM_BEGIN,
             &headBuffer,
             stream,
             streamBuffer);

  /* start looking from start of marker */
  const uint8_t *headBuf = headBuffer.head;
  const uint8_t *ptrBuf  = headBuffer.size
                           - sizeof(PPM_BEGIN) / sizeof(uint8_t)
                           - 1;  /* skip past 255\n */

  /* read backwards until first digit character */
  while ( (ptrBuf > headBuf) && ((*ptrBuf < '0') || (*ptrBuf > '9')) )
  {
    ptrBuf--;
  }

  /* read backwards until first non-digit character */
  while ( (ptrBuf > headBuf) && (*ptrBuf >= '0') && (*ptrBuf <= '9') )
  {
    ptrBuf--;
  }

  /* height */
  *outHeight = atoi((const char *)ptrBuf + 1);

  /* read backwards until first digit character */
  while ( (ptrBuf > headBuf) && ((*ptrBuf < '0') || (*ptrBuf > '9')) )
  {
    ptrBuf--;
  }

  /* read backwards until first non-digit character */
  while ( (ptrBuf > headBuf) && (*ptrBuf >= '0') && (*ptrBuf <= '9') )
  {
    ptrBuf--;
  }

  /* width */
  *outWidth = atoi((const char *)ptrBuf + 1);
}


/*
 * Read in the body of a 24 bit binary RGB PPM image
 *
 */
void ReadPPMBody (Image8_t *outRedImg,
                  Image8_t *outGreenImg,
                  Image8_t *outBlueImg,
                  FILE     *stream,
                  Buffer_t *streamBuffer)
{
  const uint8_t *endRed   = outRedImg->data
                            + outRedImg->width * outRedImg->height;
  uint8_t       *ptrRed   = outRedImg->data;

  uint8_t       *ptrGreen = outGreenImg->data;
  uint8_t       *ptrBlue  = outBlueImg->data;

  while (ptrRed != endRed)
  {
    LOOP_UNROLL_LESS(
      *ptrRed++   = ReadByteFast(stream, streamBuffer);
      *ptrGreen++ = ReadByteFast(stream, streamBuffer);
      *ptrBlue++  = ReadByteFast(stream, streamBuffer);
      )
  }
}


/*
 * PPM image files are presumed to be the "P6" kind 24 bit RGB. The marker
 * string before the image data is "255\n". This really is not very safe.
 * The same string could appear in the header as the height of the image.
 * It could also appear in the body of the image data. However, in practice,
 * this (almost) never happens.
 *
 * Another limitation is that the PPM image dimensions are ignored. The
 * width and height are taken from the pre-initialized output image buffer
 * for the red channel image.
 *
 */
void ReadPPMinRGB (Image8_t *outRedImg,
                   Image8_t *outGreenImg,
                   Image8_t *outBlueImg,
                   FILE     *stream,
                   Buffer_t *streamBuffer)
{
  const uint8_t PPM_BEGIN [] = { '2', '5', '5', '\n' };
  uint8_t markBuffer[sizeof(PPM_BEGIN) / sizeof(uint8_t) - 1];

  SeekMarker(markBuffer,
             sizeof(PPM_BEGIN) / sizeof(uint8_t),
             PPM_BEGIN,
             0,
             stream,
             streamBuffer);

  const uint8_t *endRed   = outRedImg->data
                            + outRedImg->width * outRedImg->height;
  uint8_t       *ptrRed   = outRedImg->data;

  uint8_t       *ptrGreen = outGreenImg->data;
  uint8_t       *ptrBlue  = outBlueImg->data;

  while (ptrRed != endRed)
  {
    LOOP_UNROLL_LESS(
      *ptrRed++   = ReadByteFast(stream, streamBuffer);
      *ptrGreen++ = ReadByteFast(stream, streamBuffer);
      *ptrBlue++  = ReadByteFast(stream, streamBuffer);
      )
  }
}


/*
 * This is identical of ReadPPMinRGB() except the output image is a 16 bit
 * per pixel 5/6/5 RGB image. The low 5 bits are red, the middle 6 bits are
 * green, and the upper 5 bits are blue.
 *
 */
void ReadPPMinRGB565 (Image16_t *outImg,
                      FILE      *stream,
                      Buffer_t  *streamBuffer)
{
  const uint8_t PPM_BEGIN [] = { '2', '5', '5', '\n' };
  uint8_t markBuffer[sizeof(PPM_BEGIN) / sizeof(uint8_t) - 1];

  SeekMarker(markBuffer,
             sizeof(PPM_BEGIN) / sizeof(uint8_t),
             PPM_BEGIN,
             0,
             stream,
             streamBuffer);

  const uint16_t *endImg = outImg->data + outImg->width * outImg->height;
  uint16_t       *ptrImg = outImg->data;

  while (ptrImg != endImg)
  {
    LOOP_UNROLL_LESS(
      /* 5 bits red */
      *ptrImg = (ReadByteFast(stream, streamBuffer) >> 3);
      /* 6 bits green */
      *ptrImg |= (((uint16_t)ReadByteFast(stream, streamBuffer) >> 2) << 5);
      /* 5 bits blue */
      *ptrImg |= (((uint16_t)ReadByteFast(stream, streamBuffer) >> 3) << 11);
      ptrImg++;
      )
  }
}


/*
 * This is the same as ReadPPMinRGB() except that the RGB pixel triples are
 * converted to YCbCr as they are input.
 *
 */
void ReadPPMinYCbCr (Image8_t *outYImg,
                     Image8_t *outCbImg,
                     Image8_t *outCrImg,
                     FILE     *stream,
                     Buffer_t *streamBuffer)
{
  const uint8_t PPM_BEGIN [] = { '2', '5', '5', '\n' };
  uint8_t markBuffer[sizeof(PPM_BEGIN) / sizeof(uint8_t) - 1];

  SeekMarker(markBuffer,
             sizeof(PPM_BEGIN) / sizeof(uint8_t),
             PPM_BEGIN,
             0,
             stream,
             streamBuffer);

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
                   ReadByteFast(stream, streamBuffer),
                   ReadByteFast(stream, streamBuffer),
                   ReadByteFast(stream, streamBuffer));
      )
  }
}


/*
 * This is the same as ReadPPMinRGBtoYCbCr() except that the Cb and Cr chroma
 * components are packed into a 16 bit image.
 *
 */
void ReadPPMinYCbCrPacked (Image8_t  *outYImg,
                           Image16_t *outCbCrImg,
                           FILE      *stream,
                           Buffer_t  *streamBuffer)
{
  const uint8_t PPM_BEGIN [] = { '2', '5', '5', '\n' };
  uint8_t markBuffer[sizeof(PPM_BEGIN) / sizeof(uint8_t) - 1];

  SeekMarker(markBuffer,
             sizeof(PPM_BEGIN) / sizeof(uint8_t),
             PPM_BEGIN,
             0,
             stream,
             streamBuffer);

  const uint8_t *endLuma = outYImg->data + outYImg->width * outYImg->height;
  uint8_t       *ptrLuma = outYImg->data;

  PackedCbCr_t  *ptrCbCr = (PackedCbCr_t *) outCbCrImg->data;

  while (ptrLuma != endLuma)
  {
    LOOP_UNROLL_LESS(
      YCbCrFromRGB(ptrLuma++,
                   &ptrCbCr->data[0],
                   &ptrCbCr->data[1],
                   ReadByteFast(stream, streamBuffer),
                   ReadByteFast(stream, streamBuffer),
                   ReadByteFast(stream, streamBuffer));
      ptrCbCr++;
      )
  }
}


/*
 * Write to a stream three images as the red, green and blue channels of a
 * 24 bit color PPM image
 *
 */
void WritePPMinRGB (FILE           *stream,
                    const Image8_t *red,
                    const Image8_t *green,
                    const Image8_t *blue)
{
  const size_t width  = red->width;
  const size_t height = red->height;
  const uint8_t *ptrRed   = red->data;
  const uint8_t *endRed   = red->data + width * height;
  const uint8_t *ptrGreen = green->data;
  const uint8_t *ptrBlue  = blue->data;

  fprintf(stream, "P6\n%u %u\n255\n", width, height);

  while (ptrRed != endRed)
  {
    LOOP_UNROLL_LESS(
      fprintf(stream, "%c%c%c", *ptrRed++, *ptrGreen++, *ptrBlue++);
      )
  }
}


/*
 * Write to a stream three images as the red, green and blue channels of a
 * 24 bit color PPM image
 *
 */
void WritePPMinRGB565 (FILE            *stream,
                       const Image16_t *rgb)
{
  const size_t width  = rgb->width;
  const size_t height = rgb->height;
  const uint16_t *ptrImg = rgb->data;
  const uint16_t *endImg = rgb->data + width * height;

  fprintf(stream, "P6\n%u %u\n255\n", width, height);

  while (ptrImg != endImg)
  {
    LOOP_UNROLL_LESS(
      fprintf(stream,
              "%c%c%c",
              (*ptrImg & 0x1f) << 3,
              ((*ptrImg >> 5) & 0x3f) << 2,
              ((*ptrImg >> 11) & 0x1f) << 3);
      ptrImg++;
      )
  }
}


