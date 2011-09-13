#include "testApp.h"

//#define SOFTER

#define KERNEL_SIZE 5
float kernel[KERNEL_SIZE * KERNEL_SIZE] =  {
	1, 4, 6, 4, 1,
	4, 16, 24, 16, 4,
	6, 24, 36, 24, 6,
	4, 16, 24, 16, 4,
	1, 4, 6, 4, 1,
};

void testApp::setup() {
	cam.initGrabber(320, 240);
	
	int w = cam.getWidth();
	int h = cam.getHeight();
	ofFbo::Settings settings;
	for(int i = 0; i < 4; i++) {
		settings.width = w;
		settings.height = h;
		
		ofFbo* cur = new ofFbo();
		cur->allocate(settings);
		fbos.push_back(cur);
		
		ofFbo* blurred = new ofFbo();
		blurred->allocate(settings);
		blurredFbos.push_back(blurred);
		
		w /= 2;
		h /= 2;
	}
	
	ofSetCircleResolution(64);
	
	blurShader.load("", "naivef.frag");
	
	// normalize kernel
	float sum = 0;
	int kn = KERNEL_SIZE * KERNEL_SIZE;
	for(int i = 0; i < kn; i++) {
		sum += kernel[i];
	}
	for(int i = 0; i < kn; i++) {
		kernel[i] /= sum;
	}
}

void testApp::update() {
	cam.update();
	if(cam.isFrameNew()) {
		for(int i = 0; i < fbos.size(); i++) {
			// resample image
			ofFbo* curFbo = fbos[i];
			curFbo->begin();
			int w = curFbo->getWidth();
			int h = curFbo->getHeight();
			ofSetColor(255);
			if(i == 0) {
				cam.draw(0, 0);
				ofSetColor(0);
				ofCircle(w / 2, h / 2, 80);
				ofSetColor(255);
				ofCircle(w / 2, h / 2, 40);
			} else {
				fbos[i - 1]->draw(0, 0, w, h);
			}
			curFbo->end();
			
			// blur resampled fbo
			ofFbo* curBlurred = blurredFbos[i];
			curBlurred->begin();
			blurShader.begin();
			blurShader.setUniformTexture("source", curFbo->getTextureReference(), 10);
			blurShader.setUniform1fv("coefficients", kernel, KERNEL_SIZE * KERNEL_SIZE);
			curFbo->draw(0, 0);
			blurShader.end();
			curBlurred->end();
		}
	}
}

void testApp::draw() {
	ofBackground(0);
	
	int n = fbos.size();
	ofSetColor(255);
	
	ofPushMatrix();
	for(int i = 0; i < n; i++) {
		fbos[i]->draw(0, 0);
		ofTranslate(0, fbos[i]->getHeight());
	}
	ofPopMatrix();
	ofTranslate(cam.getWidth(), 0);
	
	ofPushMatrix();
	for(int i = 0; i < n; i++) {
		blurredFbos[i]->draw(0, 0);
		ofTranslate(0, blurredFbos[i]->getHeight());
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
		blurredFbos[i]->draw(0, 0, cam.getWidth(), cam.getHeight());
	}
	ofDisableBlendMode();
	ofTranslate(0, cam.getHeight());
	
	ofSetColor(255);
	//ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
}
