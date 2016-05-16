#include "KinematicsSolver.h"

#include <cmath>

const double DH_a0 = 0.0;
const double DH_a1 = 0.0345;
const double DH_a2 = 0.2;
const double DH_a3 = 0.04;
const double DH_a4 = 0.0;
const double DH_a5 = 0.0;

const double DH_d1 = 0.297;
const double DH_d2 = 0.0;
const double DH_d3 = 0.0;
const double DH_d4 = 0.2165;
const double DH_d5 = 0.0;
const double DH_d6 = 0.1500;

const double DH_alfa0 = 0.0;
const double DH_alfa1 = -M_PI_2;
const double DH_alfa2 = 0.0;
const double DH_alfa3 = -M_PI_2;
const double DH_alfa4= M_PI_2;
const double DH_alfa5 = -M_PI_2;

inline double normalizeAngle(double rad) {
	if (rad< 0 || rad> 6.28){
	return 2*M_PI - fabs(rad);
}

return rad;
}

inline double toDegrees(double radians) {
	return radians * 180 / M_PI;
}

inline double toRadians(double degrees) {
	return degrees / 180 * M_PI;
}

double* setRange(double minDeg, double maxDeg) {
	double* range = new double[2];
	range[0] = toRadians(minDeg);
	range[1] = toRadians(maxDeg);

	return range;
}

//Got these from experimentation
const double* JL1 = setRange(-170, 170);
const double* JL2 = setRange(-150, 30);
const double* JL3 = setRange(-160, 60);
const double* JL4 = setRange(-170, 170);
const double* JL5 = setRange(-140, 140);
const double* JL6 = setRange(-360, 360);

const double MAX_TOLERANCE = 0.0001;

inline bool DBL_EQ(double x, double y) {
	if (fabs(x - y) > MAX_TOLERANCE) {
		return false;
	}

	return true;
}

inline double roundTo(double x, double y) {
	if (DBL_EQ(x, y)) {
		return y;
	}

	return x;
}

inline double normalize(double x) {
	return roundTo(roundTo(roundTo(x, -1), 1), 0);
}

inline bool sameAngles(double t[2]) {
	return DBL_EQ(t[0], t[1]);
}

KinematicsSolver::KinematicsSolver(Robot* robot) {
	this->robot = robot;
}

KinematicsSolver::~KinematicsSolver() {
}

/*
 * void InsertionSort( apvector <int> &num) {
 *		int i, j, key, numLength = num.length( );
 *		for(j = 1; j < numLength; j++) {   // Start with 1 (not 0)
 *			key = num[j];
 *			for(i = j - 1; (i >= 0) && (num[i] < key); i--) {  // Smaller values move up
 *				num[i+1] = num[i];
 *			}
 *			num[i+1] = key;    //Put key into its proper location
 *		}
 *	return;
 * 	}
 */

void KinematicsSolver::sort(vector<SolutionSet*>&ss, SolutionSet* set) {
	vector<SolutionSet*>::iterator it;

	if (ss.size() == 0) {
		//		cout << "1st weight " << set->weight << endl;
		ss.push_back(set);
		return;
	}

	it = ss.begin();
	while (set->weight > (*it)->weight) {
		it++;
		if (it == ss.end()) {
			break;
		}
	}

	if (it > ss.end()) {
		//		cout << "push weight " << set->weight << endl;
		ss.push_back(set);
	} else {
		//		cout << "insert weight " << set->weight << endl;
		ss.insert(it, set);
	}
}

void KinematicsSolver::addSolutionSet(double t1, double t2, double t3,
		double t4, double t5, double t6) {
	SolutionSet *set = new SolutionSet(t1, t2, t3, t4, t5, t6);
	double weight = selector->setBestSolution(set); //returns weight of current solution
	set->weight = weight;

	//solutions.push_back(set);
	sort(solutions, set);
}

bool KinematicsSolver::checkJointValidity(double joint, const double JL[2]) {
	return joint <= JL[1] && joint >= JL[0];
}

