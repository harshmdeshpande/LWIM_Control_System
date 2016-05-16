#include "comm/socket/socket.h"
#include "core/RobotModeManager.h"
#include "core/robotControl/motorControl/epos/EPOSController.h"

#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

using namespace std;

const int SERVER_MODE = 1;
const int USE_EPOS = 1;

const short multicast_port = 30001;

char* model = "conf/robot_dh.conf";
char* robotName = "robot_dh";

RobotModeManager* manager;

class RobotService {
public:
	boost::mutex m_mutex;

	void doMultiComm() {
		SocketServer in(2001, 1);
		while (1) {
			Socket* s = in.Accept();
			cout << "Client Connected to Broadcast Server" << endl;
			while (1) {
				string inputCommand = s->ReceiveLine();
				if (inputCommand.empty())
					break;

				string returnCommand = manager->getFeedbackData();
				s->SendLine(returnCommand);
			}
			//manager->stop();
			cout << "Client Disconnected from Broadcast Server" << endl;
		}
	}

	void doCommunication() {
		SocketServer in(2000, 1);
		while (1) {
			Socket* s = in.Accept();
			cout << "Client Connected to Request Server" << endl;
			while (1) {
				string inputCommand = s->ReceiveLine();
				if (inputCommand.empty())
					break;
				cout << "Client Says: " << inputCommand << endl;
				//boost::mutex::scoped_lock lock(m_mutex);
				string returnCommand = manager->handleCommand(inputCommand);
				cout << "I say: " << returnCommand << endl;
				s->SendLine(returnCommand);
			}
			//			manager->stop();
			cout << "Client Disconnected from Request Server" << endl;
		}
	}

	void init() {
		Robot* robot = new Robot(model, robotName);
		MotorController* mController = new MotorController(robot);

		if (USE_EPOS) {
			mController = new EPOSController(robot);
		}

		manager = new RobotModeManager(mController);
		if (!manager->isOnline()) {
			cout
					<< "Controller is not online! Please start controller and restart program"
					<< endl;
			exit(-1);
		}
	}

private:

};

void doTest() {
	manager->handleCommand("MODE_EXEC  ");
	manager->handleCommand("ready  ");
	manager->handleCommand("approach 0.3 0 0.1 -3.142 0 0 0.1  ");
	manager->handleCommand("insert -0.1  ");

}

int main(int argc, char **argv) {
	RobotService service;
	service.init();

	if (!SERVER_MODE) {
		//DO Something
		doTest();
		exit(0);
	}

	cout << "Server Started ...." << endl;

	//start broadcast thread
	boost::thread broadcastThread(boost::bind( &RobotService::doMultiComm, &service));

	//start the main communication thread
	boost::thread commThread(boost::bind( &RobotService::doCommunication, &service));

	//join the threads to the main thread
	broadcastThread.join();
	commThread.join();

	return 0;
}
