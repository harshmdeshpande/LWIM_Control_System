#define HEIGHT 288	  //Frame height from CamCapture
#define WIDTH 352	// Frame width from CamCapture
#define Z 0.75
#define OBJ_COUNT 1   // Limit for no of objects to be found
#include "ocvProcessor.h"
#include <iostream>
#include <sstream>
#include <string.h>

using namespace std;

const int clientActive = 1;

const double xScale = -1.471, xOffset = 264.5; //Xreal = -1.471*Xpix + 264.5
const double yScale = 1.499, yOffset = 121.0; //Yreal =  1.499*Ypix + 121.0
const double botXdist = 0;
const double botYdist = 0;

uchar lut[256]; //Look up Table
CvMat* lut_mat;

vector<Object> objTemplate;
vector<CvPoint> c, coord1;

int _X = WIDTH, _Y = HEIGHT;
double TOLERANCE = 1;
bool intnPt = false;

Object initObject(char* name, int corners, double* len) {
	Object obj = { name, corners, cvPoint(0, 0), cvPoint(0, 0), len, cvRect(0,
			0, 0, 0) };
	return obj;
}

ocvProcessor::ocvProcessor() {
	lut_mat = cvCreateMatHeader(1, 256, CV_8UC1); // LookUpTable for brightness & contrast adjustment
	cvSetData(lut_mat, lut, 0);

	//database of object geometry
	double* len1 = (double*) malloc(4 * sizeof(double));
	len1[0] = 54, len1[1] = 54, len1[2] = 54, len1[3] = 54;
	objTemplate.push_back(initObject("Square", 4, len1));

	double* len2 = (double*) malloc(6 * sizeof(double));
	len2[0] = 17, len2[1] = 46, len2[2] = 50, len2[3] = 54;
	len2[4] = 58, len2[5] = 64;
	objTemplate.push_back(initObject("L", 6, len2));

	double* len3 = (double*) malloc(8 * sizeof(double));
	len3[0] = 12, len3[1] = 13, len3[2] = 39, len3[3] = 40, len3[4] = 46, len3[5]
			= 47, len3[6] = 49, len3[7] = 50;
	objTemplate.push_back(initObject("T", 8, len3));

	double* len4 = (double*) malloc(8 * sizeof(double));
	len4[0] = 40, len4[1] = 40, len4[2] = 40, len4[3] = 40, len4[4] = 40, len4[5]
			= 40, len4[6] = 40, len4[7] = 40;
	objTemplate.push_back(initObject("Circle", 8, len4));

	thresh = 50;
	brightness = 50;
	contrast = 100;

	//Camera Params

	invCamera = cvCreateMat(3, 3, CV_32FC1);
	realCord = cvCreateMat(3, 1, CV_32FC1);
	pixMat = cvCreateMat(3, 1, CV_32FC1);
	intrinsics = (CvMat*) cvLoad("Intrinsics.xml");

	distortion = (CvMat*) cvLoad("Distortion.xml");
	mapX = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_32F, 1);
	mapY = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_32F, 1);


	cvInvert(intrinsics, invCamera, CV_LU);
	cvInitUndistortMap(intrinsics, distortion, mapX, mapY);

}

CvScalar ocvProcessor::convertToReal(CvPoint* pixPoint) {

	CvScalar pixVal, realPt;
	pixVal.val[0] = (pixPoint->x) * Z;
	pixVal.val[1] = (pixPoint->y) * Z;
	pixVal.val[2] = Z;

	for (int i = 0; i < 3; i++) {
		cvSetReal1D(pixMat, i, pixVal.val[i]);
	}

	cvMatMul(invCamera,pixMat,realCord);

	for (int i = 0; i < 3; i++) {
		realPt.val[i] = cvGetReal1D(realCord, i);
	}

	//cout << realPt.val[0] << ", " << realPt.val[1] << ", " << realPt.val[2] << endl;
	return realPt;

}
void ocvProcessor::init() {

	memStorage = cvCreateMemStorage(0);
	objMemStorage = cvCreateMemStorage(0);
	object = cvCreateSeq(CV_SEQ_ELTYPE_GENERIC, sizeof(CvSeq), sizeof(Object),
			objMemStorage);

	edgeImg = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 1);
	tempImg = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 3);

	mapTemp = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 3);

	noObjects = 0;

}

