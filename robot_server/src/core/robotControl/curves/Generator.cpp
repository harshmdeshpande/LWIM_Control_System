#include "Generator.h"

int getXYZ(int index, int var) {
	return (((index - 1) * 3) + (var + 1));
}

Generator::Generator(vector<ColumnVector> wpose) {
	this->wpose = wpose;
}

Generator::~Generator() {
}

Vector3d Generator::getPosVector(ColumnVector u) {
	Vector3d pVec;
	pVec.x = u(1);
	pVec.y = u(2);
	pVec.z = u(3);

	return pVec;
}

Vector3d Generator::getDirVectors(Vector3d v, Vector3d u) {
	return v - u;
}

ReturnMatrix Generator::getRotationMatrix(double rx, double ry, double rz) {
	double cA = cos(rz);
	double sA = sin(rz);

	double cB = cos(ry);
	double sB = sin(ry);

	double cG = cos(rx);
	double sG = sin(rx);

	Matrix m(3, 3);
	m = 0.0;

	m.row(1) << cA * cB << cA * sB * sG - sA * cG << cA * sB * cG + sA * sG;
	m.row(2) << sA * cB << sA * sB * sG + cA * cG << sA * sB * cG - cA * sG;
	m.row(3) << -sB << cB * sG << cB * cG;

	m.Release();

	return m;
}

//Declaration of line generator

GLine::GLine(vector<ColumnVector> wpose) :
	Generator(wpose) {

	ColumnVector endPose = wpose.at(1);
	startPose = wpose.at(0);

	//cout << "GenLine::startPose : " << startPose.AsRow();

	this->u = getPosVector(startPose);
	this->v = getPosVector(endPose);

	this->dVector = getDirVectors(this->v, this->u);

	//	JQuat* startQuat = new JQuat();
	//	JQuat* endQuat = new JQuat();

	ColumnVector vec1 = wpose.at(0);
	ColumnVector vec2 = wpose.at(1);

	//	startQuat->setAngles(vec1(4), vec1(5), vec1(6));
	//	endQuat->setAngles(vec2(4), vec2(5), vec2(6));

	//	Quaternion* temp = new Quaternion();
	//	temp->

	Matrix startR = getRotationMatrix(vec1(4), vec1(5), vec1(6));
	Matrix endR = getRotationMatrix(vec2(4), vec2(5), vec2(6));

	q0 = new Quaternion(startR);
	q1 = new Quaternion(endR);

	//	q0 = *startQuat->toQuaternion();
	//	q1 = *endQuat->toQuaternion();

	//	double* a0 = startQuat->toAngles();
	//	double* a1 = endQuat->toAngles();
	//
	//	cout << "QUAT0 " << a0[0] << ", " << a0[1]  << ", " << a0[2] << endl;
	//	cout << "QUAT1 " << a1[0] << ", " << a1[1]  << ", " << a1[2] << endl;

}

GLine::~GLine() {
}

ReturnMatrix GLine::getPosition(double k) {
	ColumnVector pos(3);
	pos(1) = u.x + (dVector.x * k);
	pos(2) = u.y + (dVector.y * k);
	pos(3) = u.z + (dVector.z * k);
	//cout << "GLINE: Pos: " << pos.AsRow();
	pos.Release();
	return pos;
}

ReturnMatrix GLine::getVelocity(double k) {
	//time = time * factor; 
	//Velocity is constant

	ColumnVector vel(3);
	vel(1) = dVector.x;
	vel(2) = dVector.y;
	vel(3) = dVector.z;

	//cout << "GLINE: Velo: " << vel.AsRow();
	vel.Release();
	return vel;
}

Quaternion GLine::getOrientation(double k) {
	return Slerp(*q0, *q1, k); //time should be between 0 & 1 to calculate Slerp
}

ReturnMatrix GLine::getOmega(double k) {
	Quaternion q = Slerp(*q0, *q1, k);
	Quaternion q_dot = Slerp_prime(*q0, *q1, k);

	return Omega(q, q_dot);
}

