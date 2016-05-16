#include "RobotControlManager.h"
#include "../../helpers/timer/Timer.h"

#include <boost/thread/thread.hpp>
#include <sstream>
#include <string.h>

string OK = "OK";
string Error = "Error : ";

const int NODES = 6;
const double NODE5_OFFSET = -90.0;

const char MODE_PVELO = 0x03;
const char MODE_PPOS = 0x01;
const char MODE_HOME = 0x06;

const double DEF_DURATION = 5;

bool haltPath = false;
bool robotReady = true;
bool jogFlag = false;

const double Kp = 0.0;
const double Ki = 0.0;
const double Kd = 0.0;

inline double round(double x) {
	return ((int) (x * 10000.0)) / 10000.0;
}

RobotControlManager::RobotControlManager(MotorController* mController) {

	this->mController = mController;
	Robot* robot = this->mController->getRobot();
	this->kineSolver = new KinematicsSolver(robot);
	this->trajController = new TrajectoryController(kineSolver);

	this->status.ReSize(NODES);
	this->ticks.ReSize(NODES);
	this->homeStat.ReSize(NODES);
	this->cQ.ReSize(NODES);
	this->cQp.ReSize(NODES);

	this->cQ = 0;
	this->cQp = 0;

	this->velo.ReSize(NODES);
	this->velo = 1000;

	this->jogVelo.ReSize(6);
	this->jogVelo = 0.0;

	for (int i = 0; i < 6; i++) {
		vpid[i] = new PID(1.3, 0.0002, 0.25);
		ppid[i] = new PID(0.5, 0.000, 0.05);
	}

	ColumnVector v;
	this->init(v);

}

RobotControlManager::~RobotControlManager() {
	delete this->mController;
	delete this->kineSolver;
	delete this->trajController;
}

//HELPERS5

string RobotControlManager::getErrorMessage(string err) {
	string msg = Error;
	msg.append(err);
	return msg;
}

string RobotControlManager::getMessage(vector<char> v) {
	string msg = "";
	ostringstream oss;
	for (unsigned i = 0; i < v.size(); i++) {
		oss << v.at(i) << "#";
	}
	msg = oss.str();
	return msg;
}

string RobotControlManager::getMessage(vector<ColumnVector> v) {
	string msg = "";
	for (unsigned i = 0; i < v.size(); i++) {
		string s = getMessage(v.at(i));
		msg.append(s);
		msg.append("#");
	}
	return msg;
}

string RobotControlManager::getMessage(ColumnVector &params) {
	string message = "";
	for (int i = 1; i <= params.size(); i++) {
		stringstream ostr;
		ostr << params(i);
		message.append(ostr.str());
		message.append(" ");
	}
	return message;
}

string RobotControlManager::getMessage(vector<int> params) {
	string message = "";
	for (unsigned int i = 0; i < params.size(); i++) {
		stringstream ostr;
		ostr << params.at(i);
		message.append(ostr.str());
		message.append(" ");
	}
	return message;
}

vector<ColumnVector> RobotControlManager::getPointsFromParams(
		ColumnVector& params) {

	int size = params.size();
	int ptNum = size / 6; //Each cartesian point consist of 6 elements

	vector<ColumnVector> ptList;
	for (int i = 0; i < ptNum; i++) {
		int index = i * 6;
		ColumnVector pt = params.Rows(index + 1, index + 6);
		ptList.push_back(pt);
	}
	return ptList;
}

// KINEMATICS

/*
 *  Returns all the available solutions for the target position
 */
string RobotControlManager::solveIPK(ColumnVector& cartesian) {
	vector<ColumnVector> solutionList;
	ColumnVector currentQ(6);
	cartesian.resize_keep(6);

	if (kineSolver->getSolutionList(cartesian, this->cQ, solutionList)) {
		return getMessage(solutionList);
	}
	return getErrorMessage("Desired point not reachable");
}

/*
 *  Sets the robot pose to target position selecting the best solution
 */
string RobotControlManager::moveToCartesianPos(ColumnVector& cartesian) {
	ColumnVector bestSolution(6), currentQ(6);

	if (kineSolver->getBestSolution(cartesian, this->cQ, bestSolution)) {
		//set the best solution on motor controller
		//		cout << "best " << bestSolution << endl;
		setJPos(bestSolution);
		//		cout << getJPos(bestSolution) << endl;
		return OK;
	}
	return getErrorMessage("Desired position cannot be reached");
}

