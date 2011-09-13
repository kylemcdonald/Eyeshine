/* "DatumHistorian.h" */
#ifndef _DATUM_HISTORIAN
#define _DATUM_HISTORIAN

//--------------------------------------------
// for MIN, MAX etc.
#include "ofMain.h"

//--------------------------------------------
// include these 2 lines for median-of-5 code:
#include <algorithm>
using std::swap;

//--------------------------------------------
class DatumHistorian {

	public:
		DatumHistorian ();
		DatumHistorian (int hlen);
		
		void	updateHistory (float newval);
		void 	updateHistoryWhileClampingOutliers (float newval);
		int 	historyLength;
		
		float getMostRecent();
		float getRunningAverage (float alpha);
		float getRunningAverageOf5Median (float alpha);
		float getRunningAverageOfMedianOfN (float alpha, int N);
	
		/*
		float getStandardDeviation();
		float getStandardDeviation(int len);
		 */
		
		float	getMedianValueFromMostRecentN (int N);
		int	getMedianIndexOf5History();
		float	getMedianValueOf5History();
		void 	setHistory (float* a, int nvals);
		float *getHistory ();
		int	getMaxHistoryLength();
		void 	clear();

	private:
		
		int 	maxHistoryLength;
		int	getMedianIndexOf5 (float *a);
		float	getMedianValueOf5 (float *a);
		
		float *history;
		float	*sortedValues;
		float *tempValues;
		float runningAverage;
		float	currentMedian;
		bool	bRunningAverageRequested;
	
};

#endif	/* _DATUM_HISTORIAN */ 