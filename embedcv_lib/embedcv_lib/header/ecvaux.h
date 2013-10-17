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


#ifndef _EMBEDCV_ECVAUX_H_
#define _EMBEDCV_ECVAUX_H_


#include <stdio.h>

#include "ecvio.h"
#include "ecvtypes.h"
#include "ecvutil.h"


/*
 * JPEG support is optional
 *
 */
#ifdef USE_JPEG
#include "jpeglib.h"

/*
 * Read in a compressed JPEG image from a stream into a buffer
 *
 * The buffer should be large enough to contain the entire compressed image.
 * For an embedded application, the size of JPEG images is typically known.
 * So it is possible to pre-allocate a buffer sufficiently large to contain
 * the image. But in general, a JPEG may be arbitrarily large.
 *
 */
void ReadJPEGinBuffer (Buffer_t *outJpegBuffer,
                       FILE     *stream,
                       Buffer_t *streamBuffer);

/*
 * Read the JPEG header
 *
 */
void ReadJPEGHead (size_t *outWidth,
                   size_t *outHeight,
                   FILE   *inStream,
                   struct jpeg_decompress_struct *cinfo,
                   struct jpeg_error_mgr         *jerr,
                   J_COLOR_SPACE                  colorspace);

/*
 * Call this after reading in the JPEG header. The body of the JPEG is
 * decompressed from the buffer and output to the three output images.
 *
 */
void ReadJPEGBody (Image8_t       *outImgA,
                   Image8_t       *outImgB,
                   Image8_t       *outImgC,
                   FILE           *inStream,
                   struct jpeg_decompress_struct *cinfo);

/*
 * Read in a JPEG image from a stream with specified colorspace.
 * This is really an internal function as a caller should not have to specify
 * a libjpeg colorspace explicitly but rather use one of the convenience
 * functions below.
 *
 */
void ReadJPEGinColorspace (Image8_t *outImgA,
                           Image8_t *outImgB,
                           Image8_t *outImgC,
                           const Buffer_t *jpegBuffer,
                           J_COLOR_SPACE  colorspace);

/*
 * Read in a JPEG image from a stream as RGB
 *
 */
void ReadJPEGinRGB (Image8_t *outImgA,
                    Image8_t *outImgB,
                    Image8_t *outImgC,
                    const Buffer_t *jpegBuffer);

/*
 * Read in a JPEG image from a stream as YCbCr
 *
 */
void ReadJPEGinYCbCr (Image8_t *outImgA,
                      Image8_t *outImgB,
                      Image8_t *outImgC,
                      const Buffer_t *jpegBuffer);
#endif



#endif
