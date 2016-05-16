#ifndef KINEMATICSSOLVER_H_
#define KINEMATICSSOLVER_H_

#include "SolutionSet.h"
#include "selector/SolutionSelector.h"

#include <roboop/robot.h>
#include <vector.h>

class KinematicsSolver {
public:
	KinematicsSolver(Robot* robot); //TODO: Pass the same roboop robot from 
	//motor controller for calculations
	virtual ~KinematicsSolver();

	bool getSolutionList(ColumnVector& cartesian, ColumnVector& currentQ,
			vector<ColumnVector>& solutionList);
	bool getBestSolution(ColumnVector& cartesian, ColumnVector& currentQ,
			ColumnVector& bestSolution);
	ReturnMatrix getCartesianPos(ColumnVector& currentQ);

	//TODO: Temp
	bool solve(ColumnVector targetQ, ColumnVector currentQ);
	ReturnMatrix getTMatrix(ColumnVector &p);
	ReturnMatrix getT(double DH_a, double DH_d, double alpha, double theta);
	ReturnMatrix getKine(ColumnVector &q);

	//vector<ColumnVector> getAll();
	//ReturnMatrix getBest();

private:
	Robot* robot;
	double r;
	double currentQ4;

	ColumnVector pos;
	Matrix R, target;

	SolutionSelector* selector;

	void sort(vector<SolutionSet*>&ss, SolutionSet* set);
	void addSolutionSet(double t1, double t2, double t3, double t4, double t5,
			double t6);
	ReturnMatrix getR(double rx, double ry, double rz);
	ReturnMatrix getP_wr(double px, double py, double pz, Matrix R);
	ReturnMatrix getTargetT(double px, double py, double pz, Matrix R);
	//	ReturnMatrix getT(double DH_a, double DH_d, double alpha, double theta);

	bool checkJointValidity(double joint, const double JL[2]);
	bool checkTheta123(double t1, double t2, double t3);
	bool solveTranscendal(double a, double b, double c, double* theta);

	bool solveThetas();
	bool solveTheta21(double t3, double F1);
	bool solveTheta1(double t3, double t2, double F1, double F2);
	bool solveTheta56(double t3, double t2, double t1);
	bool solveTheta6(double t5, double t3, double t2, double t1, Matrix T);
	bool solveTheta4(double t6, double t5, double t3, double t2, double t1,
			Matrix T);
	bool solveWristSingularity(double t4, double t3, double t2, double t1);

	bool solveTheta1x(double t3, double t2, double F1, double F2);

	std::vector<SolutionSet *> solutions;
	void showSolutionSet(SolutionSet *set);
};

#endif /*KINEMATICSSOLVER_H_*/
