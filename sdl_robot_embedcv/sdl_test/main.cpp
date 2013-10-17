
/*
���ӣ�չʾ�������SDL��ʾͼ��

ͼ���SRV���չ���,����������RGB�ֽ���

*/

#include "stdafx.h"
//
//#include "memory.h"
//extern "C" {
//#include "jpeglib.h"
//}
#include "jpeglib.h"
#include "winSoc.h"
#include <string>

#include "picframe.h"

#define PPM_FORMAT 3
#define PGM_FORMAT 1

int saveImage(unsigned char* src,long int len);

void	vision_working(Surveyor &robot, PicFrame &frame)
{

	while(1)
	{
		robot.takePhoto();

		frame.loadPic(robot.getPhoto(), robot.getPhotoSize());

		saveImage((unsigned char*)robot.getPhoto(),robot.getPhotoSize());
	}
}


int saveImage(unsigned char *src, long int size)
{

	char *fo_name="test.ppm";	
	FILE  *fo;
	
	//��������ʼ����ѹ������ͬʱ�ƶ�������Ϣ������
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);



	//���ڴ��ж�ȡͼ����Ϣ
	jpeg_mem_src(&cinfo, src, size);
	jpeg_read_header(&cinfo, TRUE);

	//����ͼ����Ϣ����һ��ͼ�񻺳���������������data
	unsigned char *data;
	data = new unsigned char[cinfo.image_width*cinfo.image_height*cinfo.num_components];

	//��ʼ����
	jpeg_start_decompress(&cinfo);
	JSAMPROW row_pointer[1];
	while (cinfo.output_scanline < cinfo.output_height)
	{
	   row_pointer[0] = &data[(cinfo.output_height - cinfo.output_scanline-1)*cinfo.image_width*cinfo.num_components];
	   jpeg_read_scanlines(&cinfo,row_pointer, 1);
	}

	//��ɽ���
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	//���������ļ�
	fo = fopen(fo_name, "wb");
	if (fo == NULL) 
	{
		printf("%s: can't open %s\n", fo_name);
		return -1;
	}
	if(cinfo.num_components==PPM_FORMAT)
	{

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
			ptrData-=2*cinfo.image_width*cinfo.num_components;
		}

	}
	else if(cinfo.num_components==PGM_FORMAT)
	{
		fprintf(fo, "P5\n%u %u\n255\n", cinfo.image_width, cinfo.image_height);
		unsigned char *ptrData=data+cinfo.image_width*(cinfo.image_height-1)*cinfo.num_components;
		for(int i=cinfo.image_height; i>0; i--)
		{
			for(int j=cinfo.image_width; j>0; j--)
				fprintf(fo, "%c", *ptrData++);
			ptrData-=2*cinfo.image_width;
		}
	}

	fclose(fo);
	delete data;

	return 0;
}

/************************************************************************/
/*									*/
/*  main							*/
/*									*/
/************************************************************************/


int _tmain(int argc, _TCHAR* argv[])
{

	Surveyor	robot(ADDRESS);
	PicFrame	frame;

	char	buf[256];
	int	arg = 0;
	int	ret;

	if(robot.openRobotLink()<0)
	{
		printf("robot network connection is failed.\n");
		return -1;
	}
	robot.setDebuging(0);
	ret = robot.getVersion(buf);
	printf("getVersion() = %d\n", ret);
	printf("SVR-1 version %s\n", buf);

	robot.setVideoMode(1);

	vision_working(robot, frame);

    return 0;   
}
	
