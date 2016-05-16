#include "jquat.h"
#include <cmath>
#include <iostream>

JQuat::JQuat() {
	this->w = 1.0;
	this->x = this->y = this->z = 0;
}

JQuat::JQuat(double _x, double _y, double _z, double _w) {
	this->x = _x;
	this->y = _y;
	this->z = _z;
	this->w = _w;
}

void JQuat::set(double _x, double _y, double _z, double _w) {
	this->x = _x;
	this->y = _y;
	this->z = _z;
	this->w = _w;
}

void JQuat::set(JQuat* q) {
	this->x = q->x;
	this->y = q->y;
	this->z = q->z;
	this->w = q->w;
}

JQuat::JQuat(double* angles) {
	this->setAngles(angles[0], angles[1], angles[2]);
}

JQuat::JQuat(JQuat* q) {
	this->x = q->x;
	this->y = q->y;
	this->z = q->z;
	this->w = q->w;
}

void JQuat::loadIdentity() {
	this->x = this->y = this->z = 0;
	this->w = 1;
}

bool JQuat::isIdentity() {
	if (this->x == 0 && this->y == 0 && this->z == 0 && this->w == 1)
		return true;
	else
		return false;
}

void JQuat::setAngles(double yaw, double roll, double pitch){
	double angle;
	double sinRoll, sinPitch, sinYaw, cosRoll, cosPitch, cosYaw;
	angle = pitch * 0.5;
	sinPitch = sin(angle);
	cosPitch = cos(angle);
	angle = roll * 0.5;
	sinRoll = sin(angle);
	cosRoll = cos(angle);
	angle = yaw * 0.5;
	sinYaw = sin(angle);
	cosYaw = cos(angle);

	// variables used to reduce multiplication calls.
	double cosRollXcosPitch = cosRoll * cosPitch;
	double sinRollXsinPitch = sinRoll * sinPitch;
	double cosRollXsinPitch = cosRoll * sinPitch;
	double sinRollXcosPitch = sinRoll * cosPitch;

	this->w = (cosRollXcosPitch * cosYaw - sinRollXsinPitch * sinYaw);
	this->x = (cosRollXcosPitch * sinYaw + sinRollXsinPitch * cosYaw);
	this->y = (sinRollXcosPitch * cosYaw + cosRollXsinPitch * sinYaw);
	this->z = (cosRollXsinPitch * cosYaw - sinRollXcosPitch * sinYaw);

	normalize();
}

double* JQuat::toAngles() {
	double sqw = this->w * this->w;
	double sqx = this->x * this->x;
	double sqy = this->y * this->y;
	double sqz = this->z * this->z;
	double unit = sqx + sqy + sqz + sqw; // if normalized is one, otherwise

	// is correction factor
	double test = x * y + z * w;
	double* angles = new double[3];

	if (test > 0.499 * unit) { // singularity at north pole
		angles[1] = 2 * atan2(x, w);
		angles[2] = M_PI / 2;
		angles[0] = 0;
	} else if (test < -0.499 * unit) { // singularity at south pole
		angles[1] = -2 * atan2(x, w);
		angles[2] = -M_PI / 2;
		angles[0] = 0;
	} else {
		angles[1] = atan2(2 * y * w - 2 * x * z, sqx - sqy - sqz + sqw); // roll or heading
		angles[2] = asin(2 * test / unit); // pitch or attitude
		angles[0] = atan2(2 * x * w - 2 * y * z, -sqx + sqy - sqz + sqw); // yaw or bank
	}
	return angles;
}

