/*************************************************
 * Class for 1D histograms
 *
 * @author Felix Touchte Codjo
 * @date November 13,2024
 * **********************************************/

#include "fhist.h"
#include <cmath>
#include <iostream>

#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"


/** @brief Constructor */
fhist1D::fhist1D(const char * _title, int _nbins, double _xmin, double _xmax) 
	: title(_title), nbins(_nbins), xmin(_xmin), xmax(_xmax), stats(3,0.0) {
	binWidth = (xmax-xmin)/nbins;
	for (int bin = 0; bin < nbins; bin++){
		xarray.push_back(xmin + (bin+0.5)*binWidth);
		warray.push_back(0);
	}
}

/** @brief Add a new entry in the histogram */
void fhist1D::Fill(double val){
	int bin = this->GetBinNumber(val);
	if (bin == -1){ 
		underflow++;
		if (flagStatOverFlows) {
			// Statistics
			stats[0] += 1;
			stats[1] += val;
			stats[2] += val*val;
		}
	}
	else if (bin == -11){
		overflow++;
		if (flagStatOverFlows) {
			// Statistics
			stats[0] += 1;
			stats[1] += val;
			stats[2] += val*val;
		}
	}
	else {
		warray[bin] += 1;
		entries++;
		// Statistics
		stats[0] += 1;
		stats[1] += val;
		stats[2] += val*val;
	}
	entriesAll++;
}

/**
 * @brief Return the bin number in which `val` falls
 * @note return -1 if val < xmin and -11 if val > xmax
 */
int fhist1D::GetBinNumber(double val){
	if (val < xmin) 
		return -1;
	if (val > xmax)
		return -11;
	if (val == xmax)
		return nbins-1;
	int thisbin = 0;
	for (int bin = 0; bin < nbins; bin++){
		if ((val >= xarray[bin]) && (val < xarray[bin+1])){
			thisbin = bin;
			break;
		}
	}
	return thisbin;
}


/**
 * @brief Return the number of entries in this bin
 *
 * @param bin bin number (numerotation starts at 0)
 * @note return -1 if bin < 0 or bin > `nBins`-1
 */
int fhist1D::GetWeight(int bin){
	if ((bin < 0) || (bin > nbins -1))
		return -1;
	return warray[bin];
}

/** @brief Return the center value of this bin */
double fhist1D::GetCentralValue(int bin){
	// Insérer une exception
	return xarray[bin];	
}

/** @brief Return the array of values
 * They are the center of each bins */
std::vector<double> fhist1D::GetXarray() {return xarray;}

/** @brief Return the array of weights */
std::vector<int> fhist1D::GetWarray() {return warray;}

/** @brief Return the mean of the distribution */
double fhist1D::GetMean(){
	if (stats[0] == 0) {return 0;}
	else {
		return stats[1]/stats[0];
	}
}

/** @brief Return the standard deviation of the distribution */
double fhist1D::GetStdev(){
	if (stats[0] == 0) {return 0;}
	else {
		return sqrt((stats[2]/stats[0]) - GetMean()*GetMean());
	}
}

/** @brief If true, over/underflow values taken into account in the statistics */
void fhist1D::SetFlagStatOverFlows(bool _flagStatOverFlows) { flagStatOverFlows = _flagStatOverFlows;}

/** @brief Print detailled informations on screen */
void fhist1D::Print(){
	std::cout << "***** " << title << std::endl;
	std::cout << "   mean       : " << GetMean() << std::endl;
	std::cout << "   stdev      : " << GetStdev() << std::endl;
	std::cout << "   entriesAll : " << entriesAll << std::endl;
	std::cout << "   entries    : " << entries << std::endl;
	std::cout << "   underflow  : " << underflow << std::endl;
	std::cout << "   overflow   : " << overflow << std::endl;
	std::cout << "   binNumber  : " << nbins << std::endl;
	std::cout << "   binwidth   : " << binWidth << std::endl;
}

/** brief Draw the contour of the histogram */
void fhist1D::DrawContour(const char * filename){
	TCanvas* canvas1 = new TCanvas("c1","c1 title",1366,768);
	TGraph* gr1 = new TGraph(nbins+2);
	gr1->SetPoint(0, xmin, 0);
	gr1->SetPoint(nbins+1, xmax, 0);
	for (int bin = 0; bin < nbins; bin++) {
		gr1->SetPoint(bin+1, xarray[bin], warray[bin]);
	}
	gr1->SetTitle(title);
	gr1->GetXaxis()->SetTitle("Value");
	gr1->GetXaxis()->SetTitleSize(0.05);
	gr1->GetYaxis()->SetTitle("Count");
	gr1->GetYaxis()->SetTitleSize(0.05);
	gr1->SetLineColor(kBlue);
	gr1->SetMarkerColor(kRed);
	gr1->SetMarkerSize(5);
	gr1->Draw("ALP");
	
	canvas1->Print(filename);	
}

