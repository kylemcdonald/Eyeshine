#include "testApp.h"

//=========================================================================
void testApp::setup() {
	cameraFilter.setup(1280, 1024);//ofGetWidth(), ofGetHeight());
	cameraFilter.setAberrationAmount(0.03);
	cameraFilter.setBlurScale(0.8);
	cameraFilter.setDistortion(0.04);
	cameraFilter.setNoiseAmount(0.05);
	//cameraFilter.setVignetteSize(1);
	cameraFilter.setBrightness(4 * 8);
	
	handyString = new char[128];
	currentEyeshineMode = EYESHINE_DIAGNOSTIC;
	startDiagnosticTime = ofGetElapsedTimeMillis();
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	bDrawInformation = true;
	drawFrameInformationRequestTime = 0;
	drawInformationDuration = 60000;
	currentPreviewVideoIndex = 0;
	
	setupControlPanel();
	setupCapture();
	setupDisplay();
}

//=========================================================================
void testApp::setupCapture(){
	
	nAccessibleLibdcCameras = ofxLibdc::getCameraCount();
	
	
	bRecording = false;
	bool useBinning = true;
	nativeWidth  = 2448;
	nativeHeight = 2048;
	if(useBinning) {
		nativeWidth  /= 2;
		nativeHeight /= 2;
	}
	camWidth  = 1224;
	camHeight = 1024;
	
	if (nAccessibleLibdcCameras > 0){
		// need to add manual everything to this camera; no auto exposure etc
		camera.setFormat7(true, useBinning ? 1 : 0); // 0 means no binning, 1 means 2x2 binning
		camera.setSize(camWidth, camHeight);
		camera.setPosition((nativeWidth - camWidth) / 2, (nativeHeight - camHeight) / 2);
		camera.setPosition(0, 0);
		camera.setup();
		camera.setMaxFramerate();
	}
	
	ofxRleImage::useFastEncoding = true;
	
	thresholded.allocate(camWidth, camHeight, OF_IMAGE_GRAYSCALE);
	cvCurrFrame.allocate(camWidth, camHeight);
	pupilFinder.setup(camWidth, camHeight);
	
	smallWidth  = 320;
	smallHeight	= 240;
	cvSmallCurrFrame.allocate(smallWidth, smallHeight);
	cvSmallPrevFrame.allocate(smallWidth, smallHeight);
	cvSmallFrameDiff.allocate(smallWidth, smallHeight);
	amountOfMotion01 = 0;
	
	motionHistorian = new DatumHistorian();
	startRecordingTimeMs = 0; 
}

//=========================================================================
void testApp::setupDisplay(){
	
	ofSetVerticalSync(true);
	ofSetFrameRate(30);//24
	bEnableLineSmooth = true;
	
	colorSourceImage.loadImage("images/turner.jpg");
	// William Turner. Landscape with a River and a Bay in the Background. 1845
	
	totalFileCounter    = 0;
	nPreviousRecordings = 0;
	nCurrentRecordings  = 0;
	
	dirLister.allowExt("rlev");
	ofSetLogLevel(OF_LOG_WARNING);
	
	string searchDirectory = "recordings-previous/"; 
	nPreviousRecordings = dirLister.listDir(searchDirectory); 
	printf("# of Rlev files in data directory = %d\n", nPreviousRecordings); 
	bool bPrintFilenames = false;
	if (bPrintFilenames){
		for (int i = 0; i < nPreviousRecordings; i++){
			const char *fname = (dirLister.getName(i)).c_str();
			printf("  Filename is: %s\n",  fname);
		}
	}
	totalFileCounter += nPreviousRecordings;
	
	// Create array of ofxRleVideo's
	currentVideoID    = 0;
	maxStorableVideos = 60; 
	maxNVideos        = 25;
	nVideos           = 1;
	videos = new ofxRleVideo*[maxStorableVideos];
	for (int i=0; i<maxStorableVideos; i++){
		videos[i] = new ofxRleVideo();
		videos[i]->load("recordings-previous/output-00000.rlev");
	}
	
	if(gui.getValueB("LOAD_ALL_AT_START")) {
		for (int i=0; i<maxStorableVideos; i++){
			loadRandomRlevVideo(false);
		}
	}
	
	hiliteR = hiliteG = hiliteB = 255;
	
}


