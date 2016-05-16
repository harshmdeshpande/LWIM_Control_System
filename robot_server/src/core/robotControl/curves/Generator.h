#ifndef GENERATOR_H_
#define GENERATOR_H_

#include <vector>
#include <roboop/quaternion.h>
#include <roboop/trajectory.h>
#include "../../../helpers/vmath/jquat.h"
#include "../../../helpers/vmath/vmath.h"

//Generator interface
class Generator {
private:
	int steps;

public:
	const static double dt = 1e-3;
	const static int X = 0;
	const static int Y = 1;
	const static int Z = 2;

	vector<ColumnVector> wpose;
	ColumnVector startPose;
	Vector3d dVector;

	Generator(vector<ColumnVector>wpose);
	virtual ~Generator();

	virtual ReturnMatrix getPosition(double k) = 0;
	virtual ReturnMatrix getVelocity(double k) = 0;

	virtual Quaternion getOrientation(double k) = 0;
	virtual ReturnMatrix getOmega(double k) = 0;

	ColumnVector getStartPose();
	ReturnMatrix getRotationMatrix(double rx, double ry, double rz);

	Vector3d getPosVector(ColumnVector u);
	Vector3d getDirVectors(Vector3d v, Vector3d u);
};

//Linear path
class GLine : public Generator {
private:
	Vector3d u, v;
	Quaternion *q0, *q1;

public:
	GLine(vector<ColumnVector>wpose);
	virtual ~GLine();

	ReturnMatrix getPosition(double k);
	ReturnMatrix getVelocity(double k);
	Quaternion getOrientation(double k);
	ReturnMatrix getOmega(double k);
};

//Circular path
class GCircle : public Generator {
private:
	double radius, theta, segmentTime;
	ColumnVector center;
	Vector3d vDir1, vDir2, vDir3, normal, orthoU, orthoV;
	Quaternion *q0, *q1;

	double getRadius(ColumnVector params);
	ReturnMatrix getCenter(ColumnVector params);

public:
	GCircle(vector<ColumnVector>wpose);
	virtual ~GCircle();

	ReturnMatrix getPosition(double k);
	ReturnMatrix getVelocity(double k);
	Quaternion getOrientation(double k);
	ReturnMatrix getOmega(double k);
};

//Bezier path
class GBezier : public Generator {
private:
	double p1;
	double p2;
	Quaternion *q0, *q1;
	ColumnVector point0, point3;
	ColumnVector controlPoints;

	ReturnMatrix calculateParams(double param, double point, int j);
	ReturnMatrix getControlPoints(vector<ColumnVector> wpose);

public:
	GBezier(vector<ColumnVector>wpose);
	virtual ~GBezier();

	ReturnMatrix getPosition(double k);
	ReturnMatrix getVelocity(double k);
	Quaternion getOrientation(double k);
	ReturnMatrix getOmega(double k);
};

//Spline path
class GSpline : public Generator {
private:
	void init();
	ReturnMatrix getPVector(ColumnVector point);
	ReturnMatrix getRVector(ColumnVector point);

	Spl_path* splineP;
	Spl_Quaternion* splineQ;
public:
	GSpline(vector<ColumnVector>wpose);
	virtual ~GSpline();

	ReturnMatrix getPosition(double k);
	ReturnMatrix getVelocity(double k);
	Quaternion getOrientation(double k);
	ReturnMatrix getOmega(double k);
};

#endif /*GENERATOR_H_*/
