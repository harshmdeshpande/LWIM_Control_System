#include "RobotModeManager.h"
#include "../helpers/strop/StringTokenizer.h"

#include <iostream>
#include <sstream>

using namespace std;

#define OK "OK"
#define ERR "ERROR"

const string TAGMODE = "MODE_";
const string TAGEND = "END_";

RobotModeManager::RobotModeManager(MotorController* mController) {
	//	this->controlManager = controlManager;
	this->mController = mController;
	this->controlManager = new RobotControlManager(mController);

	this->mdOnline = new Mode(controlManager, "Online");
	this->mdCalibration = new CalibMode(controlManager, "Calibration" );
	this->mdJog = new JogMode(controlManager, "Jog");
	this->mdExec = new ExecuteMode(controlManager, "Execute");

	modeMap["CALIB"] = mdCalibration;
	modeMap["ONLINE"] = mdOnline;
	modeMap["JOG"] = mdJog;
	modeMap["EXEC"] = mdExec;

	currentMode = mdOnline;

	//check on calibration
	if (!controlManager->isHomed()) {
		cout << "Controller is not calibrated, entering calibration mode"
				<< endl;
		switchMode(mdCalibration);
	}
}

RobotModeManager::~RobotModeManager() {
}

bool RobotModeManager::switchMode(Mode* mode) {
	if (mode == currentMode) {
		cout << "You are already in this mode dude!" << endl;
		return true;
	}

	if (mode == mdOnline || currentMode == mdOnline) {
		if (!controlManager->isHomed() && mode == mdJog) {
			cout << "Robot is not calibrated. Please calibrate first" << endl;
			return false;
		}

		this->currentMode->deinit();
		this->currentMode = mode;
		this->currentMode->init();

		cout << "Switched to mode " << currentMode->getName() << endl;
		return true;
	}

	cout << "You need to be in online mode before switching to any other"
			<< endl;

	return false;
}

ColumnVector getParameters(StringTokenizer tok) {
	int num = tok.countTokens();
	ColumnVector params(num);
	int i = 1;
	while (tok.hasMoreTokens()) {
		params(i) = tok.nextFloatToken();
		i++;
	}
	return params;
}

bool RobotModeManager::handleModeChange(string mode) {
	Mode* target = modeMap[mode];
	cout << mode << endl;
	if (target == NULL) {
		return false;
	}
	return switchMode(target);
}

bool startsWith(string input, string compare) {
	string sub = input.substr(0, compare.size());
	return !sub.compare(compare);
}

string RobotModeManager::getFeedbackData() {
	return controlManager->getFeedback();
}

string RobotModeManager::handleCommand(string inputCommand) {
	inputCommand = inputCommand.substr(0, inputCommand.size()-2);

	StringTokenizer strtok(inputCommand, " ");
	string command = strtok.nextToken(); // command

	//cout << "Command is " << command << ".... ";

	if (startsWith(command, TAGEND)) {
		switchMode(mdOnline);
		return OK;
	}

	if (startsWith(command, TAGMODE)) {
		if (handleModeChange(command.substr(TAGMODE.size(), command.size()))) {
			return OK;
		} else {
			ostringstream oss;
			oss << ERR << " Please exit from " << currentMode->getName();
			cout << "Mode Error" << endl;
			return oss.str();
		}
	}

	ColumnVector params = getParameters(strtok);
	return currentMode->handleCommand(command, params);
}

bool RobotModeManager::isOnline() {
	return controlManager->isOnline();
}

void RobotModeManager::stop() {
	ColumnVector v;
	controlManager->halt(v);
}

bool RobotModeManager::doTests() {
	return true;
}

