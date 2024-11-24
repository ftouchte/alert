/*************************************************
 * Monitor 
 *
 * @author Felix Touchte Codjo
 * @date November 20, 2024
 * **********************************************/

#include "ahdcExtractor.h"
#include "AhdcGeom.h"
#include "reader.h"

#include <string>
#include <iostream>
#include <cmath>

#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TString.h"
#include "TLegend.h"
#include "TGaxis.h"
#include "TMultiGraph.h"
#include "TLatex.h"

class DepositCircle : public TGraph {
	double x; ///< x-coordiante of the center
	double y; ///< y-coordinate of the center
	double rmax = 2; ///< maximum radius
	double fraction; ///< between 0 and 1, `fraction*rmax` is the real radius of the circle
	int resolution; ///< number of points to draw the circle
public :
	DepositCircle(double _x, double _y, double _fraction = 0.5, int _resolution = 100) : TGraph(_resolution+1), x(_x), y(_y), fraction(_fraction), resolution(_resolution) {
		for (int k = 0; k < resolution; k++){
			this->SetPoint(k,x+rmax*fraction*cos(2*M_PI*k/resolution),y+rmax*fraction*sin(2*M_PI*k/resolution)); // set circle points
		}
		this->SetPoint(resolution,x+rmax*fraction,0); // close the circle
	}
	void SetRmax(double _rmax) {rmax = _rmax;}
	/*void Draw(Option_t* chopt = "F") override {
		//this->SetFillStyle(3001);
		this->SetFillColorAlpha(kRed,1.0);
		this->TGraph::Draw(chopt);
	}*/
};


