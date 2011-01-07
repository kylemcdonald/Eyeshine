#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxRleImage.h"

class testApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();
	
	ofxRleImage rleImage;
	
	ofImage eyeshineDemoImage;
};