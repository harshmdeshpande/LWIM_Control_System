#ifndef OCVPROCESSOR_H_
#define OCVPROCESSOR_H_

#include <cv.h>
#include <string.h>
#include <highgui.h>
#include <vector.h>
#include "objects.h"
#include <sstream>
#include <boost/thread/mutex.hpp>


#include "../wxgui/ocvFrame.h"

using namespace std;

struct Object;
class ocvFrame;
class ocvProcessor
{
public:
	ocvProcessor();
	IplImage* processImage(IplImage* input);
	
	void init();
	void releaseMem();
	void edge(IplImage* edgeImg);
	void findContours(IplImage*srcImage, IplImage* edgeImg);
	CvScalar convertToReal(CvPoint *pixPoint);
	string getResult();
	bool findAxis(IplImage* src, CvSeq* cont, CvPoint* centre);
	int matchObject(Object* newO, Object* obj);
	void markObjects(IplImage* srcImg);
	void updateBrightCont(IplImage *grayImg);
	void outValue();
	virtual ~ocvProcessor();
public:
	IplImage *edgeImg, *tempImg, *cpyImg;
	IplImage *mapX, *mapY, *mapTemp;

	CvSeq *allContours, *resContours, *object;
	CvMemStorage *memStorage, *objMemStorage;
	CvBox2D box;

	int thresh, brightness, contrast;
	
private:
	string rslt;
	struct Object foundObject;
	int noObjects;
	CvMat *intrinsics, *distortion, *invCamera, *pixMat, *realCord;
	boost::mutex m_mutex;

};

#endif /*OCVPROCESSOR_H_*/
