#include "Mode.h"
#include <iostream>

using namespace std;

#define OK "OK"
#define ERR "ERROR"

Mode::Mode(RobotControlManager* controlManager, char* fName) {
	this->controlManager = controlManager;
	this->name = fName;

	//add functions
	functionMap["init"] = &RobotControlManager::init;
	functionMap["deinit"] = &RobotControlManager::deInit;

	//Kinematics
	functionMap["kine"] = &RobotControlManager::getCartesianPos;

	//Motor Controller
	//	functionMap["feedback"] = &RobotControlManager::getFeedback;
	functionMap["clearerr"] = &RobotControlManager::clearError;
	functionMap["gethomestat"] = &RobotControlManager::getJHomeStatus;
	functionMap["gethome"] = &RobotControlManager::getJHome;
	functionMap["getopmode"] = &RobotControlManager::getJOpMode;
	//functionMap["getdir"] = &RobotControlManager::getJDirection;
	functionMap["getjpos"] = &RobotControlManager::getJPos;
	functionMap["getticks"] = &RobotControlManager::getJTicks;
	functionMap["getrpm"] = &RobotControlManager::getJRPM;
	functionMap["halt"] = &RobotControlManager::halt;

	//go home, goto ready
	functionMap["gohome"] = &RobotControlManager::moveToHome;
	functionMap["ready"] = &RobotControlManager::moveToReady;

	functionMap["getlist"] = &RobotControlManager::getTrajectoryFileList;
	functionMap["read"] = &RobotControlManager::readTrajectoryFile;

}

Mode::~Mode() {
}

char* Mode::getName() {
	return name;
}

string Mode::handleCommand(string command, ColumnVector params) {
	FUN funcPtr = this->functionMap[command];
	if (funcPtr == NULL) {
		cout << "command : " << command << " is unknown for " << this->name
				<< " mode" << endl;
		return ERR;
	}
	return (controlManager->*funcPtr)(params);
}

void Mode::init() {
}

void Mode::deinit() {
}

CalibMode::CalibMode(RobotControlManager* controlManager, char* name) :
	Mode(controlManager, name) {

	//add functions
	//functionMap["sethomestat"] = &RobotControlManager::setJHomeStatus;  //should be internal
	functionMap["sethome"] = &RobotControlManager::setJHome;
	functionMap["moverpm"] = &RobotControlManager::moveJRPM;
	functionMap["incrpm"] = &RobotControlManager::incrementJRPM;
	functionMap["movejdir"] = &RobotControlManager::moveJDirection;
	//functionMap["setdir"] = &RobotControlManager::setJDirection;

}

CalibMode::~CalibMode() {
}

void CalibMode::init() {
}

void CalibMode::deinit() {
}

JogMode::JogMode(RobotControlManager* controlManager, char* name) :
	Mode(controlManager, name) {

	functionMap["setjpos"] = &RobotControlManager::setJPos;
	functionMap["setticks"] = &RobotControlManager::setJTicks;
	functionMap["setrpm"] = &RobotControlManager::setJRPM;
	functionMap["moverpm"] = &RobotControlManager::moveJRPM;
	functionMap["setdir"] = &RobotControlManager::setJDirection;

	//Trajectory
	functionMap["mark"] = &RobotControlManager::markPoint;
	functionMap["setjogvelo"] = &RobotControlManager::setJogVelo;

	functionMap["generate"] = &RobotControlManager::generateFromPoints;
	functionMap["generatemarked"]
			= &RobotControlManager::generateFromMarkedPoints;
}

JogMode::~JogMode() {

}

void JogMode::init() {
	controlManager->startJogProcess();
}

void JogMode::deinit() {
	controlManager->stopJogProcess();
}

ExecuteMode::ExecuteMode(RobotControlManager* controlManager, char* name) :
	Mode(controlManager, name) {

	//Kinematics
	functionMap["solve"] = &RobotControlManager::solveIPK;
	functionMap["moveto"] = &RobotControlManager::moveToCartesianPos;
	functionMap["setjpos"] = &RobotControlManager::setJPos;

	//Trajectory
	functionMap["generate"] = &RobotControlManager::generateFromPoints;
	functionMap["generatemarked"]
			= &RobotControlManager::generateFromMarkedPoints;
	functionMap["followcurrent"] = &RobotControlManager::followFromCurrentPos;
	functionMap["follow"] = &RobotControlManager::followPoints;
	functionMap["followFile"] = &RobotControlManager::followFromFiles;

	functionMap["followtmp"] = &RobotControlManager::followMySpline;

	functionMap["approach"] = &RobotControlManager::approach;
	functionMap["moveorientation"] = &RobotControlManager::moveOrientation;
	functionMap["insert"] = &RobotControlManager::insert;
	//TODO: Motor Control

}

ExecuteMode::~ExecuteMode() {
}

void ExecuteMode::init() {
}

void ExecuteMode::deinit() {
}

