#pragma once

#include "ofMain.h"
#include "ofxLibdcPtGrey.h"
#include "RateTimer.h"
#include "ofxRleImage.h"
#include "ofxRleVideo.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
	ofxLibdcPtGrey camera;
	ofImage curFrame;
	ofImage thresholded;
	ofxRleImage rleImage;
	
	bool recording;
	ofxRleVideo rleVideo;
	
	RateTimer cameraFps;
	
	unsigned int camWidth, camHeight;
};