/*
 *  Returns the current cartesian position
 */
string RobotControlManager::getCartesianPos(ColumnVector& noParam) {
	ColumnVector cartesian(6);

	cartesian = kineSolver->getCartesianPos(this->cQ);

	return getMessage(cartesian);
}

// TRAJECTORY

/*
 *  Mark the current cartesian position as via point
 */
string RobotControlManager::markPoint(ColumnVector& noParam) {
	ColumnVector currentCartesian = kineSolver->getCartesianPos(this->cQ);
	markedPoints.push_back(currentCartesian);
	return getMessage(currentCartesian);
}

/*
 *  Generates a trajectory file (independent of time) with given set of
 *  cartesian points. Spline(time dependent) is formed only while following
 *  the curve and not while generating it.
 */
string RobotControlManager::generateFromPoints(ColumnVector& cartesian) {

	//create vector of points and pass to trajectory controller
	vector<ColumnVector> ptList = getPointsFromParams(cartesian);

	if (trajController->generateJSPath(ptList)) {
		return OK;
	}

	return getErrorMessage("Desired path could not be generated");
}

/*
 *  Generates a trajectory file (independent of time) from previously 
 *  marked via points. Spline (time dependent) is formed only while following
 *  the curve and not while generating it.
 */
string RobotControlManager::generateFromMarkedPoints(ColumnVector& noParam) {

	//create vector of marked points and pass to same function as above.
	if (markedPoints.size() < 2) {
		return getErrorMessage("Insufficient number of points marked.");
	} else if (trajController->generateJSPath(markedPoints)) {
		return OK;
	}
	return getErrorMessage("Desired path could not be generated");
}

string RobotControlManager::getTrajectoryFileList(ColumnVector& noParam) {
	vector<int> fileList = trajController->getFileList();
	return getMessage(fileList);
}

string RobotControlManager::readTrajectoryFile(ColumnVector& pathIDs) {
	for (int i = 1; i <= pathIDs.size(); i++) {
		trajController->readFile((int)pathIDs(i));
	}
	return OK;
}

/*
 * 
 */
bool RobotControlManager::followSpline(Spl_path* spline, double duration) {
	haltPath = false;

	cout << "following..." << endl;

	if (spline == NULL) {
		cout << "cant follow a NULL spline!" << endl;
		robotReady = true;
		return false;
	}

	ColumnVector q(6), qdot(6);

	q = 0.0;
	qdot = 0.0;

	//TODO: where to put?
	spline->interpolating(0.0, q);
	this->setJPos(q);
	//setJointPositions(q);

	//cout << "path duration: " << duration << endl; 

	Timer* timer = new Timer();
	timer->startTimer();
	double time = timer->getElapsedTime();

	while (time < duration) {
		//cant get time from the trajectory file

		//TODO: If can't be interpolated then NOT_IN_RANGE returned
		short spq = spline->interpolating(time, q);
		short spqd = spline->first_derivative(time, qdot);

		//cout << spq << " " << spqd << endl;

		if (spq == 0 && spqd == 0) {
			//cout << "errQ : ";
			for (int i = 1; i <= NODES; i++) {
				double errQ = this->cQ(i) - q(i);
				//cout << errQ << " "; 

				double qout = qdot(i);// + 0.13 * -errQ; //pid->step(-errQd, 0);
				//moveNodeWithVelocity(n, (long) omegaToRPM(n, qout));
				mController->moveOmega(i, qout);
			}
		} else {
			cerr << "Problem in spline data." << endl;
			ColumnVector v;
			halt(v);
			haltPath = true;
			return false;
		}
		//cout << endl;
		time = timer->getElapsedTime();
	}
	ColumnVector v;
	halt(v);

	robotReady = true;
	delete timer;
	return true;
}

double RobotControlManager::doPID(int node, double pQC, double vQC) {
	double pEPOS = mController->getPos(node);
	double pErr = pQC - pEPOS;

	double vEPOS = mController->getOmega(node);
	double vErr = vQC - vEPOS;

	double vSet = vEPOS + vpid[node-1]->compensation(vErr, vEPOS)
			+ ppid[node-1]->compensation(pErr, pEPOS);

	//	if (node == 1) {
	//		cout << pErr << endl;
	//		cout << vErr << endl;
	//	}
	return vSet;
}

