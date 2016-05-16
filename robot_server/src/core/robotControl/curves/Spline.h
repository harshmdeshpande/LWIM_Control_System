#ifndef SPLINE_H_
#define SPLINE_H_

#include <roboop/robot.h>

class Spline {
public:
	Spline(Matrix& pts);
	virtual ~Spline();

	double* interpolate(double t);
	void scale(double k);
	double getOptimumTime(double maxEPOS);

private:
	int n;
	double time_scale_factor;
	ColumnVector time;

	ColumnVector A, B, D, H, M;
	ColumnVector C, L, U, Z;
	Matrix pts;

	void initSplineData();
	void findCoeffs();
};

#endif /*SPLINE_H_*/
