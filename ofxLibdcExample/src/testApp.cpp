#include "testApp.h"

void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	camera.setup();
}

void testApp::update() {
	if(camera.grabVideo(curFrame)) {
		curFrame.update();
	}
}

void testApp::draw() {
	curFrame.draw(0, 0);
}