string ocvProcessor::getResult() {
	// Use a scoped lock since the variable can be read and written at the same time
	boost::mutex::scoped_lock lock(m_mutex);
	return this->rslt;
}

void ocvProcessor::releaseMem() {

	allContours = 0;
	object = 0;
	cvReleaseMemStorage(&objMemStorage);
	cvReleaseMemStorage(&memStorage);
	cvReleaseImage(&edgeImg);
	cvReleaseImage(&mapTemp);
}

void ocvProcessor::outValue() {
	ostringstream result;
	CvScalar realPt;
	Object* obj = (Object*) cvGetSeqElem(object, 0);

	if (clientActive) {
		if (object->total < 1) {
			result << "Notfound";
		} else {
			for (int i = 0; i < object->total; i++) {
				//CvPoint2D32f real = convertToReal(obj[i].centre);
				realPt = convertToReal(&obj[i].centre);
				result << "#" << obj[i].name << " " << realPt.val[0] << " "
						<< realPt.val[1];
			}
		}
		boost::mutex::scoped_lock lock(m_mutex);
		this->rslt = result.str();
	} else {
		//		if (object->total < 1) {
		//			cout << "Notfound" << endl;
		//		} else {
		//			for (int i = 0; i < object->total; i++) {
		////				CvPoint2D32f real = convertToReal(obj[i].centre);
		////				cout << obj[i].name <<" " << obj[i].centre.x << " " << obj[i].centre.y<<endl;
		////				cout << obj[i].name <<" " << real.x << " " << real.y<<endl;
		//			}
		//		}

	}

}

IplImage* ocvProcessor::processImage(IplImage* input) {

	init();

	//TODO:
	cvRemap(input, mapTemp, mapX, mapY);
	cvCopy(mapTemp, input);

	cvSmooth(input, input, CV_GAUSSIAN, 3, 5);
	cvCvtColor(input, edgeImg, CV_BGR2GRAY);

	updateBrightCont(edgeImg);
	edge(edgeImg);
	findContours(input, edgeImg);
	markObjects(input);
	outValue();

	//	Always to be converted to color to display as bitmap Not able to display grayscale in wxWidgets.
	//	cvCvtColor(edgeImg,input,CV_GRAY2RGB);

	cvCvtColor(edgeImg, tempImg, CV_GRAY2RGB);
	releaseMem();

	return input;
}

void ocvProcessor::updateBrightCont(IplImage* edgeImg) {
	int _brightness = brightness - 100;
	int _contrast = contrast - 100;

	if (contrast > 0) {
		double delta = 127 * _contrast / 100;
		double a = 255 / (255 - delta * 2);
		double b = a * (_brightness - delta);
		for (int i = 0; i < 256; i++) {
			int v = cvRound(a * i + b);
			if (v < 0)
				v = 0;
			if (v > 255)
				v = 255;
			lut[i] = (uchar) v;
		}
	} else {
		double delta = -128 * _contrast / 100;
		double a = (256 - delta * 2) / 255;
		double b = a * _brightness + delta;
		for (int i = 0; i < 256; i++) {
			int v = cvRound(a * i + b);
			if (v < 0)
				v = 0;
			if (v > 255)
				v = 255;
			lut[i] = (uchar) v;
		}
	}
	cvLUT(edgeImg, edgeImg, lut_mat);
}

void ocvProcessor::edge(IplImage* edgeImg) {

	//	cvThreshold(edgeImg,edgeImg,thresh,255,CV_THRESH_BINARY_INV);
	cvThreshold(edgeImg, edgeImg, thresh, 255, CV_THRESH_BINARY);

	cvCanny(edgeImg, edgeImg, 1, 3, 3);
}

void markRecognizedObjects(IplImage* srcImg, CvRect r) {
	//cout << "MARKING FOUND OBJECTS" << endl;
	cvRectangle(srcImg, cvPoint(r.x, r.y), cvPoint(r.x + r.width, r.y
			+ r.height), CV_RGB(255, 255, 255), 3, 8, 0);
}

void addText(IplImage* srcImg, CvPoint p, char* a) {

	//cout << "ADDING TEXT" << endl;

	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1.2, 1.2, 0, 2);
	cvPutText(srcImg, a, p, &font, CV_RGB(0, 200, 0));
}

