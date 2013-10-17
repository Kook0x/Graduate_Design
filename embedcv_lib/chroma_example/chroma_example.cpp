// chroma_example.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "ecv.h"

#pragma comment(lib,"..\\..\\embedcv_lib\\embedcv_lib\\lib\\embedcv_lib.lib") //½«¾²Ì¬¿âµ¼Èë


int _tmain(int argc, _TCHAR* argv[])
{
  /* how much to right shift the squared edge magnitude */
  size_t shift = 8;  /* default */

  /* stream I/O reading is buffered */
  FILE *stdIn = fopen("..\\..\\embedcv_lib\\embedcv_lib\\images\\test.ppm", "rb");
  READBUFFER( stdInBuf, 256 )

  /* image dimensions */
  size_t width, height;

  /* read PPM header to know the image dimensions */
  ReadPPMHead(&width, &height, stdIn, &stdInBuf);

  /* three images for RGB */
  IMAGE8MALLOC( redImg, width, height )
  IMAGE8MALLOC( greenImg, width, height )
  IMAGE8MALLOC( blueImg, width, height )

  /* read in the RGB PPM image */
  ReadPPMBody(&redImg, &greenImg, &blueImg, stdIn, &stdInBuf);

  /* no need to read from stream anymore */
  fclose(stdIn);

  /* three images for luma, chroma B and chroma R */
  IMAGE8MALLOC( lumaImg, width, height )
  IMAGE8MALLOC( chromaBImg, width, height )
  IMAGE8MALLOC( chromaRImg, width, height )

  /* convert RGB to YCbCr */
  ConvertImageRGBtoYCbCr(&lumaImg, &chromaBImg, &chromaRImg,
                         &redImg, &greenImg, &blueImg);

  /* edge images in horizontal and vertical directions */
  IMAGE16MALLOC( edgeXImg, width, height )
  IMAGE16MALLOC( edgeYImg, width, height )

  /* compute edges */
  SobelEdges(&edgeXImg, &edgeYImg, &lumaImg);

  /* edge magnitude */
  IMAGE8MALLOC( edgeImg, width, height )
  EdgeImagesToSS(&edgeImg, &edgeXImg, &edgeYImg, shift);

  /* equalize histograms of the chroma images so they are easier to see */
  HISTOGRAM8( chromaBHist )
  HISTOGRAM8( chromaRHist )
  ImageHistogram( &chromaBHist, &chromaBImg );
  ImageHistogram( &chromaRHist, &chromaRImg );
  EqualizeImage( &chromaBImg, &chromaBHist );
  EqualizeImage( &chromaRImg, &chromaRHist );

  /* write the image as a 24 bit RGB PPM */
  FILE *stdOut = fopen("..\\..\\embedcv_lib\\embedcv_lib\\images\\chroma.ppm", "wb");
  WritePPMinRGB(stdOut, &chromaRImg, &edgeImg, &chromaBImg);
  fclose(stdOut);

  /* free memory */
  IMAGE8FREE( lumaImg )
  IMAGE8FREE( chromaBImg )
  IMAGE8FREE( chromaRImg )
  IMAGE8FREE( redImg )
  IMAGE8FREE( greenImg )
  IMAGE8FREE( blueImg )
  IMAGE8FREE( edgeImg )

  return 0;
}

