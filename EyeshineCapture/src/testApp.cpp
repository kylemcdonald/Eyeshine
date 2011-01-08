#include "testApp.h"

void testApp::setup() {
	recording = false;
	
	camWidth = 752;
	camHeight = 480;
	
	ofSetLogLevel(OF_LOG_VERBOSE);
	camera.setFormat7(true);
	camera.setSize(camWidth, camHeight);
	camera.setup();
	camera.setMaxFramerate();
	
	ofxRleImage::useFastEncoding = true;
	
	thresholded.allocate(camWidth, camHeight, OF_IMAGE_GRAYSCALE);
}

void testApp::update() {
	if(camera.grabVideo(curFrame)) {
		curFrame.update();
		cameraFps.tick();
		
		int threshold = (mouseX * 255) / ofGetWidth();
		unsigned char* cpix = curFrame.getPixels();
		unsigned char* tpix = thresholded.getPixels();
		int n = curFrame.getWidth() * curFrame.getHeight();
		for(int i = 0; i < n; i++) {
			tpix[i] = cpix[i] > threshold ? 255 : 0;
		}
		thresholded.update();
		
		if(recording) {
			rleVideo.add(thresholded);
		}
		
		rleImage.load(thresholded);
		rleImage.update();
	}
}

void testApp::draw() {
	ofBackground(0);
	
	ofSetColor(255);
	curFrame.draw(0, 0);
	thresholded.draw(0, 480);
	
	if(recording) {
		ofSetColor(255, 0, 0);
	}
	rleImage.draw(0, 480 + 480);
	
	ofSetColor(255);
	ofRect(0, 0, 100, 50);
	ofSetColor(255, 0, 0);
	ofDrawBitmapString("app " + ofToString((int) ofGetFrameRate()), 10, 20);
	ofDrawBitmapString("cam " + ofToString((int) cameraFps.getFramerate()), 10, 40);
}

void testApp::keyPressed(int key) {
	if(key == ' ') {
		recording = !recording;
		
		if(recording) {
			cout << "Starting video recording." << endl;
		} else {
			cout << "Stopping recording, saving video with " << rleVideo.size() << " frames." << endl;
			rleVideo.save("output.rlev");
		}
	}
}