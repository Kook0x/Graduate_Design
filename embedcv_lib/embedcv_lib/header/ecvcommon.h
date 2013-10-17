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


#ifndef _EMBEDCV_ECVCOMMON_H_
#define _EMBEDCV_ECVCOMMON_H_


/*
 * Loop unrolling macro - unroll 2x
 *
 * This is the same as the "more" macro for loop unrolling except for being
 * less aggressive. Why be more or less aggressive? Perhaps the body of the
 * loop has a lot of code. Perhaps the total number of loops is not so much.
 * In that case, looping overhead is relatively small compared to the extra
 * code space required.
 *
 * Another reason is that unrolling implies extents of a loop - i.e. that
 * data structures like histograms, images, etc are an even multiple of the
 * number of times the loop is unrolled. This macro implies only that the
 * dimensions are an even number which is typically the case. Odd sized
 * histograms and images are unusual.
 *
 */
#ifdef UNROLL_LOOPS
#define LOOP_UNROLL_LESS( CODE ) \
CODE \
CODE
#else
#define LOOP_UNROLL_LESS( CODE ) CODE
#endif


/*
 * Loop unrolling macro - unroll 8x
 *
 * Pointer arithmetic is used everywhere for speed. This likely circumvents
 * compiler loop unrolling. It is assumed this optimization depends on
 * for loops and array subscripting which are otherwise slower.
 *
 * Loops are unrolled to eight repetitions where possible. It has been
 * observed that gcc will do this with for loops of constant extents. Caution
 * is required that loop extents are always a multiple of eight which is
 * typically true for the dimensions of an image.
 *
 */
#ifdef UNROLL_LOOPS
#define LOOP_UNROLL_MORE( CODE ) \
CODE \
CODE \
CODE \
CODE \
CODE \
CODE \
CODE \
CODE

/* should always be 2 fewer unrolls than LOOP_UNROLL_MORE */
#define LOOP_UNROLL_MORE2( CODE ) \
CODE \
CODE \
CODE \
CODE \
CODE \
CODE

/* should always be 4 fewer unrolls than LOOP_UNROLL_MORE */
#define LOOP_UNROLL_MORE4( CODE ) \
CODE \
CODE \
CODE \
CODE
#else
#define LOOP_UNROLL_MORE( CODE ) CODE
#define LOOP_UNROLL_MORE2( CODE ) CODE
#define LOOP_UNROLL_MORE4( CODE ) CODE
#endif


#endif
