#define HEIGHT 288	
#define WIDTH 352

#include <math.h>


#include <iostream>
#include <highgui.h>
#include <cv.h>
#include <cvaux.h>
#include <iostream.h>

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

#include <wx/wxprec.h>
#include <wx/image.h>

#ifndef  WX_PRECOMP
#include <wx/wx.h>
#endif //precompiled headers
// other headers
#include "../ocvApp.h"
#include "../camera/ocvCamera.h"

#include "ocvCamView.h"
#include "../ip/ocvProcessor.h"



using namespace std;


BEGIN_EVENT_TABLE(ocvCamView, wxWindow)
	EVT_PAINT( ocvCamView::OnPaint )
	EVT_SIZE( ocvCamView::OnSize )
END_EVENT_TABLE()

ocvCamView::ocvCamView(wxWindow *frame, const wxPoint& pos, const wxSize& size) :
	wxWindow(frame, -1, pos, size, wxSIMPLE_BORDER) {
//	camera = NULL;

	// set my canvas width/height
	m_nWidth = size.GetWidth();
	m_nHeight = size.GetHeight();
		
	
	isDrawing = false;
	isNewImage = 0;
}

ocvCamView::~ocvCamView() {
//	camera = NULL;
	
}

bool ocvCamView::IsCaptureEnabled() {
	return ( 1 );
}

void ocvCamView::OnPaint(wxPaintEvent& event) {
	wxPaintDC dc(this);
	Draw(dc);
}

void ocvCamView::Draw(wxDC& dc) {
	// check if dc available

	if ( !dc.Ok() || isDrawing == true) {
		return;
	}

	isDrawing = true;
	//dc.BeginDrawing();

	int x, y, w, h;
	dc.GetClippingBox( &x, &y, &w, &h);
	// if there is a new image to draw

	if (isNewImage) {

		dc.DrawBitmap(bitmap, x, y);
		
		isNewImage = false;
	} else {
		// draw inter frame ?
	}

	//d/c.EndDrawing();
	isDrawing = false;

	return;
}


void ocvCamView::DrawCam(IplImage* pImg) {
	if (isDrawing)
		return;
	
	isDrawing = true;

	// If there was an image then we need to update view
	if (pImg) {
		IplImage* pDstImg = cvCloneImage(pImg);

//		int nCamWidth = camera->width;
//		int nCamHeight = camera->height;

		int nCamWidth = WIDTH;
		int nCamHeight = HEIGHT;

		// Process image from opencv to wxwidgets
		unsigned char *rawData;
		//Draw my stuff to output canvas
				
		cvGetRawData(pDstImg, &rawData, NULL, NULL);
				
		// Convert data from raw image to wxImg 
		wxImage pWxImg = wxImage(nCamWidth, nCamHeight, rawData, true);
		
		// Convert to bitmap to be used by the window to draw
		bitmap = wxBitmap(pWxImg.Scale(m_nWidth, m_nHeight));
			
		
		isNewImage = true;
		isDrawing = false;

		Refresh( FALSE);
		Update();

		cvReleaseImage(&pDstImg);
		
	}

}

void ocvCamView::CheckUpdate() {
	Update();
}

void ocvCamView::OnSize(wxSizeEvent& even) {
	int nWidth = even.GetSize().GetWidth();
	int nHeight = even.GetSize().GetHeight();

	m_nWidth = nWidth;
	m_nHeight = nHeight;

}

