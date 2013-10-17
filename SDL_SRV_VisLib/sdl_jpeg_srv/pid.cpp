#include <stdio.h>
#include <math.h>

#include "pid.h"


int PID::init()
{
  FILE *pFp;

  if (NULL == (pFp = fopen ("pid.txt", "r"))) {
    perror ("HSI carl parameters: unable to create file");
    return (-1);		/* failure */
  }

  fscanf (pFp, "%lf %lf %lf",  &Kp, &Ki, &Kd);
    
  if (0 > fclose (pFp)) {
    perror ("file close error");
    return (-1);		/* failure */
  }

  perr=0;
  PID_P=  0.0;
  PID_I = 0.0;
  PID_D = 0.0;

  return (0);
}

void PID::cal(int *left, int *right, int x)
{
	int correction;
	int leftpulse=PWM_BASE, rightpulse=PWM_BASE;

	//相对x中心偏差
	int fabError,error;
	error = x-X_BASE;
	fabError=error;
	if(fabError<0)
		fabError=-fabError;

	//在超过允许的误差范围时,调整速度
	if(fabError>TOL)
	{
		PID_P= error * Kp;

		PID_I += error;
		PID_I *= Ki;		
				
		PID_D = (error - perr)*Kd;
				
		correction = (int)(PID_P + PID_I +PID_D);

		//左右轮差速运动
		rightpulse = PWM_BASE - correction;
		leftpulse = PWM_BASE + correction;
		

		if(leftpulse > PWM_MAX)
			leftpulse = PWM_MAX;
		else 	if(leftpulse <=PWM_MIN)
			leftpulse = PWM_MIN;
		
				
		if(rightpulse > PWM_MAX)
			rightpulse = PWM_MAX;
		else 	if(rightpulse <=PWM_MIN)
			rightpulse = PWM_MIN;
	}

	//设定速度
	*left=leftpulse;
	*right=rightpulse;	
	perr=error;//存储误差用于下次运算

}


