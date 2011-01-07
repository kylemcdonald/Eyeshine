#include "testApp.h"

void testApp::setup(){
	eyeshineDemoImage.loadImage("eyeshine_demo.png");
	rleImage.setup(eyeshineDemoImage);
}

void testApp::update(){	
}

void testApp::draw(){
	ofBackground(100, 100, 100);
	
	ofSetColor(255);
	eyeshineDemoImage.draw(0, 0);
	
	ofSetColor(255, 0, 0);
	rleImage.draw(); 

	ofSetColor(255);
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()) + " fps", 10, 20);
}