bool KinematicsSolver::checkTheta123(double t1, double t2, double t3) {

	double x = pos(1);
	double y = pos(2);
	double z = pos(3);

	Matrix T = getT(DH_a0, DH_d1, DH_alfa0, t1) * getT(DH_a1, DH_d2, DH_alfa1,
			t2) * getT(DH_a2, DH_d3, DH_alfa2, t3) * getT(DH_a3, DH_d4,
			DH_alfa3, 0);

	//TODO: 0.27 0.12 0.024 is not being solved!!
	//cout << T << endl;
	//cout << x << " " << y << " " << z << endl;

	double _X = T(1, 4);
	double _Y = T(2, 4);
	double _Z = T(3, 4) - DH_d1;

	//cout << endl << "CHECKING 123 [ " << t1 << ", " << t2 << ", " << t3 << "] for ";
	//cout << "Loc: " << "[ " << x << ", " << y << ", " << z << "].....   ";

	bool _check = DBL_EQ(_X, x) && DBL_EQ(_Y, y) && DBL_EQ(_Z, z);

	//if (_check) {
	//	cout << "OK.." << _Z << endl;
	//} else {
	//	cout << "wrong!" << endl;
	//}

	return _check;
}

bool KinematicsSolver::solveTheta4(double t6, double t5, double t3, double t2,
		double t1, Matrix T) {

	double s5 = sin(t5);
	double c4 = -T(1, 3) / s5;
	double s4 = T(3, 3) / s5;

	double t4 = atan2(s4, c4);

	bool solved1 = false;
	bool solved2 = false;

	if (checkJointValidity(t4, JL4)) {
		solved1 = true;
		addSolutionSet(t1, t2, t3, t4, t5, t6);
		//cout << "solution : " << t1 << " " << t2 << " " << t3 << " " << t4 << " " << t5 << " " << t6 << endl;
	}

	return solved1 || solved2;
}

bool KinematicsSolver::solveTheta6(double t5, double t3, double t2, double t1,
		Matrix T) {
	//	cout << t5 << ", " << t3 << ", " << t2 << ", " << t1 << endl;
	double s5 = sin(t5);

	//cout << T(2,1) << endl;
	//cout << T(2,2) << endl;

	double c6 = T(2, 1) / s5;
	double s6 = -T(2, 2) / s5;

	//cout << s5 << endl;
	//cout << c6 << endl;
	//cout << s6 << endl;

	double t6 = atan2(s6, c6);

	//	cout << "T6: " << t6 << endl;

	bool solved1 = false;
	bool solved2 = false;

	if (checkJointValidity(t6, JL6)) {
		//if(checkTheta6(t6[0])){
		solved1 = solveTheta4(t6, t5, t3, t2, t1, T);
		//}
	}

	return solved1 || solved2;
}

bool KinematicsSolver::solveWristSingularity(double t4, double t3, double t2,
		double t1) {
	double c1 = cos(t1);
	double c23 = cos(t2 + t3);
	double c4 = cos(t4);
	//double c5 = 1;

	double s1 = sin(t1);
	double s23 = sin(t2 + t3);
	double s4 = sin(t4);
	//double s5 = 0;

	double r11 = R(1, 1);
	double r21 = R(2, 1);
	double r31 = R(3, 1);

	double s6 = -r11*(c1*c23*s4 - s1*c4) - r21*(s1*c23*s4 + c1*c4) + r31*(s23
			*s4);
	double c6 = r11 * (c1*c23*c4 + s1*s4) + r21 * (s1*c23*c4 - c1*s4) + r31
			* (s23*c4);

	double t6 = atan2(s6, c6);

	if (checkJointValidity(t6, JL6)) {
		addSolutionSet(t1, t2, t3, t4, 0, t6);
		return true;
		//cout << "solution : " << t1 << " " << t2 << " " << t3 << " " << t4 << " " << t5 << " " << t6 << endl;
	}

	return false;
}

bool KinematicsSolver::solveTheta56(double t3, double t2, double t1) {
	Matrix T = getT(DH_a0, DH_d1, DH_alfa0, t1) * getT(DH_a1, DH_d2, DH_alfa1,
			t2) * getT(DH_a2, DH_d3, DH_alfa2, t3);

	//	cout << "===================" << endl;
	//	cout << T.i() << endl;
	//	cout << "===================" << endl;

	T = T.i() * target;

	//	cout << "===================" << endl;
	//	cout << T << endl;
	//	cout << "===================" << endl;

	double* t5 = new double[2];
	t5[0] = -acos(T(2, 3));

	if (DBL_EQ(t5[0], 0)) {
		t5[0] = 0;
		return solveWristSingularity(currentQ4, t3, t2, t1);
	}

	// We proceed if t5 != 0;
	t5[1] = -t5[0];

	//cout << t5[0] << endl;
	bool solved1 = false;
	bool solved2 = false;

	if (checkJointValidity(t5[0], JL5)) {
		//if(checkTheta5(t5[0])){
		solved1 = solveTheta6(t5[0], t3, t2, t1, T);
		//}
	}

	if (!sameAngles(t5)) {
		if (checkJointValidity(t5[1], JL5)) {
			//if (checkTheta5(t5[1])){
			solved2 = solveTheta6(t5[1], t3, t2, t1, T);
			//}
		}
	}

	return solved1 || solved2;
}

