#include "MotorController.h"

void showMessage(const char* message) {
	MessageBox(0, message, "Controller", MB_OK);
}

void MotorController::showErrorMessage(DWORD errorCode) {
	showMessage(getErrorInfo(errorCode));
}

void MotorController::handleError(DWORD errorCode) {
	showErrorMessage(errorCode);
	//halt all motors
	//clear errors
}

MotorController::MotorController(Robot* robot) {
	this->robot = robot;
}

MotorController::~MotorController() {
}

bool MotorController::initComm() {
	return true;
}

bool MotorController::initNode(int nID) {
	return true;
}

char* MotorController::getErrorInfo(DWORD errorCode) {
	char* error;
	return error;
}

//char* MotorController::getFeedback() {
//	char* feedback;
//	return feedback;
//}

bool MotorController::clearError(int nID) {
	return true;
}

bool MotorController::getHomeStatus(int nID) {
	return true;
}

bool MotorController::setHomeStatus(int nID, bool status) {
	return true;
}

int MotorController::getHome(int nID) {
	return 0;
}

bool MotorController::setHome(int nID) {
	return true;
}

char MotorController::getOpMode(int nID) {
	char opMode = '0';
	return opMode;
}

bool MotorController::getEnableState(int nID) {
	return true;
}

bool MotorController::setEnableState(int nID, bool enable) {
	return true;
}

bool MotorController::setOpMode(int nID, char opMode) {
	return true;
}

bool MotorController::setPPosMode() {
	return true;
}

bool MotorController::setPVeloMode() {
	return true;
}

int MotorController::getDirection(int nID) {
	return 0;
}

bool MotorController::setDirection(int nID, int dir) {
	return true;
}

double MotorController::getPos(int nID) {
	//	cout << robot->get_q().AsRow() << endl;
	return robot->get_q(nID);
}

bool MotorController::setPos(int nID, double pos) {
	robot->set_q(pos, nID);
	//	cout << pos << " " << robot->get_q(nID) << endl;
	return true;
}

int MotorController::getTicks(int nID) {
	return (int)getPos(nID);
}

bool MotorController::setTicks(int nID, double ticks) {
	return true;
}

double MotorController::getOmega(int nID) {
	ColumnVector qp = robot->get_qp();
	return qp(nID);
}

bool MotorController::moveOmega(int nID, double omega) {
	double q = robot->get_q(nID);
	q = q + omega * 0.05;
	robot->set_q(q, nID);
	//	cout << q << ", " << nID << endl;
	return true;
}

long MotorController::getRPM(int nID) {
	return 0;
}

bool MotorController::setRPM(int nID, long rpm) {
	return true;
}

bool MotorController::moveRPM(int nID, long rpm) {
	cout << "MC rpm " << endl;
	return true;
}

void MotorController::setSyncVelo(ColumnVector& pos) {

}

bool MotorController::incrementRPM(int nID, long deltaRPM) {
	long rpm = getRPM(nID);
	rpm = rpm + deltaRPM;
	return moveRPM(nID, rpm);
}

bool MotorController::halt(int nID) {
	return true;
}

bool MotorController::waitUntilPositionReached() {
	return true;
}

void MotorController::setQ(ColumnVector& q) {
	robot->set_q(q);
}

void MotorController::setQp(ColumnVector& qp) {
	robot->set_qp(qp);
}

Robot* MotorController::getRobot() {
	return robot;
}

double MotorController::getMaxVelo(int nID) {
	return 0;
}

void MotorController::doJog(ColumnVector& jogVelo) {

}