void ocvProcessor::markObjects(IplImage* srcImg) {
	Object* obj = (Object*) cvGetSeqElem(object, 0);
	//cout << "MARKING OBJECTS" << endl;
	for (int i = 0; i < object->total; i++) {
		markRecognizedObjects(srcImg, obj[i].bRect);
		addText(srcImg, obj[i].centre, obj[i].name);

	}
}

//insertion sort (sort length of sides of polynomial
void sort(Object* a, int array_size) {
	int i, j;
	double indexL;
	for (i = 1; i < array_size; ++i) {
		indexL = a->length[i];
		for (j = i; j > 0 && a->length[j - 1] > indexL; j--) {
			a->length[j] = a->length[j - 1];
		}
		a->length[j] = indexL;
	}
}

double findDist(CvPoint* pt1, CvPoint* pt2) { // Distance Formula
	double dist, temp1, temp2; // P1(x,y) and P2(x,y)

	double p1x = (double) pt1->x;
	double p1y = (double) pt1->y;
	double p2x = (double) pt2->x;
	double p2y = (double) pt2->y;

	temp1 = pow(fabs(p1x - p2x), 2);
	temp2 = pow(fabs(p1y - p2y), 2);
	dist = fabs(sqrt(temp1 + temp2));
	return dist;
}

void ocvProcessor::findContours(IplImage *srcImg, IplImage* edgeImg) {

	cvFindContours(edgeImg, memStorage, &allContours);

	if (allContours != NULL) {
		for (; allContours; allContours = allContours->h_next) {
			//			if (noObjects == OBJ_COUNT){  					// Use to limit the number 
			//				break;									    // of objects to be found
			//			}
			//			noObjects++;
			//			
			double area = cvContourArea(allContours);
			if (area < 1500 || area > 10000) {
				continue;
			}

			resContours = cvApproxPoly(allContours, sizeof(CvContour),
					memStorage, CV_POLY_APPROX_DP,
					cvContourPerimeter(allContours) * 0.02, 0);
			//			cvDrawContours(srcImg,resContours,cvScalarAll(255),cvScalarAll(255),100,2,CV_AA);

			if (resContours != NULL) {
				CvPoint* temp = (CvPoint*) cvGetSeqElem(resContours, 0);

				CvPoint avg = cvPoint(0, 0);

				for (int i = 0; i < resContours->total; i++) {
					avg.x += temp[i].x;
					avg.y += temp[i].y;
				}
				avg.x = avg.x / resContours->total;
				avg.y = avg.y / resContours->total;

				//store new object in a structure

				foundObject.corners = resContours->total;
				foundObject.centre = cvPoint(avg.x, avg.y);
				foundObject.length = new double[resContours->total];

				int j = 0;
				for (int i = 0; i < resContours->total; i++) {
					//find features of the new object (length of origin vector & 
					//angles between successive origin vector)
					j = i == 0 ? resContours->total - 1 : i - 1;

					foundObject.length[i] = findDist(&avg, &temp[i]);
				}
				if (findAxis(srcImg, resContours, &foundObject.centre)) {

					c = coord1;
					cvLine(edgeImg, c.at(0), c.at(1), CV_RGB(255, 255, 255), 2,
							8, 0);
					cvLine(edgeImg, c.at(2), c.at(3), CV_RGB(255, 255, 255), 2,
							8, 0);
				}

				sort(&foundObject, foundObject.corners);

				//find a match for the newly detected object with an existing object
				for (unsigned int j = 0; j < objTemplate.size(); j++) {

					Object* obj = &objTemplate.at(j);
					if (matchObject(&foundObject, obj)) {
						box = cvMinAreaRect2(resContours, memStorage);
						cvEllipseBox(srcImg, box, cvScalar(255, 0, 0), 2, CV_AA);
						foundObject.bRect = cvBoundingRect(resContours, 1);
						foundObject.maxDist = temp[foundObject.corners - 1];
						foundObject.angle = box.angle;
						cvSeqPush(object, &foundObject);
						break;
					}

				}
			}
		}
	}
}
void getEndPoints(vector<CvPoint>* p) {
	unsigned int i, j;
	int indexX, indexY;

	for (i = 1; i < p->size(); i++) {

		indexX = p->at(i).x;
		indexY = p->at(i).y;

		for (j = i; j > 0 && p->at(j - 1).x > indexX; j--) {
			p->at(j).x = p->at(j - 1).x;
			p->at(j).y = p->at(j - 1).y;

		}
		p->at(j).x = indexX;
		p->at(j).y = indexY;
	}
	p->at(1) = p->back();
	p->resize(2);
}

