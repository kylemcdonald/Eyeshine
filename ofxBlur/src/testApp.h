#pragma once

#include "ofMain.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	
	void generateOffsets();
	
	ofVideoGrabber cam;
	vector<ofFbo*> ping, pong;
	
	vector<ofVec2f> hoffsets, voffsets;
	vector<float> coefficients;
	
	ofShader blurShader, combineShader;
	float blurTime;
	
	int reductions, level;
	float reductionFactor;
};
