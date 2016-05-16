#include "PID.h"
#include "math.h"
#include <iostream>

//using namespace std;

PID::PID(double x, double y, double z)
{
	yprev = 0;
	eIntegral = 0;
	Kp = x;
	Ki = y;
	Kd = z;
}

PID::~PID()
{
	
}

double PID::compensation(double eCurrent, double y){
	eIntegral = eIntegral + eCurrent;
	
	//cout << "eIntegral " << eIntegral << " eCurrent " << eCurrent << endl;
	//cout << "Kp " << Kp << " Ki " << Ki << " Kd " << Kd << endl;
	double u = Kp*eCurrent + Ki*eIntegral + Kd*(y - yprev);
	yprev = y;
	
	return u;
}