CvPoint findIntersection(CvPoint *p1, CvPoint* p2, CvPoint *p3, CvPoint* p4) {

	CvPoint p = cvPoint(0, 0);
	intnPt = false;

	double den = (p4->y - p3->y) * (p2->x - p1->x) - (p4->x - p3->x) * (p2->y
			- p1->y);

	if (den != 0) {
		double num1 = (p4->x - p3->x) * (p1->y - p3->y) - (p4->y - p3->y)
				* (p1->x - p3->x);
		double num2 = (p2->x - p1->x) * (p1->y - p3->y) - (p2->y - p1->y)
				* (p1->x - p3->x);
		double ua = num1 / den;
		double ub = num2 / den;

		if (ua < 1 && ua > 0 && ub < 1 && ub > 0) {
			p.x = (int) (p1->x + ua * (p2->x - p1->x));
			p.y = (int) (p1->y + ua * (p2->y - p1->y));
			intnPt = true;
		} else
			intnPt = false;
	}

	return p;
}

bool ocvProcessor::findAxis(IplImage* src, CvSeq* cont, CvPoint* centre) { //src Image to draw axis

	CvMoments mom;
	cvContourMoments(cont, &mom);
	double m00 = cvGetCentralMoment(&mom, 0, 0);
	double m11 = cvGetCentralMoment(&mom, 1, 1) / m00;
	double m20 = cvGetCentralMoment(&mom, 2, 0) / m00;
	double m02 = cvGetCentralMoment(&mom, 0, 2) / m00;

	double tmp = 2 * m11 / (m20 - m02);
	double alpha = atan(tmp) / 2;

	//	double degree = alpha*(180/3.14159265);
	//	cout << "ANGLE  4m Moments: " << degree << endl;

	double slope = tan(alpha);

	double* c = new double[4];
	c[0] = centre->y - (slope * centre->x);
	c[1] = centre->y + (centre->x / slope);

	c[2] = (_X * slope) + c[0];
	c[3] = (-_X / slope) + c[1];

	cvLine(src, cvPoint(0, (int) c[0]), cvPoint(_X, (int) c[2]), cvScalar(255,
			255, 255), 1, CV_AA);
	cvLine(src, cvPoint(0, (int) c[1]), cvPoint(_X, (int) c[3]), cvScalar(255,
			255, 255), 1, CV_AA);

	CvPoint* p = (CvPoint*) cvGetSeqElem(cont, 0);
	vector<CvPoint> coord2;

	int j = 0;
	for (int i = 0; i < cont->total; i++) {
		j = i == 0 ? cont->total - 1 : i - 1;
		CvPoint p1 = cvPoint(0, (int) c[0]);
		CvPoint p2 = cvPoint(_X, (int) c[2]);
		CvPoint intn = findIntersection(&p1, &p2, &p[i], &p[j]);

		if (intn.x != 0 && intn.y != 0) {
			coord1.push_back(intn);
		}
	}

	if (intnPt == false) {
		return false;
	}

	getEndPoints(&coord1);
	j = 0;
	for (int i = 0; i < cont->total; i++) {
		j = i == 0 ? cont->total - 1 : i - 1;
		CvPoint p1 = cvPoint(0, (int) c[1]);
		CvPoint p2 = cvPoint(_X, (int) c[3]);
		CvPoint intn = findIntersection(&p1, &p2, &p[i], &p[j]);

		if (intn.x != 0 && intn.y != 0)
			coord2.push_back(intn);
	}

	if (intnPt == false) {
		return false;
	}

	getEndPoints(&coord2);
	coord1.push_back(coord2.at(0));
	coord1.push_back(coord2.at(1));
	return true;
}

//match a new object with object which is stored in database
//by normalizing the lengths (scale invariant).
//the lengths are matched in a sorted manner

int ocvProcessor::matchObject(Object* newObject, Object* object) {

	if (newObject->corners == object->corners)
		for (int i = 0; i < object->corners; i++) {
			double obj = object->length[i] / object->length[0];
			double newO = newObject->length[i] / newObject->length[0];
			if (obj - TOLERANCE <= newO && obj + TOLERANCE >= newO) {
			} else
				return 0;
		}
	else
		return 0;
	newObject->name = object->name;
	return 1;
}

ocvProcessor::~ocvProcessor() {

}
