#pragma once

#include "ofMain.h"

#include "ofxCameraFilter.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
	ofxCameraFilter filter;
	ofMesh grid;
	bool debug;
};
