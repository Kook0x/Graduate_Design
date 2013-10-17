#ifndef PID_H
#define PID_H

#define TOL 30		//和中心X_BASE的偏移误差
#define X_BASE 160	//标定中心

#define PWM_MAX  90
#define PWM_MIN  30
#define PWM_BASE 60

/*
speeds are 2's complement 8-bit binary values - 0x00 through 0x7F is forward, 
0xFF through 0x81 is reverse, e.g. the decimal equivalent of the 4-byte sequence 
0x4D 0x32 0xCE 0x14 = 'M' 50 -50 20 (rotate right for 200ms) 
*/

class PID
{
	double Kp, Ki, Kd;
	double PID_I, PID_D, PID_P;
	int perr;
public:
	int init();
	void cal(int *left, int *right, int x);
};


#endif
