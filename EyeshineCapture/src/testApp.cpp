#include "testApp.h"

void testApp::setup() {
	recording = false;
	
	bool useBinning = true;
	
	nativeWidth = 2448;
	nativeHeight = 2048;
	
	if(useBinning) {
		nativeWidth /= 2;
		nativeHeight /= 2;
	}
	
	camWidth = 1224;
	camHeight = 1024;
	
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	// need to add manual everything to this camera, no auto exposure etc
	camera.setFormat7(true, useBinning ? 1 : 0); // 0 means no binning, 1 means 2x2 binning
	camera.setSize(camWidth, camHeight);
	camera.setPosition((nativeWidth - camWidth) / 2, (nativeHeight - camHeight) / 2);
	camera.setPosition(0, 0);
	camera.setup();
	camera.setMaxFramerate();
	
	ofxRleImage::useFastEncoding = true;
	
	thresholded.allocate(camWidth, camHeight, OF_IMAGE_GRAYSCALE);
	curFrameCv.allocate(camWidth, camHeight);
	pupilFinder.setup(camWidth, camHeight);
	
	setupControlPanel();
}

void testApp::setupControlPanel(){
	gui.setup("PupilFinding", 20, 20, 300, 500);
	gui.addPanel(" Main Controls", 1);
	
	gui.setWhichPanel(0);
	gui.setWhichColumn(0);
	gui.addToggle("Naive", "NAIVE", false);
	gui.addToggle("Draw Offset", "DRAW_OFFSET", true);
	gui.addSlider("AdapThresh BlockSize", "ADAPTHRESH_BLOCKSIZE", 8, 1, 25, true);	
	gui.addSlider("AdapThresh Constant", "ADAPTHRESH_CONSTANT", 45, 0, 100, true);
	gui.addSlider("MinContourArea", "MIN_CONTOUR_AREA", 1, 0,  50, true);
	gui.addSlider("MaxContourArea", "MAX_CONTOUR_AREA", 300, 0, 1000, true);
	gui.addSlider("CircularityThresh", "CIRCULARITY_THRESH", 0.8, 0, 1.0, false);
	gui.loadSettings("controlPanelSettings.xml");
}

void testApp::update() {
	if(camera.grabVideo(curFrame)) {
		curFrame.update();
		cameraFps.tick();
		
		curFrameCv.setFromPixels(curFrame.getPixels(), camWidth, camHeight);
		
		if(gui.getValueB("NAIVE")) {
			int threshold = (mouseX * 255) / ofGetWidth();
			curFrameCv.threshold(threshold);
			thresholded.setFromPixels(curFrameCv.getPixels(), camWidth, camHeight, OF_IMAGE_GRAYSCALE);
		} else {
			pupilFinder.updateBinaryPupils (curFrameCv,
											gui.getValueI("ADAPTHRESH_BLOCKSIZE", 0), 
											gui.getValueI("ADAPTHRESH_CONSTANT", 0),
											gui.getValueI("MIN_CONTOUR_AREA", 0),
											gui.getValueI("MAX_CONTOUR_AREA", 0),
											gui.getValueF("CIRCULARITY_THRESH", 0));
			thresholded.setFromPixels(pupilFinder.ofxCv8u_binaryPupils.getPixels(), camWidth, camHeight, OF_IMAGE_GRAYSCALE);
		}
		
		rleImage.load(thresholded);
		rleImage.update();
		
		if(recording) {
			rleVideo.add(thresholded);
		}
	}
}

void testApp::draw() {
	ofBackground(0);
	
	ofSetColor(255);
	curFrame.draw(0, 0);
	
	if(recording) {
		ofSetColor(255, 0, 0);
	} else {
		ofSetColor(0, 0, 255);
	}
	ofxRleImage::useDrawOffset = gui.getValueB("DRAW_OFFSET");
	rleImage.draw(0, 0);
	
	ofSetColor(255);
	ofRect(0, 0, 100, 50);
	ofSetColor(255, 0, 0);
	ofDrawBitmapString("app " + ofToString((int) ofGetFrameRate()), 10, 20);
	ofDrawBitmapString("cam " + ofToString((int) cameraFps.getFramerate()), 10, 40);
}

void testApp::keyPressed(int key) {
	if(key == 'r') {
		recording = !recording;
		
		if(recording) {
			cout << "Starting video recording." << endl;
		} else {
			cout << "Stopping recording, saving video with " << rleVideo.size() << " frames." << endl;
			rleVideo.save("output.rlev");
		}
	}
}