//Declaration of circle generator

GCircle::GCircle(vector<ColumnVector> wpose) :
	Generator(wpose) {

	//algorithm from "Circle3d.pdf - Knowledge/Maths concepts"
	ColumnVector params(4);
	ColumnVector endPose, pose1;
	Vector3d vPos1, vPos2, vPos3, s, e, a, leg1, leg2;

	startPose = wpose.at(0);
	pose1 = wpose.at(1);
	endPose = wpose.at(2);

	vPos1 = getPosVector(startPose);
	vPos2 = getPosVector(pose1);
	vPos3 = getPosVector(endPose);

	vDir1 = getDirVectors(vPos2, vPos1);
	vDir2 = getDirVectors(vPos3, vPos1);
	vDir3 = getDirVectors(vPos3, vPos2);

	normal = vDir1.crossProduct(vDir2);

	params(1) = vDir1.lengthSq();
	params(2) = vDir2.lengthSq();
	params(3) = vDir3.lengthSq();
	params(4) = normal.lengthSq();

	radius = getRadius(params);
	center = getCenter(params);

	normal.normalize();

	orthoU = getDirVectors(vPos1, getPosVector(center));
	orthoU.normalize();

	orthoV = normal.crossProduct(orthoU);
	orthoV.normalize();

	s = getDirVectors(getPosVector(startPose), getPosVector(center));
	s.normalize();

	e = getDirVectors(getPosVector(endPose), getPosVector(center));
	e.normalize();

	theta = s.dotProduct(e);
	a = s.crossProduct(e);
	theta = atan2(normal.dotProduct(a), s.dotProduct(e));
	if (theta < 0)
		theta = (2 * M_PI) + theta;

	leg1 = vPos1 - getPosVector(center);
	leg2 = vPos2 - getPosVector(center);

	leg1.normalize();
	leg2.normalize();

	double midAngle = acos(abs(leg1.dotProduct(leg2)));

	segmentTime = midAngle / theta;

	//TODO: make option to move full circle or arc!
	//factor = theta / duration;			//since it is not a full circle,
	//instead of 2 * PI / Period,
	//it is theta(central angle) / Duration

}

GCircle::~GCircle() {
}

double GCircle::getRadius(ColumnVector params) {
	double radius;

	radius = params(1) * params(2) * params(3);
	radius = radius / params(4);
	radius = 0.5 * sqrt(radius);

	return radius;
}

ReturnMatrix GCircle::getCenter(ColumnVector params) {

	ColumnVector center(3);
	double dp1 = vDir2.dotProduct(vDir3);
	double dp2 = vDir1.dotProduct(vDir3);

	center(1) = (startPose(1) + (params(1) * dp1 * vDir2[0] - params(2) * dp2
			* vDir1[0]) / (2 * params(4)));

	center(2) = (startPose(2) + (params(1) * dp1 * vDir2[1] - params(2) * dp2
			* vDir1[1]) / (2 * params(4)));

	center(3) = (startPose(3) + (params(1) * dp1 * vDir2[2] - params(2) * dp2
			* vDir1[2]) / (2 * params(4)));

	center.Release();
	return center;
}

ReturnMatrix GCircle::getPosition(double k) {
	ColumnVector pos(3);
	double a = theta * k; //TODO: full circle or circular arc
	//if circular arc, a = theta * k;
	//if complete circle, a = 2 * pi * k

	pos(1) = center(1) + (radius
			* ((cos(a) * orthoU[0]) + (sin(a) * orthoV[0])));
	pos(2) = center(2) + (radius
			* ((cos(a) * orthoU[1]) + (sin(a) * orthoV[1])));
	pos(3) = center(3) + (radius
			* ((cos(a) * orthoU[2]) + (sin(a) * orthoV[2])));

	pos.Release();
	return pos;
}