//=========================================================================
void testApp::setupControlPanel(){
	gui.setup("EyeShine", 20, 70, 300, 500);
	gui.addPanel(" Capture Controls", 1);
	gui.addPanel(" Display Controls", 1);
	gui.addPanel(" Detection Controls", 1);	
	
	// Capture controls
	gui.setWhichPanel(0);
	gui.setWhichColumn(0);
	gui.addToggle("Naive", "NAIVE", false);
	gui.addToggle("Draw Offset", "DRAW_OFFSET", true);
	gui.addSlider("AdapThresh BlockSize", "ADAPTHRESH_BLOCKSIZE", 10, 1, 25, true);	
	gui.addSlider("AdapThresh Constant", "ADAPTHRESH_CONSTANT", 30, 0, 100, true);
	gui.addSlider("MinContourArea", "MIN_CONTOUR_AREA", 8, 0,  50, true);
	gui.addSlider("MaxContourArea", "MAX_CONTOUR_AREA", 700, 0, 1000, true);
	gui.addSlider("CircularityThresh", "CIRCULARITY_THRESH", 0.5, 0, 1.0, false);
	
	// Display controls
	gui.setWhichPanel(1);
	gui.setWhichColumn(0);
	gui.addToggle("LoadAllAtStart", "LOAD_ALL_AT_START", true);
	gui.addSlider("FetchVideoProbability", "FETCH_VIDEO_PROBABILITY", 0.005, 0.000, 0.025, false);	
	gui.addSlider("MaxNVideos", "MAX_N_VIDEOS", 25, 1, 60, true); 
	
	// Detection controls
	gui.setWhichPanel(2);
	gui.setWhichColumn(0);
	gui.addSlider("FrameDiffThreshold", "FRAME_DIFF_THRESHOLD", 40, 1, 255, true);	
	gui.addSlider("LoMotionThreshold", "LO_MOTION_THRESHOLD", 0.010, 0.000, 0.300, false);
	gui.addSlider("HiMotionThreshold", "HI_MOTION_THRESHOLD", 0.300, 0.100, 0.500, false);
	gui.addSlider("MinRecordingLength", "MIN_RECORDING_LENGTH", 1.0, 0.5, 5.0, false);
	
	
	gui.loadSettings("settings/controlPanelSettings.xml");
}

//=========================================================================
void testApp::update() {
	updateCapture();
	updateDisplay();
}



//=========================================================================
void testApp::updateCapture(){
	
	//-------------------------------------------
	// grab the camera and threshold it.
	if (nAccessibleLibdcCameras > 0){
		if(camera.grabVideo(currFrame)) {
			currFrame.update();
			cameraFps.tick();
			cvCurrFrame.setFromPixels(currFrame.getPixels(), camWidth, camHeight);
			
			if(gui.getValueB("NAIVE")) {
				int threshold = (mouseX * 255) / ofGetWidth();
				cvCurrFrame.threshold(threshold);
				thresholded.setFromPixels(cvCurrFrame.getPixels(), camWidth, camHeight, OF_IMAGE_GRAYSCALE);
			} else {
				pupilFinder.updateBinaryPupils (cvCurrFrame,
												gui.getValueI("ADAPTHRESH_BLOCKSIZE", 0), 
												gui.getValueI("ADAPTHRESH_CONSTANT", 0),
												gui.getValueI("MIN_CONTOUR_AREA", 0),
												gui.getValueI("MAX_CONTOUR_AREA", 0),
												gui.getValueF("CIRCULARITY_THRESH", 0));
				thresholded.setFromPixels(pupilFinder.ofxCv8u_binaryPupils.getPixels(), camWidth, camHeight, OF_IMAGE_GRAYSCALE);
			}
			
			// send the thresholded image over to the live rleImage preview.
			rleImage.load(thresholded);
			rleImage.update();
			ofxRleImage::useDrawOffset = gui.getValueB("DRAW_OFFSET");
			
			// Reckon whether we are recording, and if so, add to the current video.
			updateWhetherOrNotWeAreRecording();
			if(bRecording) {
				rleVideo.add(thresholded);
			}
		}
	}
	
	
	
}

