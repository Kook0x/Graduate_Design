#include "SDL_visLib.h"

#include "common.h"
#include "vislib.h"

#include "vlUtility.h"
#include "pid.h"

static void SDLSurface2vlImage(SDL_Surface *sdl_image, vlImage *vl_image)
{	
	vlPixel *p = (vlPixel *)vl_image->pixel;
	unsigned char *ptrData=(unsigned char*)sdl_image->pixels;
	unsigned char *ptrDataEnd=(unsigned char*)sdl_image->pixels+sdl_image->h*sdl_image->w*3;

	//pixels is RGB dataflow
	while(ptrData < ptrDataEnd)
	{
		*p++ = *(ptrData+2); 
		*p++ = *(ptrData+1); 
		*p++  = *(ptrData);
		
		ptrData+=3;
	}
}


static void vlImage2SDLSurface(vlImage *vl_image, SDL_Surface *sdl_image)
{
	
	//update the duplicatecd with pixels after processed
	vlPixel *p = vl_image->pixel;
	unsigned char *ptrData=(unsigned char*)sdl_image->pixels;
	unsigned char *ptrDataEnd=(unsigned char*)sdl_image->pixels+sdl_image->h*sdl_image->w*3;

	//pixels is RGB dataflow
	while(ptrData < ptrDataEnd)
	{
		*(ptrData+2)=(unsigned char)*p++;
		*(ptrData+1)=(unsigned char)*p++;
		*(ptrData)=(unsigned char)*p++;
		
		ptrData+=3;
	}	
}

static void filterProcess(vlImage *vl_src)
{
  vlImage *pic1=vl_src;
  vlImage *pic2 = VL_IMAGE_CREATE();
  vlWindow *window;

  vlMask *smoothMask, *edgeMask;
  float smoothKernel[3][3] = {{0.11, 0.11, 0.11},
			      {0.11, 0.11, 0.11},
			      {0.11, 0.11, 0.11}};
  float edgeKernel[3][3] = {{0, -1, 0},
			    {-1, 4, -1},
			    {0, -1, 0}};

  smoothMask = vlMaskCreate (3, 3, &smoothKernel[0][0]);
  edgeMask = vlMaskCreate (3, 3, &edgeKernel[0][0]);
  
  window = vlWindowCreate (0, 0, pic1->width, pic1->height);

  vlSmooth(pic1, 5, window, pic2);
  vlConvolve (pic2, edgeMask, window, pic1);
  vlSmoothMedian (pic1, 5, window, pic2);

  vlImageCopy(pic2, pic1);

  vlImageDestroy(pic2);
  vlWindowDestroy(window);
}



static void gridProcess(vlImage *pic,int gridType)
{
	
	if (gridType==FINE && 0 != vlDrawGrid (pic)) 
		printf ("*** ERROR IN SUPERIMPOSING GRID***\n");
	if (gridType==COARSE && 0 != vlDrawCenteredGrid (pic)) 
		printf ("*** ERROR IN SUPERIMPOSING GRID***\n");
}


static int vlDealBinPic(vlImage *src,vlHSI_carl_tol_t *para,vlImage *dest)
{
   
   vlWindow *window= vlWindowCreate (0, 0, src->width, src->height);
   
   //vlRgb2Binary_tol(src,para,window,dest); 
   vlBinary(src,para, dest);
   vlBinaryOpen(dest,3,2,window);
   vlBinaryClose(dest,3,2,window);
   
   vlWindowDestroy(window);
   return (0);
}


static void findMaxBlob(vlImage *img, blob *obj)
{	
	int id;
	int totalblobs=0;
	blob blobs[BLOB_MAX_NUM];
	
	vlHSI_carl_tol_t *para=(vlHSI_carl_tol_t *)malloc(sizeof(vlHSI_carl_tol_t));

	vlImage *dest=vlImageCreate(GRAY, img->width, img->height);

	//HSI threshold parameters
	load_sample_hsi_carl_params(para);

	memset(blobs, 0, sizeof(blob)*BLOB_MAX_NUM);
	
	vlImageSave(img, "original.ppm");

	vlDealBinPic(img,para,dest);

	vlImageSave(dest, "binary.ppm");
	
	/* find all blobs */
	totalblobs=vlFindAllBlobs(dest,  blobs);
	printf ("There are %d blobs.\n", totalblobs);

	//the result image are copyed into img from dest
	vlImageSave(dest, "blobs.ppm");

	vlPixel *ptrData=img->pixel;
	vlPixel *ptrDataEnd=img->pixel+img->height*img->width*3;
	vlPixel *ptrDest=dest->pixel;
	while(ptrData < ptrDataEnd)
	{
		*ptrData++ = *ptrDest;
		*ptrData++ = *ptrDest;
		*ptrData++ = *ptrDest++;
	}
	if(!totalblobs)
	{ 
		/*not exist any blob*/
		VL_ERROR("vlFindBlobs: not exist any blob \n");
		obj->valid=-1;
	}
	else
	{
		/*get the max area blob*/
		id=vlGetMaxAreaBlob(totalblobs, blobs);

		centroid(dest, id, blobs);
		printf ("Blob # %d valid %d area %d TL %d %d Xbaric %d Ybaric %d\n",id,
	                blobs[id].valid,
	                blobs[id].area,
	                blobs[id].topleft/dest->width,
	                blobs[id].topleft%dest->width,
	                blobs[id].xcentroid,
	                blobs[id].ycentroid);//y is horztial, x is vertical


		obj->valid= blobs[id].valid;
		obj->area=blobs[id].area;
		obj->topleft=blobs[id].topleft;
		obj->xcentroid=blobs[id].xcentroid;
		obj->ycentroid=blobs[id].ycentroid;


		//vlMarkCentroid(img, blobs, id);
		/*get all blobs information*/
		//vlDumpAllBlobs(totalblobs, blobs, dest);
	}
	free(para);
	vlImageDestroy(dest);
}


void visLibProcess(SDL_Surface *sdl_src, SDL_Surface *sdl_dest, blob *obj)
{
	vlImage *vl_src;

	vl_src=vlImageCreate(RGB, sdl_src->w,sdl_src->h);
	SDLSurface2vlImage(sdl_src, vl_src);

//	filterProcess(vl_src);
//	gridProcess(vl_src, FINE);


	findMaxBlob(vl_src,obj);	

	vlImage2SDLSurface(vl_src, sdl_dest);
	vlImageDestroy(vl_src);
}


