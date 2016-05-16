#ifndef SPLINESET_H_
#define SPLINESET_H_

#include "Spline.h"
#include <vector.h>

class SplineSet {
public:
	SplineSet();
	double calcOptimumTime(ColumnVector& maxVelos);
	double getOptimumTime();
	double getDuration();
	bool scaleSplineSet(double k);
	
	void addSpline(Spline* s);
	Spline* get(int index);
	unsigned size();
	
	virtual ~SplineSet();

private:
	double period;
	double optimumTime;
	vector<Spline*> ss;
};

#endif /*SPLINESET_H_*/