ReturnMatrix JQuat::toRotationMatrix() {
	double _norm = norm();
	// we explicitly test norm against one here, saving a division
	// at the cost of a test and branch.  Is it worth it?
	double s = (_norm == 1) ? 2 : (_norm > 0) ? 2 / _norm : 0;

	// compute xs/ys/zs first to save 6 multiplications, since xs/ys/zs
	// will be used 2-4 times each.
	double xs = x * s;
	double ys = y * s;
	double zs = z * s;
	double xx = x * xs;
	double xy = x * ys;
	double xz = x * zs;
	double xw = w * xs;
	double yy = y * ys;
	double yz = y * zs;
	double yw = w * ys;
	double zz = z * zs;
	double zw = w * zs;

	Matrix result(3, 3);
	// using s=2/norm (instead of 1/norm) saves 9 multiplications by 2 here
	result.Row(1) << 1 - (yy + zz) << (xy - zw) << (xz + yw);
	result.Row(2) << (xy + zw) << 1 - (xx + zz) << (yz - xw);
	result.Row(3) << (xz - yw) << (yz + xw) << 1 - (xx + yy);
	result.release();

	return result;
}

Quaternion* JQuat::toQuaternion() {
	return new Quaternion(this->w, this->x, this->y, this->z);
}

void JQuat::setRotationMatrix(const Matrix &m) {

	const Real* mx = mx = m.const_data();

	double t = mx[0] + mx[4] + mx[6];

	// we protect the division by s by ensuring that s>=1
	if (t >= 0) { // |w| >= .5
		double s = sqrt(t+1); // |s|>=1 ...
		w = 0.5f * s;
		s = 0.5f / s;                 // so this division isn't bad
		x = (mx[7] - mx[5]) * s;
		y = (mx[2] - mx[6]) * s;
		z = (mx[3] - mx[1]) * s;
	} else if ((mx[0] > mx[4]) && (mx[0] > mx[8])) {
		double s = sqrt(1.0f + mx[0] - mx[4] - mx[8]); // |s|>=1
		x = s * 0.5f; // |x| >= .5
		s = 0.5f / s;
		y = (mx[3] + mx[1]) * s;
		z = (mx[2] + mx[6]) * s;
		w = (mx[7] - mx[5]) * s;
	} else if (mx[4] > mx[8]) {
		double s = sqrt(1.0f + mx[4] - mx[0] - mx[8]); // |s|>=1
		y = s * 0.5f; // |y| >= .5
		s = 0.5f / s;
		x = (mx[3] + mx[1]) * s;
		z = (mx[7] + mx[5]) * s;
		w = (mx[2] - mx[6]) * s;
	} else {
		double s = sqrt(1.0f + mx[8] - mx[0] - mx[4]); // |s|>=1
		z = s * 0.5f; // |z| >= .5
		s = 0.5f / s;
		x = (mx[2] + mx[6]) * s;
		y = (mx[7] + mx[5]) * s;
		w = (mx[3] - mx[1]) * s;
	}
}

JQuat* JQuat::mult(double scalar) {
	return new JQuat(scalar * x, scalar * y, scalar * z, scalar * w);
}

double JQuat::dot(JQuat* q) {
	return w * q->w + x * q->x + y * q->y + z * q->z;
}

double JQuat::norm() {
	return w * w + x * x + y * y + z * z;
}

void JQuat::normalize() {
	double n = 1.0 / sqrt(norm());
	x *= n;
	y *= n;
	z *= n;
	w *= n;
}

JQuat* JQuat::inverse() {
	double _norm = norm();
	if (_norm > 0.0) {
		double invNorm = 1.0f / _norm;
		return new JQuat(-x * invNorm, -y * invNorm, -z * invNorm, w * invNorm);
	}
	// return an invalid result to flag the error
	return new JQuat();
}

JQuat* JQuat::inverseLocal() {
	float _norm = norm();
	if (_norm > 0.0) {
		double invNorm = 1.0f / _norm;
		x *= -invNorm;
		y *= -invNorm;
		z *= -invNorm;
		w *= invNorm;
	}
	return this;
}

void JQuat::negate() {
	x *= -1;
	y *= -1;
	z *= -1;
	w *= -1;
}

JQuat::~JQuat() {
}
