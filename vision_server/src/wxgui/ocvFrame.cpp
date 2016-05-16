#define ID_THRESH1 1
#define ID_THRESH2 2
#define ID_THRESH3 3

#include <highgui.h>
#include <cv.h>
#include <cvaux.h>

#include <wx/wxprec.h>
#include <wx/statline.h>
#include <wx/minifram.h>
#include <wx/thread.h>

#ifndef  WX_PRECOMP
#include <wx/wx.h>
#endif //precompiled headers
#include "../ocvApp.h"
#include "../ocvThread.h"
#include "../camera/ocvCamera.h"
#include "ocvCamView.h"
#include "../ip/ocvProcessor.h"

#include <iostream>
using namespace std;
// WIDOWS specific define
#if defined(_WINDOWS)
#include <vfw.h>
#endif

#include "../camera.xpm"


#include "ocvFrame.h"




BEGIN_EVENT_TABLE(ocvFrame, wxFrame)

	EVT_SLIDER(ID_THRESH1,ocvFrame::OnThresh)
	EVT_SLIDER(ID_THRESH2,ocvFrame::OnBright)
	EVT_SLIDER(ID_THRESH3,ocvFrame::OnContrast)
	EVT_BUTTON(wxID_EXIT,ocvFrame::OnExit)
	
END_EVENT_TABLE()

ocvFrame::ocvFrame(){
	
}

ocvFrame::ocvFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
		const wxSize& size) :
			wxFrame(
					frame,
					-1,
					title,
					pos,
					size,
					wxSYSTEM_MENU | wxMAXIMIZE_BOX | wxMINIMIZE_BOX |wxCLOSE_BOX| wxCAPTION |wxRESIZE_BORDER)

{
		
	wxIcon icon(camera_xpm);
	SetIcon(icon);

// set bg color for my frame 
//	SetBackgroundColour( *wxLIGHT_GREY);
	SetBackgroundColour( wxColour(128,128,128));
	;

	// create status bar
	CreateStatusBar( 1);
	
	int width, height;
	this->GetClientSize(&width, &height);

	mainPanel = new wxPanel(this, -1, wxPoint(0,0), wxSize(360,315));
	
	ctrlPanel = new wxPanel(this, -1, wxPoint(365,0), wxSize(360,315)); 
	
	
	wxStaticBox *camBox = new wxStaticBox( mainPanel, wxID_ANY, "Camera View",
			wxPoint(5,0), wxSize(362,640));

	wxStaticBox *slideBox = new wxStaticBox(ctrlPanel,wxID_ANY, "Controls",
			wxPoint(370,15),wxSize(210,150));
		
	
	wxStaticBoxSizer *pMainSizer = new wxStaticBoxSizer( camBox, wxVERTICAL );
	
	wxStaticBoxSizer *slideSizer = new wxStaticBoxSizer(slideBox, wxHORIZONTAL);
	

	// build cam canvas
	srcView = new ocvCamView( camBox, wxPoint(-1,-1), wxSize(352, 288) );
	
	camView = new ocvCamView( camBox, wxPoint(-1,-1), wxSize(352,288));
	

	threshSlide = new wxSlider(slideBox,ID_THRESH1,50,1,255,wxPoint(70,30),wxSize(200,50),
									wxSL_LABELS);
	wxStaticText* threshText = new wxStaticText(slideBox,wxID_STATIC,wxT("Threshold"),wxPoint(10,50));
	
	
	brightSlide = new wxSlider(slideBox,ID_THRESH2,50,1,255,wxPoint(70,80),wxSize(200,50),
									wxSL_LABELS);	
	wxStaticText* brightText = new wxStaticText(slideBox,wxID_STATIC,wxT("Brightness"),wxPoint(10,100));
	
	
	contrastSlide = new wxSlider(slideBox,ID_THRESH3,50,1,255,wxPoint(70,130),wxSize(200,50),
										wxSL_LABELS);
	wxStaticText* contrastText = new wxStaticText(slideBox,wxID_STATIC,wxT("Contrast"),wxPoint(10,150));
	
//	wxBitmap exitImg(wxT("exit.png"),wxBITMAP_TYPE_PNG);
	wxButton *exitBut = new wxButton(slideBox,wxID_EXIT,"Exit",wxPoint(135,200),wxSize(-1,-1));	
//	wxBitmapButton *exitBut = new wxBitmapButton(slideBox,wxID_EXIT,exitImg,wxPoint(135,200),wxSize(-1,-1));
	
	
	
	pMainSizer->Add(camView, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND);
	pMainSizer->Add(srcView, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND);
	
	mainPanel->SetSizer(pMainSizer);
	ctrlPanel->SetSizer(slideSizer);
	
	pMainSizer->SetSizeHints(mainPanel);
	slideSizer->SetSizeHints(ctrlPanel);

	mainPanel->SetAutoLayout( TRUE);
	ctrlPanel->SetAutoLayout( TRUE);


	wxBoxSizer *pTopSizer = new wxBoxSizer(wxHORIZONTAL);
	pTopSizer->Add(mainPanel, 1, wxALIGN_CENTER|wxEXPAND, 0);
	pTopSizer->Add(ctrlPanel, 1, wxALIGN_CENTER|wxEXPAND, 0);

	SetSizer(pTopSizer);

	// display my stuff
	SetAutoLayout( TRUE);
}

ocvFrame::~ocvFrame() {
	// first pause camera
//	camView->camera->PauseResume();
	worker->Delete();
	// clean on exit
	delete( camView );
	delete( srcView);
	delete( mainPanel );
	delete(ctrlPanel);
	

	// set to null
	parentApp->worker = NULL;
	mainPanel = NULL;
	ctrlPanel = NULL;
	camView = NULL;
	srcView = NULL;

	worker = NULL;
	parentApp = NULL;
}

void ocvFrame::SetParentApp(ocvApp* parent) {
	parentApp = parent;
}

ocvCamView* ocvFrame::GetCameraView() {
	return camView;
}

ocvCamView* ocvFrame::GetCamView() {
	return srcView;
}

void ocvFrame::SetStatusBarText(const char* text) {
	GetStatusBar()->SetStatusText(text);
}

void ocvFrame::ResetLayout() {
	GetSizer()->Layout();
	mainPanel->Layout();
	ctrlPanel->Layout();
}

void ocvFrame::OnExit(wxCommandEvent& WXUNUSED(pEvent) ) {
	
	Close();
	frameIsClosed = true;
	return;
}

void ocvFrame::OnThresh(wxCommandEvent& wxEVT_COMMAND_SLIDER_UPDATE){
	
	worker->camera->processor->thresh =  threshSlide->GetValue();
  	
}
void ocvFrame::OnBright(wxCommandEvent& wxEVT_COMMAND_SLIDER_UPDATE){
	
	worker->camera->processor->brightness =  brightSlide->GetValue();
			
}

void ocvFrame::OnContrast(wxCommandEvent& wxEVT_COMMAND_SLIDER_UPDATE){
	
	worker->camera->processor->contrast =  contrastSlide->GetValue();
	
}
		

