#pragma once

#include "ofMain.h"
#include "ofxBlur.h"

class RateTimer {
protected:
	double lastTick, averagePeriod, smoothing;
	bool secondTick;
public:
	RateTimer(float smoothing = .9) :
	smoothing(smoothing) {
		reset();
	}
	void reset() {
		lastTick = 0;
		averagePeriod = 0;
		secondTick = false;
	}
	double getFrameRate() {
		return averagePeriod == 0 ? 0 : 1 / averagePeriod;
	}
	double getPeriodSeconds() {
		return averagePeriod;
	}
	double getPeriodMilliseconds() {
		return averagePeriod * 1000;
	}
	double getPeriodMicroseconds() {
		return averagePeriod * 1000 * 1000;
	}
	void tick() {
		double curTick = ofGetElapsedTimef();
		if(lastTick == 0) {
			secondTick = true;
		} else {
			double curDiff = curTick - lastTick;
			if(secondTick) {
				averagePeriod = curDiff;
				secondTick = false;
			} else {
				averagePeriod = ofLerp(curDiff, averagePeriod, smoothing);
			}
		}
		lastTick = curTick;
	}
};	

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	
	void generateOffsets();
	
	ofVideoGrabber cam;
	ofxBlur blur;
	RateTimer timer;
};
