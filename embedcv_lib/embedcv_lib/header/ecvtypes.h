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


#ifndef _EMBEDCV_ECVTYPES_H_
#define _EMBEDCV_ECVTYPES_H_


#include <stddef.h>
#include <stdlib.h>
#include "types.h"


/*
 * Basic 8 bit image struct
 *
 */
typedef struct
{
  uint8_t *data;
  size_t  width;
  size_t  height;
} Image8_t;


/*
 * Convenience macro for defining an 8 bit image
 *
 */
#define IMAGE8( NAME, WIDTH, HEIGHT ) \
  Image8_t NAME ; \
  uint8_t NAME ## data[ ( WIDTH ) * ( HEIGHT ) ]; \
  NAME .data = NAME ## data; \
  NAME .width = WIDTH ; \
  NAME .height = HEIGHT ;

/* dynamically allocate on heap */
#define IMAGE8MALLOC( NAME, WIDTH, HEIGHT ) \
  Image8_t NAME ; \
  NAME .data = (uint8_t *)malloc( sizeof(uint8_t) * ( WIDTH ) * ( HEIGHT ) ); \
  NAME .width = WIDTH ; \
  NAME .height = HEIGHT ;

#define IMAGE8FREE( NAME ) free( NAME .data );


/*
 * Basic 16 bit image struct
 *
 * Note: This can also be used for signed int16_t pixels as well. The
 *       image structure manipulating functions do not care about signed
 *       versus unsigned types. They just copy memory.
 *
 */
typedef struct
{
  uint16_t *data;
  size_t   width;
  size_t   height;
} Image16_t;


/*
 * Convenience macro for defining an 16 bit image
 *
 */
#define IMAGE16( NAME, WIDTH, HEIGHT ) \
  Image16_t NAME ; \
  uint16_t NAME ## data[ ( WIDTH ) * ( HEIGHT ) ]; \
  NAME .data = NAME ## data; \
  NAME .width = WIDTH ; \
  NAME .height = HEIGHT ;

/* dynamically allocate on heap */
#define IMAGE16MALLOC( NAME, WIDTH, HEIGHT ) \
  Image16_t NAME ; \
  NAME .data = (uint16_t *)malloc( sizeof(uint16_t) * ( WIDTH ) * ( HEIGHT ) ); \
  NAME .width = WIDTH ; \
  NAME .height = HEIGHT ;

#define IMAGE16FREE( NAME ) free( NAME .data );


/*
 * Basic 32 bit image struct
 *
 * Note: This can also be used for signed int32_t pixels as well. The
 *       image structure manipulating functions do not care about signed
 *       versus unsigned types. They just copy memory.
 *
 */
typedef struct
{
  uint32_t *data;
  size_t   width;
  size_t   height;
} Image32_t;


/*
 * Convenience macro for defining a 32 bit image
 *
 */
#define IMAGE32( NAME, WIDTH, HEIGHT ) \
  Image32_t NAME ; \
  uint32_t NAME ## data[ ( WIDTH ) * ( HEIGHT ) ]; \
  NAME .data = NAME ## data; \
  NAME .width = WIDTH ; \
  NAME .height = HEIGHT ;

#define IMAGE32MALLOC( NAME, WIDTH, HEIGHT ) \
  Image32_t NAME ; \
  NAME .data = (uint32_t *)malloc( sizeof(uint32_t) * ( WIDTH ) * ( HEIGHT ) ); \
  NAME .width = WIDTH ; \
  NAME .height = HEIGHT ;

#define IMAGE32FREE( NAME ) free( NAME .data );


/*
 * Store histogram information inside this struct. This includes: the counts
 * for each bin (probability density histogram); the cumulative distribution;
 * the partial expectation values for every index.
 *
 */
typedef struct
{
  size_t *bins;
  size_t *sumBins;
  size_t *meanBins;
  size_t numberBins;
  size_t numberCounts;
} Histogram_t;


/*
 * Convenience macro for defining a histogram
C语言宏定义时#(井号)和##(双井号)的用法
#在英语里面叫做 pound
在C语言的宏定义中，一个#表示字符串化；两个#代表concatenate
 */
#define HISTOGRAM( NAME, NUMBINS ) \
  Histogram_t NAME ; \
  size_t NAME ## bins[ NUMBINS ]; \
  size_t NAME ## sumbins[ NUMBINS ]; \
  size_t NAME ## meanbins[ NUMBINS ]; \
  NAME .bins = NAME ## bins; \
  NAME .sumBins = NAME ## sumbins; \
  NAME .meanBins = NAME ## meanbins; \
  NAME .numberBins = NUMBINS ; \
  memset( NAME ## bins, 0, NUMBINS * sizeof(size_t) );

/*
 * Convenience macros for defining common image histograms
 *
 */
#define HISTOGRAM8( NAME ) HISTOGRAM( NAME, 256 )
#define HISTOGRAM16( NAME ) HISTOGRAM( NAME, 65536 )
#define HISTOGRAMCBCR( NAME ) HISTOGRAM( NAME, 361 )


/*
 * Image I/O is buffered.
 *
 */
typedef struct
{
  uint8_t       *head;      /* permanent pointer to head of array */
  uint8_t       *position;  /* index into the buffer array */
  const uint8_t *tail;      /* permanent pointer to tail of array */
  const uint8_t *size;      /* how much data is in the buffer array */
} Buffer_t;


/*
 * Buffer for ReadByte() to use
 *
 */
#define READBUFFER( NAME, LENGTH ) \
  Buffer_t NAME; \
  uint8_t NAME ## data[ LENGTH ]; \
  NAME .head = NAME ## data; \
  NAME .tail = NAME .head + LENGTH; \
  NAME .size = NAME .head; \
  NAME .position = NAME .head;


/*
 * Packed 16 bit words of CbCr pixels are useful for pure chroma images.
 *
 */
typedef union
{
  uint16_t CbCr;
  uint8_t  data[2];
} PackedCbCr_t;



#endif