bool RobotControlManager::followSpline(SplineSet* ss) {
	haltPath = false;

	ColumnVector q(6), qdot(6), qLast(6);

	q = 0.0;
	qdot = 0.0;

	//TODO: where to put?
	for (int i = 0; i < 5; i++) {
		double* x = ss->get(i)->interpolate(0);
		q(i + 1) = x[0];
	}

	this->setJPos(q);
	double duration = ss->getDuration();

	Timer* timer = new Timer();
	double time = 0;
	ColumnVector prevQp(6);
	ColumnVector prevQ(6);

	timer->startTimer();
	while (!haltPath && round(time) < duration) {

		time = timer->getElapsedTime();
		//TODO: change back to 6;
		for (int i = 1; i <= 5; i++) {
			double* x = ss->get(i - 1)->interpolate(round(time));

			if (x != NULL) {
				double qout = doPID(i, x[0], x[1]);//x[1] + Ki * errQ + Kp * errQp;// + Kd * errQpp;

				mController->moveOmega(i, qout);
			} else {
				ColumnVector v;
				halt(v);
				cerr << "Problem in spline data." << endl;
				haltPath = true;
				return false;
			}
		}
	}
	ColumnVector v;
	halt(v);

	robotReady = true;
	delete timer;
	return true;
}

SplineSet* RobotControlManager::createSplines(vector<ColumnVector>& jsPoints) {
	ColumnVector maxVelos(6);
	SplineSet* ss = trajController->createSplines(jsPoints);
	for (unsigned i = 0; i < ss->size(); i++) {
		double maxVelo = mController->getMaxVelo(i + 1);
		maxVelos(i + 1) = maxVelo;
	}
	ss->calcOptimumTime(maxVelos);
	return ss;
}

string RobotControlManager::followMySpline(ColumnVector& cartesian) {
	ColumnVector currentQ(6), bestSol(6);
	vector<ColumnVector> jsList;
	vector<ColumnVector> cartesianList = this->getPointsFromParams(cartesian);

	double percentage = 50;

	int rem = cartesian.size() % 6;
	if (rem != 0) {
		int i = cartesianList.size() * 6;
		if (cartesian(i + 1) < 100)
			percentage = cartesian(i + 1);
	}
	percentage = 100.0 / percentage;

	currentQ = this->cQ;
	for (unsigned i = 0; i < cartesianList.size(); i++) {
		ColumnVector c = cartesianList.at(i);
		kineSolver->getBestSolution(c, currentQ, bestSol);
		jsList.push_back(bestSol);
		currentQ = bestSol;
	}

	SplineSet* ss = this->createSplines(jsList);

	if (ss->scaleSplineSet(percentage)) {
		if (this->followSpline(ss)) {
			return "Path followed successfully";
		} else {
			return getErrorMessage("Path could not be followed");
		}
	}

	return getErrorMessage("Spline could not be created");
}

/*
 *  Follows the given set of cartesian via points.
 *  The curve generator selected depends on no. of via points.
 *  The last parameter is duration of curve. If not use default time.
 */
string RobotControlManager::followPoints(ColumnVector& cartesian) {

	// - get joint space points
	// - create spline
	// - follow spline

	vector<ColumnVector> cartesianList = this->getPointsFromParams(cartesian);
	double percentage = 50;

	int rem = cartesian.size() % 6;
	if (rem != 0) {
		int i = cartesianList.size() * 6;
		if (cartesian(i + 1) < 100)
			percentage = cartesian(i + 1);
	}
	percentage = 100.0 / percentage;
	vector<ColumnVector> jsList = trajController->getJSPoints(cartesianList);

	SplineSet* ss = this->createSplines(jsList);
	if (ss->scaleSplineSet(percentage)) {
		if (this->followSpline(ss)) {
			return "Path followed successfully";
		} else {
			return getErrorMessage("Path could not be followed");
		}
	}

	return getErrorMessage("Spline could not be created");
}

/*
 *  Follows the given set of cartesian via points with starting point as
 *  current location. The curve generator selected depends on no. of
 *  via points.
 *  The last parameter is duration of curve. If not use default time.
 */
