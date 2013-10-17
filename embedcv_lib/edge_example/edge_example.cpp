// edge_example.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "ecv.h"

#pragma comment(lib,"..\\..\\embedcv_lib\\embedcv_lib\\lib\\embedcv_lib.lib") //½«¾²Ì¬¿âµ¼Èë

int _tmain(int argc, _TCHAR* argv[])
{
  /* how to display edge images */
  enum { ALL, HORIZONTAL, VERTICAL, COMBINED } pickOut = COMBINED; /* default */

  /* how much to right shift the squared edge magnitudes */
  size_t shift = 5;  /* default */

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

  /*
   * Note: The "X" image uses the Sobel kernel that detects vertical edges.
   *       The "Y" image uses the Sobel kernel that detects horizontal edges.
   *
   * This is somewhat counter-intuitive but makes more sense from what the
   * image processing convolution is doing.
   *
   */

  /* compute edges */
  SobelEdges(&edgeXImg, &edgeYImg, &lumaImg);

  /* edge magnitude */
  IMAGE8MALLOC( aImg, width, height )
  IMAGE8MALLOC( bImg, width, height )
  if (pickOut == COMBINED)
  {
    EdgeImagesToSS(&aImg, &edgeXImg, &edgeYImg, shift);
  }
  else
  {
    EdgeImagesToSS(&aImg, &edgeXImg, &edgeXImg, shift);
    EdgeImagesToSS(&bImg, &edgeYImg, &edgeYImg, shift);
  }

  /* write the image as a 24 bit RGB PPM */
  FILE *stdOut = fopen("..\\..\\embedcv_lib\\embedcv_lib\\images\\edge.ppm", "wb");
  if (pickOut == ALL)
  {
    WritePPMinRGB(stdOut, &aImg, &bImg, &lumaImg);
  }
  else if (pickOut == HORIZONTAL)
  {
    WritePPMinRGB(stdOut, &bImg, &bImg, &bImg);
  }
  else if (pickOut == VERTICAL)
  {
    WritePPMinRGB(stdOut, &aImg, &aImg, &aImg);
  }
  else if (pickOut == COMBINED)
  {
    WritePPMinRGB(stdOut, &aImg, &aImg, &aImg);
  }
  fclose(stdOut);

  /* free memory */
  IMAGE8FREE( aImg )
  IMAGE8FREE( bImg )
  IMAGE16FREE( edgeXImg )
  IMAGE16FREE( edgeYImg )
  IMAGE8FREE( lumaImg )
  IMAGE8FREE( chromaBImg )
  IMAGE8FREE( chromaRImg )
  IMAGE8FREE( redImg )
  IMAGE8FREE( greenImg )
  IMAGE8FREE( blueImg )

  return 0;
}