//=========================================================================
void testApp::updateWhetherOrNotWeAreRecording(){
	
	if (nAccessibleLibdcCameras > 0){
	
		// compute frame difference
		cvSmallPrevFrame = cvSmallCurrFrame;
		cvSmallCurrFrame.scaleIntoMe( cvCurrFrame, CV_INTER_NN);
		cvSmallFrameDiff.absDiff( cvSmallCurrFrame, cvSmallPrevFrame );
		
		// threshold frame difference and count remaining
		float frameDiffthresh = gui.getValueI("FRAME_DIFF_THRESHOLD", 0);
		cvSmallFrameDiff.threshold(frameDiffthresh, false);
		int numNonZero = cvSmallFrameDiff.countNonZeroInRegion(0,0,smallWidth,smallHeight);	
		float numNonZero01 = sqrtf (numNonZero / (float)(smallWidth * smallHeight)); 
		
		motionHistorian->updateHistory (numNonZero01);
		amountOfMotion01 = motionHistorian->getRunningAverageOfMedianOfN (0.97, 19);
		
		
		float loMotionThreshold = gui.getValueF("LO_MOTION_THRESHOLD", 0);
		float hiMotionThreshold = gui.getValueF("HI_MOTION_THRESHOLD", 0);
		float midMotionThreshold = (loMotionThreshold + hiMotionThreshold)/2.0;
		long now = ofGetElapsedTimeMillis();
		
		
		// If we weren't recording, and the motion has gone above the loMotionThreshold, 
		// in a reasonable way, then start recording. 
		if (!bRecording){
			if (now > 5000){ // ignore burst when program first begins
				if ((amountOfMotion01 > loMotionThreshold) && 
					(amountOfMotion01 < hiMotionThreshold)){
					
					rleVideo.clear();
					bRecording = true;
					startRecordingTimeMs = now;
					printf("START at %d\n", (int)now); 
				}
			}
		}
		
		
		// If we were recording, and the amount of motion has dropped below 0.8*loMotionThreshold, stop & save.
		if (bRecording){
			if (amountOfMotion01 < 0.45*loMotionThreshold){
				bRecording = false;
				long now = ofGetElapsedTimeMillis();
				float minRecordingLength = gui.getValueF("MIN_RECORDING_LENGTH", 0);
				if ((now - startRecordingTimeMs) > (1000.0* minRecordingLength)){
					saveVideo();
				} else {
					rleVideo.clear();
				}
				
			}
		}
		
		if (amountOfMotion01 > hiMotionThreshold){
			if (bRecording){
				bRecording = false;
				rleVideo.clear();
			}
		}
	}
}

//=========================================================================
void testApp::updateDisplay(){
	 
	if (currentEyeshineMode == EYESHINE_DISPLAY){
		
		maxNVideos = max(1, min(maxStorableVideos, gui.getValueI("MAX_N_VIDEOS", 0)));
		float fetchVideoProbability = gui.getValueF("FETCH_VIDEO_PROBABILITY", 0);
		if (ofRandomuf() < fetchVideoProbability){ 
			loadRandomRlevVideo (true);
		}
		
		for (int i=0; i<nVideos; i++){
			videos[i]->update();
		}
		
	} else if (currentEyeshineMode == EYESHINE_PREVIEW){
		videos[0]->update();
	}
}

//=========================================================================
void testApp::draw() {
	ofClear(0, 0, 0, 255);
	
	cameraFilter.begin();
	
	switch (currentEyeshineMode){
			
		//--------------------
		case EYESHINE_DIAGNOSTIC:
			drawEyeshineDiagnostic();
			gui.show();
			break;
		
		//--------------------
		case EYESHINE_PREVIEW:
			drawEyeshinePreview();
			gui.hide();
			break;
			
		//--------------------
		default:
		case EYESHINE_DISPLAY:
			drawEyeshineDisplay();
			gui.hide();
			break;
	}
	
	cameraFilter.end();
	cameraFilter.draw();
}




