#ifndef ROBOTCONTROLMANAGER_H_
#define ROBOTCONTROLMANAGER_H_

//Understands robot commands related to different modules
// - Kinematics
// - Trajectory
// - Curve Generator
// - Motor Controller

//#include "curves/Generator.h"
#include "kinematics/KinematicsSolver.h"
#include "motorControl/MotorController.h"
#include "trajectory/TrajectoryController.h"
#include "../pid/PID.h"

//#include <roboop/robot.h>

class RobotControlManager {
public:
	RobotControlManager(MotorController* mController);
	virtual ~RobotControlManager();

	string getMessage(vector<char> v);
	string getMessage(ColumnVector &params);
	string getMessage(vector<ColumnVector> v);
	string getMessage(vector<int> v);
	string getErrorMessage(string err);

	// Kinematics
	string solveIPK(ColumnVector& cartesian);
	string moveToCartesianPos(ColumnVector& cartesian);
	string getCartesianPos(ColumnVector& noParam);

	// Trajectory
	string markPoint(ColumnVector& noParam);

	string generateFromPoints(ColumnVector& cartesian);
	string generateFromMarkedPoints(ColumnVector& noParam);
	string getTrajectoryFileList(ColumnVector& noParam);
	string readTrajectoryFile(ColumnVector& pathIDs);

	double doPID(int node, double pQC, double vQC);
	//TODO: test with non-roboop splines
	string followMySpline(ColumnVector& cartesian);

	string followFromCurrentPos(ColumnVector& cartesian);
	string followPoints(ColumnVector& cartesian);
	string followFromFiles(ColumnVector& pathIDs);

	string setJogVelo(ColumnVector& jogVelo);
	void startJogProcess();
	void stopJogProcess();

	// TODO: Motor Control
	string init(ColumnVector& noParam);
	string deInit(ColumnVector& noParam);
	string getErrorInfo(ColumnVector& noParam);
	string getFeedback();
	string clearError(ColumnVector& noParam);
	bool isHomed();
	bool isOnline();

	string getJPos(ColumnVector& noParam);
	string setJPos(ColumnVector& jPos);
	string getJTicks(ColumnVector& noParam);
	string setJTicks(ColumnVector& jTicks);
	string getJRPM(ColumnVector& noParam);
	string setJRPM(ColumnVector& jRPM);
	string moveJRPM(ColumnVector& jRPM);
	string incrementJRPM(ColumnVector& jDeltaRPM);
	string getJHomeStatus(ColumnVector& noParam);
	string setJHomeStatus(ColumnVector& jHomeStatus);
	string getJHome(ColumnVector& noParam);
	string setJHome(ColumnVector& noParam);
	string getJOpMode(ColumnVector& noParam);
	string setJOpMode(ColumnVector& jOpMode);
	string getJDirection(ColumnVector& noParam);
	string setJDirection(ColumnVector& jDir);
	string moveJDirection(ColumnVector& jDir);
	string halt(ColumnVector& noParam);
	string moveToReady(ColumnVector& noParam);
	string moveToHome(ColumnVector& noParam);

	//Application-level commands
	string approach(ColumnVector& point);
	string moveOrientation(ColumnVector& newOrien);
	string insert(ColumnVector& dist);

	//temp
	ReturnMatrix getRotM(double rx, double ry, double rz);

protected:

	KinematicsSolver* kineSolver;
	TrajectoryController* trajController;
	MotorController* mController;
	PID* vpid[6], *ppid[6];
	

	vector<ColumnVector> markedPoints;
	ColumnVector ticks, cQ, cQp, homeStat, status, velo;
	ColumnVector jogVelo;

	bool online;

	vector<ColumnVector> getPointsFromParams(ColumnVector& params);
	bool followSpline(Spl_path* spline, double duration);
	bool followSpline(SplineSet* ss);
	void update();
	void doJog();
	SplineSet* createSplines(vector<ColumnVector>& jsPoints);
};

#endif /*ROBOTCONTROLMANAGER_H_*/
