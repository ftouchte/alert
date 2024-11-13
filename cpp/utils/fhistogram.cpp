/*************************************************
 * Class for 1D histograms
 *
 * @author Felix Touchte Codjo
 * @date November 13,2024
 * **********************************************/

#include "fhistogram.h"

/** Constructor */
FH1D::FH1D(const char * _title, int _nBins, double _xmin, double _xmax) 
	: title(_title), nBins(_nBins), xmin(_xmin), xmax(_xmax) {
	binWidth = (xmax-xmin)/nBins;
	for (int bin = 0; bin < nBins; bin++){
		Values.push_back(xmin + 1.5*bin*binWidth);
		Weights.push_back(0);
	}
}

/** Add a new entry in the histogram */
void FH1D::Fill(double val){
	int bin = this->GetBinNumber(val);
	if (bin == -1) 
		underflow++;
	if (bin == -11){
		overflow++;
	}
	Weights[i]++;
	nEntries++;
}

/**
 * Return the bin number in which `val` falls
 * @note return -1 if val < xmin and -11 if val > xmax
 */
int FH1D::GetBinNumber(double val){
	for (int bin = 0; bin < binNumber; bin++){
		if (val < xmin)
			return -1;
		if (val > xmax)
			return -11;
		if (val == xmax) 
			return nBins - 1;
		if ((val >= Values[bin]) && (val < Values[bin+1]))
			return bin;
	}
}


/**
 * Return the number of entries in this bin
 *
 * @param bin bin number (numerotation starts at 0)
 * @note return -1 if bin < 0 or bin > `nBins`-1
 */
int FH1D::GetWeight(int bin){
	if ((bin < 0) || (bin > nBins -1))
		return -1;
	return Weights[bin];
}

double FH1D::GetCentralValue(int bin){
	// Insérer une exception
	return Values[bin];	
}

std::vector<double> FH1D::GetValues() {return Values;}
std::vector<double> FH1D::GetWeights() {return Weights;}

