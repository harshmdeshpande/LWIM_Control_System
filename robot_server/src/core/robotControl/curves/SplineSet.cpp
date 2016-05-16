#include "SplineSet.h"

SplineSet::SplineSet() {
	optimumTime = 0.0;
}

void SplineSet::addSpline(Spline* s) {
	ss.push_back(s);
}

double SplineSet::getOptimumTime() {
	return this->optimumTime;
}

double SplineSet::getDuration() {
	return this->period;
}

double SplineSet::calcOptimumTime(ColumnVector& maxVelos) {
	for (unsigned i = 0; i < ss.size(); i++) {
		Spline* s = ss.at(i);
		double maxima = s->getOptimumTime(maxVelos(i + 1));
		if (maxima > optimumTime) {
			optimumTime = maxima;
		}
	}
	return this->scaleSplineSet(1.0);
}

bool SplineSet::scaleSplineSet(double k) {
	period = optimumTime * k;
	if (period < optimumTime) {
		period = optimumTime;
		cerr << "TOO FAST TOO FURIOUS!!" << endl;
		return false;
	}

	for (unsigned i = 0; i < ss.size(); i++) {
		Spline* s = ss.at(i);
		s->scale(k);
	}
	return true;
}

Spline* SplineSet::get(int index) {
	return ss.at(index);
}

unsigned SplineSet::size() {
	return ss.size();
}

SplineSet::~SplineSet() {
}
