/****************************************************
 * This code generates histograms of AHDC::adc
 * bank entries just after the use of the 
 * ahdcExtractor.
 *
 * @author Felix Touchte Codjo
 * @date November 23, 2024
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

	// 1D Histograms (AHDC::adc entries from extractor output)
	double tmin = 0; // time plot limit
	double tmax = 2000;
	int bins = 200;
	TH1D* hist1d_adcMax = new TH1D("hist1d_adcMax", "histogram : adcMax (adc)",bins,0,ahdcExtractor::ADC_LIMIT);
	TH1D* hist1d_integral = new TH1D("hist1d_integral", "histogram : integral (adc per 44 ns)",bins,0,60000);
	TH1D* hist1d_mcEtot = new TH1D("hist1d_mcEtot", "histogram : mcEtot (keV)",bins,0,100);
	TH1D* hist1d_adcOffset = new TH1D("hist1d_adcOffset", "histogram : adcOffest (adc)",bins,100,500);
	TH1D* hist1d_timeMax = new TH1D("hist1d_timeMax", "histogram : timeMax (ns)",bins,tmin,tmax);
	TH1D* hist1d_timeRiseCFA = new TH1D("hist1d_timeRiseCFA", "histogram : timeRiseCFA (ns)",bins,tmin,tmax);
	TH1D* hist1d_timeOT = new TH1D("hist1d_timeOT", "histogram : timeOT (ns)",bins,tmin,tmax);
	TH1D* hist1d_timeCFD = new TH1D("hist1d_timeCFD", "histogram : timeCFD (ns)",bins,tmin,tmax);
	TH1D* hist1d_mctime = new TH1D("hist1d_mctime", "histogram : mctime (ns)",bins,tmin,400);

	// loop over events
	while( r.next(list)){
		//if (nEvent % 1000 == 0)
		//	std::cout << "***** starting event " << nEvent << std::endl;
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
			double integral = output["integral"];
			double mcEtot = list[0].getFloat("mcEtot",col);
			double adcOffset = output["adcOffset"];
			double timeMax = output["timeMax"];
			double timeRiseCFA = output["timeRiseCFA"];
			double timeOT = output["timeOverThresholdCFA"];
			double timeCFD = output["timeCFD"];
			double mctime = list[0].getFloat("mctime",col);
			//timeMax -= mctime;
			//timeRiseCFA -= mctime;
			//timeCFD -= mctime;
			hist1d_adcMax->Fill(adcMax);
			hist1d_integral->Fill(integral);
			hist1d_mcEtot->Fill(mcEtot);
			hist1d_adcOffset->Fill(adcOffset);
			hist1d_timeMax->Fill(timeMax);
			hist1d_timeRiseCFA->Fill(timeRiseCFA);
			hist1d_timeCFD->Fill(timeCFD);
			hist1d_timeOT->Fill(timeOT);
			hist1d_mctime->Fill(mctime);
		} // end loop over columns of AHDC::wf:136
		nEvent++;
	} // end loop over events

	// Plots
	//TCanvas* canvas1 = new TCanvas("c1","c1 title",2000,3000);
	TCanvas* canvas1 = new TCanvas("c1","c1 title",3500,2000);
	canvas1->Divide(3,3); // (nx,ny)
	gStyle->SetOptStat("nemruo");

	// adcMax
        canvas1->cd(1);
        hist1d_adcMax->GetXaxis()->SetTitle("adcMax (adc)");
        hist1d_adcMax->GetXaxis()->SetTitleSize(0.05);
        hist1d_adcMax->GetYaxis()->SetTitle("count");
        hist1d_adcMax->GetYaxis()->SetTitleSize(0.05);
        hist1d_adcMax->Draw();
	
	// integral 
        canvas1->cd(2);
        hist1d_integral->GetXaxis()->SetTitle("integral (adc per 44 ns)");
        hist1d_integral->GetXaxis()->SetTitleSize(0.05);
        hist1d_integral->GetYaxis()->SetTitle("count");
        hist1d_integral->GetYaxis()->SetTitleSize(0.05);
        hist1d_integral->Draw();

	// mcEtot
        canvas1->cd(3);
        hist1d_mcEtot->GetXaxis()->SetTitle("mcEtot (keV)");
        hist1d_mcEtot->GetXaxis()->SetTitleSize(0.05);
        hist1d_mcEtot->GetYaxis()->SetTitle("count");
        hist1d_mcEtot->GetYaxis()->SetTitleSize(0.05);
        hist1d_mcEtot->Draw();

	// adcOffset
        canvas1->cd(4);
        hist1d_adcOffset->GetXaxis()->SetTitle("adcOffset (adc)");
        hist1d_adcOffset->GetXaxis()->SetTitleSize(0.05);
        hist1d_adcOffset->GetYaxis()->SetTitle("count");
        hist1d_adcOffset->GetYaxis()->SetTitleSize(0.05);
        hist1d_adcOffset->Draw();
	
	
	// timeMax
        canvas1->cd(5);
        hist1d_timeMax->GetXaxis()->SetTitle("timeMax (ns)");
        hist1d_timeMax->GetXaxis()->SetTitleSize(0.05);
        hist1d_timeMax->GetYaxis()->SetTitle("count");
        hist1d_timeMax->GetYaxis()->SetTitleSize(0.05);
        hist1d_timeMax->Draw();

	// timeRiseCFA
        canvas1->cd(6);
        hist1d_timeRiseCFA->GetXaxis()->SetTitle("timeRiseCFA (ns)");
        hist1d_timeRiseCFA->GetXaxis()->SetTitleSize(0.05);
        hist1d_timeRiseCFA->GetYaxis()->SetTitle("count");
        hist1d_timeRiseCFA->GetYaxis()->SetTitleSize(0.05);
        hist1d_timeRiseCFA->Draw();
	
	// timeCFD
        canvas1->cd(7);
        hist1d_timeCFD->GetXaxis()->SetTitle("timeCFD (ns)");
        hist1d_timeCFD->GetXaxis()->SetTitleSize(0.05);
        hist1d_timeCFD->GetYaxis()->SetTitle("count");
        hist1d_timeCFD->GetYaxis()->SetTitleSize(0.05);
        hist1d_timeCFD->Draw();
	
	// timeOT
        canvas1->cd(8);
        hist1d_timeOT->GetXaxis()->SetTitle("timeOT (ns)");
        hist1d_timeOT->GetXaxis()->SetTitleSize(0.05);
        hist1d_timeOT->GetYaxis()->SetTitle("count");
        hist1d_timeOT->GetYaxis()->SetTitleSize(0.05);
        hist1d_timeOT->Draw();
	
	// mctime
        canvas1->cd(9);
        hist1d_mctime->GetXaxis()->SetTitle("mctime (ns)");
        hist1d_mctime->GetXaxis()->SetTitleSize(0.05);
        hist1d_mctime->GetYaxis()->SetTitle("count");
        hist1d_mctime->GetYaxis()->SetTitleSize(0.05);
        hist1d_mctime->Draw();
	
	// output
	canvas1->Print(TString::Format("histAhdcAdc.pdf"));
	
}
