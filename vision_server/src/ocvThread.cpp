// system headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/stat.h>

// wxwidgets includes
#include <wx/wxprec.h>
#include <wx/thread.h>
#include <wx/string.h>
#include <wx/regex.h>

#ifndef  WX_PRECOMP
#include <wx/wx.h>
#endif //precompiled headers
// opencv includes
#include <highgui.h>
#include <cv.h>
#include <cvaux.h>

#include "ocvApp.h"
#include "wxgui/ocvFrame.h"
#include "camera/ocvCamera.h"
#include "ocvThread.h"

ocvThread::ocvThread(ocvFrame* frame) :
	wxThread(wxTHREAD_DETACHED) {

	// get frame refrence
	this->frame = frame;

	// start life
	m_bLife = 1;

	m_pMutex = new wxMutex( );

	return;
}

ocvThread::~ocvThread() {
	m_pMutex=NULL;
	camera=NULL;
	frame = NULL;
}

void ocvThread::OnExit() {
	delete( m_pMutex );
}

double ocvThread::GetTime(void) {

	struct timeb timeStamp;
	// get a time stamp
	ftime( &timeStamp);
	double nTime = (double) timeStamp.time*1000 + timeStamp.millitm;
	return (nTime );
}

void *ocvThread::Entry() {
	m_bLife = 1;

	camera->Start1();

	while (m_bLife) {
		if (TestDestroy() == 1)
			break;

		///////////////////////////////////////////////////////////
		// START PROCESING BLOCK
		// first tell camera object to process one frame - if camera
		// is running
		if (camera->isRunning) {
			camera->Run1();
		}

		wxThread::Sleep( 3);
	}

	camera->Stop1();

	return NULL;
}
