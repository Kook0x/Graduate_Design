// jpeg_decode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "memory.h"
extern "C" {
#include "jpeglib.h"
}

int _tmain(int argc, _TCHAR* argv[])
{
	char *strSourceFileName="test.jpg";
	char *outfilename="test.ppm";
	unsigned char *data;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;


	FILE *f, *fo;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);


	f = fopen(strSourceFileName,"rb");
	if (f==NULL)
	{
	   printf("Open file error!\n");
	   return -1;
	}

	// 
	jpeg_stdio_src(&cinfo, f);

	jpeg_read_header(&cinfo, TRUE);	

	data = new unsigned char[cinfo.image_width*cinfo.image_height*cinfo.num_components];

	jpeg_start_decompress(&cinfo);

	JSAMPROW row_pointer[1];
	while (cinfo.output_scanline < cinfo.output_height)
	{
	   row_pointer[0] = &data[(cinfo.output_height - cinfo.output_scanline-1)*cinfo.image_width*cinfo.num_components];
	   jpeg_read_scanlines(&cinfo,row_pointer, 1);
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);



	/* Open the output file. */
	fo = fopen(outfilename, "wb");
	if (fo == NULL) 
	{
		printf("%s: can't open %s\n", outfilename);
		return -1;
	}

	fprintf(fo, "P6\n%u %u\n255\n", cinfo.image_width, cinfo.image_height);
	unsigned char *ptrData=data+cinfo.image_width*(cinfo.image_height-1)*cinfo.num_components;

	for(int i=cinfo.image_height; i>0; i--)
	{
		for(int j=cinfo.image_width; j>0; j--)
		{
			fprintf(fo, "%c", *ptrData++);
			fprintf(fo, "%c", *ptrData++);
			fprintf(fo, "%c", *ptrData++);
		}
		ptrData-=2*cinfo.image_width*3;
	}

	fclose(fo);
	fclose(f);

	delete data;


	return 0;
}

