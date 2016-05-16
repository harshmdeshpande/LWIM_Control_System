#ifndef ROBOTMODEMANAGER_H_
#define ROBOTMODEMANAGER_H_

#include <map>
#include "mode/Mode.h"
//#include "robotControl/RobotControlManager.h"

class RobotModeManager {
public:
	RobotModeManager(MotorController* mController);
	virtual ~RobotModeManager();

	string getFeedbackData();
	string handleCommand(string inputCommand);
	bool isOnline();

	bool switchMode(Mode* mode);

	bool doTests();
	void stop();

private:
	RobotControlManager* controlManager;
	MotorController* mController;
	Mode* currentMode;

	Mode* mdOnline;
	Mode* mdCalibration;
	Mode* mdJog;
	Mode* mdExec;

	bool handleModeChange(string mode);

	map<string, Mode*> modeMap;
};

#endif /*ROBOTMODEMANAGER_H_*/
