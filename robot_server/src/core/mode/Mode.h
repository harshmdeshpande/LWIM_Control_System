#ifndef MODE_H_
#define MODE_H_

//#include "../motorController/BaseController.h"
//#include <roboop/robot.h>
#include "../robotControl/RobotControlManager.h"
#include <map>


class Mode {
public:
	typedef string (RobotControlManager::*FUN)(ColumnVector&);

	Mode(RobotControlManager* controlManager, char* name);
	virtual ~Mode();

	std::string handleCommand(std::string command, ColumnVector params);
	char* getName();
	
	virtual void init();
	virtual void deinit();

protected:
	RobotControlManager* controlManager;
	char* name;
	map<string, FUN> functionMap;
};

class CalibMode : public Mode {
public:
	CalibMode(RobotControlManager* controlManager, char* name);
	virtual ~CalibMode();

	void init();
	void deinit();
};

class JogMode : public Mode {
public:
	JogMode(RobotControlManager* controlManager, char* name);
	virtual ~JogMode();

	void init();
	void deinit();
};

class ExecuteMode : public Mode {
public:
	ExecuteMode(RobotControlManager* controlManager, char* name);
	virtual ~ExecuteMode();
	
	void init();
	void deinit();
};

#endif /*MODE_H_*/
