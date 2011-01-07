#pragma once

#include "ofMain.h"

class RlePoint2d {
public:
	float x, y;
};

class ofxRleImage {
protected:
	unsigned int width, height;
	vector<int> data;
	vector<RlePoint2d> lines;
	
public:	
	void setup(ofImage& img);
	void draw();
	
	static bool useDrawOffset;
};