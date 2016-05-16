#ifndef MOTORCONTROLLER_H_
#define MOTORCONTROLLER_H_

#include <roboop/robot.h>
#include <windows.h>

class MotorController {
public:
	MotorController(Robot* robot);
	virtual ~MotorController();

	virtual bool initComm();
	virtual bool initNode(int nID);
	virtual char* getErrorInfo(DWORD errorCode);
//	virtual char* getFeedback();
	virtual bool clearError(int nID);
	
	void handleError(DWORD errorCode);
	void showErrorMessage(DWORD errorCode);

	virtual bool getHomeStatus(int nID);
	virtual bool setHomeStatus(int nID, bool status);
	virtual int getHome(int nID);
	virtual bool setHome(int nID);
	virtual char getOpMode(int nID);
	virtual bool setOpMode(int nID, char opMode);
	virtual bool getEnableState(int nID);
	virtual bool setEnableState(int nID, bool enable);

	virtual int getDirection(int nID);
	virtual bool setDirection(int nID, int dir);
	virtual double getPos(int nID);
	virtual bool setPos(int nID, double pos);
	virtual double getOmega(int nID);
	virtual bool moveOmega(int nID, double omega);
	virtual int getTicks(int nID);
	virtual bool setTicks(int nID, double ticks);
	virtual long getRPM(int nID);
	virtual bool setRPM(int nID, long rpm);
	virtual void setSyncVelo(ColumnVector& pos);
	virtual bool moveRPM(int nID, long rpm);
	bool incrementRPM(int nID, long deltRPM);
	virtual bool halt(int nID);
	virtual bool setPPosMode();
	virtual bool setPVeloMode();
	virtual bool waitUntilPositionReached();
	virtual double getMaxVelo(int nID);
	
	//TODO: temp
	virtual void doJog(ColumnVector& jogVelo);
	
	void setQ(ColumnVector& q);
	void setQp(ColumnVector& qp);
	
	Robot* getRobot();
	
private:
	Robot* robot;

};

#endif /*MOTORCONTROLLER_H_*/