int main(int argc, char const *argv[]){
	
	if (argc < 2) {
                std::cout << "Please, provide a filename..." << std::endl;
                return 0;
        }
	using namespace std::__cxx11; // for the to_string() method
	using namespace std; 

	TCanvas* canvas1 = new TCanvas("c1","c1 title",2000,3000);

	/*********************************************
	 * A representation of the top face (z=0 cut)
	 * of the AHDC stored in the multigraph mg
	 * ******************************************/
	AhdcGeom ahdc;
	TMultiGraph  *mg  = new TMultiGraph();
	TMultiGraph  *mg_adcMax  = new TMultiGraph();
	TMultiGraph  *mg_timeOT  = new TMultiGraph();
	TMultiGraph  *mg_integral  = new TMultiGraph();
	// title
	mg_adcMax->SetTitle("adcMax");
	mg_timeOT->SetTitle("TimeOT");
	mg_integral->SetTitle("integral");

	TLegend* legend = new TLegend(); 
	//for (int entry = 0; entry < ahdc.GetDataBaseSize(); entry++) {
	// loop over (super)layers
	for (int entry = ahdc.GetDataBaseSize()-1; entry >= 0; entry--) {
		int info = ahdc.GetDataBaseEntry(entry);
		int sector = info/10000;
		int superlayer = (info-10000*sector)/1000;
		int layer = (info-10000*sector-1000*superlayer)/100;
		int NumberOfWires = info-10000*sector-1000*superlayer-100*layer;
		TGraph* gr1t = new TGraph(NumberOfWires);
		for (int component = 1; component <= NumberOfWires; component++){
			AhdcWire wire(sector,superlayer,layer,component);
			// top face wires (z == 0)
			gr1t->SetPoint(component-1,wire.start.x,wire.start.y);
		}
		// setting 
		gr1t->SetMarkerStyle(4);
		//gr1t->SetMarkerColor(40+superlayer);
		//gr1t->SetMarkerColor(40);
		gr1t->SetMarkerColorAlpha(40, 0.35);
		gr1t->SetMarkerSize(1);
		gr1t->SetLineColor(40+superlayer);
		// add this layer in mg
		mg->Add(gr1t);
		mg_adcMax->Add(gr1t);
		mg_timeOT->Add(gr1t);
		mg_integral->Add(gr1t);
		legend->AddEntry(gr1t,TString::Format("layer %d%d",superlayer,layer).Data(),"p");
	}
	// top face center
	TGraph* gr2t = new TGraph(1);
	gr2t->SetPoint(0,0,0);
	gr2t->SetMarkerStyle(2);
	gr2t->SetMarkerSize(2);
	gr2t->SetMarkerColor(kBlack);
	mg->Add(gr2t);
	mg_adcMax->Add(gr2t);
	mg_timeOT->Add(gr2t);
	mg_integral->Add(gr2t);
	// at this stage mg is well defined
	
	// set axis name
	mg_adcMax->GetXaxis()->SetTitle("x");
	mg_adcMax->GetYaxis()->SetTitle("y");
	mg_timeOT->GetXaxis()->SetTitle("x");
	mg_timeOT->GetYaxis()->SetTitle("y");
	mg_integral->GetXaxis()->SetTitle("x");
	mg_integral->GetYaxis()->SetTitle("y");
	
	/*********************************************
	 * Event visualisation 
	 * ******************************************/
	//cout << "argv[0] : " << argv[0] << endl;	
	// open file and read banks
	const char* filename = argv[1];
	hipo::reader  r(filename);
        hipo::banklist list = r.getBanks({"AHDC::adc","AHDC::wf:136"});
        long unsigned int nEvent =0;

	// Setting parameters
        double amplitudeFractionCFA = 0.5;
        double fractionCFD = 0.3;
        int binDelayCFD = 5;
	
	// Print AHDC without event
	canvas1->Divide(2,3);
	TLatex latex;
	latex.SetTextSize(0.02);
	
	// adcMax
	canvas1->cd(1);
	mg_adcMax->Draw("AP");
	latex.DrawLatex(0+1,0+1,"no event");
	// timeOT
	canvas1->cd(3);
	mg_timeOT->Draw("AP");
	latex.DrawLatex(0+1,0+1,"no event");
	// integral
	canvas1->cd(5);
	mg_integral->Draw("AP");
	latex.DrawLatex(0+1,0+1,"no event");
	// output
	canvas1->Print("./thisEvent.pdf");

	// loop over events
        while( r.next(list)){
		//if (nEvent != 0) {break;}
		/************************  Refresh AHDC  ************************/
		canvas1->Clear();
		canvas1->Divide(2,3);
		TLatex latex;
		latex.SetTextSize(0.025);
		// adcMax
		canvas1->cd(1);
		mg_adcMax->Draw("AP");
		latex.DrawLatex(0+1,0+1,TString::Format("ev : %ld",nEvent));
		// timeOT
		canvas1->cd(3);
		mg_timeOT->Draw("AP");
		latex.DrawLatex(0+1,0+1,TString::Format("ev : %ld",nEvent));
		// integral
		canvas1->cd(5);
		mg_integral->Draw("AP");
		latex.DrawLatex(0+1,0+1,TString::Format("ev : %ld",nEvent));
		/******************************************************************/

                // loop over columns of AHDC::wf:136 i.e HITS
                for(int col = 0; col < list[1].getRows(); col++){
                        std::vector<short> samples; // waveform samples
                        for (int bin=0; bin < 136; bin++){
                                std::string binName = "s" + to_string(bin+1);
                                short thisSample = list[1].getInt(binName.c_str(),col);
                                samples.push_back(thisSample);
                        }
			// Apply extraction
                        ahdcExtractor T(44.0,amplitudeFractionCFA,binDelayCFD,fractionCFD);
			T.adcOffset = (short) (samples[0] + samples[1] + samples[2] + samples[3] + samples[4])/5;
                        std::map<std::string,double> output = T.extract(samples);

                        // extracted data
                        double adcMax = output["adcMax"];
			double integral = output["integral"];
                        double timeMax = output["timeMax"];
                        double timeRiseCFA = output["timeRiseCFA"];
                        double timeOT = output["timeOverThresholdCFA"];
                        double timeCFD = output["timeCFD"];
                        //double mctime = list[0].getFloat("mctime",col);
			
			// wire id
			int sector = list[1].getInt("sector",col);
			int layerId = list[1].getInt("layer",col);
			int superlayer = layerId/10;
			int layer = layerId % 10;
			int component = list[1].getInt("component",col);

			// deducte its geometry
			AhdcGeom ahdc;
			if (ahdc.IsAhdcWire(sector,superlayer,layer,component)){
				AhdcWire wire(sector,superlayer,layer,component);
				// adcMax
				double fraction_adcMax = adcMax/3600; // to be improved 
				if (fraction_adcMax < 0) fraction_adcMax = 0;
				if (fraction_adcMax > 1) fraction_adcMax = 1;	
				DepositCircle * depo_adcMax = new DepositCircle(wire.start.x,wire.start.y,fraction_adcMax);
				depo_adcMax->SetFillColorAlpha(kRed,1.0);
				canvas1->cd(1);
				depo_adcMax->Draw("F");
				// timeOT
				double fraction_timeOT = timeOT/1400; // to be improved
				if (fraction_timeOT < 0) fraction_timeOT = 0;
				if (fraction_timeOT > 1) fraction_timeOT = 1;	
				DepositCircle * depo_timeOT = new DepositCircle(wire.start.x,wire.start.y,fraction_timeOT);
				depo_timeOT->SetFillColorAlpha(kRed,1.0);
				canvas1->cd(3);
				depo_timeOT->Draw("F");
				// integral
				double fraction_integral = integral/60000; // to be improved
				if (fraction_integral < 0) fraction_integral = 0;
				if (fraction_integral > 1) fraction_integral = 1;	
				DepositCircle * depo_integral = new DepositCircle(wire.start.x,wire.start.y,fraction_integral);
				depo_integral->SetFillColorAlpha(kRed,1.0);
				canvas1->cd(5);
				depo_integral->Draw("F");
			} else { // should not happen
				cout << "BAD    : sector(" << sector << "), superlayer(" << superlayer << "), layer(" << layer << "), component(" << component << ")\n" << endl;
			}
                } // end loop over columns of AHDC::wf:136
        	nEvent++;
		/*********************  API    ***********************/
		string action;
		cout << "Choose (n=next, p=previous, q=quit, h=help), Type action : ";
		cin >> action;
		while (action != string("q")){
			if (action == string("n")){
				canvas1->Print("./thisEvent.pdf");
				break;
			}
			cout << "Choose (n=next, p=previous, q=quit, h=help), Type action : ";
			cin >> action;
		}
		if (action == string("q")) {
			cout << "API action : event number >= " << nEvent-1 << " not read" << endl;
			break;
		}
		/*******************  END API  ***********************/
        } // end loop over events
	
	// print canvas
	//canvas1->Print("./thisEvent.pdf");
}


