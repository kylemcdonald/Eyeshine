#pragma once

#include "ofMain.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	
	ofVideoGrabber cam;
	vector<ofFbo*> fbos;
};
