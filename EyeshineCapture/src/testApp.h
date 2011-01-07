#pragma once

#include "ofMain.h"
#include "ofxLibdcPtGrey.h"
#include "RateTimer.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	
	ofxLibdcPtGrey camera;
	ofImage curFrame;
	
	RateTimer cameraFps;
};