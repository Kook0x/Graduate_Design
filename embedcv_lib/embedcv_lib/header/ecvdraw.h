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


#ifndef _EMBEDCV_ECVDRAW_H_
#define _EMBEDCV_ECVDRAW_H_



/*
 * Draw a character into an image
 *
 * Note the font used is "fixed" and there is no bounds checking on the
 * specified column and row coordinates. The backShift value affects the
 * background image pixels. If zero, then nothing is done. If positive,
 * then the background image pixel values are left shifted (made brighter).
 * If negative, then the background image pixel values are right shifted
 * (made dimmer). The amount of the shift is the absolute value of the
 * backShift argument.
 *
 */
void DrawImageChar (Image8_t     *outImg,
                    const char    character,
                    const size_t  column,
                    const size_t  row,
                    const uint8_t foreground,
                    const int     backShift);



/*
 * Draw a string into an image
 *
 * Note the font used is "fixed" and there is no bounds checking on the
 * specified column and row coordinates.
 *
 */
void DrawImageString (Image8_t     *outImg,
                      const char   *text,
                      const size_t  column,
                      const size_t  row,
                      const uint8_t foreground,
                      const int     backShift);



/*
 * Draw a bounding box (no fill) into an image
 *
 */
void DrawImageBoundingBox (Image8_t     *outImg,
                           const size_t  column,
                           const size_t  row,
                           const size_t  boxWidth,
                           const size_t  boxHeight,
                           const uint8_t foreground);



#endif
