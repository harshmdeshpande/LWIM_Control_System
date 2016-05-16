#ifndef OCVCAMVIEW_H_
#define OCVCAMVIEW_H_

#include <cv.h>
#include <highgui.h>
#include <cvaux.h>

#include <wx/wxprec.h>

#ifndef  WX_PRECOMP
#include <wx/wx.h>
#endif //precompiled headers
#include "../ip/ocvProcessor.h"

//class ocvCamera;

class ocvCamView : public wxWindow {
public:
	ocvCamView(wxWindow *frame, const wxPoint& pos, const wxSize& size);
	virtual ~ocvCamView();

	bool IsCaptureEnabled();
	void CheckUpdate();

	// Draw method
	void DrawCam(IplImage* pImg);
	void Draw(wxDC& dc);
	
	
	
	// Public data
public:
//	ocvCamera *camera;

protected:
	IplImage image;
	wxBitmap bitmap;
	ocvProcessor* processor;

	bool isDrawing;
	bool isNewImage;

	int m_nWidth;
	int m_nHeight;

	// private methods
private:
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& even);

	// protected data
protected:

DECLARE_EVENT_TABLE()
};

#endif /*OCVCAMVIEW_H_*/
