/*************************************************
 * Class for 1D histograms
 *
 * @author Felix Touchte Codjo
 * @date November 13,2024
 * **********************************************/

#ifndef FELIX_HIST_H
#define FELIX_HIST_H

#include <vector>

class FH1D {
	double xmin; ///< lower x value
	double xmax; ///< upper x value
	int nBins; ///< number of bins
	double binWidth; ///<bin width
	unsigned long int nEntries = 0; ///< Number of entries
	std::vector<double> Values; ///< array values (they correspond to the center of each bin)
	std::vector<int> Weights; ///< array of weights
	unsigned long int underflow = 0;
	unsigned long int overflow = 0;

	const char * title;
	//const char * xtitle;
	//const char * ytitle;

	public :
		FH1D(const char * _title, int _nBins, double _xmin, double _xmax); 
		// : title(_title), nBins(_nBins), xmin(_xmin), xmax(_xmax) {}
		void Fill(double val);
		int    GetBinNumber(double val); 
		double GetWeight(int bin);
		double GetCentralValue(int bin);
		std::vector<double> GetValues();
		std::vector<double> GetWeights();



};


#endif
