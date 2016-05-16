#define HEIGHT 240
#define WIDTH 320

#include <highgui.h>
//#include <vfw.h>
#include <cv.h>
#include <cvaux.h>
#include <cvcam.h>
#include <iostream.h>

// other local includes
#include "../ocvApp.h"
#include "../ocvThread.h"
#include "../wxgui/ocvCamView.h"

//#include "../wxgui/srcCamView.h"

#include "../wxgui/ocvFrame.h"
#include "../ip/ocvProcessor.h"
#include "ocvCamera.h"

using namespace std;


ocvCamera::ocvCamera() {
	worker = NULL;
	isRunning = false;
	isPause = false;
	// the new camera implementation
	videoImg = NULL;
	fps = -1;
	fpsAlpha = 0.1;
	isAvi = false;

	isChange = 0;
	
	processor = new ocvProcessor();
}

ocvCamera::~ocvCamera() {
	//Uninitialize( );
	if (isRunning)
		cvReleaseCapture( &capture);
	if (videoImg) {
		cvReleaseImage( &videoImg);
	}

	videoImg = NULL;
	worker = NULL;
	capture = NULL;
	camView = NULL;
	srcView = NULL;
	
	frame = NULL;
	processor = NULL;
}

int ocvCamera::Init1() {
	timeCurrFrameStamp = worker->GetTime();
	timePrevFrameStamp = timeCurrFrameStamp;

	return ( 1 );
}

int ocvCamera::GetSize() {
	// now get my properties
		
	width = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	height = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	
	// set camview size
	srcView->SetSize(width, height);
	
	frame->ResetLayout();

	return ( 0 );
}

void ocvCamera::Start1() {
	
	
	capture = cvCaptureFromCAM( -1 );
	
	//SETTING CAMERA WIDTH & HEIGHT NOT WORKING
		
	// grab first frame to initialize format
	//IplImage* pFrame = 
	cvQueryFrame(capture);
	// get camera's size
	GetSize();

	isRunning = true;
}

void ocvCamera::PauseResume() {
	if (isPause) {
		isPause = false;
	} else {
		isPause = true;
	}
}

void ocvCamera::IsChanged() {
	isChange = 1;
	isPause = true;
}

void ocvCamera::Stop1() {
	isRunning = false;

	cvReleaseCapture( &capture);
	if (videoImg) {
		cvReleaseImage( &videoImg);
		videoImg = NULL;
	}

}

void ocvCamera::GetNextFrame(void*) {
	IplImage* pFrame= NULL;

	// get current frame time stamp
	timeCurrFrameStamp = worker->GetTime();
	if (timeCurrFrameStamp - timePrevFrameStamp < 5)
		return;
	else
		timePrevFrameStamp = timeCurrFrameStamp;

	pFrame = cvQueryFrame(capture);

	
	// if this was avi and frame is zero(end or none?) stop
	if (pFrame == 0 && isAvi) {
		//this->StopAvi( 0,0 );
		return;
	}

	if (pFrame) {
		// if no video image 
		if ( !videoImg) {
			cvReleaseImage( &videoImg);
			videoImg = cvCreateImage(cvSize(width, height), 8, 3);
			
		}

		// check for the last origin of the frame
		if (pFrame->origin == 1) {
			cvConvertImage(pFrame, videoImg, 
			CV_CVTIMG_FLIP | CV_CVTIMG_SWAP_RB);
		} else {
			cvCopy(pFrame, videoImg, 0);
		}
		
		videoImg = processor->processImage(videoImg); //OPENCV PROCESSOR
		srcView->DrawCam(processor->tempImg);
		camView->DrawCam(videoImg);
		
		cvReleaseImage(&videoImg);
		cvReleaseImage(&processor->tempImg);
	}

	// If camera started
	if (isRunning) {
		// get current frame time stamp
		timeCurrFrameStamp = worker->GetTime();
		// update fps
		if (fps < 0) {
			fps = 1000 / (timeCurrFrameStamp - timePrevFrameStamp );
		} else {
			fps = ( 1 - fpsAlpha ) * fps + fpsAlpha * 1000
					/ (timeCurrFrameStamp - timePrevFrameStamp );
		}
		// set current time stamp as previuos
		timePrevFrameStamp = timeCurrFrameStamp;
		// get info of number of frames per second in a string
		// for debuging/etc
		sprintf(fpsString, "FPS: %5.1f", fps);
		frame->SetStatusBarText(fpsString);

	}
}

IplImage *ocvCamera::GetIFrame() {
	return (videoImg );
}

int ocvCamera::Run1() {

	if ( !isPause) {
		// Get my next frame
		this->GetNextFrame( NULL);
	} else {
		if (isChange == 1) {
			// check size
			Stop1();
			Start1();
			isChange = 0;
			isPause = 0;
		}
	}

	return ( 0 );
}
