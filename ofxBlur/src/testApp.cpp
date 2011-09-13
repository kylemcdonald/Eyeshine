#include "testApp.h"

//#define SOFTER

void PascalsTriangle(int level, vector<float>& row) {
	if(level < 0) {
		return;
	}
	int n = row.size();
	row.push_back(1);
	for(int i = n - 1; i > 0; i--) {
		row[i] += row[i - 1];
	}
	PascalsTriangle(level - 1, row);
}

void testApp::generateOffsets() {
	vector<float> row;
	PascalsTriangle(level, row);
	float sum = 0;
	for(int i = 0; i < row.size(); i++) {
		sum += row[i];
	}
	for(int i = 0; i < row.size(); i++) {
		row[i] /= sum;
	}
	
	coefficients.clear();
	hoffsets.clear();
	voffsets.clear();
	int center = row.size() / 2;
	coefficients.push_back(row[center]);
	for(int i = center + 1; i < row.size(); i += 2) {
		float sum = row[i] + row[i + 1];
		int left = i - center;
		int right = left + 1;
		float weightedSum = left * row[i] + right * row[i + 1];
		float offset = weightedSum / sum;
		offset *= ofMap(mouseX, 0, ofGetWidth(), 1, 10);
		coefficients.push_back(sum);
		hoffsets.push_back(ofVec2f(offset, 0));
		voffsets.push_back(ofVec2f(0, offset));
	}
	/*
	cout << ofToString(coefficients) << endl;
	cout << ofToString(hoffsets) << endl;
	cout << ofToString(voffsets) << endl;
	*/
}

void testApp::setup() {
	cam.initGrabber(320, 240);
	
	reductions = 6;
	reductionFactor = 2./3.;
	level = 8;
	blurShader.load("", "blur" + ofToString(level) + ".frag");
	combineShader.load("", "combine6.frag");
	
	int w = cam.getWidth();
	int h = cam.getHeight();
	ofFbo::Settings settings;
	for(int i = 0; i < reductions; i++) {
		settings.width = w;
		settings.height = h;
		
		ofFbo* curPing = new ofFbo();
		ofFbo* curPong = new ofFbo();
		
		curPing->allocate(settings);
		curPong->allocate(settings);
		
		ping.push_back(curPing);
		pong.push_back(curPong);
		
		w *= reductionFactor;
		h *= reductionFactor;
	}
}

void testApp::update() {
	generateOffsets();
	
	cam.update();
	if(cam.isFrameNew()) {
		unsigned long startTime = ofGetSystemTimeMicros();
		for(int i = 0; i < ping.size(); i++) {
			// resample image (into ping)
			ofFbo* curPing = ping[i];
			curPing->begin();
			int w = curPing->getWidth();
			int h = curPing->getHeight();
			ofSetColor(255);
			if(i == 0) {
				cam.draw(0, 0);
			} else {
				ping[i - 1]->draw(0, 0, w, h);
			}
			curPing->end();
		}
		
		// horizontal pass blur on resampled fbo (into pong)
		for(int i = 0; i < pong.size(); i++) {
			ofFbo* curPong = pong[i];
			curPong->begin();
			blurShader.begin();
			blurShader.setUniformTexture("source", ping[i]->getTextureReference(), 0);
			blurShader.setUniform2fv("offsets", (float*) &(hoffsets[0]), hoffsets.size());
			ping[i]->draw(0, 0);
			blurShader.end();
			curPong->end();
		}
		
		// vertical pass blur on resampled fbo (into ping)
		for(int i = 0; i < ping.size(); i++) {
			ofFbo* curPing = ping[i];
			curPing->begin();
			blurShader.begin();
			blurShader.setUniformTexture("source", pong[i]->getTextureReference(), 0);
			blurShader.setUniform2fv("offsets", (float*) &(voffsets[0]), voffsets.size());
			pong[i]->draw(0, 0);
			blurShader.end();
			curPing->end();
		}
		unsigned long stopTime = ofGetSystemTimeMicros();
		blurTime = (stopTime - startTime) / 1000.;
	}
}

void testApp::draw() {
	ofBackground(0);
	
	int n = ping.size();
	ofSetColor(255);
	
	ofPushMatrix();
	for(int i = 0; i < n; i++) {
		pong[i]->draw(0, 0);
		ofTranslate(0, pong[i]->getHeight());
	}
	ofPopMatrix();
	ofTranslate(cam.getWidth(), 0);
	
	ofPushMatrix();
	for(int i = 0; i < n; i++) {
		ping[i]->draw(0, 0);
		ofTranslate(0, ping[i]->getHeight());
	}
	ofPopMatrix();
	ofTranslate(cam.getWidth(), 0);	
	
	combineShader.begin();
	for(int i = 0; i < 6; i++) {
		string name = "s" + ofToString(i);
		combineShader.setUniformTexture(name.c_str(), ping[i]->getTextureReference(), i);
	}
	ofEnableNormalizedTexCoords();
	int w = ping[0]->getWidth();
	int h = ping[0]->getHeight();
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2i(0, 0);
	glTexCoord2i(w, 0); glVertex2i(w, 0);
	glTexCoord2i(w, h); glVertex2i(w, h);
	glTexCoord2i(0, h); glVertex2i(0, h);
	glEnd();
	ofDisableNormalizedTexCoords();
	combineShader.end();
	
	ofTranslate(0, cam.getHeight());
	
	ofSetColor(255);
	ofDrawBitmapString(ofToString(blurTime), 10, 20);
}
