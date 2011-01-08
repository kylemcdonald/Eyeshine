#pragma once

#include "ofMain.h"
#include "ofxRleVideo.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	
	ofxRleVideo rleVideo;
};