//=========================================================================
void testApp::drawEyeshineDiagnostic(){
	
	//-------------------------------------------
	// jump out of diagnostic mode after a timeout
	long now = ofGetElapsedTimeMillis();
	long elapsed = now - startDiagnosticTime; 
	long remaining =  (drawInformationDuration - elapsed)/1000;
	if ((now - startDiagnosticTime) > drawInformationDuration){
		currentEyeshineMode = EYESHINE_DISPLAY;
	}
	
	ofBackground(0,0,0);
	ofShowCursor();
	glDisable(GL_LINE_SMOOTH);
	
	ofSetColor(255);
	currFrame.draw(0, 0);

	if(bRecording) {
		ofSetColor(255, 0, 0);
	} else {
		ofSetColor(0, 0, 255);
	}
	rleImage.draw(0, 0);
	
	ofSetColor(255);
	cvSmallFrameDiff.draw(ofGetWidth()-smallWidth-10,10,smallWidth,smallHeight);
	

	ofSetColor(255, 144, 0);
	ofDrawBitmapString("Switching to display view (key 'v') in: " + ofToString(remaining), 10, 20);
	if (nAccessibleLibdcCameras > 0){
		ofDrawBitmapString("App framerate: " + ofToString((int) ofGetFrameRate()),         10, 35);
		ofDrawBitmapString("Cam framerate: " + ofToString((int) cameraFps.getFramerate()), 10, 50);
		ofDrawBitmapString("Motion (0..1): " + ofToString(amountOfMotion01),               10, 65);
	} else {
		ofDrawBitmapString("NO LIBDC CAMERAS CONNECTED.",  10, 35);
		ofSetColor(128, 0, 0);
		ofLine(0,0, ofGetWidth(), ofGetHeight()); 
		ofLine(ofGetWidth(),0, 0, ofGetHeight()); 
	}
	
}

//=========================================================================
void testApp::drawEyeshineDisplay(){
	
	ofBackground(0,0,0);
	ofHideCursor();
	if (bEnableLineSmooth){
		glEnable(GL_LINE_SMOOTH);
	} else {
		glDisable(GL_LINE_SMOOTH);
	}
	
	
	//-------------------------
	if (bDrawInformation){
		
		// only display this for one minute. 
		long now = ofGetElapsedTimeMillis();
		long elapsed = now - drawFrameInformationRequestTime;
		int secsRemaining = (drawInformationDuration - elapsed)/1000;
		if (elapsed > drawInformationDuration){
			bDrawInformation = false;
		}
		
		ofSetColor(255, 128, 0);
		sprintf (handyString, "Eyeshine: %3.1f fps, frame: %d", ofGetFrameRate(), ofGetFrameNum()); 
		ofDrawBitmapString(handyString, 10, 20);
		ofDrawBitmapString("Press 'i' to toggle this 'info' view", 10, 35); 
		ofDrawBitmapString("Press 'v' to toggle live camera view", 10, 50); 
		ofDrawBitmapString("This info will hide in: " + ofToString(secsRemaining) + "s", 10, 65);
		
		for (int i=0; i<nVideos; i++){
			
			if (i==currentVideoID){
				ofSetColor(128, 64, 0);
			} else {
				ofSetColor(128, 0, 0);
			}
			
			int curF    = videos[i]->currentFrame;
			int nFra    = videos[i]->getNFrames();
			char* fName = videos[i]->myFilename;
			sprintf(handyString, "%s\t %4d / %4d", fName, curF, nFra); 
			
			//ofDrawBitmapString("frame " + ofToString(curF) + " / " + ofToString(nFra), 10, 40 + i*15);
			ofDrawBitmapString(handyString, 10, 85 + i*15);
		}
	}
	
	//-------------------------
	// draw all of the other eyeshine videos. 
	//ofEnableAlphaBlending();
	//huntForBlendFunc(2000, 4,6);
	
	float videoAlpha01 = 0.65;
	for (int i=0; i<nVideos; i++){
		glPushMatrix();
		
		ofPoint col = videos[i]->color;
		ofSetColor(col.x, col.y, col.z, videoAlpha01*255.0);
		videos[i]->draw();
		
		glPopMatrix();
	}
	
	//-------------------------
	// draw the current eyeshine image, in white. 
	float targetR = 255;
	float targetG = 245;
	float targetB = 235;
	if(bRecording) {
		targetR = 255;
		targetG =  65;
		targetB = 0;
	}
	float A = 0.85;
	float B = 1.0-A;
	hiliteR = A*hiliteR + B*targetR;
	hiliteG = A*hiliteG + B*targetG;
	hiliteB = A*hiliteB + B*targetB;
	ofSetColor(hiliteR,hiliteG,hiliteB, 220);
	rleImage.draw(0, 0);
	
	
	
	
}



