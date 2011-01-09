#pragma once

#include "ofMain.h"
#include "ofxLibdcPtGrey.h"
#include "RateTimer.h"
#include "ofxRleImage.h"
#include "ofxRleVideo.h"
#include "PupilFinder.h"
#include "ofxAutoControlPanel.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
	ofxLibdcPtGrey camera;
	ofImage curFrame;
	ofxCvGrayscaleImage curFrameCv;
	ofImage thresholded;
	ofxRleImage rleImage;
	
	bool recording;
	ofxRleVideo rleVideo;
	
	RateTimer cameraFps;
	
	PupilFinder pupilFinder;
	
	unsigned int nativeWidth, nativeHeight;
	unsigned int camWidth, camHeight;
	
	float adapThreshBlockSize, adapThreshConstant, minContourArea, maxContourArea, circularityThresh;
	
	ofxAutoControlPanel gui;
	void setupControlPanel();
};