string RobotControlManager::followFromCurrentPos(ColumnVector& cartesian) {
	//	cout << "following " << endl;
	ColumnVector currentQ(6), bestSol(6);
	//	vector<ColumnVector> jsList;

	vector<ColumnVector> cartesianList = this->getPointsFromParams(cartesian);
	double percentage = 30;

	int rem = cartesian.size() % 6;
	if (rem != 0) {
		int i = cartesianList.size() * 6;
		if (cartesian(i + 1) < 100)
			percentage = cartesian(i + 1);
	}
	percentage = 100.0 / percentage;

	//TODO: get current Q
	currentQ = this->cQ;
	ColumnVector currentCartesian = kineSolver->getCartesianPos(currentQ);

	cartesianList.insert(cartesianList.begin(), currentCartesian);

	vector<ColumnVector> jsList = trajController->getJSPoints(cartesianList);
	SplineSet* ss = this->createSplines(jsList);

	if (ss->scaleSplineSet(percentage)) {
		if (this->followSpline(ss)) {
			return "Path followed successfully";
		} else {
			return getErrorMessage("Path could not be followed");
		}
	}

	return getErrorMessage("Spline could not be created");
}

/*
 *  Follows the given combination of path IDs. The trajectory file
 * (independent of time) has to be generated before calling the function.
 *  The last parameter is duration of curve. If not use default time.
 *  In case of multiple files, combine all paths to form a single spline.
 */
string RobotControlManager::followFromFiles(ColumnVector& pathIDs) {

	//TODO: params can be in the form of - ID1 T1 ID2 T2 ....  (ID = path id, T = duration of that segment)

	// - Read joint space path from all files.
	// - Combine all joint space points 
	// - create and follow spline

	vector<ColumnVector> pts;
	vector<ColumnVector>::iterator it;

	int size = pathIDs.size();
	for (int i = 1; i <= size; i++) {
		ColumnVector jsPts = trajController->readFile((int)pathIDs(i));
		if (jsPts.size() != 0) {
			vector<ColumnVector> vec = getPointsFromParams(jsPts);
			it = pts.end();
			pts.insert(it, vec.begin(), vec.end());
		} else {
			return getErrorMessage("Invalid trajectory file.");
		}
	}

	//
	//	for (unsigned int i = 0; i < pts.size(); i++) {
	//		cout << i << " " << pts.at(i).as_row() << endl;
	//	}

	Spl_path* spline = trajController->createSpline(pts, DEF_DURATION * size);

	if (spline != NULL) {
		if (this->followSpline(spline, DEF_DURATION * size)) {
			return "Path followed successfully";
		} else {
			return getErrorMessage("Path could not be followed");
		}
		return OK;
	}

	return getErrorMessage("Spline could not be created");
}

string RobotControlManager::setJogVelo(ColumnVector& velo) {
	velo.resize_keep(6);
	this->jogVelo = velo;
	velo.Release();
	return OK;
}

void RobotControlManager::doJog() {
	while (jogFlag) {
		mController->doJog(this->jogVelo);
	}
	cout << "Jog Ended" << endl;
}

void RobotControlManager::startJogProcess() {
	jogFlag = true;
	mController->setPVeloMode();

	boost::thread jog(boost::bind(&RobotControlManager::doJog, this));
}

void RobotControlManager::stopJogProcess() {
	jogFlag = false;
	jogVelo = 0.0;
}

//MOTOR CONTROL

/*
 *  initializes the motor controller
 */
string RobotControlManager::init(ColumnVector& noParam) {
	if (mController->initComm()) {
		for (int i = 1; i <= 5; i++) {
			if (!mController->initNode(i)) {
				ostringstream oss;
				oss << "Node " << i << " could not be initialized.";
				return getErrorMessage(oss.str());
			}
		}
		mController->setPVeloMode();
		boost::thread thr1(boost::bind(&RobotControlManager::update, this));
		online = true;
		return OK;
	}
	return getErrorMessage("Motor controller is offline.");
}

string RobotControlManager::deInit(ColumnVector& noParam) {
	for (int i = 1; i <= 5; i++) {
		if (!mController->setEnableState(i, false)) {
			ostringstream oss;
			oss << "Node " << i << " could not be initialized.";
			return getErrorMessage(oss.str());
		}
	}
	return OK;
}

