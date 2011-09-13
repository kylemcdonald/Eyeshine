#include "ofxRleVideo.h"

//----------------------------------------------------------------
ofxRleVideo::ofxRleVideo() {
	myFilename = new char[256];
}

//----------------------------------------------------------------
ofxRleVideo::~ofxRleVideo() {
	for(int i = 0; i < frames.size(); i++) {
		delete frames[i];
	}
}

//----------------------------------------------------------------
void ofxRleVideo::add(ofImage& img) {
	frames.push_back(new ofxRleImage());
	frames.back()->load(img);
	frames.back()->update();
}

//----------------------------------------------------------------
void ofxRleVideo::clear(){
	
	for (int i = 0; i < frames.size(); i++) {
		delete frames[i];
	}
	
	frames.clear();
	
	currentFrame = 0; 
	drawOffsetX = 0;
	drawOffsetY = 0;
}

//----------------------------------------------------------------
int ofxRleVideo::size() const {
	return frames.size();
}

//----------------------------------------------------------------
ofxRleImage* ofxRleVideo::operator[](unsigned int i) const {
	return frames[i];
}

//----------------------------------------------------------------
ostream& operator<<(ostream& out, const ofxRleVideo& video) {
	unsigned int dataSize = video.frames.size();
	out.write((char*) &(dataSize), sizeof(dataSize));
	for(int i = 0; i < dataSize; i++) {
		out << *(video[i]);
	}
	return out;
}

//----------------------------------------------------------------
void ofxRleVideo::save(string filename) const {
	ofstream file;
	file.open(ofToDataPath(filename).c_str(), ios::out | ios::trunc | ios::binary);
	if(file.is_open()) {
		file << *this;
		//
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
		video.frames[i] = new ofxRleImage();
		in >> *(video.frames[i]);
	}
	return in;
}

//----------------------------------------------------------------
void ofxRleVideo::load(string filename) {
	ifstream file;
	file.open(ofToDataPath(filename).c_str(), ios_base::in | ios::binary);
	if(file.is_open()) {
		file >> *this;
		sprintf(myFilename, "%s",  filename.c_str()); 

	} else {
		ofLog(OF_LOG_ERROR, "Could not open file " + filename + " for loading.");
	}
	file.close();
	
	currentFrame = 0;
	drawOffsetX = 0;
	drawOffsetY = 0;
}

//----------------------------------------------------------------
int ofxRleVideo::getNFrames(){
	return frames.size();
}

//----------------------------------------------------------------
void ofxRleVideo::update(){
	if(frames.size() > 0){
		currentFrame = (currentFrame+1)%(frames.size());
	}
}	

//----------------------------------------------------------------
void ofxRleVideo::draw(){
	glPushMatrix();
	glTranslatef(drawOffsetX, drawOffsetY, 0);
	frames[currentFrame]->draw();
	glPopMatrix();
}

//----------------------------------------------------------------
void ofxRleVideo::setDrawOffset (float ox, float oy){
	drawOffsetX = ox;
	drawOffsetY = oy;
}





