// example.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "ecv.h"//函数原型声明 

#pragma comment(lib,"..\\..\\embedcv_lib\\embedcv_lib\\lib\\embedcv_lib.lib") //将静态库导入

int _tmain(int argc, _TCHAR* argv[])
{
  size_t repeat = 1;  /* default is blur only once */


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


  /* blurred images, one for each channel */
  IMAGE8MALLOC( redBlur, width, height )
  IMAGE8MALLOC( greenBlur, width, height )
  IMAGE8MALLOC( blueBlur, width, height )

  Image8_t *ptrRedSrc   = &redImg;
  Image8_t *ptrGreenSrc = &greenImg;
  Image8_t *ptrBlueSrc  = &blueImg;

  Image8_t *ptrRedDest   = &redBlur;
  Image8_t *ptrGreenDest = &greenBlur;
  Image8_t *ptrBlueDest  = &blueBlur;

  Image8_t *tmp;

  while (repeat--)
  {
    BlurImage33Fast(ptrRedDest, ptrRedSrc);
    BlurImage33Fast(ptrGreenDest, ptrGreenSrc);
    BlurImage33Fast(ptrBlueDest, ptrBlueSrc);

    tmp = ptrRedDest;
    ptrRedDest = ptrRedSrc;
    ptrRedSrc = tmp;

    tmp = ptrGreenDest;
    ptrGreenDest = ptrGreenSrc;
    ptrGreenSrc = tmp;

    tmp = ptrBlueDest;
    ptrBlueDest = ptrBlueSrc;
    ptrBlueSrc = tmp;
  }

 /* write the image as a 24 bit RGB PPM */
  FILE *stdOut = fopen("..\\..\\embedcv_lib\\embedcv_lib\\images\\blur.ppm", "wb");
  WritePPMinRGB(stdOut, ptrRedSrc, ptrGreenSrc, ptrBlueSrc);
  fclose(stdOut);

  /* release image memory */
  IMAGE8FREE( redImg )
  IMAGE8FREE( greenImg )
  IMAGE8FREE( blueImg )
  IMAGE8FREE( redBlur )
  IMAGE8FREE( greenBlur )
  IMAGE8FREE( blueBlur )

  return 0;

}