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



#define _GNU_SOURCE

#include <stdio.h>
#include <stddef.h>


#include "types.h"
#include "ecvio.h"

#include "ecvaux.h"

/*
 * JPEG support is optional
 *
 */
#ifdef USE_JPEG
#include <jpeglib.h>


/*
 * Load the buffer with a compressed JPEG image
 *
 * This is a bit different from the uncompressed bitmap case (i.e. PPM files)
 * as there is a true decompression step required. That motivates an extra
 * buffering stage.
 *
 */
void ReadJPEGinBuffer (Buffer_t *outJpegBuffer,
                       FILE     *stream,
                       Buffer_t *streamBuffer)
{
  const uint8_t JPEG_BEGIN [] = { 0xff, 0xd8, 0xff, 0xe0 };
  uint8_t markBuffer[sizeof(JPEG_BEGIN) / sizeof(uint8_t) - 1];

  uint8_t *outData        = outJpegBuffer->head;
  outJpegBuffer->position = outJpegBuffer->head;
  outJpegBuffer->size     = outJpegBuffer->head + 4;

  outData[0] = 0xff;
  outData[1] = 0xd8;
  outData[2] = 0xff;
  outData[3] = 0xe0;
 
  SeekMarker(markBuffer,
             sizeof(JPEG_BEGIN) / sizeof(uint8_t),
             JPEG_BEGIN,
             outJpegBuffer,
             stream,
             streamBuffer);
}


/*
 * Read the JPEG header
 *
 */
void ReadJPEGHead (size_t *outWidth,
                   size_t *outHeight,
                   FILE   *inStream,
                   struct jpeg_decompress_struct *cinfo,
                   struct jpeg_error_mgr         *jerr,
                   J_COLOR_SPACE                  colorspace)
{
  cinfo->err                  = jpeg_std_error(jerr);
  jpeg_create_decompress(cinfo);
  jpeg_stdio_src(cinfo, inStream);
  jpeg_read_header(cinfo, TRUE);

  cinfo->dither_mode          = JDITHER_NONE;
  cinfo->dct_method           = JDCT_IFAST;
  cinfo->do_fancy_upsampling  = FALSE;
  cinfo->two_pass_quantize    = FALSE;
  cinfo->quantize_colors      = FALSE;
  cinfo->out_color_space      = colorspace;

  jpeg_start_decompress(cinfo);

  *outWidth  = cinfo->output_width;
  *outHeight = cinfo->output_height;
}


/*
 * Call this after reading in the JPEG header. The body of the JPEG is
 * decompressed from the buffer and output to the three output images.
 *
 */
void ReadJPEGBody (Image8_t       *outImgA,
                   Image8_t       *outImgB,
                   Image8_t       *outImgC,
                   FILE           *inStream,
                   struct jpeg_decompress_struct *cinfo)
{
  /* JPEG buffer */
  size_t     rowStride = cinfo->output_width * cinfo->output_components;
  JSAMPARRAY buffer    = (cinfo->mem->alloc_sarray)
                           ( (j_common_ptr) cinfo, JPOOL_IMAGE, rowStride, 1);

  /* decompress JPEG and write to output stream */
  uint8_t *outDataA = outImgA->data;
  uint8_t *outDataB = outImgB->data;
  uint8_t *outDataC = outImgC->data;
  uint8_t *bufptr;
  size_t i;
  while (cinfo->output_scanline < cinfo->output_height)
  {
    jpeg_read_scanlines(cinfo, buffer, 1);
    bufptr = buffer[0];
    for (i = rowStride / 3; i; i-- )
    {
      *outDataA++ = *bufptr++;
      *outDataB++ = *bufptr++;
      *outDataC++ = *bufptr++;
    }
  }

  /* cleanup */
  jpeg_finish_decompress(cinfo);
  jpeg_destroy_decompress(cinfo);
}


/*
 * Decompress the compressed JPEG image in the buffer according to the
 * specified colorspace
 *
 */
void ReadJPEGinColorspace (Image8_t *outImgA,
                           Image8_t *outImgB,
                           Image8_t *outImgC,
                           const Buffer_t *jpegBuffer,
                           J_COLOR_SPACE  colorspace)
{
  /* memory stream for JPEG buffer (non GNU libc may not have this) */
  FILE *fin = fmemopen(jpegBuffer->head,
                       jpegBuffer->size - jpegBuffer->head,
                       "r");

  /* JPEG decompression settings */
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr         jerr;

  size_t imgWidth, imgHeight;
  ReadJPEGHead (&imgWidth, &imgHeight, fin, &cinfo, &jerr, colorspace);
  ReadJPEGBody (outImgA, outImgB, outImgC, fin, &cinfo);

  /* close memory stream */
  fclose(fin);
}


/*
 * Decompress a compressed JPEG as 24 bit RGB
 *
 */
void ReadJPEGinRGB (Image8_t *outImgA,
                    Image8_t *outImgB,
                    Image8_t *outImgC,
                    const Buffer_t *jpegBuffer)
{
  ReadJPEGinColorspace(outImgA, outImgB, outImgC, jpegBuffer, JCS_RGB);
}


/*
 * Decompress a compressed JPEG as 24 bit YCbCr
 *
 */
void ReadJPEGinYCbCr (Image8_t *outImgA,
                      Image8_t *outImgB,
                      Image8_t *outImgC,
                      const Buffer_t *jpegBuffer)
{
  ReadJPEGinColorspace(outImgA, outImgB, outImgC, jpegBuffer, JCS_YCbCr);
}


#endif


