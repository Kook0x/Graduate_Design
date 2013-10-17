
#ifndef _blob
#define _blob

#include "vislib.h"

#define FIFO_NAME "/tmp/fifo_coordinate"

#define BLOB_MAX_NUM 255
#define DISTANCE_MIN 10	/*Minimal distance in height for the acknowledgment of the two BLOB*/

/*#define GAIN 0		Initial camera gain*/
#define SHUTTER 65535	/*Initial shutter speed*/

#define ROW_FROM_INDEX(i) (i/WIDTH)
#define COL_FROM_INDEX(i) (i%WIDTH)
#define INDEX_FROM_RC(i,j) (i*WIDTH+j)


/*int sogliaValida=0;	memorizza il valore minimo della soglia che permette il riconoscimento dei BLOB*/

typedef struct
{
	int valid;		/*column 0 of blobs indicates wether blob is a valid one*/
	int area;		/*column 1 of blobs contains blob area*/
	int topleft;	/*column 2 of blobs contains number of topleft pixel*/
	int xcentroid;     /*column 3 of blobs contains X coordinate of centroid*/
	int ycentroid;     /*column 4 of blobs contains Y coordinate of centroid*/
}blob;

void vlPutSquared (vlImage* pic,int x, int y, int width, int height);
void centroid (vlImage* pic, int nobject, blob *blobs);
void clearBorders (vlImage* pic);


//递归寻找8邻域内blob，现已改进为非递归版本。
void vlFindBlob (vlImage* pic, int n, int i, int j, blob *blobs);



int vlFindAllBlobs(vlImage *pic, blob *blobs);
int vlGetMaxAreaBlob(int numberBlobs, blob *blobs);
int vlDumpAllBlobs(int numberBlobs, blob *blobs, vlImage *pic);
void vlMarkCentroid(vlImage *img, blob *blobs, int nobject);

#endif



