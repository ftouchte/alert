/****************************************************
 * This code studies the correlation between the
 * various extracted time (timeMax, timeRiseCFA, 
 * timeCFD, timeOverThresholdCFA) and the amplitude (adcMax) 
 * of the signal.
 *
 * @author Felix Touchte Codjo
 * @date November 13, 2024
 * *************************************************/

#include "ahdcExtractor.h"
#include "reader.h"

#include <string>
#include <iostream>

#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TString.h"


int main(int argc, char const *argv[]){
	
	if (argc < 2) {
		std::cout << "Please, provide a filename..." << std::endl;
		return 0;
	}

	using namespace std::__cxx11; // for the to_string() method
	// open file and read bank
	const char* filename = argv[1];
	hipo::reader  r(filename);
	hipo::banklist list = r.getBanks({"AHDC::adc","AHDC::wf:136"});
	long unsigned int nEvent =0;
	
	// Setting parameters
	double amplitudeFractionCFA = 0.5;
	double fractionCFD = 0.3;
	int binDelayCFD = 5;

	// 2D Histograms : Time (X) versus Amplitude (Y)
	int nBinX = 200;
	int nBinY = 200;
	double tmin = 0;
	double tmax = 2000;
	TH2D* hist2d_timeMax = new TH2D("hist2d_timeMax", "timeMax vs adcMax", nBinX, tmin, tmax, nBinY, 0, ahdcExtractor::ADC_LIMIT);
	TH2D* hist2d_timeRiseCFA = new TH2D("hist2d_timeRiseCFA", TString::Format("timeRiseCFA vs adcMax, fraction = %.2f",amplitudeFractionCFA), nBinX, tmin, tmax, nBinY, 0, ahdcExtractor::ADC_LIMIT);
	TH2D* hist2d_timeOT = new TH2D("hist2d_timeOT", TString::Format("timeOverThresholdCFA vs adcMax, fraction = %.2f",amplitudeFractionCFA), nBinX, tmin, tmax, nBinY, 0, ahdcExtractor::ADC_LIMIT);
	TH2D* hist2d_timeCFD = new TH2D("hist2d_timeCFD", TString::Format("timeCFD vs adcMax, fraction = %.2f, binDelay = %d",fractionCFD,binDelayCFD), nBinX, tmin, tmax, nBinY, 0, ahdcExtractor::ADC_LIMIT);

	// loop over events
	while( r.next(list)){
		if (nEvent % 1000 == 0)
			std::cout << "***** starting event " << nEvent << std::endl;
		// loop over columns of AHDC::wf:136
		for(int col = 0; col < list[1].getRows(); col++){ 
			std::vector<short> samples; // waveform samples
			for (int bin=0; bin < 136; bin++){
				std::string binName = "s" + to_string(bin+1);
				short thisSample = list[1].getInt(binName.c_str(),col);
				samples.push_back(thisSample);
			}
			ahdcExtractor T(44.0,amplitudeFractionCFA,binDelayCFD,fractionCFD);
			T.adcOffset = (short) (samples[0] + samples[1] + samples[2] + samples[3] + samples[4])/5;
			std::map<std::string,double> output = T.extract(samples);
			
			// Fill histograms
			double adcMax = output["adcMax"];
			double timeMax = output["timeMax"];
			double timeRiseCFA = output["timeRiseCFA"];
			double timeOT = output["timeOverThresholdCFA"];
			double timeCFD = output["timeCFD"];
			double mctime = list[0].getFloat("mctime",col);
			timeMax -= mctime;
			timeRiseCFA -= mctime;
			timeCFD -= mctime;
			hist2d_timeMax->Fill(timeMax,adcMax);
			hist2d_timeRiseCFA->Fill(timeRiseCFA,adcMax);
			hist2d_timeCFD->Fill(timeCFD,adcMax);
			hist2d_timeOT->Fill(timeOT,adcMax);

		} // end loop over columns of AHDC::wf:136
	nEvent++;
	} // end loop over events

	// Plots
	//TCanvas* canvas1 = new TCanvas("c1","c1 title",2000,3000);
	TCanvas* canvas1 = new TCanvas("c1","c1 title",3500,2000);
	//canvas1->Divide(1,3);
	canvas1->Divide(2,2);
	gStyle->SetOptStat("nemruo");

	// timeMax vs adcMax
	canvas1->cd(1);
	hist2d_timeMax->GetXaxis()->SetTitle("timeMax");
	hist2d_timeMax->GetXaxis()->SetTitleSize(0.05);
	hist2d_timeMax->GetYaxis()->SetTitle("adcMax");
	hist2d_timeMax->GetYaxis()->SetTitleSize(0.05);
	hist2d_timeMax->Draw("COLZ");

	// timeRiseCFA vs adcMax
	canvas1->cd(2);
	hist2d_timeRiseCFA->GetXaxis()->SetTitle("timeRiseCFA");
	hist2d_timeRiseCFA->GetXaxis()->SetTitleSize(0.05);
	hist2d_timeRiseCFA->GetYaxis()->SetTitle("adcMax");
	hist2d_timeRiseCFA->GetYaxis()->SetTitleSize(0.05);
	hist2d_timeRiseCFA->Draw("COLZ");
	
	// timeCFD vs adcMax
	canvas1->cd(3);
	hist2d_timeCFD->GetXaxis()->SetTitle("timeCFD");
	hist2d_timeCFD->GetXaxis()->SetTitleSize(0.05);
	hist2d_timeCFD->GetYaxis()->SetTitle("adcMax");
	hist2d_timeCFD->GetYaxis()->SetTitleSize(0.05);
	hist2d_timeCFD->Draw("COLZ");

	// timeOT vs adcMax
	canvas1->cd(4);
	hist2d_timeOT->GetXaxis()->SetTitle("timeOT");
	hist2d_timeOT->GetXaxis()->SetTitleSize(0.05);
	hist2d_timeOT->GetYaxis()->SetTitle("adcMax");
	hist2d_timeOT->GetYaxis()->SetTitleSize(0.05);
	hist2d_timeOT->Draw("COLZ");

	// output
	canvas1->Print(TString::Format("CorrelationTimeAmplitude.pdf"));
	
}
