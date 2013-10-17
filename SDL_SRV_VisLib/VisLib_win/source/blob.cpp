#include <stdio.h>
#include "vislib.h"
#include "global.h"
#include "a_hsi_carl.h"

/* *********************************************************************************************************/
void vlPutSquared (vlImage* pic,int x, int y, int width, int height)
{
	int i,j;
	for (i=x;i<x+height;i++)
		for (j=y;j<y+width;j++)
		{
			pic->pixel[INDEX_FROM_RC(i,j)]=255;
		}
}
	


void vlMarkCentroid(vlImage *img, blob *blobs, int nobject)
{
	int i,j;
	if(img->format==GRAY||img->format==BINARY)
	{
       		for(i=blobs[nobject].xcentroid-3;i<=blobs[nobject].xcentroid+3;i++)
          		for(j=blobs[nobject].ycentroid-3;j<=blobs[nobject].ycentroid+3;j++)
	             		img->pixel[i*img->width+j]=160;
	}
	else
	{/*RGB*/
		for(i=blobs[nobject].xcentroid-3;i<=blobs[nobject].xcentroid+3;i++)
         		for(j=blobs[nobject].ycentroid-3;j<=blobs[nobject].ycentroid+3;j++)
              		{
              			img->pixel[i*img->width+j]=125;
						img->pixel[i*img->width+j+1]=125;
						img->pixel[i*img->width+j+2]=125;
         			}
	}

}


/* *********************************************************************************************************/
void centroid (vlImage* pic, int nobject, blob *blobs)
{
	int i,j;
	int baricx,baricy,weight;
	unsigned char find;

	baricy=0;
	baricx=0;
	weight=0;

	for (i=ROW_FROM_INDEX(blobs[nobject].topleft); i<HEIGHT;i++)
	{
		find=FALSE;
		for (j=0;j<WIDTH;j++)
		{
			if (pic->pixel[INDEX_FROM_RC(i,j)]==nobject)/*blob id(1-255)*/
			{
				find=TRUE;
				baricx+=i;
				baricy+=j;
				weight++;
			}
		}
		if (!find) break;
	}

	if(weight)
	{
		blobs[nobject].xcentroid=baricx/weight;
		blobs[nobject].ycentroid=baricy/weight;

		//vlMarkCentroid(pic, blobs, nobject);
	}
	else
	{
		VL_ERROR("weight is zero\n");
	}

}

/* *********************************************************************************************************
* Clears the image borders, to eliminate need for border checking in vlFindBlob
*/

void clearBorders (vlImage* pic)
{
	int i;
	for (i=0;i<pic->width;i++)
	{
		pic->pixel[i]=0;	/*Clear first row*/
		pic->pixel[pic->width*pic->height-i-1]=0; /*Clear last row*/
	}
	for (i=0;i<pic->height;i++)
	{
		pic->pixel[INDEX_FROM_RC(i,0)]=0;	/*Clear first column*/
		pic->pixel[INDEX_FROM_RC(i,0)+pic->width-1]=0;	/*Clear last column*/
	}
}

/* *********************************************************************************************************
* performs connectivity analysis in a recursive way.
*/


void vlFindBlob (vlImage* pic, int n, int i, int j,blob *blobs)
{
	int index;
/*	printf ("Enter into vlFindBlob\n");*/
	
	index=i*pic->width+j;

	if (index <0) return;
	if (index >=pic->width*pic->height) return;
	if (pic->pixel[index]!=0) return;/*original 255*/
	
	pic->pixel[index]=n;
	blobs[n].area++; 
	
	vlFindBlob(pic,n,i-1,j-1,blobs);
	vlFindBlob(pic,n,i-1,j,blobs);
	vlFindBlob(pic,n,i-1,j+1,blobs);
	vlFindBlob(pic,n,i,j-1,blobs);
	vlFindBlob(pic,n,i,j+1,blobs);
	vlFindBlob(pic,n,i+1,j-1,blobs);
	vlFindBlob(pic,n,i+1,j,blobs);
	vlFindBlob(pic,n,i+1,j+1,blobs);
}


static void vlFindBlobStack(vlImage *pic, int n, int index, blob *blobs)
{    
	sqstack s;
	int w=pic->width;
	int h=pic->height;
	int lastIndex=w*h-1;
	int i,j;

	initstack(&s);

    push(&s, index);
	while(pop(&s,&index))
    {
		i=index/w;
		j=index%w;
		
		if(pic->pixel[index]==0)
		{
			pic->pixel[index]=n;
			blobs[n].area++; 
		
			if(i>0)
			{
				push(&s, index-w);
				if(j<w-1)
					push(&s, index-w+1);
				if(j>0)
					push(&s, index-w-1);
			}
			if(i<h-1)
			{
				push(&s, index+w);
				if(j<w-1)
					push(&s, index+w+1);
				if(j>0)
					push(&s, index+w-1);
			}
			if(j-1>=0)
				push(&s, index-1);
			if(j+1<=w-1)
				push(&s, index+1);
		}
	}
	destroystack(&s);
}

int vlFindAllBlobs(vlImage *pic, blob *blobs)
{
	int i,  blobnumber;
	
	  if (!pic) {
	    VL_ERROR ("FindAllBlobs error: NULL image\n");
	    return (-1);		/* failure */
	  }
	  

	long totalPixels=(pic->width)*(pic->height);

	pic->format=GRAY;
	blobnumber=1;
	totalPixels=pic->width*pic->height;
       for (i=0;i<totalPixels;i++)
	{ 
	          if (pic->pixel[i]==0) 
		   {	
			     blobs[blobnumber].topleft=i;
			     vlFindBlobStack (pic, blobnumber, i, blobs);
			     blobnumber++;
			     if (blobnumber>BLOB_MAX_NUM)
			      {
					VL_ERROR("Blob error: Too many blobs!\n");
					return (-1);
			       }
		   }
        }
	return blobnumber-1;
}	

int vlGetMaxAreaBlob(int numberBlobs, blob *blobs)
{
	int i, t=1;
	long int max_area=0;

	if (!numberBlobs) {
	    VL_ERROR ("GetMaxAreaBlob error: not exist blob\n");
	    return (-1);		/* failure */
	}
	
	for(i=1;i<=numberBlobs;i++)
	{
		if(blobs[i].area>max_area)
		{	
		   max_area=blobs[i].area;
	       t=i; 
        }
	}
	return t;
}


int vlDumpAllBlobs(int numberBlobs, blob *blobs, vlImage *pic)
{
	int i;
	if (!numberBlobs) {
	    VL_ERROR ("DumpAllBlobs error: not exist blob\n");
	    return (0);		/* failure */
	}

       for (i=1;i<=numberBlobs;i++) 
       { 
              		centroid(pic,i,blobs);
	      		printf ("Blob # %d valid %d area %d TL %d %d Xbaric %d Ybaric %d\n",i,
                      blobs[i].valid,blobs[i].area,
                      blobs[i].topleft/pic->width,
                      blobs[i].topleft%pic->width,
                      blobs[i].xcentroid,blobs[i].ycentroid);
       }
	return 1; 
}

