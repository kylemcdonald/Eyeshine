#pragma once

#include "ofMain.h"

class RlePoint2d {
public:
	int x, y;
};

class ofxRleImage {
	 friend ostream& operator<<(ostream& out, const ofxRleImage& img);
	 friend istream& operator>>(istream& in, ofxRleImage& img);
	
protected:
	unsigned int width, height;
	vector<unsigned int> data;
	vector<RlePoint2d> lines;
	
public:
	ofxRleImage();
	/* ~ofxRleImage(); */
	
	void load(ofImage& img);
	void update();
	void draw(int x = 0, int y = 0) const;
	unsigned int getWidth() const;
	unsigned int getHeight() const;
	
	void save(string filename) const;
	void load(string filename);
	
	static bool useDrawOffset;
	static bool useFastEncoding;
	
	void computeCentroid();
	ofPoint centroid;
};