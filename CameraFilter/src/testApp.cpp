#include "testApp.h"

void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	//ofSetVerticalSync(true);
	
	filter.setup(1280, 1024);
	debug = false;
	
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
	filter.begin();
	ofClear(0);
	ofCircle(mouseX, mouseY, 32);
	//grid.draw();
	filter.end();
}

void testApp::draw() {
	ofBackground(0);
	
	ofSetColor(255);
	filter.draw();

	ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
}

void testApp::keyPressed(int key) {
	if(key == 'd') {
		debug = !debug;
	}
}