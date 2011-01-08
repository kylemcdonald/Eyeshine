#include "ofxRleVideo.h"

ofxRleVideo::~ofxRleVideo() {
	for(int i = 0; i < frames.size(); i++) {
		delete frames[i];
	}
}

void ofxRleVideo::add(ofImage& img) {
	frames.push_back(new ofxRleImage());
	frames.back()->load(img);
	frames.back()->update();
}

int ofxRleVideo::size() const {
	return frames.size();
}

ofxRleImage* ofxRleVideo::operator[](unsigned int i) const {
	return frames[i];
}

ostream& operator<<(ostream& out, const ofxRleVideo& video) {
	unsigned int dataSize = video.frames.size();
	out.write((char*) &(dataSize), sizeof(dataSize));
	for(int i = 0; i < dataSize; i++) {
		out << *(video[i]);
	}
	return out;
}

void ofxRleVideo::save(string filename) const {
	ofstream file;
	file.open(ofToDataPath(filename).c_str(), ios::out | ios::trunc | ios::binary);
	if(file.is_open()) {
		file << *this;
	} else {
		ofLog(OF_LOG_ERROR, "Could not open file " + filename + " for saving.");
	}
	file.close();
}

istream& operator>>(istream& in, ofxRleVideo& video) {
	unsigned int dataSize;
	in.read((char*) &(dataSize), sizeof(dataSize));
	video.frames.resize(dataSize);
	for(int i = 0; i < dataSize; i++) {
		ofxRleImage* cur = video[i];
		cur = new ofxRleImage();
		in >> *cur;
	}
	
	return in;
}

void ofxRleVideo::load(string filename) {
	ifstream file;
	file.open(ofToDataPath(filename).c_str(), ios_base::in | ios::binary);
	if(file.is_open()) {
		file >> *this;
	} else {
		ofLog(OF_LOG_ERROR, "Could not open file " + filename + " for loading.");
	}
	file.close();
}
