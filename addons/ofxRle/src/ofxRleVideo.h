#pragma once

#include "ofxRleImage.h"

class ofxRleVideo {
	friend ostream& operator<<(ostream& out, const ofxRleVideo& video);
	friend istream& operator>>(istream& in, ofxRleVideo& video);
	
protected:
	vector<ofxRleImage*> frames;
	
public:
	~ofxRleVideo();
	void add(ofImage& img);
	int size() const;
	ofxRleImage* operator[](unsigned int i) const;
	void save(string filename) const;
	void load(string filename);
};