ReturnMatrix GCircle::getVelocity(double k) {
	ColumnVector vel(3);
	//time = time * factor;

	vel(1) = radius * ((-sin(k) * orthoU[0]) + (cos(k) * orthoV[0]));
	vel(2) = radius * ((-sin(k) * orthoU[1]) + (cos(k) * orthoV[1]));
	vel(3) = radius * ((-sin(k) * orthoU[2]) + (cos(k) * orthoV[2]));

	vel.Release();
	return vel;
}

Quaternion GCircle::getOrientation(double k) {

	//	JQuat* qStart = new JQuat();
	//	JQuat* qEnd = new JQuat();

	// between segments 0 and 1
	double delta = k / (segmentTime - 0);

	int startIndex = 0;
	int endIndex = 1;

	if (k > segmentTime) {
		// between segments 1 and 2
		delta = (k - segmentTime) / (1 - segmentTime);
		startIndex = 1;
		endIndex = 2;
	}

	ColumnVector vec1 = wpose.at(startIndex);
	ColumnVector vec2 = wpose.at(endIndex);

	//	qStart->setAngles(vec1(4), vec1(5), vec1(6));
	//	qEnd->setAngles(vec2(4), vec2(5), vec2(6));
	//
	//	q0 = *qStart->toQuaternion();
	//	q1 = *qEnd->toQuaternion();

	Matrix startR = getRotationMatrix(vec1(4), vec1(5), vec1(6));
	Matrix endR = getRotationMatrix(vec2(4), vec2(5), vec2(6));

	q0 = new Quaternion(startR);
	q1 = new Quaternion(endR);

	return Slerp(*q0, *q1, delta);
}

ReturnMatrix GCircle::getOmega(double k) {
	Quaternion q = Slerp(*q0, *q1, k);
	Quaternion q_dot = Slerp_prime(*q0, *q1, k);

	return Omega(q, q_dot);
}

//Declaration of Bezer Generator

GBezier::GBezier(vector<ColumnVector> wpose) :
	Generator(wpose) {

	p1 = 0.33;
	p2 = 0.67;

	point0.ReSize(3);
	point3.ReSize(3);
	controlPoints = getControlPoints(wpose);

	//factor = 1 / duration;
}

GBezier::~GBezier() {
}

ReturnMatrix GBezier::calculateParams(double param, double point, int j) {

	ColumnVector x(2);
	x(1) = 3 * param * pow(1 - param, 2) * (point - (pow(1 - param, 3)
			* point0(j + 1)) - (pow(param, 3) * point3(j + 1)));

	x(2) = 3 * pow(param, 2) * (1 - param) * (point - (pow(1 - param, 3)
			* point0(j + 1)) - (pow(param, 3) * point3(j + 1)));

	x.Release();
	return x;
}

ReturnMatrix GBezier::getControlPoints(vector<ColumnVector> wpose) {
	double a1 = 0, a2 = 0, a12 = 0, c1x = 0, c1y = 0, c1z = 0, c2x = 0,
			c2y = 0, c2z = 0;
	double paramVal[] = { 0, p1, p2, 1 };
	ColumnVector controlPoints(12), cx(2), cy(2), cz(2);

	//	for(int i = 0; i < 3; i++){
	//		point0(i + 1) = wpose(get(0, i));
	//		point3(i + 1) = wpose(get(3, i));
	//	}
	point0 = wpose.at(0);
	point3 = wpose.at(3);

	for (int i = 0; i < 4; i++) {
		a1 = a1 + (pow(paramVal[i], 2) * pow(1 - paramVal[i], 4));
		a2 = a2 + (pow(paramVal[i], 4) * pow(1 - paramVal[i], 2));
		a12 = a12 + (pow(paramVal[i], 3) * pow(1 - paramVal[i], 3));

		ColumnVector _wpose = wpose.at(i);

		cx = calculateParams(paramVal[i], _wpose(1), X);
		cy = calculateParams(paramVal[i], _wpose(2), Y);
		cz = calculateParams(paramVal[i], _wpose(3), Z);

		c1x = c1x + cx(1);
		c1y = c1y + cy(1);
		c1z = c1z + cz(1);

		c2x = c2x + cx(2);
		c2y = c2y + cy(2);
		c2z = c2z + cz(2);

	}

	a1 = 9 * a1;
	a2 = 9 * a2;
	a12 = 9 * a12;

	for (int i = 1; i <= 3; i++)
		controlPoints(i) = point0(i);

	controlPoints(4) = ((a2 * c1x) - (a12 * c2x)) / ((a1 * a2) - pow(a12, 2));
	controlPoints(5) = ((a2 * c1y) - (a12 * c2y)) / ((a1 * a2) - pow(a12, 2));
	controlPoints(6) = ((a2 * c1z) - (a12 * c2z)) / ((a1 * a2) - pow(a12, 2));

	controlPoints(7) = ((a1 * c2x) - (a12 * c1x)) / ((a1 * a2) - pow(a12, 2));
	controlPoints(8) = ((a1 * c2y) - (a12 * c1y)) / ((a1 * a2) - pow(a12, 2));
	controlPoints(9) = ((a1 * c2z) - (a12 * c1z)) / ((a1 * a2) - pow(a12, 2));

	int j = 1;
	for (int i = 10; i <= 12; i++) {
		controlPoints(i) = point3(j);
		j++;
	}

	return controlPoints;
}

