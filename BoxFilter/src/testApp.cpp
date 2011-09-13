#include "testApp.h"

#define SOFTER

void testApp::setup() {
	cam.initGrabber(640, 480);
	
	int w = cam.getWidth();
	int h = cam.getHeight();
	ofFbo::Settings settings;
	for(int i = 0; i < 4; i++) {
		ofFbo* cur = new ofFbo();
		settings.width = w;
		settings.height = h;
		cur->allocate(settings);
		fbos.push_back(cur);
		w /= 2;
		h /= 2;
	}
}

void testApp::update() {
	cam.update();
	if(cam.isFrameNew()) {
	
		ofSetColor(255);
		for(int i = 0; i < fbos.size(); i++) {
			ofFbo* curFbo = fbos[i];
			curFbo->begin();
			if(i == 0) {
				cam.draw(0, 0);
			} else {
				int w = curFbo->getWidth();
				int h = curFbo->getHeight();
				fbos[i - 1]->draw(0, 0, w, h);
			}
			curFbo->end();
		}
	}
}

void testApp::draw() {
	ofBackground(0);

	int n = fbos.size();
	
	ofPushMatrix();
	ofSetColor(255);
	for(int i = 0; i < n; i++) {
		fbos[i]->draw(0, 0);
		ofTranslate(0, fbos[i]->getHeight());
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
		fbos[i]->draw(0, 0, cam.getWidth(), cam.getHeight());
	}
	ofDisableBlendMode();
}
