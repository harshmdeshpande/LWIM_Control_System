#include "EPOSController.h"
#include "Definitions.h"

using namespace std;

HANDLE keyHandle; // handler to store the existing communication with EPOS
DWORD errorCode; // variable to store error codes during operation

//Operating modes
const char MODE_VELO = 0xFE;
const char MODE_PVELO = 0x03;
const char MODE_PPOS = 0x01;
const char MODE_HOME = 0x06;

const double _epsilon = 4.37114e-05;

WORD CALIB_INDEX1 = 0x200C;
BYTE CALIB_INDEX2 = 0x01;

const double NODE5_OFFSET = -90.0;

bool isSingular = false;

/*===================================================
 * IMPORTANT MOTOR DATA, PLEASE DONT CHANGE
 * ================================================== */
const double kRPM2OMEGA = 2 * M_PI / 60.0;
const double kOMEGA2RPM = 1.0 / kRPM2OMEGA;

const long MOTOR_L_MAXV = 5000;//5000; //3500
const long MOTOR_M_MAXV = 7000;//7000; //6000
const long MOTOR_S_MAXV = 8000;//10000; //10000

const long MOTOR_MAX_VELO[6] = { MOTOR_L_MAXV, MOTOR_L_MAXV, MOTOR_M_MAXV, //max velo of joint 1 = 0 temporarily 
		MOTOR_M_MAXV, MOTOR_M_MAXV, MOTOR_S_MAXV };
const long ACC[6] = { 7000, 7000, 7000, 5000, 5000, 5000 };
//const int DIR[6] = { 1, 1, 1, 1, 1, 1 };

const double MOTOR_L_RATIO = 637.0 / 2.0; //319
const double MOTOR_M_RATIO = 421824.0 / 1715.0; //246
const double MOTOR_S_RATIO = 13824.0 / 125.0; // 111

const double MOTOR_RATIO[6] = { MOTOR_L_RATIO * 2, MOTOR_L_RATIO * 2,
		MOTOR_M_RATIO * 2, MOTOR_M_RATIO, MOTOR_M_RATIO * 2, MOTOR_S_RATIO };

// Encoder ticks calculations (Pulse Per Rotation or PPR)
// Large Motor = 500 * 4 qc * Ratio (637 / 2) = 637000
const int MOTOR_L_PPR = 637000;

// Medium Motor = 500 * 4 qc * Ratio (421824 / 1715) = 491923.0307
const int MOTOR_M_PPR = 491923;

// Small Motor = 512 * 4 qc * Ratio (13824 / 125) = 226492
const int MOTOR_S_PPR = 226492;

// Hall Sensors PPR = 6 * 4qc * Ratio(13824 / 125) = 2654; 
const int HALL_PPR = 680;//2654;

// init an array if the motor ppr based on node ID
const int MOTOR_PPR[6] = { MOTOR_L_PPR * 2, MOTOR_L_PPR * 2, MOTOR_M_PPR * 2,
		MOTOR_M_PPR, MOTOR_M_PPR * 2, HALL_PPR };

int MOTOR_VELO[6] = { MOTOR_L_MAXV, MOTOR_L_MAXV, MOTOR_M_MAXV, MOTOR_M_MAXV,
		MOTOR_M_MAXV, MOTOR_S_MAXV };

/*=====================================================================
 * MOTOR DATA SECTION ENDS HERE 
 * ====================================================================*/

inline long limitNodeVelo(int nodeID, long rpm) {
	long max = MOTOR_MAX_VELO[nodeID - 1];
	if (rpm > max) {
		rpm = max;
	}

	if (rpm < -max) {
		rpm = -max;
	}

	return rpm;
}

inline double omegaToRPM(int nodeID, double omega) {
	double mr = MOTOR_RATIO[nodeID - 1];
	return (omega * kOMEGA2RPM * mr);
}

inline double rpmToOMEGA(int nodeID, long rpm) {
	double mr = MOTOR_RATIO[nodeID - 1];
	return rpm * kRPM2OMEGA / mr;
}

inline long ticksFromRad(int nodeID, double rad) {
	int ppr = MOTOR_PPR[nodeID - 1];
	long ticks = (long) (ppr / (2 * M_PI) * rad);

	return ticks;
}

inline double radFromTicks(int nodeID, long ticks) {
	int ppr = MOTOR_PPR[nodeID - 1];
	double rad = (2 * M_PI / ppr) * ticks;
	if (fabs(rad) < _epsilon)
		rad = 0;
	return rad;
}

inline double degFromTicks(int nodeID, long ticks) {
	int ppr = MOTOR_PPR[nodeID - 1];
	double deg = (double) ticks / ppr * 360;

	return deg;
}

