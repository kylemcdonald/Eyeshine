#include "testApp.h"

void testApp::setup() {
	rleVideo.load("output.rlev");
}

void testApp::update() {
}

void testApp::draw() {
	ofBackground(0);
	
	ofSetColor(255);
	int curFrame = (int) ofGetFrameNum() % rleVideo.size();
	rleVideo[curFrame]->draw();
	
	ofSetColor(255);
	ofRect(0, 0, 100, 50);
	ofSetColor(255, 0, 0);
	ofDrawBitmapString("app " + ofToString((int) ofGetFrameRate()), 10, 20);
	ofDrawBitmapString("frame " + ofToString(curFrame), 10, 40);
}
