#ifndef AHDC_SIGNAL_H
#define AHDC_SIGNAL_H


/**
 * @class ahdcSignal
 * 
 * @brief ahdc signal simulation
 *
 * This class simulates the waveform of the ahdc signal and provide 
 * algorithms to extract relevant informations from this signal.
 *
 * As it is written, it can only be used on the ifarm :-(
 *
 * @author Felix Touchte Codjo
 */
class ahdcSignal {
	// MHit or wires identifiers
	public : 
		int hitn; ///< n-th MHit of the event, also corresponds to the n-th activated wire
		int sector; ///< sector, first wire identifier
		int layer; ///< layer, second wire identifer
		int component; ///< component, third wire identifier
		int nsteps; ///< number of steps in this MHit, i.e number of Geant4 calculation steps in the sensitive area of the wire 
	// vectors
	private :
		std::vector<double> Edep; ///< array of deposited energy in each step [keV]
		std::vector<double> G4Time; ///< array of Geant4 time corresponding to each step [ns]
		std::vector<double> Doca; ///< array of distance of closest approach corresponding each step [mm]
		std::vector<double> DriftTime; ///< array of drift time corresponding each step [ns]
		
		/**
		 * @brief Fill the arrays Doca and DriftTime
		 * 
		 * Compute the doca corresponding to each step and
		 * deducte the driftime using a "time to distance"
		 * relation
		 *
		 * @param aHit an object derived from Geant4 "G4VHit" class
		 */
		void ComputeDocaAndTime(MHit * aHit);
		std::vector<short> Dgtz; ///< Array containing the samples of the simulated signal
		std::vector<short> Noise; ///< Array containing the samples of the simulated noise
	// setting parameters for digitization
	private : 
		const double tmin; ///< lower limit of the simulated time window
		const double tmax; ///< upper limit of the simulated time window
		const double timeOffset; ///< time offset for simulation purpose
		const double samplingTime; ///< sampling time [ns]
		const double Landau_width; ///< Width pararemeter of the Landau distribution
		double electronYield = 9500;   ///< ADC gain
		static const int ADC_LIMIT = 4095; ///< ADC limit, corresponds to 12 digits : 2^12-1
	// public methods
	public :
		/** @brief Default constructor */
		ahdcSignal() = default;
		
		/** @brief Constructor */
		ahdcSignal(MHit * aHit, int _hitn, double _tmin, double _tmax, double _timeOffset, double _samplingTime, double _Landau_width) 
		: tmin(_tmin), tmax(_tmax), timeOffset(_timeOffset), samplingTime(_samplingTime), Landau_width(_Landau_width) {
			// read identifiers
			hitn = _hitn;
			vector<identifier> identity = aHit->GetId();
			sector = 0;
			layer = 10 * identity[0].id + identity[1].id ; // 10*superlayer + layer
			component = identity[2].id;
			// fill vectors
			Edep = aHit->GetEdep();
			nsteps = Edep.size();
			for (int s=0;s<nsteps;s++){Edep.at(s) = Edep.at(s)*1000;} // convert MeV to keV
			G4Time = aHit->GetTime();
			this->ComputeDocaAndTime(aHit); // fills Doca and DriftTime
		}
		
		/** @brief Destructor */
		~ahdcSignal(){;}
		
		/** @brief Return the value of the attribut `electronYield` */
		double GetElectronYield() {return electronYield;}
		
		/** @brief Return the content of the attribut `Edep` */
		std::vector<double>                     GetEdep() 		{return Edep;}
		
		/** @brief Return the content of the attribut `G4Time` */
		std::vector<double>                     GetG4Time()		{return G4Time;}
		
		/** @brief Return the content of the attribut `Doca` */
		std::vector<double>                     GetDoca()		{return Doca;}
		
		/** @brief Return the content of the attribut `DriftTime` */
		std::vector<double>                     GetDriftTime()		{return DriftTime;}
		
		/** @brief Return the content of the attribut `Noise` */
		std::vector<short>                     GetNoise()              {return Noise;}
		
		/** @brief Return the content of the attribut `Dgtz` */
		std::vector<short> 			GetDgtz()		{return Dgtz;}
		
		/**
		 * @brief Set the electron yield. 
		 * 
		 * Only relevant before the use of the method `Digitize`
		 */
		void SetElectronYield(double electronYield_)		{electronYield = electronYield_;}
		
		/**
		 * @brief Overloaded `()` operator to get the value of the signal at a given time.
		 * 
		 * @param t Time at which to calculate the signal's value
		 *
		 * @return Value of the signal at the time `t`
		 */
		double operator()(double timePoint){
			using namespace Genfun;
			double signalValue = 0;
			for (int s=0; s<nsteps; s++){
				// setting Landau's parameters
				Landau L;
				L.peak() = Parameter("Peak",DriftTime.at(s),tmin,tmax); 
				L.width() = Parameter("Width",Landau_width,0,400); 
				signalValue += Edep.at(s)*L(timePoint-timeOffset);
			}
			return signalValue;
		}
		
		/**
		 * @brief Digitize the simulated signal
		 *
		 * This method perfoms several steps
		 * - step 1 : it produces samples from the simulated signal (using `samplingTime`)
		 * - step 2 : it converts keV/ns in ADC units (using `electronYield`)
		 * - step 3 : it adds noise
		 *
		 * @return Fill the attributs `Dgtz` and `Noise` 
		 */
		void Digitize();

		/** @brief Generate gaussian noise
		 *
		 * @return Fill the attribut `Noise`
		 */
		void GenerateNoise(double mean, double stdev);
		
		double GetMCTime(); // tmp
		double GetMCEtot(); // tmp
		
		/**
		 * @brief Extract various informations from the digitized signal 
		 *
		 * This method computes
		 * - `binMax`, `binOffset`, `adcMax`, `timeMax`, `integral` 
		 * - `timeRiseCFA`, `timeFallCFA`, `timeOverThresholdCFA`, `timeCFD` 
		 */
		std::map<std::string,double> Extract(bool expression = true);

		void PrintBeforeProcessing(bool expression = true); // e.g : this->PrintBeforeProcessing(this->nsteps > 10)
		void PrintAllShapes(bool expression = true);
		void PrintAfterProcessing(bool expression = true);
		void PrintNoise(bool expression = true);
		void PrintSignal(bool expression = true);
};



#endif