inline double degToRAD(double deg) {
	return deg / 180 * M_PI;
}

//inline int getValidNode(double nodeID) {
//	if (nodeID < 1)
//		return 1;
//	if (nodeID > NODES)
//		return NODES;
//
//	return (int) nodeID;
//}

inline int fixPercentage(int value) {
	if (value > 100)
		return 100;
	if (value < 0)
		return 0;

	return value;
}

inline int clip(int value, int lowerlimit, int maxlimit) {
	if (value > maxlimit) {
		return maxlimit;
	}

	if (value < lowerlimit) {
		return lowerlimit;
	}

	return value;
}
//
//void showMessage(const char* message) {
//	MessageBox(0, message, "Controller", MB_OK);
//}
//
//void EPOSController::showErrorMessage(DWORD errorCode) {
//	showMessage(getErrorInfo(errorCode));
//}
//
//void EPOSController::handleError(DWORD errorCode) {
//	showErrorMessage(errorCode);
//	//halt all motors
//	//clear errors
//}

EPOSController::EPOSController(Robot* robot) :
	MotorController(robot) {

	this->robot = robot;
	//	direction.ReSize(6);
}

EPOSController::~EPOSController() {
}

bool EPOSController::initComm() {
	// constructor must also open the communication with the master EPOS 
	//HANDLE newKeyHandle = VCS_OpenDevice("EPOS", "MAXON_RS232", "RS232",
	//"COM1", &errorCode);
	//if (errorCode != 0) {

	HANDLE newKeyHandle = VCS_OpenDeviceDlg(&errorCode);

	online = false;
	if (newKeyHandle) {
		//Close Previous Device
		if (keyHandle) {
			VCS_CloseDevice(keyHandle, &errorCode);
		}

		keyHandle = newKeyHandle; // set the handler to the current comm
		online = true;
	} else {
		//		showMessage("Can't open Device");
		cout << "can't open device" << endl;
		online = false;
	}
	return online;
}

bool EPOSController::initNode(int nID) {
	if (clearError(nID)) {
		//Write Operation Mode
		if (setOpMode(nID, MODE_PVELO)) {
			if (moveRPM(nID, 0)) {
				if (setEnableState(nID, true)) {
					return true;
				}
			}
		}
	}
	handleError(errorCode);
	return false;
}

bool EPOSController::getEnableState(int nID) {
	BOOL enable = false;
	if (!VCS_GetEnableState(keyHandle, nID, &enable, &errorCode)) {
		handleError(errorCode);
	}
	return enable;
}

bool EPOSController::setEnableState(int nID, bool enable) {
	if (enable) {
		if (VCS_SetEnableState(keyHandle, nID, &errorCode)) {
			return true;
		}
	} else {
		if (VCS_SetDisableState(keyHandle, nID, &errorCode)) {
			return true;
		}
	}
	handleError(errorCode);
	return false;
}

char* EPOSController::getErrorInfo(DWORD errorCode) {
	char* pStrErrorInfo;

	if ((pStrErrorInfo = (char*) malloc(100)) != NULL) {
		if (VCS_GetErrorInfo(errorCode, pStrErrorInfo, 100)) {
			return pStrErrorInfo;
		}
	}

	return "Not enough memory to allocate "
		"buffer for error information string";
}

//char* EPOSController::getFeedback() {
//	char* feedback;
//	return feedback;
//}

bool EPOSController::clearError(int nID) {
	if (VCS_ClearFault(keyHandle, nID, &errorCode)) {
		return true;
	}
	handleError(errorCode);
	return false;
}

bool EPOSController::getHomeStatus(int nID) {
	DWORD read;
	long calibration = 0;
	if (!VCS_GetObject(keyHandle, nID, CALIB_INDEX1, CALIB_INDEX2,
			&calibration, 4, &read, &errorCode)) {
		handleError(errorCode);
	}
	return calibration;
}

bool EPOSController::setHomeStatus(int nID, bool status) {
	int wvalue = status ? 1 : 0;
	DWORD written;
	if (!VCS_SetObject(keyHandle, nID, CALIB_INDEX1, CALIB_INDEX2, &wvalue, 4,
			&written, &errorCode)) {
		handleError(errorCode);
		return false;
	}
	return true;
}

int EPOSController::getHome(int nID) {
	return 0;
}

