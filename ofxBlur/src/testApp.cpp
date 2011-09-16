#include "testApp.h"

void testApp::setup() {
	cam.initGrabber(1280, 720);
	
	blur.setup(ofGetWidth(), ofGetHeight());
}

void testApp::update() {	
	cam.update();
	blur.setScale(ofMap(mouseX, 0, ofGetWidth(), 0, 4));
}

void testApp::draw() {
	ofBackground(0);
	
	blur.begin();
	ofSetColor(255);
	cam.draw(0, 0);
	ofSetCircleResolution(64);
	ofCircle(mouseX, mouseY, 100);
	blur.end();
	
	timer.tick();
	ofDrawBitmapString(ofToString((int) timer.getPeriodMilliseconds()) + " ms", 10, 20);
}
