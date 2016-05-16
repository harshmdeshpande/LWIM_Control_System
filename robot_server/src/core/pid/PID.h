#ifndef PID_H_
#define PID_H_

class PID {
public:
	PID(double Kp, double Ki, double Kd);
	virtual ~PID();
	
	double compensation(double eCurrent, double y);
	
	
private:
	double Kp;
	double Ki;
	double Kd;
	
	double yprev;
	double eIntegral;
};

#endif /*PID_H_*/
