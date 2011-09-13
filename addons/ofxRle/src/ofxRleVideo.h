#pragma once

#include "ofxRleImage.h"

class ofxRleVideo {
	friend ostream& operator<<(ostream& out, const ofxRleVideo& video);
	friend istream& operator>>(istream& in, ofxRleVideo& video);
	
protected:
	vector<ofxRleImage*> frames;
	
public:
	
	ofxRleVideo(); 
	~ofxRleVideo();
	
	void add(ofImage& img);
	int size() const;
	ofxRleImage* operator[](unsigned int i) const;
	void save(string filename) const;
	void load(string filename);
	void clear();
	
	int  currentFrame;
	int  getNFrames();
	void update();
	void draw();
	
	void setDrawOffset (float ox, float oy);
	float drawOffsetX;
	float drawOffsetY;
	
	//-------------------------------------
	ofPoint color;
	char *myFilename; 
};

