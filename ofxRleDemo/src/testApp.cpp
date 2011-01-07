#include "testApp.h"

void testApp::setup(){
	eyeshineDemoImage.loadImage("eyeshine_demo.png");
	rleImage.compress(eyeshineDemoImage);
	rleImage.save("output.rle");
	
	loaded.load("output.rle");
	cout << "loaded has " << loaded.getWidth() << " " << loaded.getHeight() << endl;
}

void testApp::update(){	
}

void testApp::draw(){
	ofBackground(100, 100, 100);
	
	ofSetColor(255);
	eyeshineDemoImage.draw(0, 0);
	
	ofSetColor(255, 0, 0);
	loaded.draw(); 

	ofSetColor(255);
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()) + " fps", 10, 20);
}
