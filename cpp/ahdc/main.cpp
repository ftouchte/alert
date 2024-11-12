#include "ahdcExtractor.h"
#include "reader.h"

#include <string>
#include <iostream>

int main(int argc, char const *argv[]){
	using namespace std::__cxx11;
	std::cout << "argc : " << argc << std::endl;
	if (argc < 2) {
		std::cout << "Please, provide a filename..." << std::endl;
		return 0;
	}
	//const char* filename = "/homeijclab/touchte-codjo/Bureau/alert/data/alert_10k.hipo";
	const char* filename = argv[1];
	hipo::reader  r(filename);
	hipo::banklist list = r.getBanks({"AHDC::adc","AHDC::wf:136"});
	long unsigned int nEvent =0;

	std::vector<short> samples;	
	// loop over events
	while( r.next(list)){
		// ...
		if (nEvent != 0) {break;}
		std::cout << "***** event " << nEvent << std::endl;
		// loop over columns
		for(int itr = 0; itr < list[1].getRows(); itr++){ 
			// Inside, you can read each row
			// ...
			if (itr != 0) {break;}
			std::vector<short> samples;
			for (int bin=0; bin < 136; bin++){
				std::string binName = "s" + to_string(bin+1);
				short sample = list[1].getInt(binName.c_str(),itr);
				samples.push_back(sample);
				//std::cout << binName << " : " << sample << std::endl; 
			}
			ahdcExtractor T(44,0.5f,5,0.3f);
			T.adcOffset = (short) (samples[0] + samples[1] + samples[2] + samples[3] + samples[4])/5;
			std::map<std::string,double> output = T.extract(samples);
			
			std::cout << "   adcMax = " << output["adcMax"] << std::endl;
			std::cout << "   integral = " << output["integral"] << std::endl;
			std::cout << "   timeMax = " << output["timeMax"] << std::endl;
			std::cout << "   timeRiseCFA = " << output["timeRiseCFA"] << std::endl;
			std::cout << "   timeOVR = " << output["timeOverThresholdCFA"] << std::endl;
			std::cout << "   timeCFD = " << output["timeCFD"] << std::endl;

		}
	nEvent++;
	}
}
