#pragma once

#include "ofMain.h"

class RlePoint2d {
public:
	float x, y;
};

class ofxRleImage {
	 friend ostream& operator<<(ostream& out, const ofxRleImage& img);
	 friend istream& operator>>(istream& in, ofxRleImage& img);
	
private:
	void decompress();
	
protected:
	unsigned int width, height;
	vector<unsigned int> data;
	vector<RlePoint2d> lines;
	
public:
	void compress(ofImage& img);
	void draw() const;
	unsigned int getWidth() const;
	unsigned int getHeight() const;
	
	void save(string filename) const;
	void load(string filename);
	
	static bool useDrawOffset;
	static bool useFastEncoding;
};