bool EPOSController::setHome(int nID) {
	char uMode;
	char hM = 35;

	DWORD zero = 100;

	VCS_GetOperationMode(keyHandle, nID, &uMode, &errorCode);
	VCS_SetOperationMode(keyHandle, nID, MODE_HOME, &errorCode);

	long homepos = 0;
	if (nID == 5) {
		homepos = ticksFromRad(5, deg2rad(NODE5_OFFSET));
	}
	if (VCS_SetHomingParameter(keyHandle, nID, zero, zero, zero, 0, zero,
			homepos, &errorCode)) {
		//cout << "Params Set" << endl;
	}
	VCS_FindHome(keyHandle, nID, hM, &errorCode);
	VCS_SetOperationMode(keyHandle, nID, uMode, &errorCode);

	//det home status as true
	setHomeStatus(nID, true);
	return true;
}

char EPOSController::getOpMode(int nID) {
	char opMode = '0';
	if (!VCS_GetOperationMode(keyHandle, nID, &opMode, &errorCode)) {
		handleError(errorCode);
	}
	return opMode;
}

bool EPOSController::setOpMode(int nID, char opMode) {
	if (VCS_SetOperationMode(keyHandle, nID, opMode, /*0x03*//*Profile Velocity Mode*/
	&errorCode)) {
		//		//Write Profile Objects
		if (opMode == MODE_PVELO) {
			if (VCS_SetVelocityProfile(keyHandle, nID, ACC[nID - 1],
					ACC[nID - 1], &errorCode)) {
				return true;
			} else {
				handleError(errorCode);
			}
		}
		if (opMode == MODE_PPOS) {
			if (VCS_SetPositionProfile(keyHandle, nID, MOTOR_MAX_VELO[nID
			- 1], ACC[nID - 1], ACC[nID - 1], &errorCode)) {
				return true;
			} else {
				handleError(errorCode);
			}
		}
		if (opMode == MODE_VELO) {
			if (VCS_SetOperationMode(keyHandle, nID, MODE_VELO, &errorCode)) {
				return true;
			} else {
				handleError(errorCode);
			}
		}

	}
	return false;
}

//TODO: change back to 6 joints
bool EPOSController::setPPosMode() {
	for (int i = 1; i <= 5; i++) {
		setOpMode(i, MODE_PPOS);
	}
	return true;
}

//TODO: Change back to Profile Velo Mode
bool EPOSController::setPVeloMode() {
	for (int i = 1; i <= 5; i++) {
		setOpMode(i, MODE_VELO);
	}
	return true;
}

int EPOSController::getDirection(int nID) {
	return 0;
}

bool EPOSController::setDirection(int nID, int dir) {
	//	direction(nID) = dir;
	return false;
}

double EPOSController::getPos(int nID) {
	long ticks = getTicks(nID);
	return radFromTicks(nID, ticks);
}

bool EPOSController::setPos(int nID, double pos) {
	long ticks = ticksFromRad(nID, pos);
	return setTicks(nID, ticks);
}

int EPOSController::getTicks(int nID) {
	long ticks = 0; //TODO:currentPositions[nodeID - 1];
	if (!VCS_GetPositionIs(keyHandle, nID, &ticks, &errorCode)) {
		handleError(errorCode);
	}
	return ticks;
}

//TODO: Use the gear reductions to determine the final max speeds
void EPOSController::setSyncVelo(ColumnVector &pos) {
	ColumnVector delta(6);
	double maxTime = 0.0;
	double t[6];

	//TODO: change back to 6
	for (int i = 0; i < 5; i++) {
		delta(i + 1) = ticksFromRad((i + 1), pos(i + 1)) - getTicks(i + 1);
		t[i] = fabs(delta(i + 1) / MOTOR_MAX_VELO[i]);
		if (t[i] > maxTime) {
			maxTime = t[i];
		}

	}

	if (maxTime == 0.0) {
		return;
	}

	//TODO: change back to 6 joints
	for (int i = 1; i <= 5; i++) {
		long syncVelo = (long) (fabs(delta(i) / maxTime));
		setRPM(i, syncVelo);
	}
}

bool EPOSController::setTicks(int nID, long ticks) {
	//	bool switchback = false;
	//	if (getOpMode(nID) != MODE_PPOS) {
	//		switchback = true;
	//		setOpMode(nID, MODE_PPOS);
	//	}
	//	setOpMode(nID, MODE_PPOS);

	//	DWORD pVel, pAcc, pDec;

	//	VCS_GetPositionProfile(keyHandle, nID, &pVel, &pAcc, &pDec, &errorCode);
	//	cout << "POS : " << nID << " " << pVel << " " << pAcc << " " << pDec
	//			<< endl;
	if (!VCS_MoveToPosition(keyHandle, nID, ticks, true, true, &errorCode)) {
		handleError(errorCode);
		return false;
	}
	//	if (switchback) {
	//		setOpMode(nID, MODE_PVELO);
	//	}
	//	setOpMode(nID, MODE_PVELO);
	return true;
}

