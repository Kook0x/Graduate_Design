#include "SDL_visLib.h"
#include "picframe.h"
#include "vislib.h"


PicFrame::PicFrame()
{
	width=320;
	height=240;
	bpp=3;

	if (SDL_Init(SDL_INIT_VIDEO) != 0) 
	{
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		return;
	}

	screen = SDL_SetVideoMode(width, 2*height, 8*bpp, SDL_SWSURFACE);
	if (screen == NULL) 
	{
		printf("Unable to set video mode: %s\n", SDL_GetError());
		return;
	}

	image=image_dup=NULL;

	pixels= (unsigned char*)malloc(width*height*3);//rgb dataflow
}

SDL_Surface *PicFrame::getImage()
{
	return image;
}

PicFrame::~PicFrame()
{
	if(image)
		SDL_FreeSurface(image);
	if(image_dup)
		SDL_FreeSurface(image_dup);
	if(pixels)
		free(pixels);
}

int PicFrame::processing(void *mem, int size, blob *obj)
{
	SDL_Event	event;

	for (;SDL_PollEvent(&event) != 0;)
	{
		switch (event.type)
		{ 
		  case SDL_QUIT:
			return -1;
			break;
		  case SDL_KEYDOWN:
			// If escape is pressed set the Quit-flag
			if (event.key.keysym.sym == SDLK_ESCAPE)
				return -1;
			break;
		}
	}

	SDL_Surface *temp;

	//read the image from jpeg dataflow
	temp = IMG_LoadJPG_RW(SDL_RWFromMem(mem, size));

	if(image)
		SDL_FreeSurface(image);
	image = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	SDL_Rect src, dest;
 
	src.x = 0;
	src.y = 0;
	src.w = image->w;
	src.h = image->h;

	//at (0,0) show the original image
	dest.x = 0;
	dest.y = 0;
	dest.w = image->w;
	dest.h = image->h;
	SDL_BlitSurface(image, &src, screen, &dest);

	//duplicate the origial image surface
	if(image_dup)
		SDL_FreeSurface(image_dup);
	image_dup=SDL_ConvertSurface(image, screen->format, 0);

	//process
	visLibProcess(image, image_dup, obj);

	//process the duplicated image
	unsigned char *p = pixels;
	unsigned char *ptrData=(unsigned char*)image_dup->pixels;
	unsigned char *ptrDataEnd=(unsigned char*)image_dup->pixels+width*height*bpp;

	//pixels is RGB dataflow
	while(ptrData < ptrDataEnd)
	{
		*(p) = *(ptrData+2); 
		*(p+1) = *(ptrData+1); 
		*(p+2)  = *(ptrData);
		
		ptrData+=bpp;
		p+=3;
	}

	//update the duplicatecd with pixels after processed
	p = pixels;
	ptrData=(unsigned char*)image_dup->pixels;
	ptrDataEnd=(unsigned char*)image_dup->pixels+width*height*bpp;

	//pixels is RGB dataflow
	while(ptrData < ptrDataEnd)
	{
		*(ptrData+2)=*p;
		*(ptrData+1)=*(p+1);
		*(ptrData)=*(p+2);
		
		ptrData+=bpp;
		p+=3;
	}

	//at (0,240) show the duplicated image
	dest.x = 0;
	dest.y = height;
	dest.w = image->w;
	dest.h = image->h;
	SDL_BlitSurface(image_dup, &src, screen, &dest);	
	SDL_Flip(screen);

	
	/*
	//(optional) write pixels to a file
	FILE *fo = fopen("AAA.PPM", "wb");
	if (fo == NULL) 
	{
		printf("%s: can't open %s\n", "AAA.ppm");
		exit( -1);
	}

	fprintf(fo, "P6\n%u %u\n255\n", width, height);

	ptrData=(unsigned char*)pixels;
	ptrDataEnd=(unsigned char*)pixels+width*height*3;

	while(ptrData < ptrDataEnd)
	{
		r=*ptrData++;
		g=*ptrData++;
		b=*ptrData++;
		fprintf(fo, "%c%c%c", r, g, b);
	}

	fclose(fo);
	*/	
	return 0;
}
