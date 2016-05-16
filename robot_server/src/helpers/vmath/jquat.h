#ifndef JQUAT_H_
#define JQUAT_H_

#include <roboop/quaternion.h>

class JQuat {
private:
	double x, y, z, w;

public:
	JQuat();
	JQuat(double x, double y, double z, double w);
	JQuat(double angles[]);
	JQuat(JQuat* q);

	virtual ~JQuat();

	void set(double x, double y, double z, double w);
	void set(JQuat* q);
	void loadIdentity();
	bool isIdentity();

	void setAngles(double yaw, double roll, double pitch);
	double* toAngles();

	void setRotationMatrix(const Matrix &m);
	ReturnMatrix toRotationMatrix();

	JQuat* mult(double scalar);
	double dot(JQuat* q);
	double norm();
	void normalize();
	JQuat* inverse();
	JQuat* inverseLocal();
	void negate();
	bool equals(JQuat j);

	Quaternion* toQuaternion();

	double W() {return w;};
	double X() {return x;};
	double Y() {return y;};
	double Z() {return z;};
};


#endif /*JQUAT_H_*/
