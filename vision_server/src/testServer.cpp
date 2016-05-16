#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include "socket/socket.h"
#include "ocvApp.h"
#include "ip/ocvProcessor.h"
#include "camera/ocvCamera.h"

using namespace std;


//int main(int argc, char **argv) {
//	
//	ocvApp* myApp = new ocvApp();
//	wxApp::SetInstance(myApp);
//	wxEntryStart(argc,argv);
//	wxTheApp->CallOnInit();
//				
//	//start the main communication thread
//	boost::thread commThread(boost::bind( myApp->doCommunication, &myApp));
//	wxTheApp->OnRun();
//	wxEntryCleanup();
//	return 0;
//}


//ocvApp *visionApp = new ocvApp;


class VisionService {
public:
	boost::mutex m_mutex;

	//multicast the controller data
	void doCommunication() {
		SocketServer in(2000, 1);
		while (1) {
			Socket* s = in.Accept();
			cout << "Client Connected to Request Server" << endl;
			while (1) {
				
				if (visionApp->f->frameIsClosed){
					cout<<"BREAk"<<endl;
					break;
				}
				string inputCommand = s->ReceiveLine();
				if (inputCommand.empty())
					break;
				
				visionApp->camera->processor->handleCmd();
				string returnCommand = visionApp->camera->processor->rslt;
				
				cout<< "Sending 4m Server : "<< returnCommand <<endl;
				boost::mutex::scoped_lock lock(m_mutex);
				
				s->SendLine(returnCommand);
//				cout<< "Sending 4m Server : "<< returnCommand <<endl;
				
				
			}
			
			cout << "Client Disconnected from Request Server" << endl;
			break;
		}
	}
	

private:

};
//
////
////int main(int argc, char **argv) {
////	VisionService service;
////
////	wxApp::SetInstance(visionApp);
////	wxEntryStart(argc,argv);
////	wxTheApp->CallOnInit();
////				
////	//start the main communication thread
////	boost::thread commThread(boost::bind( &VisionService::doCommunication, &service));
////	wxTheApp->OnRun();
////	wxEntryCleanup();
////
////	
////
////	
////	return 0;
////}