//=========================================================================
void testApp::drawEyeshinePreview(){
	ofBackground(0,0,0);
	ofHideCursor();
		
	ofSetColor(255, 0, 0);
	int curF    = videos[0]->currentFrame;
	int nFra    = videos[0]->getNFrames();
	char* fName = videos[0]->myFilename;
	sprintf(handyString, "%s\t %4d / %4d", fName, curF, nFra); 
	ofDrawBitmapString(handyString, 10, 20);
	
	ofSetColor(255,255,255);
	videos[0]->draw();
}

//============================================================
void testApp::loadVideo (int which){
	if ((which >= 0) && (which < nPreviousRecordings)){
		
		string searchDirectory = "recordings-previous/";
		string aFileName = searchDirectory + dirLister.getName(which);
		videos[0]->load(aFileName);
		videos[0]->color.set(255,255,255);
		videos[0]->setDrawOffset(0,0);
	}
}

//============================================================
void testApp::loadRandomRlevVideo (bool bUseCurrentFilesToo){
	ofSetLogLevel(OF_LOG_WARNING);
	
	// clear the oldest video in the ring buffer.
	nVideos = MIN(maxNVideos, nVideos+1);
	currentVideoID = (currentVideoID+1)%maxNVideos;
	delete (videos[currentVideoID]);
	videos[currentVideoID] = new ofxRleVideo();
	
	int maxNFilesToSearch = nPreviousRecordings;
	string searchDirectory = "recordings-previous/";
	if (bUseCurrentFilesToo){
		maxNFilesToSearch = totalFileCounter;
	}
	
	int randomFileId = (int)(totalFileCounter * ofRandomuf() * 0.9999);
	if (randomFileId < nPreviousRecordings){
		searchDirectory = "recordings-previous/";
		int numRlevFiles = dirLister.listDir(searchDirectory);
		randomFileId = (int)(numRlevFiles * ofRandomuf() * 0.9999);
	} else {
		searchDirectory = "recordings-current/";
		int numRlevFiles = dirLister.listDir(searchDirectory);
		randomFileId = (int)(numRlevFiles * ofRandomuf() * 0.9999);
	}
	string randomFileName = searchDirectory + dirLister.getName(randomFileId);
	videos[currentVideoID]->load(randomFileName);
	
	
	/*
	string searchDirectory = "recordings-previous/"; //recordings-current/
	int numRlevFiles = dirLister.listDir(searchDirectory);
	int randomFileId = (int)(numRlevFiles * ofRandomuf() * 0.99999);
	string randomFileName = searchDirectory + dirLister.getName(randomFileId);
	videos[currentVideoID]->load(randomFileName);
	*/

	
	//float ox = ofRandomuf() * (ofGetWidth() -1280);
	//float oy = ofRandomuf() * (ofGetHeight()-1024);
	float ox = ofRandomf() * (ofGetWidth()  * 0.2);
	float oy = ofRandomf() * (ofGetHeight() * 0.2);
	videos[currentVideoID]->setDrawOffset(ox,oy);
	
	// assign the video the color of a random pixel in the colorSourceImage
	int iw = colorSourceImage.width;
	int ih = colorSourceImage.height;
	int rx = (int) ofRandom(0, iw-1); 
	int ry = (int) ofRandom(0, ih-1);
	int index = ry*iw*3 + rx*3; 
	unsigned char *cpix = colorSourceImage.getPixels();
	videos[currentVideoID]->color.set(cpix[index],cpix[index+1],cpix[index+2]);
	
	/*
	 int gs = ofGetSeconds();
	 int gm = ofGetMinutes();
	 int gh = ofGetHours();
	 printf("At %02d:%02d:%02d, Loaded RLEV file: %s\n", gh,gm,gs, randomFileName.c_str());
	 */
	ofSetLogLevel(OF_LOG_VERBOSE);
}


