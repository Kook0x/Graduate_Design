#ifndef _VISLIB_MYHIST_H_
#define _VISLIB_MYHIST_H_

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "vislib.h"

typedef struct
{
  size_t *bins;
  size_t *sumBins;
  size_t *meanBins;
  size_t numberBins;
  size_t numberCounts;
} Histogram_t;

typedef  unsigned short int uint16_t;



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

#define HHISTOGRAM( NAME ) HISTOGRAM( NAME, 360 )
#define SHISTOGRAM( NAME ) HISTOGRAM( NAME, 100 )
#define IHISTOGRAM( NAME ) HISTOGRAM( NAME, 100 )


#define UINTDIFF( v1, v2 ) ( (v1 < v2) ? v2 - v1 : v1 - v2 )


size_t HistogramMedian (const Histogram_t *inHistogram);

void ImageHistogram (Histogram_t *outHistogram,  
                     const vlImage *inImg);/*360 */

void ImageHistogramDist (Histogram_t *outHistogram,  
                     const vlImage *inImg,
                     uint16_t   value);
size_t OtsuThreshold (const Histogram_t *inHistogram);

void cal_threshold(vlImage *pic, vlHSI_carl_tol_t *para );


#endif

