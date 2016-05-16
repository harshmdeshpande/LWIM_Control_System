#ifndef OCVFRAME_H_
#define OCVFRAME_H_

#include <wx/wxprec.h>

#ifndef  WX_PRECOMP
#include <wx/wx.h>
#endif //precompiled headers

class ocvApp;
class ocvCamView;

class ocvThread;
class ocvProcessor;

class ocvFrame : public wxFrame {
public:
	ocvFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
			const wxSize& size);
	ocvFrame();
	virtual ~ocvFrame();
	
	ocvCamView* GetCameraView( );
	ocvCamView* GetCamView();
	void SetParentApp( ocvApp *parentApp );
	void SetStatusBarText( const char* strText );
	void ResetLayout( );
	
public:
	
	ocvApp*			parentApp;
	ocvThread*		worker;
	ocvProcessor*   processorCpy;
	bool frameIsClosed;
	
	wxSlider  	*threshSlide, *brightSlide, *contrastSlide;

// Protected data
protected: 
	ocvCamView	*camView, *srcView;

	wxPanel    *mainPanel, *ctrlPanel;
		

// message map functions
protected:
	void OnExit( wxCommandEvent& WXUNUSED(pEvent) );
	void OnThresh(wxCommandEvent& event);
	void OnBright(wxCommandEvent& event);
	void OnContrast(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

#endif /*OCVFRAME_H_*/
