/**********************************************
 * This code is a simple test or use of the 
 * ahdcExtractor class.
 *
 * @author Felix Touchte Codjo
 * *******************************************/

#include "ahdcExtractor.h"
#include "reader.h"

#include <string>
#include <iostream>

int main(int argc, char const *argv[]){
	
	if (argc < 2) {
		std::cout << "Please, provide a filename..." << std::endl;
		std::cout << "Example" << std::endl; 
		std::cout << "   ./simpleTest.exe alert_10k.hipo" << std::endl;
		return 0;
	}

	using namespace std::__cxx11; // for the to_string() method
	const char* filename = argv[1];
	hipo::reader  r(filename);
	hipo::banklist list = r.getBanks({"AHDC::adc","AHDC::wf:136"});
	long unsigned int nEvent =0;
	
	// loop over events
	while( r.next(list)){
		if (nEvent != 0) {break;}
		std::cout << "***** event " << nEvent << std::endl;
		// loop over columns of AHDC::wf:136
		for(int col = 0; col < list[1].getRows(); col++){ 
			if (col != 0) {break;}
			int layer = list[1].getInt("layer",col);
			int component = list[1].getInt("component",col);
			std::vector<short> samples;
			for (int bin=0; bin < 136; bin++){
				std::string binName = "s" + to_string(bin+1);
				short thisSample = list[1].getInt(binName.c_str(),col);
				samples.push_back(thisSample);
			}
			ahdcExtractor T(44,0.5f,5,0.3f);
			T.adcOffset = (short) (samples[0] + samples[1] + samples[2] + samples[3] + samples[4])/5;
			std::map<std::string,double> output = T.extract(samples);
		
			std::cout << "   layer = " << layer << std::endl;
			std::cout << "   component = " << component << std::endl;
			std::cout << "   adcMax = " << output["adcMax"] << std::endl;
			std::cout << "   integral = " << output["integral"] << std::endl;
			std::cout << "   timeMax = " << output["timeMax"] << std::endl;
			std::cout << "   timeRiseCFA = " << output["timeRiseCFA"] << std::endl;
			std::cout << "   timeOVR = " << output["timeOverThresholdCFA"] << std::endl;
			std::cout << "   timeCFD = " << output["timeCFD"] << std::endl;
			
			std::string outputName1 = "SignalDecoded_e" + to_string(nEvent) + "l" + to_string(layer) + "c" + to_string(component) + ".pdf"; 
			std::string outputName2 = "SignalCFD_e" + to_string(nEvent) + "l" + to_string(layer) + "c" + to_string(component) + ".pdf";
			T.Show(outputName1.c_str());
			T.ShowCFD(outputName2.c_str());

		} // end loop over columns of AHDC::wf:136
	nEvent++;
	} // end loop over events
}