bool KinematicsSolver::solveTheta1(double t3, double t2, double F1, double F2) {
	//cout << "T2: " << t2 << endl;
	try {
		double G1 = roundTo( (cos(t2) * F1 - sin(t2) * F2) + DH_a1, 0);
		//cout << endl << "G1 is " << G1;

		if(DBL_EQ(G1, 0)) {
			return solveTheta56(t3, t2, 0);
		}

		Matrix G(2, 2);
		G.Row(1) << G1 << 0;
		G.Row(2) << 0 << G1;

		ColumnVector p(2);
		p(1) = pos(1);
		p(2) = pos(2);

		ColumnVector tmp = G.i() * p;

		double __tmp1 = roundTo(tmp(1), 0);
		double __tmp2 = roundTo(tmp(2), 0);

		double t1 = atan2(__tmp2, __tmp1);

		//cout << "T1: " << t1 << endl;

		bool solved = false;

		if (checkJointValidity(t1, JL1)) {
			if(checkTheta123(t1, t2, t3)) {
				//cout<< "Thetas 123: " << t1 << " " << t2 << " " << t3 << endl;
				solved = solved || solveTheta56(t3, t2, t1);
			}
		}

		return solved;
	} catch (Exception e) {
		cout << e.what() << endl;
	}

	return false;
}

bool KinematicsSolver::solveTheta21(double t3, double F1) {
	//Let out T3

	double _f2 = DH_a3 * sin(t3) + DH_d4 * cos(t3);
	double F2 = roundTo(_f2, 0);

	/*
	 * Transcendal Equation 
	 * g3 = z = s2sin(alpha1)F1 + c2sin(alpha1)F2 
	 * F2 c2 + F1 s2 = z
	 * 
	 * here sin alpha1 = -1
	 */
	double* t2= new double[2];

	if (!solveTranscendal(F2, F1, -pos(3), t2)) {
		return false;
	}

	bool solved = false;
	if (checkJointValidity(t2[0], JL2)) {
		solved = solved || solveTheta1(t3, t2[0], F1, F2);
	}

	if (!sameAngles(t2)) {
		if (checkJointValidity(t2[1], JL2)) {
			solved = solved || solveTheta1(t3, t2[1], F1, F2);
		}
	}
	return solved;
}

bool KinematicsSolver::solveThetas() {
	r = pow(pos(1), 2) + pow(pos(2), 2) + pow(pos(3), 2);

	//cout << "r " << r << endl;

	double a = 2 * (r + pow(DH_a1, 2));
	double b = pow(DH_a1, 4) + 4 * pow(DH_a1, 2) * pow(pos(3), 2) + pow(r, 2)
			- 2 * r * pow(DH_a1, 2);

	//cout << "a, b " << a << ", " << b << endl;

	double* f = new double[2];

	double tmp = pow(a, 2) - 4 * b;
	f[0] = (a + sqrt(tmp)) / 2;
	f[1] = (a - sqrt(tmp)) / 2;

	//cout << "f1, f2 " << f[0] << " " << f[1] << endl;

	a = 2 * DH_a2 * DH_a3;
	b = -2 * DH_a2 * DH_d4;

	double c1 = f[0] - pow(DH_a2, 2) - pow(DH_a3, 2) - pow(DH_d4, 2);
	double c2 = f[1] - pow(DH_a2, 2) - pow(DH_a3, 2) - pow(DH_d4, 2);

	//cout << "c1, c2 " << c1 << ", " << c2 << endl;

	double* t3_1 = new double[2];
	double* t3_2 = new double[2];

	double F1;

	bool solved = false;

	//Here we are solving for theta3
	//Four values of Theta3 are obtained 
	//each will lead to a solution branch

	//solved = solved || solveTheta21(t3_2[0], F1); didn't work
	//Did not enter the method

	if (solveTranscendal(a, b, c1, t3_1)) {
		if (checkJointValidity(t3_1[0], JL3)) {
			F1 = DH_a3 * cos(t3_1[0]) - DH_d4 * sin(t3_1[0]) + DH_a2;
			bool s1 = solveTheta21(t3_1[0], F1);
			solved = solved || s1;
		}

		if (checkJointValidity(t3_1[1], JL3)) {
			F1 = DH_a3 * cos(t3_1[1]) - DH_d4 * sin(t3_1[1]) + DH_a2;
			bool s2 = solveTheta21(t3_1[1], F1);
			solved = solved || s2;
		}
	}

	if (solveTranscendal(a, b, c2, t3_2)) {
		if (checkJointValidity(t3_2[0], JL3)) {
			F1 = DH_a3 * cos(t3_2[0]) - DH_d4 * sin(t3_2[0]) + DH_a2;
			bool s3 = solveTheta21(t3_2[0], F1);
			solved = solved || s3;
		}

		if (checkJointValidity(t3_2[1], JL3)) {
			F1 = DH_a3 * cos(t3_2[1]) - DH_d4 * sin(t3_2[1]) + DH_a2;
			bool s4 = solveTheta21(t3_2[1], F1);
			solved = solved || s4;
		}
	}

	return solved;
}

