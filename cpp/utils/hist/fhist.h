/*************************************************
 * Class for 1D histograms
 *
 * @author Felix Touchte Codjo
 * @date November 13,2024
 * **********************************************/

#ifndef FELIX_HIST_H
#define FELIX_HIST_H

#include <vector>

class fhist1D {
	const char * title;///< title
	int nbins; ///< number of bins
	double xmin; ///< lower x value
	double xmax; ///< upper x value
	double binWidth; ///<bin width
	unsigned long int entriesAll = 0; ///< number of entries
	std::vector<double> xarray; ///< array values (they correspond to the center of each bin)
	std::vector<int> warray; ///< array of weights
	unsigned long int underflow = 0; ///< number of entries lower xmin
	unsigned long int overflow = 0; ///< number of entries bigger than xmax

	// Statistic quantities
	bool flagStatOverFlows = false; ///< flag to use under/overflows
	unsigned long int entries = 0; ///< effective number of entries (taking or not into account under/overflows)
	
	/**
	 * @brief Statistics array
	 * - stats[0]  sum of w
	 * - stats[1]  sum of w*x
	 * - stats[2]  sum of w*x*x
	 * 
	 */
	std::vector<double> stats;
	unsigned long int wmin = 0; ///< min of weights for plotting
	unsigned long int wmax = 0; ///< max of weights for plotting

	public :
		fhist1D(const char * _title, int _nbins, double _xmin, double _xmax); 
		void Fill(double val);
		int    GetBinNumber(double val); 
		int GetWeight(int bin);
		double GetCentralValue(int bin);
		std::vector<double> GetXarray();
		std::vector<int> GetWarray();
		
		// Statistics
		double GetMean();
		double GetStdev();

		void SetFlagStatOverFlows(bool _flagStatOverFlows);

		void Print();
		void DrawContour(const char * filename);
};

class fhist2D {
	const char * title; ///< title
	int nbinsx; ///< number of bin in the x axis
	double xmin; ///< lower x value 
	double xmax; ///< upper x value
	int nbinsy; ///< number of bin in the y axis
	double ymin; ///< lower y value
	double ymax; ///< upper y value
	double binwx; ///< bin width in the x axis
	double binwy; ///< bin width in the y axis
	unsigned long int entriesAll = 0; ///< number of entries

	double * xarray = nullptr; ///< 1D array containing the center values of each x bins
	double * yarray = nullptr; ///< 1D array containing the center values of each y bins
	double ** warray = nullptr; ///< 2D array containing the the weight of each (x,y) entries
	
	unsigned long int xunderflow = 0; ///< number of underflow in the x axis 
	unsigned long int xoverflow = 0; ///< number of overflow in the x axis
	unsigned long int yunderflow = 0; ///< number of underflow in the y axis
	unsigned long int yoverflow = 0; ///< number of overflow in the y axis

	bool flagStatOverFlows = false; ///< flag for taking or not into account under/overflows

	unsigned long int entries = 0; ///< number of entries taking or not into account under/overflows

	/**
	 * @brief Statistics array
	 * 
	 * - stats[0]  sum of w
	 * - stats[1]  sum of w*x
	 * - stats[2]  sum of w*y
	 * - stats[3]  sum of w*x*x
	 * - stats[4]  sum of w*y*y
	 * - stats[5]  sum of w*x*y
	 */
	double * stats = nullptr;
	
	public :
		fhist2D(const char * _title, int _nbinsx, double _xmin, double _xmax, int _nbinsy, double _ymin, double _ymax);
		fhist2D(const fhist2D & hist);
		~fhist2D();
		void Fill(double xval, double yval);
		int GetBinNumberX(double xval) const;
		int GetBinNumberY(double yval) const;
		int GetWeightXY(int xbin, int ybin) const;
		void GetCentralValueXY(int xbin, int ybin, double & xval, double & yval) const;

		double CetXmean() const;
		double GetYmean() const;
		double GetXstdev() const;
		double GetYstdev() const;
		double GetCov() const;

		void SetFlagStatOverFlows(bool _flagStatOverFlows);

		void Print() const;

		fhist1D ProjectionX(int ystart, int yend) const;
		fhist1D ProjectionY(int xstart, int xend) const;
};

#endif