void RobotControlManager::update() {
	while (true) {
		for (int i = 1; i <= 5; i++) {
			//			this->status(i) = mController->getEnableState(i);
			//			this->ticks(i) = mController->getTicks(i);
			this->cQ(i) = mController->getPos(i);
			this->cQp(i) = mController->getOmega(i);
			//			this->homeStat(i) = mController->getHomeStatus(i);

		}
		//		cout << "cq " << cQ.as_row() << endl;

		//checkSingularity();
		//robot->set_q(localQ);
		//robot->set_qp(localQp);
		//		mController->setQ(cQ);
		//		mController->setQp(cQp);

	}
}

/*
 *  gets information about Error in motor controller
 */
string RobotControlManager::getErrorInfo(ColumnVector& noParam) {
	DWORD errorCode;
	mController->getErrorInfo(errorCode);
	return "";
}

/*
 *  gets data about motor controller
 */
string RobotControlManager::getFeedback() {
	//	mController->getFeedback();
	ostringstream oss;

	oss << "cartesianString";

	for (int i = 1; i <= 5; i++) {
		oss << "#" << status(i) << " " << ticks(i) << " " << cQ(i) << " "
				<< cQp(i) << " " << homeStat(i);
	}
	string s = oss.str();
	return s;
}

/*
 *  clears the Error from motor controller
 */
string RobotControlManager::clearError(ColumnVector& noParam) {
	bool clear = true;
	for (int i = 1; i <= 5; i++) {
		clear = clear && mController->clearError(i);
	}
	if (clear) {
		return OK;
	}
	return getErrorMessage("Errors could not be cleared.");
}

bool RobotControlManager::isHomed() {
	ColumnVector homeStatus(NODES);
	for (int i = 1; i <= 5; i++) {
		if (!mController->getHomeStatus(i)) {
			return false;
		}
	}
	return true;
}

bool RobotControlManager::isOnline() {
	return online;
}

/*
 *  gets the home status of robot
 */
string RobotControlManager::getJHomeStatus(ColumnVector& noParam) {
	ColumnVector homeStatus(NODES);
	for (int i = 1; i <= 5; i++) {
		if (mController->getHomeStatus(i)) {
			homeStatus(i) = 1;
		} else {
			homeStatus(i) = 0;
		}
	}
	return getMessage(homeStatus);
}

/*
 * 	sets the home status
 */
string RobotControlManager::setJHomeStatus(ColumnVector& jHomeStaus) {
	if (jHomeStaus.size() == 2) {
		int id = (int)jHomeStaus(1);
		if (jHomeStaus(2) > 0) {
			mController->setHomeStatus(id, true);
		} else {
			mController->setHomeStatus(id, false);
		}
	} else {
		jHomeStaus.resize_keep(NODES);
		for (int i = 1; i <= 5; i++) {
			if (jHomeStaus(i) > 0) {
				mController->setHomeStatus(i, true);
			} else {
				mController->setHomeStatus(i, false);
			}
		}
	}
	return OK;
}

/*
 *  gets the home position 
 */
string RobotControlManager::getJHome(ColumnVector& noParam) {
	ColumnVector home(NODES);
	for (int i = 1; i <= 5; i++) {
		home(i) = mController->getHome(i);
	}
	return getMessage(home);
}

/*
 *  sets the current joint position as home
 */
string RobotControlManager::setJHome(ColumnVector& noParam) {
	if (noParam.size() == 1) {
		int id = (int)noParam(1);
		mController->setHome(id);
	} else {
		for (int i = 1; i <= 5; i++) {
			mController->setHome(i);
		}
	}
	return OK;
}

/*
 *  gets operating mode - position profile or velocity profile
 */
string RobotControlManager::getJOpMode(ColumnVector& noParam) {
	vector<char> opModes;
	for (int i = 1; i <= 5; i++) {
		char m = mController->getOpMode(i);
		opModes.push_back(m);
	}
	return getMessage(opModes);
}

/*
 *  sets the operating mode - position profile or velocity profile
 */
//TODO: how to pass op mode?
string RobotControlManager::setJOpMode(ColumnVector& jOpMode) {
	char opMode = 0x03; //temp setting to profile velo
	jOpMode.resize_keep(NODES);
	for (int i = 1; i <= 5; i++) {
		mController->setOpMode(i, opMode);
	}
	return OK;
}

