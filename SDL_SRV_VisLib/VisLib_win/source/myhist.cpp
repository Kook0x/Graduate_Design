#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include "vislib.h"
#include "global.h"
typedef unsigned int size_t;

/*
 * Compute the histogram of pixel values in an image
 *
 * The output Histogram_t must the same number of bins as the maximum pixel
 * value in the image. So a general 8 bit image implies 256 bins. However,
 * if the image is quantized to a limited set of values, fewer bins may be ok.
 * For example, if the pixel values in an image take on the values 0, 1, 2, 3
 * only, then a Histogram_t with only 4 bins will work.
 *
 * To be safe, it is recommended to have 256 bins in the Histogram_t.
 *
 */
void ImageHistogram (Histogram_t    *outHistogram,
                     const vlImage *inImg)
{
  /* density histogram */
  const size_t numPixels = outHistogram->numberCounts
                         = inImg->width * inImg->height;
  const vlPixel *ptrImg  = inImg->pixel;
  const vlPixel *endImg  = ptrImg + numPixels;
  size_t        *ptrBins = outHistogram->bins;
  ///////////////////////
  if(inImg->format==H)
  	{
  	while (ptrImg != endImg)
		{
		//LOOP_UNROLL_MORE( ptrBins[ *ptrImg++ ]++; )
		ptrBins[ HVAL_TO_HDEG(*ptrImg++) ]++;
		}
	}
  else if(inImg->format==S)
  	{
  	while (ptrImg != endImg)
		{
		//LOOP_UNROLL_MORE( ptrBins[ *ptrImg++ ]++; )
		ptrBins[ SVAL_TO_SPCT(*ptrImg++) ]++;
		}
  	}
  else
  	{
  	while (ptrImg != endImg)
		{
		//LOOP_UNROLL_MORE( ptrBins[ *ptrImg++ ]++; )
		ptrBins[ IVAL_TO_IPCT(*ptrImg++) ]++;
		}
  	}
  ////////////////////////////////////

  /* cumulative and partial expectation distributions */
  size_t *ptrSumBins  = outHistogram->sumBins;
  size_t *ptrMeanBins = outHistogram->meanBins;
  size_t accumSum     = 0;
  size_t accumMean    = 0;

  const size_t numBins = outHistogram->numberBins;
  size_t tmp, i;
  for (i = 0; i < numBins; i++)
  {
    tmp       = *ptrBins++;
    accumSum  = *ptrSumBins++  = accumSum + tmp;
    accumMean = *ptrMeanBins++ = accumMean + i * tmp;
  }
}


/*
 * Compute the histogram of distances from a reference value to the pixel
 * values in an image
 *
 * This is identical to ImageHistogram() except for tabulating the differences
 * between a reference value and the pixel values. There must be as many bins
 * as the largest difference value encountered plus one. For example, for a
 * general 8 bit image, the pixel values range from 0 to 255. If the reference
 * value is 127, then the largest possible difference is 128. So there must
 * be at least 129 bins in the output Histogram_t.
 *
 * To be safe, it is recommended to have 256 bins in the Histogram_t.
 *
 */
void ImageHistogramDist (Histogram_t    *outHistogram,
                         const vlImage *inImg,
                         const vlPixel value)
{
  /* density histogram */
  const size_t numPixels = outHistogram->numberCounts
                         = inImg->width * inImg->height;
  const vlPixel *ptrImg  = inImg->pixel;
  const vlPixel *endImg  = ptrImg + numPixels;
  size_t *ptrBins        = outHistogram->bins;
  ////////////////////////////////////////////////////
  if(inImg->format==H)
  	{
  	while (ptrImg != endImg)
		{
		LOOP_UNROLL_MORE( ptrBins[ HVAL_TO_HDEG(UINTDIFF( *ptrImg++, value )) ]++; )
		}
	}
  else if(inImg->format==S)
  	{
  	while (ptrImg != endImg)
		{
		LOOP_UNROLL_MORE( ptrBins[ SVAL_TO_SPCT(UINTDIFF( *ptrImg++, value )) ]++; )
		}
  	}
  else
  	{
  	while (ptrImg != endImg)
		{
		LOOP_UNROLL_MORE( ptrBins[ IVAL_TO_IPCT(UINTDIFF( *ptrImg++, value )) ]++; )
		}
  	}
////////////////////////////////////////////////////////////
  /* cumulative and partial expectation distributions */
  size_t *ptrSumBins  = outHistogram->sumBins;
  size_t *ptrMeanBins = outHistogram->meanBins;
  size_t accumSum     = 0;
  size_t accumMean    = 0;

  const size_t numBins = outHistogram->numberBins;
  size_t tmp, i;
  for (i = 0; i < numBins; i++)
  {
    tmp       = *ptrBins++;
    accumSum  = *ptrSumBins++  = accumSum + tmp;
    accumMean = *ptrMeanBins++ = accumMean + i * tmp;
  }
}