ReturnMatrix GBezier::getPosition(double k) {
	ColumnVector pos(3);

	pos(1) = (pow(1 - k, 3) * controlPoints(getXYZ(1, X))) + (3 * k * pow( 1
			- k, 2) * controlPoints(getXYZ(2, X))) + (3 * pow(k, 2) * (1 - k)
			* controlPoints(getXYZ(3, X))) + (pow(k, 3) * controlPoints(getXYZ(
			4, X)));

	pos(2) = (pow(1 - k, 3) * controlPoints(getXYZ(1, Y))) + (3 * k * pow( 1
			- k, 2) * controlPoints(getXYZ(2, Y))) + (3 * pow(k, 2) * (1 - k)
			* controlPoints(getXYZ(3, Y))) + (pow(k, 3) * controlPoints(getXYZ(
			4, Y)));

	pos(3) = (pow(1 - k, 3) * controlPoints(getXYZ(1, Z))) + (3 * k * pow( 1
			- k, 2) * controlPoints(getXYZ(2, Z))) + (3 * pow(k, 2) * (1 - k)
			* controlPoints(getXYZ(3, Z))) + (pow(k, 3) * controlPoints(getXYZ(
			4, Z)));

	pos.Release();
	return pos;
}

ReturnMatrix GBezier::getVelocity(double k) {
	ColumnVector vel(3);

	vel(1) = -(3 * (1 - pow(k, 2)) * controlPoints(getXYZ(1, X))) + (3 * (pow(
			k, 2) - (6 * k) + 1) * controlPoints(getXYZ(2, X))) + (3 * k * (k
			+ 1) * controlPoints(getXYZ(3, X))) + (3 * pow(k, 2)
			* controlPoints(getXYZ(4, X)));

	vel(2) = -(3 * (1 - pow(k, 2)) * controlPoints(getXYZ(1, Y))) + (3 * (pow(
			k, 2) - (6 * k) + 1) * controlPoints(getXYZ(2, Y))) + (3 * k * (k
			+ 1) * controlPoints(getXYZ(3, Y))) + (3 * pow(k, 2)
			* controlPoints(getXYZ(4, Y)));

	vel(3) = -(3 * (1 - pow(k, 2)) * controlPoints(getXYZ(1, Z))) + (3 * (pow(
			k, 2) - (6 * k) + 1) * controlPoints(getXYZ(2, Z))) + (3 * k * (k
			+ 1) * controlPoints(getXYZ(3, Z))) + (3 * pow(k, 2)
			* controlPoints(getXYZ(4, Z)));

	vel.Release();
	return vel;
}

