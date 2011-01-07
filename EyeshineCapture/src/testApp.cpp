#include "testApp.h"

void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	camera.setFormat7(true);
	camera.setSize(1024, 4);
	camera.setup();
	camera.setMaxFramerate();
}

void testApp::update() {
	if(camera.grabVideo(curFrame)) {
		curFrame.update();
		cameraFps.tick();
	}
}

void testApp::draw() {
	ofSetColor(255);
	curFrame.draw(0, 0);
	
	ofRect(0, 0, 100, 50);
	ofSetColor(255, 0, 0);
	ofDrawBitmapString("app " + ofToString((int) ofGetFrameRate()), 10, 20);
	ofDrawBitmapString("cam " + ofToString((int) cameraFps.getFramerate()), 10, 40);
}