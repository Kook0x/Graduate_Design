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


#ifndef _EMBEDCV_ECVIO_H_
#define _EMBEDCV_ECVIO_H_


#include <stdio.h>

#include "ecvtypes.h"
#include "ecvutil.h"



/*
 * Buffered reading of bytes from a stream
 *
 * This is the slow but safer version. It returns indication of failure so
 * that seeking to a marker string in a stream does not become stuck in an
 * infinite loop in case of failure.
 *
 */
#ifdef USE_INLINE
static inline
#endif
int ReadByte (uint8_t  *outValue,
              FILE     *inStream,
              Buffer_t *inoutBuffer)
#ifdef USE_INLINE
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
#else
;
#endif


/*
 * Buffered reading of bytes from a stream
 *
 * This is the fast and unsafe version. No indication of success or failure is
 * returned. This should only be used when reading image data. An infinite loop
 * is not possible in the case of I/O error. Only a finite number of pixels
 * will be read in from the stream.
 *
 */
#ifdef USE_INLINE
static inline
#endif
uint8_t ReadByteFast (FILE     *inStream,
                      Buffer_t *inoutBuffer)
#ifdef USE_INLINE
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
#else
;
#endif


/*
 * Seek to a mark string in the stream. This is typically useful for finding
 * the start of an image as they begin with magic strings. If a non-null buffer
 * is passed in, then the stream is saved in the buffer as it is read. This
 * includes the marker string.
 *
 */
void SeekMarker (uint8_t       *markBuffer,
                 const size_t   markLength,
                 const uint8_t *markString,
                 Buffer_t      *outBuffer,      /* optional - may be null */
                 FILE          *stream,
                 Buffer_t      *streamBuffer);


/*
 * Read in a RGB PPM image header from a string. The image dimensions are read
 * and returned. The stream is positioned at the head of the image data after
 * this call.
 *
 */
void ReadPPMHead (size_t   *outWidth,
                  size_t   *outHeight,
                  FILE     *stream,
                  Buffer_t *streamBuffer);


/*
 * Read in the body of a 24 bit binary RGB PPM image
 *
 */
void ReadPPMBody (Image8_t *outRedImg,
                  Image8_t *outGreenImg,
                  Image8_t *outBlueImg,
                  FILE     *stream,
                  Buffer_t *streamBuffer);


/*
 * Read in a RGB PPM image from a stream
 *
 */
void ReadPPMinRGB (Image8_t *outRedImg,
                   Image8_t *outGreenImg,
                   Image8_t *outBlueImg,
                   FILE     *stream,
                   Buffer_t *streamBuffer);


/*
 * Read in a RGB PPM image from a stream and convert to packed 5/6/5 R/G/B
 *
 */
void ReadPPMinRGB565 (Image16_t *outImg,
                      FILE      *stream,
                      Buffer_t  *streamBuffer);



/*
 * Read in a RGB PPM image from a stream and convert to YCbCr
 *
 */
void ReadPPMinYCbCr (Image8_t *outYImg,
                     Image8_t *outCbImg,
                     Image8_t *outCrImg,
                     FILE     *stream,
                     Buffer_t *streamBuffer);


/*
 * Read in a RGB PPM image from a stream and convert to YCbCr in packed format
 *
 */
void ReadPPMinYCbCrPacked (Image8_t  *outYImg,
                           Image16_t *outCbCrImg,
                           FILE      *stream,
                           Buffer_t  *streamBuffer);


/*
 * Write out a RGB PPM image to a stream
 *
 */
void WritePPMinRGB (FILE           *stream,
                    const Image8_t *red,
                    const Image8_t *green,
                    const Image8_t *blue);


/*
 * Write out a 5/6/5 RGB PPM image to a stream
 *
 */
void WritePPMinRGB565 (FILE            *stream,
                       const Image16_t *rgb);



#endif