//=========================================================================
void testApp::keyPressed(int key) {
	switch (key){
			
		//--------------------
		case 'v':
		case 'V':
			switch (currentEyeshineMode){
				case EYESHINE_DIAGNOSTIC:
					currentEyeshineMode = EYESHINE_DISPLAY;
					break;
				case EYESHINE_PREVIEW:
					currentEyeshineMode = EYESHINE_DIAGNOSTIC;
					break;
				case EYESHINE_DISPLAY:
					currentEyeshineMode = EYESHINE_DIAGNOSTIC;
					break;
				
			}
			if (currentEyeshineMode == EYESHINE_DIAGNOSTIC){
				startDiagnosticTime = ofGetElapsedTimeMillis();
			}
			break;
		
		case 'P':	
		case 'p':
			currentEyeshineMode = EYESHINE_PREVIEW;
			drawFrameInformationRequestTime = ofGetElapsedTimeMillis();
			startDiagnosticTime = ofGetElapsedTimeMillis();
			break;
		case '.':
			currentPreviewVideoIndex = (currentPreviewVideoIndex+1)%nPreviousRecordings;
			loadVideo (currentPreviewVideoIndex); 
			break;
		case ',':
			currentPreviewVideoIndex = (currentPreviewVideoIndex-1+nPreviousRecordings)%nPreviousRecordings;
			loadVideo (currentPreviewVideoIndex); 
			break;	
			
		case 'i':
			bDrawInformation = !bDrawInformation;
			if (bDrawInformation){
				drawFrameInformationRequestTime = ofGetElapsedTimeMillis();
			}
			break;
		
		//--------------------
		case 'r':
		case 'R':
			bRecording = !bRecording;
			if (!bRecording) {
				saveVideo();
			}
			break;
		
		//--------------------
		case 'L':
		case 'l':
			loadRandomRlevVideo(true);
			break;
		
		//--------------------
		case 'a':
			bEnableLineSmooth = !bEnableLineSmooth;
			break;
			
		//--------------------
		case 'f':
			ofToggleFullscreen();
			break;
			
	}
}


//============================================================
void testApp::saveVideo(){
	bRecording = false;
	
	string saveDirectory = "recordings-current/"; 
	sprintf(handyString, "%soutput-%05d.rlev", saveDirectory.c_str(), totalFileCounter); 
	cout << "Stopping recording, saving "<< handyString << " with " << rleVideo.size() << " frames." << endl;
	
	rleVideo.save(handyString);
	rleVideo.clear();
	
	nCurrentRecordings++;
	totalFileCounter++;
}

//============================================================
void testApp::mousePressed( int x, int y, int button ){
	
}

//======================================================================================
void testApp::huntForBlendFunc (int period, int defaultSid, int defaultDid){
	// sets all possible combinations of blend functions,
	// changing modes every [period] milliseconds. 
	
	int sfact[] = {
		GL_ZERO, 
		GL_ONE,
		GL_DST_COLOR, 
		GL_ONE_MINUS_DST_COLOR, 
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA,
		GL_DST_ALPHA,
		GL_ONE_MINUS_DST_ALPHA,
		GL_SRC_ALPHA_SATURATE
	};
	
	int dfact[] = {
		GL_ZERO,
		GL_ONE,
		GL_SRC_COLOR,
		GL_ONE_MINUS_SRC_COLOR,
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA,
		GL_DST_ALPHA,
		GL_ONE_MINUS_DST_ALPHA
	};
	
	glEnable(GL_BLEND);
	
	if ((defaultSid == -1) && (defaultDid == -1)) {
		
		int sid =  (ofGetElapsedTimeMillis()/(8*period))%9;
		int did =  (ofGetElapsedTimeMillis()/period)%8;
		glBlendFunc(sfact[sid], dfact[did]);
		printf("SRC %d	DST %d\n", sid, did);
		
	} else if (defaultDid == -1){
		
		int did =  (ofGetElapsedTimeMillis()/period)%8;
		glBlendFunc(sfact[defaultSid], dfact[did]);
		printf("SRC %d	DST %d\n", defaultSid, did);
		
	} else if (defaultSid == -1){
		
		int sid =  (ofGetElapsedTimeMillis()/(8*period))%9;
		glBlendFunc(sfact[sid], dfact[defaultDid]);
		printf("SRC %d	DST %d\n", sid, defaultDid);
		
	} else {
		
		glBlendFunc(sfact[defaultSid], dfact[defaultDid]);
		
	}
}

