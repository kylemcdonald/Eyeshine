#include "ofxRleImage.h"

// This adds a 1-pixel vertical offset, which is important for drawing lines on some graphics cards.
bool ofxRleImage::useDrawOffset = false;

void ofxRleImage::setup(ofImage& img) {
	if(img.type == OF_IMAGE_GRAYSCALE) {
		data.clear();
		width = img.getWidth();
		height = img.getHeight();
		
		unsigned char *raw = img.getPixels();
		
		int nPixels = width * height;
	
		// Scan through the image in 8-bytes chunks. (unsigned long long, or uint64_t). 
		// Achieve additional optimizations by assuming that the image is mostly black.
		
		int bytesPerChunk = sizeof(uint64_t);
		
		uint64_t *firstChunk = (uint64_t*) raw;
		uint64_t *curChunk = &firstChunk[1];
		uint64_t *lastChunk =	&firstChunk[(nPixels / bytesPerChunk) - 1];
		
		// Note: I ignore the first 8 and last 8 bytes of the image, for now. 
		// (that's the 1 and -1 above.)
		
		int pixelIndex = 0;
		unsigned char prevVal = 0;
		while (curChunk < lastChunk){
			uint64_t curChunkVal = *curChunk;
			if (curChunkVal != 0){
				uint64_t nextChunkVal = *(curChunk+1);
				unsigned char *triplet = (unsigned char *) (curChunk - 1);
				
				// We store all 24 bytes but only record edges 
				// in the curr (middle) set of 8 chars. 
				unsigned char prevVal = triplet[bytesPerChunk - 1];
				for (int curIndex = bytesPerChunk; curIndex < (bytesPerChunk + bytesPerChunk + 1); curIndex++){
					unsigned char curVal = triplet[curIndex];
					if (curVal != prevVal) {
						if (curIndex < (bytesPerChunk + bytesPerChunk)){
							data.push_back( ((curChunk - firstChunk - 1) * bytesPerChunk) + curIndex );
						} else if (nextChunkVal == 0){
							data.push_back( ((curChunk - firstChunk - 1) * bytesPerChunk) + curIndex );
						}
					}
					prevVal = curVal;
				}
			}
			curChunk++;
		}
		
		// Convert RLE data into lines.
		int nLineEndpoints = data.size();	
		lines.clear();
		lines.resize(nLineEndpoints);
		for (int i = 0; i < nLineEndpoints; i++){
			int curIndex = data[i];	
			RlePoint2d& curPoint = lines[i];
			curPoint.x = (curIndex % width);
			curPoint.y = (curIndex / width);
		}
	}		
}

void ofxRleImage::draw() {
	int nLineEndpoints = data.size();	
	
	glPushMatrix();
	
	if(useDrawOffset) {
		glTranslatef(0, 1, 0);
	}

	// Draw lines as a vertex array.
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, &lines[0]);
	glDrawArrays(GL_LINES, 0, lines.size());
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glPopMatrix();
}