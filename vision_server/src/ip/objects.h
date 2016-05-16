#ifndef OBJECTS_H_
#define OBJECTS_H_

#include <cv.h>

struct Object{
	char* name;
	int corners;
	CvPoint centre;
	CvPoint maxDist;
	double* length;
	CvRect bRect;
	double angle;
};

struct storeObject{
	char* name;
	CvPoint centre;
};



#endif /*OBJECTS_H_*/
