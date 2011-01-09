#include "PupilFinder.h"


//--------------------------------------------------------------
void PupilFinder::setup (int w, int h){
	
	//-----------------------------
	// Buffers used in the pupil detection
	imgW = w;
	imgH = h;

	ofxCv8u_temp1.allocate(imgW, imgH);
	ofxCv8u_finalGray.allocate(imgW, imgH);
	ofxCv8u_binaryPupils.allocate(imgW, imgH);
	
	//ofxCv8u_raw.setFromPixels(loadedImagePixels, imgW, imgH);
	//ofxCv8u_temp1.setFromPixels(loadedImagePixels, imgW, imgH);
	ofxCv8u_finalGray.set(0);
	ofxCv8u_binaryPupils.set(0);	
	
	//-----------------------------
	// Contour finder and related vector data
	cvImgTemp = cvCreateImage( cvSize(imgW, imgH), IPL_DEPTH_8U, 1); 
	MAX_N_CONTOUR_POINTS = 128;
	cvpts    = new CvPoint*[1];
	cvpts[0] = new CvPoint[MAX_N_CONTOUR_POINTS];
	for (int i=0; i<MAX_N_CONTOUR_POINTS; i++){
		cvpts[0][i] = cvPoint(0,0);
	}
	ncvpts = new int[1];
	ncvpts[0] = 0;
	
	
	
}

//--------------------------------------------------------------
void PupilFinder::updateBinaryPupils (ofxCvGrayscaleImage &ofxCv8u_raw,
									  int adapThreshBlockSize,
									  int adapThreshConstant,
									  int minContourArea,
									  int maxContourArea,
									  float circularityThreshold){
	
	// uses cvAdaptiveThreshold
	//---------------------------
	// Initialize and clear arrays.
	ofxCv8u_temp1 = ofxCv8u_raw;
	ofxCv8u_binaryPupils.set(0.0f); 
	cvSetZero(cvImgTemp);
	
	
	//---------------------------
	// Perform adaptive thresholding of the image. 
	int blockSize = 1 + 2*adapThreshBlockSize;  
	double offsetC = (double)(0 - adapThreshConstant);   
	
	cvAdaptiveThreshold(ofxCv8u_raw.getCvImage(), 
						ofxCv8u_temp1.getCvImage(), 
						255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV,
						blockSize, offsetC);
	
	cvSub(ofxCv8u_raw.getCvImage(), 
		  ofxCv8u_temp1.getCvImage(), 
		  ofxCv8u_finalGray.getCvImage(), 0);
	ofxCv8u_finalGray.flagImageChanged();
	ofxCv8u_finalGray.threshold(1, false);
	
	//---------------------------
	// Compute the contours of visible blobs.
	int maxNBlobsToFind = 64;
	contourFinder.findContours(ofxCv8u_finalGray, 
							   minContourArea, maxContourArea, maxNBlobsToFind, false);
	
	//---------------------------
	// Find blobs that meet area and circularity criteria;
	// render them into ofxCv8u_binaryPupils
	// http://en.wikipedia.org/wiki/Shape_factor_%28image_analysis_and_microscopy%29
	
	vector<ofxCvBlob> blobs = contourFinder.blobs;
	int nBlobs = blobs.size();
	
	// Request the area and perimeter for each blob.
	CvScalar color_white = CV_RGB(255,255,255);
	for (int i=0; i<nBlobs; i++){
		ofxCvBlob blobi = blobs[i];
		float area = blobi.area;
		float perimeter = blobi.length;
		float circularity = 4.0*PI*area / (perimeter * perimeter); 
		
		// If the area:perimeter ratio is good
		if (circularity > circularityThreshold){
			
			// Extract the contour, and draw it into a clean blank image.
			vector <ofPoint>    pts  = blobi.pts;    
			int                 nPts = blobi.nPts; 
			nPts = MIN(MAX_N_CONTOUR_POINTS, nPts); 
			for (int j=0; j<nPts; j++){ 
				ofPoint pt = pts[j];
				int px = (int) pt.x; // 0...imgW
				int py = (int) pt.y; // 0...imgH
				cvpts[0][j].x = px;
				cvpts[0][j].y = py;
			}
			ncvpts[0] = nPts;
			cvFillPoly(cvImgTemp, cvpts, ncvpts, 1, color_white, 8, 0 );
		}
	}
	ofxCv8u_binaryPupils = cvImgTemp;
	ofxCv8u_binaryPupils.flagImageChanged();
	
}
//--------------------------------------------------------------
void PupilFinder::drawBinaryPupils (float x, float y, float w, float h){
	
	glColor3f(1,0,0); 
	ofxCv8u_binaryPupils.draw(x,y, w,h);
	
	glColor3f(1,1,1); 
	contourFinder.draw(x,y, w,h);
}
