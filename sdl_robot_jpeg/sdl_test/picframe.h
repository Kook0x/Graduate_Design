#ifndef _PicFrame_
#include "SDL/SDL.h"
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_thread.h>

class	PicFrame
{
	SDL_Surface *screen;
	SDL_Surface *image;

	SDL_Surface *image_dup;

	unsigned char* pixels;

	int bpp;
	int width;
	int height;

  public:
	PicFrame();
	~PicFrame();

	SDL_Surface *getImage();

	int	loadPic(void *, int);
};
#endif
