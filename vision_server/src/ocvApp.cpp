#include <iostream>

#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include "socket/socket.h"

#include <highgui.h>
#include <cv.h>
#include <cvaux.h>

// wxwindows headers
#include <wx/wxprec.h>
#include <wx/statline.h>
#include <wx/minifram.h>
#include <wx/thread.h>

#ifndef  WX_PRECOMP
#include <wx/wx.h>
#endif //precompiled headers
// custom headers
#include "camera/ocvCamera.h"
#include "wxgui/ocvFrame.h"
#include "wxgui/ocvCamView.h"
#include "ocvThread.h"
#include "ocvApp.h"


using namespace std;

//IMPLEMENT_APP(ocvApp)  //Implenents the main funtion in wxWidgets.Comment to provide custom main fn.


BEGIN_EVENT_TABLE(ocvApp, wxApp)
END_EVENT_TABLE()

ocvApp::ocvApp() {
}

ocvApp::~ocvApp() {
}

bool ocvApp::OnInit() {
	camera = new ocvCamera();
	f = new ocvFrame( NULL, "Vision Engine",
			wxPoint(-1, -1), wxSize(800, 600) );   
	
	
	f->Show(TRUE);
	
	SetTopWindow(f);
	f->parentApp = this;
	

	//initialize the worker thread
	worker = new ocvThread(f);
	if (worker->Create() != wxTHREAD_NO_ERROR) {
		wxExit(); // error occured on creation of worker thread
	}

	ocvCamView *camView = f->GetCameraView();
	ocvCamView *srcView = f->GetCamView();

	//assign the interdependent objects
	f->worker = worker;
	worker->camera = camera;
	camera->worker = worker;

	// initialize camera 
	if (camera->Init1() == 0) {
		wxMessageBox("Can't initialize camera. Try to change format", "Error");
	}

	camera->camView = camView;
	camera->srcView = srcView;
	camera->frame = f;
	


// Link camera view to camera object
//	camView->camera = camera;
//	srcView->camera = camera;

	// start the thread
	if (worker->Run() != wxTHREAD_NO_ERROR) {
		wxExit(); // error running the worker thread
	}
	return true;
}

int ocvApp::OnExit() {
	if (worker != NULL) {
		worker->Delete();
	}
	worker = NULL;
	cout<<"AppClosed" << endl;
	delete( camera );

	return wxApp::OnExit();
}

int main(int argc, char **argv) {
	
	testSocket* sock = new testSocket();
	ocvApp* myApp = new ocvApp();
	wxApp::SetInstance(myApp);
	wxEntryStart(argc,argv);
	wxTheApp->CallOnInit();

	//start the main communication thread
	boost::thread commThread(boost::bind( &testSocket::doCommunication , sock, myApp));
	
	wxTheApp->OnRun();	
	wxEntryCleanup();
	return 0;
}

