// simple_example.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include "vislib.h"
#include "common.h"

#pragma comment(lib,"..\\..\\SDL_SRV_VisLib\\VisLib_win\\lib\\VisLib_win.lib") //½«¾²Ì¬¿âµ¼Èë




static int vlDealBinPic(vlImage *src,vlHSI_carl_tol_t *para,vlImage *dest)
{
   
   vlWindow *window= vlWindowCreate (0, 0, src->width, src->height);
   
   vlBinary(src, para, dest);
   vlBinaryOpen(dest,3,2,window);
   vlBinaryClose(dest,3,2,window);
   
   vlWindowDestroy(window);
   return (0);
}


static void findMaxBlob(vlImage *img)
{	
	int id;
	int totalblobs=0;
	blob blobs[BLOB_MAX_NUM];
	
	vlHSI_carl_tol_t para={0};//=(vlHSI_carl_tol_t *)malloc(sizeof(vlHSI_carl_tol_t));

	vlImage *dest=vlImageCreate(GRAY, img->width, img->height);

	//HSI threshold parameters
	load_sample_hsi_carl_params(&para);

	memset(blobs, 0, sizeof(blob)*BLOB_MAX_NUM);
	
	vlImageSave(img, "original.ppm");

	vlDealBinPic(img,&para,dest);

	vlImageSave(dest, "binary.ppm");

	/* find all blobs */
	totalblobs=vlFindAllBlobs(dest,  blobs);

	vlImageSave(dest, "blobs.ppm");

	printf ("There are %d blobs.\n", totalblobs);

	if(!totalblobs)
	{ 
		/*not exist any blob*/
		VL_ERROR("vlFindBlobs: not exist any blob \n");
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
	                blobs[id].ycentroid);

		/*get all blobs information*/
		//vlDumpAllBlobs(totalblobs, blobs, dest);
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	vlImage *img = vlImageCreate(RGB, 1, 1);

	vlImageLoad(img, "a.ppm");

	vlImageSave(img, "copy.ppm");

	findMaxBlob(img);

	vlImageSave(img, "copy.ppm");

	vlImageDestroy(img);
	return 0;
}

