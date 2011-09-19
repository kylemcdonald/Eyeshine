#include "ofxCameraFilter.h"

#include "CameraFilter.frag.h"

ofxCameraFilter::ofxCameraFilter()
:distortion(.05)
,aberrationAmount(.1)
,vignetteSharpness(3.)
,vignetteSize(.6)
,noiseAmount(.1) {
}

void ofxCameraFilter::setup(int width, int height) {
ofSetLogLevel(OF_LOG_VERBOSE);
	fbo.allocate(width, height);
	blur.setup(width, height, 12, .1, 4, .4);
	shader.setupShaderFromSource(GL_FRAGMENT_SHADER, cameraFilterSource);
	shader.linkProgram();
}

void ofxCameraFilter::begin() {
	blur.begin();
	ofClear(0, 0, 0, 255);
}

void ofxCameraFilter::end() {
	blur.end(false);
	fbo.begin();
	ofClear(0);
	blur.draw();
	fbo.end();
}

void ofxCameraFilter::draw() {	
	shader.begin();		
	shader.setUniformTexture("tex", fbo.getTextureReference(), 1);
	shader.setUniform1f("time", ofGetElapsedTimef());
	shader.setUniform2f("imageSize", fbo.getWidth(), fbo.getHeight());
	shader.setUniform1f("imageRadius", ofDist(0, 0, fbo.getWidth(), fbo.getHeight()) / 2.);
	shader.setUniform1f("distortion", distortion);
	shader.setUniform1f("aberrationAmount", aberrationAmount);
	shader.setUniform1f("vignetteSharpness", vignetteSharpness);
	shader.setUniform1f("vignetteSize", vignetteSize);
	shader.setUniform1f("noiseAmount", noiseAmount);
	shader.setUniform1f("brightness", brightness);
	fbo.draw(0, 0);
	shader.end();
}

void ofxCameraFilter::setBlurScale(float scale) {
	blur.setScale(scale);
}

void ofxCameraFilter::setBlurRotation(float rotation) {
	blur.setRotation(rotation);
}

void ofxCameraFilter::setDistortion(float distortion) {
	this->distortion = distortion;
}

void ofxCameraFilter::setAberrationAmount(float aberrationAmount) {
	this->aberrationAmount = aberrationAmount;
}

void ofxCameraFilter::setNoiseAmount(float noiseAmount) {
	this->noiseAmount = noiseAmount;
}

void ofxCameraFilter::setVignetteSharpness(float vignetteSharpness) {
	this->vignetteSharpness = vignetteSharpness;
}

void ofxCameraFilter::setVignetteSize(float vignetteSize) {
	this->vignetteSize = vignetteSize;
}

void ofxCameraFilter::setBrightness(float brightness) {
	this->brightness = brightness;
}