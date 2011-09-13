#pragma once

#include "ofMain.h"
#include "ofxLibdcPtGrey.h"
#include "RateTimer.h"
#include "ofxRleImage.h"
#include "ofxRleVideo.h"
#include "PupilFinder.h"
#include "ofxAutoControlPanel.h"
#include "DatumHistorian.h"

enum EyeshineMode {
	EYESHINE_DIAGNOSTIC,
	EYESHINE_PREVIEW,
	EYESHINE_DISPLAY	//this one is special and should always be last - set ofSetLogLevel to OF_SILENT to not receive any messages
};


class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void mousePressed( int x, int y, int button );
	
	void setupControlPanel();
	void setupCapture();
	void setupDisplay();
	
	void updateCapture();
	void updateDisplay();
	
	EyeshineMode currentEyeshineMode;
	long startDiagnosticTime;
	
	void drawEyeshineDiagnostic();
	void drawEyeshineDisplay();
	
	int  nAccessibleLibdcCameras;
	ofxLibdcPtGrey		camera;
	ofImage				currFrame;
	ofxCvGrayscaleImage cvCurrFrame;
	ofImage				thresholded;
	ofxRleImage			rleImage;
	
	bool				bRecording;
	ofxRleVideo			rleVideo;
	void saveVideo();
	
	RateTimer cameraFps;
	PupilFinder pupilFinder;
	ofxAutoControlPanel gui;
	
	unsigned int nativeWidth,	nativeHeight;
	unsigned int camWidth,		camHeight;
	unsigned int smallWidth,	smallHeight;
	
	float adapThreshBlockSize, adapThreshConstant, minContourArea, maxContourArea, circularityThresh;
	
	int currentPreviewVideoIndex; 
	void drawEyeshinePreview();
	void loadVideo (int which);
	
	//---------------------------------
	void updateWhetherOrNotWeAreRecording();
	
	ofxCvGrayscaleImage	cvSmallCurrFrame;
	ofxCvGrayscaleImage	cvSmallPrevFrame;
	ofxCvGrayscaleImage cvSmallFrameDiff;
	float amountOfMotion01;
	DatumHistorian *motionHistorian;
	long startRecordingTimeMs; 
	
	//---------------------------------
	char *handyString;
	
	ofImage	colorSourceImage;
	ofDirectory	dirLister;
	
	int nVideos;
	int maxNVideos;
	int maxStorableVideos; 
	int currentVideoID;
	ofxRleVideo **videos;
	
	int  nPreviousRecordings;
	int  nCurrentRecordings;
	int  totalFileCounter; 
	void loadRandomRlevVideo (bool bUseCurrentFilesToo);
	void huntForBlendFunc (int period, int defaultSid, int defaultDid);
	
	bool bEnableLineSmooth;
	bool bDrawInformation; 
	long drawFrameInformationRequestTime; 
	long drawInformationDuration;
	
	float hiliteR;
	float hiliteG;
	float hiliteB;

};