fhist2D::fhist2D(const char * _title, int _nbinsx, double _xmin, double _xmax, int _nbinsy, double _ymin, double _ymax) 
	: title(_title), nbinsx(_nbinsx), xmin(_xmin), xmax(_xmax), nbinsy(_nbinsy), ymin(_ymin), ymax(_ymax) {
	binwx = (xmax-xmin)/nbinsx;
	binwy = (ymax-ymin)/nbinsy;
	// allocate space for xarray
	if (!xarray)
		delete[] xarray;
	xarray = new double[nbinsx];
	// fill xarray
	for (int bin = 0; bin < nbinsx; bin++){
		xarray[bin] = xmin + (bin+0.5)*binwx;
	}
	// allocate space for yarray
	if (!yarray)
		delete[] yarray;
	yarray = new double[nbinsy];
	// fill yarray
	for (int bin = 0; bin < nbinsy; bin++){
		yarray[bin] = ymin + (bin+0.5)*binwy;
	}
	// allocate space for warray
	if (!warray)
		delete[] warray;
	warray = new double*[nbinsx];
	for (int bin = 0; bin< nbinsx; bin++) {
		if (!warray[bin])
			delete warray[bin];
		warray[bin] = new double[nbinsy];
	}
	// initialise warray
	for (int xbin = 0; xbin < nbinsx; xbin++){
		for (int ybin = 0; ybin < nbinsy; ybin++){
			warray[xbin][ybin] = 0;
		}
	}
	// allocate space for stats
	if (!stats)
		delete[] stats;
	stats = new double[6];
	// initialise stats
	for (int itr = 0; itr < 6; itr++){
		stats[itr] = 0;
	}
}

fhist2D::fhist2D(const fhist2D & hist)	: title(hist.title) {
	this->nbinsx = hist.nbinsx;
	this->xmin = hist.xmin;
	this->xmax = hist.xmax;
	this->nbinsy = hist.nbinsy;
	this->ymin = hist.ymin;
	this->ymax = hist.ymax;
	this->binwx = hist.binwx;
	this->binwy = hist.binwy;
	this->entriesAll = hist.entriesAll;
	this->xunderflow = hist.xunderflow;
	this->xoverflow = hist.xoverflow;
	this->yunderflow = hist.yunderflow;
	this->yoverflow = hist.yoverflow;
	this->flagStatOverFlows = hist.flagStatOverFlows;
	this->entries = hist.entries;
	// free and allocate memory space for this xarray, yarray, warray and stats
	if (!xarray)
		delete[] xarray;
	if (!yarray)
		delete[] yarray;
	for (int bin = 0; bin < nbinsx; bin++){
		if (!warray[bin])
			delete[] warray[bin];
	}
	if (!warray)
		delete[] warray;
	warray = new double*[this->nbinsx];
	for (int bin = 0; bin < nbinsx; bin++){
		if (!warray[bin])
			delete[] warray[bin];
		warray[bin] = new double[nbinsy];
	}
	if (!this->stats)
		delete[] stats;
	stats = new double[6];
	// copy the quantities
	for (int bin = 0; bin < nbinsx; bin++){
		this->xarray[bin] = hist.xarray[bin];
	}
	for (int bin = 0; bin < nbinsy; bin++){
		this->yarray[bin] = hist.yarray[bin];
	}
	for (int xbin = 0; xbin < nbinsx; xbin++){
		for (int ybin = 0; ybin < nbinsy; ybin++){
			this->warray[xbin][ybin] = hist.warray[xbin][ybin];
		}
	}
	for (int itr = 0; itr < 6; itr++){
		this->stats[itr] = hist.stats[itr];
	}
}

fhist2D::~fhist2D(){
	if (!xarray)
		delete[] xarray;
	if (!yarray)
		delete[] yarray;
	for (int bin = 0; bin < nbinsx; bin++){
		if (!warray[bin])
			delete[] warray[bin];
	}
	if (!warray)
		delete[] warray;
	if (!stats){
		delete[] stats;
	}
}

void fhist2D::Fill(double xval, double yval){
	int xbin = GetBinNumberX(xval);
	int ybin = GetBinNumberY(yval);
	/*if (xbin == -1){
		xunderflow++;
		if (flagStatOverFlows){
			// Statistics
			stats[0] += 1;
			stats[1] += xval;
			stats[2] += yval;
			stats[3] += xval*xval;
			stats[4] += yval*yval;
			stats[5] += xval*yval;
		}
	}
	warray[xbin][ybin] += 1;

	if (flagStatOverFlows){ 
		
	}*/
	/**
	 * ça m'a l'air très long
	 *
	 *
	 *
	 */
	warray[xbin][ybin] += 1;
	entriesAll++;
}


/**
 * @brief Return the x bin number in which `xval` falls
 * @note return -1 if val < xmin and -11 if val > xmax
 */
int fhist2D::GetBinNumberX(double xval) const{
	if (xval < xmin) 
		return -1;
	if (xval > xmax)
		return -11;
	if (xval == xmax)
		return nbinsx-1;
	int thisbin = 0;
	for (int bin = 0; bin < nbinsx; bin++){
		if ((xval >= xarray[bin]) && (xval < xarray[bin+1])){
			thisbin = bin;
			break;
		}
	}
	return thisbin;
}


/**
 * @brief Return the x bin number in which `xval` falls
 * @note return -1 if val < xmin and -11 if val > xmax
 */
int fhist2D::GetBinNumberY(double yval) const{
	if (yval < ymin) 
		return -1;
	if (yval > ymax)
		return -11;
	if (yval == ymax)
		return nbinsy-1;
	int thisbin = 0;
	for (int bin = 0; bin < nbinsy; bin++){
		if ((yval >= yarray[bin]) && (yval < yarray[bin+1])){
			thisbin = bin;
			break;
		}
	}
	return thisbin;
}


void fhist2D::Print() const{

}
