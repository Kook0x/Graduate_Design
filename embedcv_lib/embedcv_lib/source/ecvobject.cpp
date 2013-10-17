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
#include "ecvobject.h"

void ObstacleImage (Image8_t        *outImg,
                    const Image32_t *inImg,
                    const size_t     size)
{
  const size_t width  = inImg->width;
  const size_t height = inImg->height;

  const size_t stepSize       = size >> 2;
  const size_t numStepsHeight = (height - (size << 2)) >> 1;

  const size_t *ptrLeft, *ptrCenter, *ptrRight;
  const size_t *ptrUp, *ptrDown;

  const size_t *endUp;

  
}


