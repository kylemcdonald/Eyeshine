#include "testApp.h"

void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	//ofSetVerticalSync(true);
	cam.initGrabber(640, 480);
	filter.setup(cam.getWidth(), cam.getHeight());
	
	ofSetCircleResolution(64);
	grid.setMode(OF_PRIMITIVE_LINES);
	for(int y = 0; y < ofGetHeight(); y += 16) {
		for(int x = 0; x < ofGetWidth(); x += 16) {
			grid.addVertex(ofVec2f(x, 0));
			grid.addVertex(ofVec2f(x, ofGetHeight()));
			grid.addVertex(ofVec2f(0, y));
			grid.addVertex(ofVec2f(ofGetWidth(), y));
		}
	}
}

void testApp::update() {	
	cam.update();
	if(cam.isFrameNew()) {
	}
}

void testApp::draw() {
	ofBackground(0);
	
	filter.begin();
	ofSetColor(255);
	cam.draw(0, 0);
	ofCircle(mouseX, mouseY, 32);
	grid.draw();
	filter.end();
	
	filter.draw();
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
}

void testApp::keyPressed(int key) {
	if(key == ' ') {
		filter.reload();
	}
}