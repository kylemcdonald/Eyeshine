#pragma once

#include "ofMain.h"
#include "ofxBlur.h"

class ofxCameraFilter {
protected:
	ofFbo fbo;
	ofxBlur blur;
	ofShader shader;
	
	float distortion;
	float aberrationAmount;
	float noiseAmount;
	float vignetteSharpness, vignetteSize;
	float brightness;
public:
	ofxCameraFilter();
	void setup(int width, int height);
	void begin();
	void end();
	void draw();
	
	void setBlurScale(float scale);
	void setBlurRotation(float rotation);
	void setDistortion(float distortion);
	void setAberrationAmount(float aberrationAmount);
	void setNoiseAmount(float noiseAmount);
	void setVignetteSharpness(float vignetteSharpness);
	void setVignetteSize(float vignetteSize);
	void setBrightness(float brightness);
};