void KinematicsSolver::showSolutionSet(SolutionSet *set) {
	cout << setprecision(4) << " [ " << set->theta[0] << ", " << set->theta[1]
			<< ", " << set->theta[2] << ", " << set->theta[3] << ", "
			<< set->theta[4] << ", " << set->theta[5] << "]      [ "
			<< toDegrees(set->theta[0]) << ", " << toDegrees(set->theta[1])
			<< ", " << toDegrees(set->theta[2]) << ", "
			<< toDegrees(set->theta[3]) << ", " << toDegrees(set->theta[4])
			<< ", " << toDegrees(set->theta[5]) << "] " << endl;
}

ReturnMatrix KinematicsSolver::getT(double DH_a, double DH_d, double alpha,
		double theta) {

	Matrix T(4, 4);
	T = 0.0;

	T.Row(1) << cos(theta) << -sin(theta) << 0 << DH_a;
	T.Row(2) << sin(theta) * cos(alpha) << cos(alpha) * cos(theta)
			<< -sin(alpha) << -sin(alpha) * DH_d;
	T.Row(3) << sin(theta) * sin(alpha) << sin(alpha) * cos(theta)
			<< cos(alpha) << cos(alpha) * DH_d;
	T(4, 4) = 1.0;

	T.Release();
	return T;
}

ReturnMatrix KinematicsSolver::getTargetT(double px, double py, double pz,
		Matrix R) {

	Matrix result(4, 4);
	result = 0.0;

	result.submatrix(1, 3, 1, 3) = R;

	result(1, 4)= px;
	result(2, 4)= py;
	result(3, 4)= pz;
	result(4, 4)= 1.0;

	result.Release();
	return result;
}

ReturnMatrix KinematicsSolver::getTMatrix(ColumnVector &p) {

	Matrix result(4, 4);
	result = 0.0;

	result.submatrix(1, 3, 1, 3) = getR(p(4), p(5), p(6));

	result(1, 4)= p(1);
	result(2, 4)= p(2);
	result(3, 4)= p(3);
	result(4, 4)= 1.0;

	result.Release();
	return result;
}

ReturnMatrix KinematicsSolver::getKine(ColumnVector &p) {

	p.resize_keep(6);

	Matrix T = getT(DH_a0, DH_d1, DH_alfa0, p(1)) * getT(DH_a1, DH_d2,
			DH_alfa1, p(2)) * getT(DH_a2, DH_d3, DH_alfa2, p(3)) * getT(DH_a3,
			DH_d4, DH_alfa3, p(4)) * getT(DH_a4, DH_d5, DH_alfa4, p(5)) * getT(
			DH_a5, DH_d6, DH_alfa5, p(6));

	T.Release();
	return T;
}

ReturnMatrix KinematicsSolver::getR(double rx, double ry, double rz) {
	rx = normalizeAngle(rx);
	ry = normalizeAngle(ry);
	rz = normalizeAngle(rz);

	double cA = cos(rz);
	double sA = sin(rz);

	double cB = cos(ry);
	double sB = sin(ry);

	double cG = cos(rx);
	double sG = sin(rx);

	Matrix m(3, 3);
	m = 0.0;

	m.row(1) << cA*cB << cA*sB*sG - sA*cG << cA*sB*cG + sA*sG;
	m.row(2) << sA*cB << sA*sB*sG + cA*cG << sA*sB*cG - cA*sG;
	m.row(3) << -sB << cB*sG << cB*cG;

	m.Release();

	return m;

}

