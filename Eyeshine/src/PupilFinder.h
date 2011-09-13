#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

class PupilFinder {
public:
	int imgW, imgH;
	ofxCvGrayscaleImage ofxCv8u_temp1;
	ofxCvGrayscaleImage ofxCv8u_finalGray;
	ofxCvGrayscaleImage ofxCv8u_binaryPupils;
	
	ofxCvContourFinder contourFinder;
	int MAX_N_CONTOUR_POINTS;
	CvPoint **cvpts;
	int *ncvpts;
	IplImage* cvImgTemp;
	
	void setup (int w, int h);
	void updateBinaryPupils (ofxCvGrayscaleImage &ofxCv8u_raw,
							 int adapThreshBlockSize,
							 int adapThreshConstant,
							 int minContourArea,
							 int maxContourArea,
							 float circularityThreshold);
	void drawBinaryPupils (float x, float y, float w, float h);
};
