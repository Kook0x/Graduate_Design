
/*
例子：展示如何利用SDL显示图像

图像从SRV接收过来,经过解码变成RGB字节流

*/
#include "stdafx.h"
#include <string.h>

#include "winSoc.h"

#include "picframe.h"
#include "pid.h"

//将静态库导入
#pragma comment(lib,"..\\..\\SDL_SRV_VisLib\\VisLib_win\\lib\\VisLib_win.lib") 

static int	vision_working(Surveyor &robot, PicFrame &frame, blob *obj)
{
	int ret;
	robot.takePhoto();

	ret=frame.processing(robot.getPhoto(), robot.getPhotoSize(), obj);
	return ret;
}

static int navigation_working(Surveyor &robot, PID &pid, blob *obj)
{
	int rw, lw;
	static int nofound = 0;

	if(obj->valid==-1)
	{
		//连续多帧图像没有发现目标
		if(++nofound >= 5)
		{	
			//应该通过随机数控制机器人随机旋转一个角度
			printf("robot is seeking object...");
			//robot.drive(30, -30, 500);
			//robot.drive(30, 30);
			//robot.drive(0,0);
		}
	}
	else
	{
		nofound = 0;
		pid.cal(&lw, &rw, obj->ycentroid);
		printf("left=%d, right=%d\n", lw, rw);
		//robot.drive(lw, rw);		
	}

	return 1;
}

/************************************************************************/
/*									*/
/*  main							*/
/*									*/
/************************************************************************/


int _tmain(int argc, _TCHAR* argv[])
{
	char	buf[256];
	int	arg = 0;
	int	ret;

	Surveyor	robot(ADDRESS);
	PicFrame	frame;
	PID pid;

	blob obj;

	if(pid.init()<0)
	{
		printf("PID parameters are not set properly.\n");
		return -1;
	}

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


	//
	//robot.drive(60,60,1000);
	//robot.drive(0,0);
	

	while(1)
	{
		//vision
		if(vision_working(robot, frame, &obj)<0)
		{
			robot.drive(0,0,0);
			printf("system is exiting now.\n");
			break;
		}
		//navigation
		navigation_working(robot, pid, &obj);
	}
    return 0;   
}
	