ReturnMatrix KinematicsSolver::getP_wr(double px, double py, double pz, Matrix R) {
	ColumnVector p(3);

	p(1) = px - R(1, 3) * DH_d6;
	p(2) = py - R(2, 3) * DH_d6;
	p(3) = pz - R(3, 3) * DH_d6;

	p.Release();
	return p;
}

bool KinematicsSolver::solveTranscendal(double a, double b, double c,
		double* theta) {
	/*
	 * Solution for Transcendal Eqn
	 * aCos(x) + bSin(x) = c;
	 */

	if (DBL_EQ(a, 0) && !DBL_EQ(b, 0) ) {
		// then Sin(x) = c/b;
		theta[0] = asin(c/b);
		theta[1] = M_PI - theta[0];

		return true; // solved!
	}

	if (DBL_EQ(b, 0) && !DBL_EQ(a, 0) ) {
		// then Cos(x) = c/a;
		theta[0] = acos(c/a);
		theta[1] = -theta[0];

		return true; // solved!
	}

	double denom = roundTo(a + c, 0.0);
	double rootTerm = roundTo(pow(b, 2) + pow(a, 2) - pow(c, 2) , 0);

	// TODO: Check for denom == 0 or rootTerm < 0 first???
	// check for a + c = 0;
	if (DBL_EQ(denom, 0) ) {
		theta[0] = M_PI; //u1 and u2 will be +180. -180 resp
		theta[1] = -M_PI;

		return true; // solved! :)
	}

	if (rootTerm < 0) {
		//cout<<"Error!: Solution did not converge" << endl;
		return false; // solution is imaginary and hence does not converge :(
	}

	rootTerm = sqrt(rootTerm);

	//distinct solutions possible

	theta[0] = 2 * atan( (b + rootTerm ) / denom);
	theta[1] = 2 * atan( (b - rootTerm ) / denom);

	return true; // solved!
}

bool KinematicsSolver::solve(ColumnVector targetPose, ColumnVector currentQ) {
	this->currentQ4 = currentQ(4);

	selector = new SolutionSelector(currentQ);
	solutions.clear();

	//TODO: combine following next 3 lines into a single function
	//getTMatrix(ColumnVector jointAngles);

	targetPose(3) = targetPose(3) - DH_d1;
	R = getR(targetPose(4), targetPose(5), targetPose(6));

	target = getTargetT(targetPose(1), targetPose(2), targetPose(3), R);

	pos = getP_wr(targetPose(1), targetPose(2), targetPose(3), R);

	solveThetas();
	return solutions.size() > 0;
}

bool KinematicsSolver::getSolutionList(ColumnVector& cartesian,
		ColumnVector& currentQ, vector<ColumnVector>& solutionList) {

	if (this->solve(cartesian, currentQ)) {
		for (unsigned int i = 0; i < solutions.size(); i++) {
			ColumnVector sol(6);
			SolutionSet* set = solutions.at(i);
			for (int j = 0; j < 6; j++) {
				sol(j + 1) = set->theta[j];
			}
			//			cout << "solution " << (i + 1) << " : " << sol.AsRow();
			solutionList.push_back(sol);
		}
		return true;
	}

	return false;
}

bool KinematicsSolver::getBestSolution(ColumnVector& cartesian,
		ColumnVector& currentQ, ColumnVector& bestSolution) {

	if (this->solve(cartesian, currentQ)) {
		SolutionSet* set = selector->getBest();
		ColumnVector soln(6);
		for (int i = 0; i < 6; i++) {
			soln(i + 1) = set->theta[i];
		}
		bestSolution = soln;
		return true;
	}

	return false;
}

//TODO:
ReturnMatrix KinematicsSolver::getCartesianPos(ColumnVector& currentQ) {
	ColumnVector cartesian(6);

	cartesian = 0;
	Matrix Rot;
	ColumnVector p;

	robot->set_q(currentQ);
	robot->kine(Rot, p);

	cartesian.Rows(1, 3) = p;
	cartesian.Rows(4, 6) = irpy(Rot);//this->getAnglesFromRotationMatrix(Rot);

	//	cout << "bc " << cartesian.AsRow();
	//	
	//	cartesian(4) = normalizeAngle(cartesian(4));
	//	cartesian(5) = normalizeAngle(cartesian(5));
	//	cartesian(6) = normalizeAngle(cartesian(6));
	//	
	//	cout << "ad " << cartesian.AsRow() << endl;

	cartesian.Release();
	return cartesian;
}

