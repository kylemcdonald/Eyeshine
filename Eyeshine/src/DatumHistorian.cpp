
#include "DatumHistorian.h"





//==============================================================
DatumHistorian::DatumHistorian(){
	historyLength = 0;
	maxHistoryLength = 64;
	history 		= new float[maxHistoryLength]; 
	sortedValues 	= new float[maxHistoryLength]; 
	tempValues		= new float[maxHistoryLength]; 
	clear();
}
//--------------------------------------------------------------
DatumHistorian::DatumHistorian (int hlen){
	historyLength = 0;
	maxHistoryLength = MAX(5, hlen);
	history 		= new float[maxHistoryLength];  //new float[maxHistoryLength];
	sortedValues 	= new float[maxHistoryLength];  
	tempValues		= new float[maxHistoryLength];  
	clear();
}

/*
//--------------------------------------------------------------
float DatumHistorian::getStandardDeviation(){
	float out = 0;
	if (historyLength > 2){
		Ipp32f pStdDev = 0;
		ippsStdDev_32f(history, historyLength, &pStdDev, ippAlgHintNone);
		out = pStdDev;
	}
	return out;
}
//--------------------------------------------------------------
float DatumHistorian::getStandardDeviation (int len){
	float out = 0;
	len = MAX(1, MIN(len, historyLength));
	if (historyLength > 2){
		Ipp32f pStdDev = 0;
		ippsStdDev_32f(history, len, &pStdDev, ippAlgHintNone);
		out = pStdDev;
	}
	return out;
}
*/


//==============================================================
static int float_compare (const void * a, const void * b){
	int out = 0;
	float fa = *(float*)a;
	float fb = *(float*)b;
	if (fa > fb){	out =  1; }
	else {			out = -1; }
	return out;
}

//--------------------------------------------------------------
void DatumHistorian::clear(){
	for (int i=0; i<maxHistoryLength; i++){
		sortedValues[i] = 0;
		history[i] = 0;
	}
	historyLength = 0;
	runningAverage = 0;
	currentMedian = 0;
	bRunningAverageRequested = false;
}
//--------------------------------------------------------------
void DatumHistorian::setHistory (float* a, int nvals){
	int n = MAX(0, MIN(nvals, maxHistoryLength)); 
	for (int i=0; i<n; i++){
		history[i] = a[i];
	}
}

//--------------------------------------------------------------
float *DatumHistorian::getHistory(){
	return history;
}
//--------------------------------------------------------------
int DatumHistorian::getMaxHistoryLength(){
	return maxHistoryLength;
}

//--------------------------------------------------------------
void DatumHistorian::updateHistoryWhileClampingOutliers (float newval){
	
	if ((bRunningAverageRequested == false) || (historyLength < 10)){
		updateHistory(newval);
		
	}	else {
	
		// store what may or may not become our new value.
		float newvalChecked = newval;
		
		// if the incoming newval differs by an absolute percentage in 0..1, don't accept it.
		float maxAcceptableAbsoluteChange = 0.10;
		float newvalDelta = fabs(newval - history[0]);
		if (newvalDelta > maxAcceptableAbsoluteChange){
			// instead, clamp it to the maxAcceptableAbsoluteChange.
			float sign = ((newval - history[0]) < 0) ? -1 : 1;
			newvalChecked = history[0] + sign*maxAcceptableAbsoluteChange;
			newvalChecked = MAX(0, MIN(1, newvalChecked));
		}
		
		// push all of the data down the array.
		for (int i=(maxHistoryLength-1); i>0; i--){
			history[i] = history[i-1];
		}
		
		// calculate the standard deviation of the derivatives (differences)
		// of the history array. If the current value represents too big a jump from 
		// the previous value, just copy (the previous value + an estimation of velocity). 
		history[0] = newvalChecked;
		
		// update overall properties
		historyLength = MIN(maxHistoryLength, historyLength+1);
	}
}


//--------------------------------------------------------------
void DatumHistorian::updateHistory (float newval){
	for (int i=(maxHistoryLength-1); i>0; i--){
		history[i] = history[i-1];
	}
	history[0] = newval;
	historyLength = MIN(maxHistoryLength, historyLength+1);
	
	if (bRunningAverageRequested == false){
		runningAverage = newval;
	}
}

float DatumHistorian::getMostRecent(){
	return history[0];
}

