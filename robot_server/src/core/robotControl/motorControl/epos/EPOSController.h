#ifndef EPOSCONTROLLER_H_
#define EPOSCONTROLLER_H_

#include "../MotorController.h"
#include <windows.h>

class EPOSController : public MotorController {
public:
	EPOSController(Robot* robot);
	virtual ~EPOSController();

	bool initComm();
	bool initNode(int nID);
	char* getErrorInfo(DWORD errorCode);
	//	char* getFeedback();
	bool clearError(int nID);
	//	void handleError(DWORD errorCode);
	//	void showErrorMessage(DWORD errorCode);

	bool getHomeStatus(int nID);
	bool setHomeStatus(int nID, bool status);
	int getHome(int nID);
	bool setHome(int nID);
	char getOpMode(int nID);
	bool setOpMode(int nID, char opMode);
	bool setPPosMode();
	bool setPVeloMode();
	bool getEnableState(int nID);
	bool setEnableState(int nID, bool enable);

	int getDirection(int nID);
	bool setDirection(int nID, int dir);
	double getPos(int nID);
	bool setPos(int nID, double pos);
	int getTicks(int nID);
	bool setTicks(int nID, long ticks);
	double getOmega(int nID);
	bool moveOmega(int nID, double omega);
	long getRPM(int nID);
	bool setRPM(int nID, long rpm);
	bool moveRPM(int nID, long rpm);
	bool incrementRPM(int nID, long deltaRPM);
	bool halt(int nID);
	bool waitUntilPositionReached();
	void setSyncVelo(ColumnVector& delta);

	void doJog(ColumnVector& jogVelo);

	double getMaxVelo(int nID);

private:
	Robot* robot;
	bool online;

	bool isPositionReached(int nID);
};

#endif /*EPOSCONTROLLER_H_*/
