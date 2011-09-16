#pragma once

#include "ofMain.h"

class ofxBlur {
protected:
	vector<ofFbo> ping, pong;
	ofShader blurShader, combineShader;
	
	int radius;
	int reductions;
	float reductionFactor;
	float scale;
public:
	ofxBlur();

	// change reductions to be 0 based
	// change ofFbo* to be ofFbo for reallocation
	// change reduction name to multipass
	// add variance setting
	void setup(int width, int height, int radius = 32, int reductions = 1, float reductionFactor = .5);
	
	void setScale(float scale);
	
	void begin();
	void end(bool autoDraw = true);
	void draw();
};