//==============================================================
float DatumHistorian::getRunningAverage (float alpha){
	bRunningAverageRequested = true;
	float A = MAX(0, MIN(1, alpha));
	float B = 1.0 - alpha;
	runningAverage = A*runningAverage + B*history[0];
	return runningAverage;
}

//--------------------------------------------------------------
float DatumHistorian::getRunningAverageOf5Median (float alpha){
	bRunningAverageRequested = true;
	float A = MAX(0, MIN(1, alpha));
	float B = 1.0 - alpha;
	currentMedian = getMedianValueOf5History();
	runningAverage = A*runningAverage + B*currentMedian;
	return runningAverage;
}
//--------------------------------------------------------------
float DatumHistorian::getRunningAverageOfMedianOfN (float alpha, int N){
	bRunningAverageRequested = true;
	float A = MAX(0, MIN(1, alpha));
	float B = 1.0 - alpha;
	currentMedian = getMedianValueFromMostRecentN(N);
	runningAverage = A*runningAverage + B*currentMedian;
	return runningAverage;
}





//--------------------------------------------------------------
int DatumHistorian::getMedianIndexOf5History(){
	return getMedianIndexOf5(history);
} 
//--------------------------------------------------------------
float DatumHistorian::getMedianValueOf5History(){
	currentMedian = getMedianValueOf5(history);
	return currentMedian;
} 

//--------------------------------------------------------------
float DatumHistorian::getMedianValueFromMostRecentN (int N){

	int medianLength = MIN(historyLength, N);
	if (medianLength < 2){
		currentMedian = history[0];
		return currentMedian;
	}
	
	for (int i=0; i<maxHistoryLength; i++){
		sortedValues[i] = history[i]; // copy the history
	}
	qsort( sortedValues, medianLength, sizeof(float), float_compare);

	if (N%2 == 1){
		int medianIndex = medianLength/2; 
		currentMedian = sortedValues[medianIndex];
	} else {
		int medianIndex0 = medianLength/2 -1;
		int medianIndex1 = medianLength/2;
		currentMedian = 0.5 * (sortedValues[medianIndex0] + sortedValues[medianIndex1]);
	}
	return currentMedian;
}

//==============================================================
int DatumHistorian::getMedianIndexOf5 (float* a) {
	float a0 = a[0];
	float a1 = a[1];
	float a2 = a[2];
	float a3 = a[3];
	float a4 = a[4];

	// Next, we literally perform insertion sort on the five registers, 
	// swapping register values until a0 through a4 are in order. 
	// Because the memory is barely touched, this is fast: 

	if (a1 < a0) 	swap(a0, a1);
	if (a2 < a0)	swap(a0, a2); 
	if (a3 < a0)	swap(a0, a3);
	if (a4 < a0)	swap(a0, a4);

	if (a2 < a1)	swap(a1, a2);
	if (a3 < a1) 	swap(a1, a3);
	if (a4 < a1)	swap(a1, a4);

	if (a3 < a2)	swap(a2, a3);
	if (a4 < a2)	swap(a2, a4);

	// Although we now have the median value, we still need the median index. 
	// Although we could have watched the median index throughout the computation,
	// on my machine the extra registers required for this caused a great deal of spilling and slowdown. 
	// Instead, we simply compare the median value to each of the original array elements, 
	// returning the index that matches: 

	if (a2 == a[0])	return 0;
	if (a2 == a[1])	return 1;
	if (a2 == a[2])	return 2;
	if (a2 == a[3])	return 3;
	// else if (a2 == a[4])
		return 4;
}
//--------------------------------------------------------------
float DatumHistorian::getMedianValueOf5 (float *a){
	float a0 = a[0];
	float a1 = a[1];
	float a2 = a[2];
	float a3 = a[3];
	float a4 = a[4];

	if (a1 < a0) 	swap(a0, a1);
	if (a2 < a0)	swap(a0, a2); 
	if (a3 < a0)	swap(a0, a3);
	if (a4 < a0)	swap(a0, a4);
	if (a2 < a1)	swap(a1, a2);
	if (a3 < a1) 	swap(a1, a3);
	if (a4 < a1)	swap(a1, a4);
	if (a3 < a2)	swap(a2, a3);
	if (a4 < a2)	swap(a2, a4);
	
	return a2;
}
