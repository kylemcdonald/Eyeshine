#include "testApp.h"

//#define SOFTER

#define OFFSET_SIZE (5)
float hoffsets[] = {
	-2,0,
	-1,0,
	+0,0,
	+1,0,
	+2,0
};
float voffsets[] = {
	0,-2,
	0,-1,
	0,+0,
	0,+1,
	0,+2,
};

void testApp::setup() {
	cam.initGrabber(320, 240);
	
	int w = cam.getWidth();
	int h = cam.getHeight();
	ofFbo::Settings settings;
	for(int i = 0; i < 4; i++) {
		settings.width = w;
		settings.height = h;
		
		ofFbo* curPing = new ofFbo();
		ofFbo* curPong = new ofFbo();
		
		curPing->allocate(settings);
		curPong->allocate(settings);
		
		ping.push_back(curPing);
		pong.push_back(curPong);
		
		w /= 2;
		h /= 2;
	}
	
	ofSetCircleResolution(64);
	
	blurShader.load("", "separable.frag");
}

void testApp::update() {
	cam.update();
	if(cam.isFrameNew()) {
		for(int i = 0; i < ping.size(); i++) {
			// resample image (into ping)
			ofFbo* curPing = ping[i];
			curPing->begin();
			int w = curPing->getWidth();
			int h = curPing->getHeight();
			ofSetColor(255);
			if(i == 0) {
				cam.draw(0, 0);
				ofSetColor(0);
				ofCircle(w / 2, h / 2, 80);
				ofSetColor(255);
				ofCircle(w / 2, h / 2, 40);
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
			blurShader.setUniform2fv("offsets", hoffsets, OFFSET_SIZE);
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
			blurShader.setUniform2fv("offsets", voffsets, OFFSET_SIZE);
			pong[i]->draw(0, 0);
			blurShader.end();
			curPing->end();
		}
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
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	for(int i = 0; i < n; i++) {
#ifdef SOFTER
		float total = n * (n + 1) / 2;
		float weight = (i + 1) / total;
		ofSetColor(ofFloatColor(weight));
#else
		ofSetColor(ofFloatColor(1. / n));
#endif
		ping[i]->draw(0, 0, cam.getWidth(), cam.getHeight());
	}
	ofDisableBlendMode();
	ofTranslate(0, cam.getHeight());
}