/*
 *  gets the direction of motion of motor
 */
string RobotControlManager::getJDirection(ColumnVector& noParam) {
	ColumnVector dir(NODES);
	for (int i = 1; i <= 5; i++) {
		dir(i) = mController->getDirection(i);
	}
	return getMessage(dir);
}

/*
 *  sets the direction of motion of motor
 */
string RobotControlManager::setJDirection(ColumnVector& jDir) {
	//	if (jDir.size() == 2) {
	//		int id = (int)jDir(1);
	//		int dir = (int)(jDir(2) / fabs(jDir(2)));
	//		//		mController->setDirection(id, dir);
	//		direction(id) = dir;
	//	} else {
	//		jDir.resize_keep(NODES);
	//		//		for (int i = 1; i <= NODES; i++) {
	//		//			int dir = (int)(jDir(i) / fabs(jDir(i)));
	//		////			mController->setDirection(i, dir);
	//		//		}
	//		direction = jDir;
	//	}
	return OK;
}

/*
 *  gets the joint angle (degrees or radians??)
 */
string RobotControlManager::getJPos(ColumnVector& currentQ) {
	ColumnVector jPos(NODES);
	for (int i = 1; i <= 5; i++) {
		jPos(i) = mController->getPos(i);
	}
	currentQ = jPos;
	return getMessage(jPos);
}

/*
 *  sets the joint angle (degrees or radians??)
 */
string RobotControlManager::setJPos(ColumnVector& jPos) {
	mController->setPPosMode();
	if (jPos.size() == 2) {
		int id = (int)jPos(1);
		mController->setPos(id, jPos(2));
	} else {
		jPos.resize_keep(6);
		mController->setSyncVelo(jPos);
		for (int i = 1; i <= 5; i++) {
			mController->setPos(i, jPos(i));
		}
	}
	//wait till complete
	mController->waitUntilPositionReached();
	mController->setPVeloMode();
	return OK;
}

/*
 *  gets the encoder count
 */
string RobotControlManager::getJTicks(ColumnVector& noParam) {
	ColumnVector ticks(NODES);
	for (int i = 1; i <= 5; i++) {
		ticks(i) = mController->getTicks(i);
	}
	return getMessage(ticks);
}

/*
 *  sets the encoder count
 */
string RobotControlManager::setJTicks(ColumnVector& jTicks) {
	mController->setPPosMode();
	if (jTicks.size() == 2) {
		int id = (int)jTicks(1);
		mController->setTicks(id, jTicks(2));
	}
	for (int i = 1; i <= 5; i++) {
		mController->setTicks(i, (long)jTicks(i));
	}
	//wait until reached (check from Labview code)
	mController->waitUntilPositionReached();
	mController->setPVeloMode();
	return OK;
}

/*
 *  gets joint rpm
 */
string RobotControlManager::getJRPM(ColumnVector& noParam) {
	ColumnVector jRPM(NODES);
	for (int i = 1; i <= 5; i++) {
		jRPM(i) = mController->getRPM(i);
	}
	return getMessage(jRPM);
}

/*
 *  sets joint rpm
 */
string RobotControlManager::setJRPM(ColumnVector& jRPM) {
	if (jRPM.size() == 2) {
		int id = (int)jRPM(1);
		mController->setRPM(id, (long)jRPM(2));
	} else {
		jRPM.resize_keep(NODES);
		for (int i = 1; i <= 5; i++) {
			mController->setRPM(i, (long)jRPM(i));
		}
	}
	return OK;
}

string RobotControlManager::moveJRPM(ColumnVector& jRPM) {
	if (jRPM.size() == 2) {
		int id = (int)jRPM(1);
		mController->moveRPM(id, (long)jRPM(2));
	} else {
		jRPM.resize_keep(NODES);
		for (int i = 1; i <= 5; i++) {
			mController->moveRPM(i, (long)jRPM(i));
		}
	}
	return OK;
}

string RobotControlManager::incrementJRPM(ColumnVector& jDeltaRPM) {
	if (jDeltaRPM.size() == 2) {
		int id = (int)jDeltaRPM(1);
		//		mController->incrementRPM(id, (long)jDeltaRPM(2));
		velo(id) = velo(id) + jDeltaRPM(2);
	} else {
		jDeltaRPM.resize_keep(NODES);
		for (int i = 1; i <= 5; i++) {
			//			mController->incrementRPM(i, (long)jDeltaRPM(i));
			velo(i) = velo(i) + jDeltaRPM(i);
		}
	}
	return OK;
}