size_t OtsuThreshold (const Histogram_t *inHistogram)
{
  const size_t numBins      = inHistogram->numberBins;
  const size_t numCounts    = inHistogram->numberCounts;

  const size_t *ptrMeanBins = inHistogram->meanBins;
  const size_t mean         = ptrMeanBins[numBins - 1] / numCounts;

  const size_t *ptrSumBins  = inHistogram->sumBins;
  const size_t *headSumBins = ptrSumBins;
  const size_t *endSumBins  = ptrSumBins + numBins;

  size_t last = 0, curr, a, p;

  /* find threshold that maximizes interclass variance */
  while (ptrSumBins != endSumBins)
  {
    p    = *ptrSumBins++;
    a    = UINTDIFF( *ptrMeanBins++, mean * p );
    
    curr = (a * a) / (p * (numCounts - p));
    if (curr < last)
    {
      break;
    }
    last = curr;
  }

  return ptrSumBins - headSumBins;
}
size_t HistogramMedian (const Histogram_t *inHistogram)
{
  const size_t  halfCounts  = inHistogram->numberCounts >> 1;

  /* initial guess is middle of histogram */

  const size_t *lowerBound = inHistogram->sumBins; 
  const size_t *upperBound = inHistogram->sumBins + inHistogram->numberBins;
  const size_t *ptrSumBins = inHistogram->sumBins
                             + (inHistogram->numberBins >> 1);

  /*
   * when upper and lower bounds are equal or differ by one, the distance
   * between them divided by two is always zero so must terminate
   */
  while ( (upperBound - lowerBound) & ~0x1 )
  {
    if (*ptrSumBins < halfCounts)
    {
      /* need to search upper half */
      lowerBound = ptrSumBins;
      ptrSumBins += (upperBound - lowerBound) >> 1;
    }
    else if (*ptrSumBins > halfCounts)
    {
      /* need to search lower half */
      upperBound = ptrSumBins;
      ptrSumBins -= (upperBound - lowerBound) >> 1;
    }
    else
    {
      /* extreme luck, median value with exactly half number of counts */
      break;
    }
  }

  return ptrSumBins - inHistogram->sumBins;
}


void cal_threshold(vlImage *pic, vlHSI_carl_tol_t *para )
{

  vlImage *dest,*desth,*dests,*desti;
  
  vlWindow *window;
 
  /*Create window*/	
  window = vlWindowCreate (0, 0, pic->width, pic->height);
  /* init vars */
  dest= VL_IMAGE_CREATE (); 
  
  desth= VL_IMAGE_CREATE (); 
  dests= VL_IMAGE_CREATE (); 
  desti= VL_IMAGE_CREATE ();  
  
  HHISTOGRAM( outHistogramh );
  SHISTOGRAM( outHistograms );
  IHISTOGRAM( outHistogrami );
 
  vlRgb2Hsi(pic,window,dest);

  vlHsi2H(dest,window,desth);
  vlHsi2S(dest,window,dests);
  vlHsi2I(dest,window,desti);

  ImageHistogram(&outHistogramh,desth);
  ImageHistogram(&outHistograms,dests);
  ImageHistogram(&outHistogrami,desti);
  
  ImageHistogramDist(&outHistogramh,desth, para->h);
  ImageHistogramDist(&outHistograms,dests, para->s);
  ImageHistogramDist(&outHistogrami,desti, para->i);

  para->h_tol=OtsuThreshold (&outHistogramh);
  para->s_tol=OtsuThreshold (&outHistograms);
  para->i_tol=OtsuThreshold (&outHistogrami);

  printf("%d\n",  para->h_tol);
  printf("%d\n",  para->s_tol);
  printf("%d\n",  para->i_tol);

  vlImageDestroy(dest);
  vlImageDestroy(desth);
  vlImageDestroy(dests);
  vlImageDestroy(desti);

  return;

}

