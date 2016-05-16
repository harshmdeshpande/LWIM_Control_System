#ifndef OCVCAMERA_H_
#define OCVCAMERA_H_

#include "../ip/ocvProcessor.h"
class ocvFrame;
class ocvThread;
class ocvCamView;
//class srcCamView;
//class ocvProcessor; now

class ocvCamera {
public:
	ocvCamera();
	virtual ~ocvCamera();

	IplImage* GetIFrame();

	// initialize camera frame def
	int Init1();
	int Run1();
	void Start1();
	void Stop1();
	void PauseResume();
	int GetSize();
	void IsChanged();

public:
	bool isRunning;
	bool isAvi;
	bool isPause;

	bool isProcBusy;
	int isChange;

	CvCapture* capture;
	ocvCamView *camView,*srcView;
	
	ocvFrame* frame;
	ocvThread* worker;
	ocvProcessor* processor;
	int width;
	int height;

	// protected data
protected:
	IplImage *videoImg;
	
	double fpsAlpha;
	double fps;
	char fpsString[255];
	double timePrevFrameStamp;
	double timeCurrFrameStamp;
	int totalFrames;
	
	

	// Implementation
protected:
	void GetNextFrame(void*);

};

#endif /*OCVCAMERA_H_*/
