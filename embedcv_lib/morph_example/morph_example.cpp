// morph_example.cpp : Defines the entry point for the console application.
//
/*
 * Read a 24 bit binary RGB PPM image from standard input
 * Morphologically erode/dilate/open/close the image channel specified
 * (default is eroding all channels)
 * Write to standard output
 *
                "  which color channel to process (default is all channels)\n"
               "      -r red image channel only\n"
               "      -g green image channel only\n"
               "      -b blue image channel only\n"
               "  morphological operation (default is -e 1)\n"
               "      -d number of times repeat dilation (expand region)\n"
               "      -e number of times repeat erosion (shrink region)\n"
               "      -o number of times repeat opening (remove small blobs)\n"
               "      -c number of times repeat closing (fill in holes)\n",
 */

#include "stdafx.h"

#include "ecv.h"

#pragma comment(lib,"..\\..\\embedcv_lib\\embedcv_lib\\lib\\embedcv_lib.lib") //½«¾²Ì¬¿âµ¼Èë

int _tmain(int argc, _TCHAR* argv[])
{
  /* which color channel to erode */
  enum { ALLCOLORS, RED, GREEN, BLUE } pickImg = ALLCOLORS;  /* default */
  enum { ERODE, DILATE, OPEN, CLOSE } pickOp = ERODE;  /* default */

  size_t repeat = 1;  /* default number of times to perform operation */
  pickImg = GREEN;
  pickOp = OPEN;



  /* stream I/O reading is buffered */
  FILE *stdIn = fopen("..\\..\\embedcv_lib\\embedcv_lib\\images\\seg.ppm", "rb");
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

  /* erode the specified image (default is all of them) */
  size_t num = repeat;
  switch (pickImg)
  {
    case (ALLCOLORS):
      if (pickOp == ERODE)
      {
        while (repeat--)
        {
          RegionErode55(&redImg, 0);
          RegionErode55(&greenImg, 0);
          RegionErode55(&blueImg, 0);
        }
      }
      else if (pickOp == DILATE)
      {
        while (repeat--)
        {
          RegionDilate55(&redImg, 0xff);
          RegionDilate55(&greenImg, 0xff);
          RegionDilate55(&blueImg, 0xff);
        }
      }
      else if (pickOp == OPEN)
      {
        while (num--)
        {
          RegionErode55(&redImg, 0);
          RegionErode55(&greenImg, 0);
          RegionErode55(&blueImg, 0);
        }
        while (repeat--)
        {
          RegionDilate55(&redImg, 0xff);
          RegionDilate55(&greenImg, 0xff);
          RegionDilate55(&blueImg, 0xff);
        }
      }
      else
      {
        while (num--)
        {
          RegionDilate55(&redImg, 0xff);
          RegionDilate55(&greenImg, 0xff);
          RegionDilate55(&blueImg, 0xff);
        }
        while (repeat--)
        {
          RegionErode55(&redImg, 0);
          RegionErode55(&greenImg, 0);
          RegionErode55(&blueImg, 0);
        }
      }
      break;

    case (RED):
      if (pickOp == ERODE)
      {
        while (repeat--)
        {
          RegionErode55(&redImg, 0);
        }
      }
      else if (pickOp == DILATE)
      {
        while (repeat--)
        {
          RegionDilate55(&redImg, 0xff);
        }
      }
      else if (pickOp == OPEN)
      {
        while (num--)
        {
          RegionErode55(&redImg, 0);
        }
        while (repeat--)
        {
          RegionDilate55(&redImg, 0xff);
        }
      }
      else  /* CLOSE */
      {
        while (num--)
        {
          RegionDilate55(&redImg, 0xff);
        }
        while (repeat--)
        {
          RegionErode55(&redImg, 0);
        }
      }
    break;

    case (GREEN):
      if (pickOp == ERODE)
      {
        while (repeat--)
        {
          RegionErode55(&greenImg, 0);
        }
      }
      else if (pickOp == DILATE)
      {
        while (repeat--)
        {
          RegionDilate55(&greenImg, 0xff);
        }
      }
      else if (pickOp == OPEN)
      {
        while (num--)
        {
          RegionErode55(&greenImg, 0);
        }
        while (repeat--)
        {
          RegionDilate55(&greenImg, 0xff);
        }
      }
      else  /* CLOSE */
      {
        while (num--)
        {
          RegionDilate55(&greenImg, 0xff);
        }
        while (repeat--)
        {
          RegionErode55(&greenImg, 0);
        }
      }
      break;

    case (BLUE):
      if (pickOp == ERODE)
      {
        while (repeat--)
        {
          RegionErode55(&blueImg, 0);
        }
      }
      else if (pickOp == DILATE)
      {
        while (repeat--)
        {
          RegionDilate55(&blueImg, 0xff);
        }
      }
      else if (pickOp == OPEN)
      {
        while (num--)
        {
          RegionErode55(&blueImg, 0);
        }
        while (repeat--)
        {
          RegionDilate55(&blueImg, 0xff);
        }
      }
      else  /* CLOSE */
      {
        while (num--)
        {
          RegionDilate55(&blueImg, 0xff);
        }
        while (repeat--)
        {
          RegionErode55(&blueImg, 0);
        }
      }
      break;
  }

  /* write the image as a 24 bit RGB PPM */
  FILE *stdOut = fopen("..\\..\\embedcv_lib\\embedcv_lib\\images\\morph.ppm", "wb");
  WritePPMinRGB(stdOut, &redImg, &greenImg, &blueImg);
  fclose(stdOut);

  /* free memory */
  IMAGE8FREE( redImg )
  IMAGE8FREE( greenImg )
  IMAGE8FREE( blueImg )

  return 0;
}