double EPOSController::getOmega(int nID) {
	long rpm = getRPM(nID);
	return rpmToOMEGA(nID, rpm);
}

bool EPOSController::moveOmega(int nID, double omega) {
	long rpm = (long)omegaToRPM(nID, omega);
	return moveRPM(nID, rpm);
}

long EPOSController::getRPM(int nID) {
	long rpm = 0;
	if (VCS_GetVelocityIs(keyHandle, nID, &rpm, &errorCode)) {
		return rpm;
	} else {
		handleError(errorCode);
	}
	return rpm;
}

bool EPOSController::setRPM(int nID, long rpm) {
	long max = MOTOR_MAX_VELO[nID - 1];

	if (rpm > max) {
		rpm = max;
	}

	if (nID == 6) {
		if (rpm < 900) {
			rpm = 900;
		}
	} else {
		if (rpm < 1) {
			rpm = 1;
		}
	}

	DWORD written;
	//TODO: check object code : 0x6081 = profile velocity 
	if (!VCS_SetObject(keyHandle, nID, 0x6081, 0x00, &rpm, 4, &written,
			&errorCode)) { //(keyHandle, nodeID, velo, &errorCode)) {
		handleError(errorCode);
		return false;
	}

	return true;
}

//TODO: Keep new function for Velocity Mode 
// Abheek changed the move-with-velo command
bool EPOSController::moveRPM(int nID, long rpm) {
	if (VCS_SetVelocityMust(keyHandle, nID, rpm, &errorCode)) {
		return true;
	}
	//	if (VCS_MoveWithVelocity(keyHandle, nID, rpm, &errorCode)) {
	//		return true;
	//	}
	handleError(errorCode);
	return false;
}

bool EPOSController::incrementRPM(int nID, long deltaRPM) {
	long rpm = getRPM(nID);
	if (rpm >= 0) {
		rpm = rpm + deltaRPM;
		rpm = clip(rpm, 0, MOTOR_VELO[nID - 1]);
	} else {
		rpm = rpm - deltaRPM;
		rpm = clip(rpm, -MOTOR_VELO[nID - 1], 0);
	}
	return moveRPM(nID, rpm);
}

bool EPOSController::halt(int nID) {
	if (getOpMode(nID) == MODE_PPOS) {
		if (!VCS_HaltPositionMovement(keyHandle, nID, &errorCode)) {
			handleError(errorCode);
		}
	}

	if (getOpMode(nID) == MODE_VELO) {
		if (!VCS_SetVelocityMust(keyHandle, nID, 0, &errorCode)) {
			handleError(errorCode);
		}
		//		if (VCS_MoveWithVelocity(keyHandle, nID, 0, &errorCode)) {
		//			return true;
		//		}
	}

	return true;
}

bool EPOSController::isPositionReached(int nID) {
	BOOL targetReached;
	if (!VCS_GetMovementState(keyHandle, nID, &targetReached, &errorCode)) {
		handleError(errorCode);
	}
	return targetReached;
}

//TODO: change back to 6 joints
bool EPOSController::waitUntilPositionReached() {
	while (true) {
		bool reached = true;
		for (int i = 1; i <= 5; i++) {
			reached = reached && isPositionReached(i);
		}
		if (reached) {
			break;
		}
	}
	return true;
}

void EPOSController::doJog(ColumnVector& jogVelo) {
	ColumnVector qpd = robot->jacobian_DLS_inv(0.001, 0.003) * jogVelo;
	cout << qpd.AsRow() << endl; 

	//if any node exceeds its speed limit, ratio = Vmax / Vdesired;
	//normalize other joints with this ratio
	double normRatio = 1;
	//	for (int i = 0; i < 6; i++) {
	//		double rpm = (long) omegaToRPM(i + 1, qpd(i + 1));
	//		double k = rpm / MOTOR_MAX_VELO[i];
	//		if (fabs(k) > normRatio) {
	//			normRatio = fabs(k);
	//		}
	//	}

	//make the actual motion here
	for (int i = 1; i <= 5; i++) {
		long rpm = (long) (omegaToRPM(i, qpd(i)) / (normRatio));

		if (isSingular && (i == 4 || i == 6)) {
			rpm = 0;
		}

		this->moveRPM(i, rpm);
	}
}

double EPOSController::getMaxVelo(int nID) {
	long mRPM = MOTOR_MAX_VELO[nID - 1];
	double omega = rpmToOMEGA(nID, mRPM);
	return omega;
}

