
/*
例子：展示如何利用SDL显示图像

图像从SRV接收过来

*/

#include "winSoc.h"
#include <string>

#include "picframe.h"


Surveyor	robot(ADDRESS);

PicFrame	frame;


void	myUsleep(long wait)
{
	int i;
	Sleep(10);

	for (i=0; i < wait; i++)
	{
		Sleep(10);
		robot.takePhoto();

		frame.loadPic(robot.getPhoto(), robot.getPhotoSize());
	}
}



/************************************************************************/
/*									*/
/*  main							*/
/*									*/
/************************************************************************/


int main( int argc, char *argv[])
{
	char	buf[256];
	int	arg = 0;
	int	ret;

	if(robot.openRobotLink()==-1)
	{
		printf("robot connection is failed.\n");
		return -1;
	}
	robot.setDebuging(0);
	ret = robot.getVersion(buf);
	printf("getVersion() = %d\n", ret);
	printf("SVR-1 version %s\n", buf);

	robot.setVideoMode(1);

	myUsleep(100);

    return 0;   
}
	