Quaternion GBezier::getOrientation(double k) {

	//	return Slerp(q0, q1, time/duration);
	//	JQuat* qStart = new JQuat();
	//	JQuat* qEnd = new JQuat();

	// between segments 0 and 1
	double delta = k / (p1 - 0);

	int startIndex = 0;
	int endIndex = 1;

	if (k > p1) {
		// between segments 1 and 2
		delta = (k - p1) / (p2 - p1);
		startIndex = 1;
		endIndex = 2;
	}

	if (k > p2) {
		// between segments 2 and 3
		delta = (k - p2) / (1 - p2);
		startIndex = 2;
		endIndex = 3;
	}

	ColumnVector vec1 = wpose.at(startIndex);
	ColumnVector vec2 = wpose.at(endIndex);

	//	qStart->setAngles(vec1(4), vec1(5), vec1(6));
	//	qEnd->setAngles(vec2(4), vec2(5), vec2(6));
	//
	//	q0 = *qStart->toQuaternion();
	//	q1 = *qEnd->toQuaternion();

	Matrix startR = getRotationMatrix(vec1(4), vec1(5), vec1(6));
	Matrix endR = getRotationMatrix(vec2(4), vec2(5), vec2(6));

	q0 = new Quaternion(startR);
	q1 = new Quaternion(endR);

	return Slerp(*q0, *q1, delta);
}

ReturnMatrix GBezier::getOmega(double k) {
	Quaternion q = Slerp(*q0, *q1, k);
	Quaternion q_dot = Slerp_prime(*q0, *q1, k);

	return Omega(q, q_dot);
}

//Declaration of Spline generator

GSpline::GSpline(vector<ColumnVector> wpose) :
	Generator(wpose) {
	init();
}

void GSpline::init() {

	int size = wpose.size();
	Matrix pts(4, size);
	quat_map quat;
	//	double delta = 1 / ((double) size - 1);

	//	int i = 1;
	//	for (double t = 0; t <= 1; t += delta) {

	for (int i = 0; i < size; i++) {
		double time = (0.999 * i) / (size - 1);
		//		double k = ((i * 1.0) / size);
		ColumnVector p = wpose.at(i);

		//position
		pts(1, i + 1) = time;
		pts.SubMatrix(2, 4, i + 1, i + 1) = p.Rows(1, 3);

		//quaternion
		//		JQuat* jq = new JQuat();
		//		jq->setAngles(r(1), r(2), r(3));
		//		Quaternion* q = jq->toQuaternion();

		Matrix R = getRotationMatrix(p(4), p(5), p(6));
		Quaternion *q = new Quaternion(R);
		quat.insert(quat_map::value_type(time, *q));
		//		i++;
	}
	cout << pts << endl;
	splineP = new Spl_path(pts);
	splineQ = new Spl_Quaternion(quat);
}

ReturnMatrix GSpline::getPVector(ColumnVector p) {
	ColumnVector pos(3);
	pos(1) = p(1);
	pos(2) = p(2);
	pos(3) = p(3);

	pos.Release();
	return pos;
}

ReturnMatrix GSpline::getRVector(ColumnVector p) {
	ColumnVector r(3);
	r(1) = p(4);
	r(2) = p(5);
	r(3) = p(6);

	r.Release();
	return r;
}

ReturnMatrix GSpline::getPosition(double k) {
	ColumnVector p(3);
	splineP->interpolating(k, p);

	p.Release();
	return p;
}

ReturnMatrix GSpline::getVelocity(double k) {
	ColumnVector pd(3);
	splineP->first_derivative(k, pd);

	pd.Release();
	return pd;
}

Quaternion GSpline::getOrientation(double k) {
	Quaternion *q = new Quaternion();
	splineQ->quat(k, *q);
	return *q;
}

ReturnMatrix GSpline::getOmega(double k) {
	ColumnVector w(3);
	Quaternion* q = new Quaternion();
	splineQ->quat_w(k, *q, w);

	delete q;
	w.Release();
	return w;
}

GSpline::~GSpline() {
}
