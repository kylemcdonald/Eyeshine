#pragma once

#include "ofMain.h"
#include "ofxBlur.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	
	ofVideoGrabber cam;
	ofxBlur blur;
};
