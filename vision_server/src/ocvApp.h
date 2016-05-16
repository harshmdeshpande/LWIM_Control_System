#ifndef OCVAPP_H_
#define OCVAPP_H_

#include <wx/wxprec.h>
#include <wx/thread.h>

#include "testSocket.h"
#include "camera/ocvCamera.h"
#include "wxgui/ocvFrame.h"




#ifndef  WX_PRECOMP
  #include <wx/wx.h>
#endif //precompiled headers

class ocvThread;
//class ocvCamera;
//class ocvFrame;
class testSocket;

class ocvApp : public wxApp
{
public:
	ocvApp();
	virtual ~ocvApp();
	bool OnInit( );
	int OnExit( );
//	void doCommunication();
	
public:
	ocvThread*	worker;
	ocvCamera*  camera;
	ocvFrame*  f;
//	testSocket* sock;

	
protected:
//	bool OnInit( );
//	int OnExit( );
	

	DECLARE_EVENT_TABLE()
};

#endif /*OCVAPP_H_*/
