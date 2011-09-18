#pragma once

#include "ofMain.h"

class CameraFilter {
protected:
	ofShader shader;
	ofFbo fbo;
public:
	void setup(int width, int height) {
		fbo.allocate(width, height);
		reload();
	}
	void reload() {
		shader.unload();
		shader.setupShaderFromFile(GL_FRAGMENT_SHADER, "cameraFilter.frag");
		shader.linkProgram();
	}
	void begin() {
		fbo.begin();
	}
	void end() {
		fbo.end();
	}
	void draw() {		
		shader.begin();		
		shader.setUniformTexture("tex", fbo.getTextureReference(), 1);
		shader.setUniform1f("time", ofGetElapsedTimef());
		fbo.draw(0, 0);
		shader.end();
	}
};

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
	ofVideoGrabber cam;
	CameraFilter filter;
	ofMesh grid;
};