string RobotControlManager::moveJDirection(ColumnVector& jDirection) {
	if (jDirection.size() == 2) {
		int id = (int)jDirection(1);
		double v = jDirection(2) * velo(id);
		cout << "dir "<< v << endl;
		mController->moveRPM(id, (long)v);
	} else {
		jDirection.resize_keep(NODES);
		for (int i = 1; i <= 5; i++) {
			double v = jDirection(i) * velo(i);
			mController->moveRPM(i, (long)v);
		}
	}
	return OK;
}

string RobotControlManager::moveToReady(ColumnVector& noParam) {
	ColumnVector ready(6);
	ready << 0 << -2.2 << 0.8 << 0 << 1.42 << 0;
	return setJPos(ready);
}

string RobotControlManager::moveToHome(ColumnVector& noParam) {
	ColumnVector h(6);
	h = 0;
	h(5) = deg2rad(NODE5_OFFSET);
	return setJPos(h);
}

string RobotControlManager::halt(ColumnVector& noParam) {
	for (int i = 1; i <= 5; i++) {
		mController->halt(i);
	}
	this->stopJogProcess();

	return OK;
}

//Application level commands
string RobotControlManager::approach(ColumnVector& point) {
	if (point.size() < 7) {
		return getErrorMessage("Insufficient data.");
	}

	double height = point(7);
	point.resize_keep(6);

	ColumnVector appPoint = point;
	appPoint(3) = point(3) + height;

	ColumnVector vec(12);
	vec.Rows(1, 6) = appPoint;
	vec.Rows(7, 12) = point;

	//	cout << appPoint.AsRow() << endl;
	//	cout << point.AsRow() << endl;
	string msg = this->moveToCartesianPos(appPoint);
	size_t s = msg.find(OK);
	if (s != string::npos) {
		return this->followMySpline(vec);
	}
	return msg;
}

string RobotControlManager::moveOrientation(ColumnVector& newOrien) {
	if (newOrien.size() < 3) {
		return getErrorMessage("Insufficient data.");
	}

	ColumnVector currentQ = this->cQ;
	ColumnVector currentCartesian = kineSolver->getCartesianPos(currentQ);

	ColumnVector newCartesian = currentCartesian;
	newCartesian.Rows(4, 6) = newOrien;

	//	ColumnVector viaPoint = (newCartesian + currentCartesian) / 2;
	//
	//	ColumnVector circle(18);
	//	circle.Rows(1, 6) = currentCartesian;
	//	circle.Rows(7, 12) = viaPoint;
	//	circle.Rows(13, 18) = newCartesian;

	//	cout << "current " << currentCartesian.AsRow();
	//	cout << "new " << newCartesian.as_row();
	//	cout << "via " << viaPoint.AsRow();
	//	cout << "circle " << circle;

	//	return followPoints(circle);
	//	return followMySpline(circle);
	return moveToCartesianPos(newCartesian);
}

string RobotControlManager::insert(ColumnVector& dist) {
	if (dist.size() == 0) {
		return getErrorMessage("Insufficient data.");
	}
	ColumnVector currentQ = this->cQ;
	ColumnVector currentCartesian = kineSolver->getCartesianPos(currentQ);

	double d = -dist(1); //-ve dist is in the direction of EE

	ColumnVector dVec;// = currentCartesian;
	ColumnVector newV = currentCartesian;
	newV.resize_keep(12);
	newV.Rows(7, 12) = currentCartesian;

	ColumnVector up(3);
	up << 0 << 0 << 1.0;

	Matrix _m = getRotM(newV(4), newV(5), newV(6));
	dVec = _m * up * d;
	newV(7) = newV(1) - dVec(1);
	newV(8) = newV(2) - dVec(2);
	newV(9) = newV(3) - dVec(3);

	//	cout << currentCartesian.AsRow() << endl;
	//	cout << newV.AsRow() << endl;
	return followMySpline(newV);//followFromCurrentPos(newV);
}

ReturnMatrix RobotControlManager::getRotM(double rx, double ry, double rz) {
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
