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
		this->SetPoint(resolution,x+rmax*fraction,y); // close the circle
	}
	void SetRmax(double _rmax) {rmax = _rmax;}
	/*void Draw(Option_t* chopt = "F") override {
		//this->SetFillStyle(3001);
		this->SetFillColorAlpha(kRed,1.0);
		this->TGraph::Draw(chopt);
	}*/
};

class DepositBar : public TGraph {
	double x; ///< x-coordiante of the center 
	double y; ///< y-coordinate of the center
	double width = 0.1;
public :
	DepositBar(double _x, double _y) : TGraph(5), x(_x), y(_y) {
		this->SetPoint(0,x-width,0);
		this->SetPoint(1,x+width,0);
		this->SetPoint(2,x+width,y);
		this->SetPoint(3,x-width,y);
		this->SetPoint(4,x-width,0); // close the rectangle for the filling step
	}
	void SetWidth(double _width) {width = _width;}
};

int layer2number(int superlayer, int layer) {
	int id = 10*superlayer + layer;
	if (id == 11) {
		return 1;
	} else if (id == 21) {
		return 2;
	} else if (id == 22) {
		return 3;
	} else if (id == 31) {
		return 4;
	} else if (id == 32) {
		return 5;
	} else if (id == 41) {
		return 6;
	} else if (id == 42) {
		return 7;
	} else { // (id == 51)
		return 8;
	}
}

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
	mg_adcMax->GetXaxis()->SetTitle("x (mm)");
	mg_adcMax->GetYaxis()->SetTitle("y (mm)");
	mg_timeOT->GetXaxis()->SetTitle("x (mm)");
	mg_timeOT->GetYaxis()->SetTitle("y (mm)");
	mg_integral->GetXaxis()->SetTitle("x (mm)");
	mg_integral->GetYaxis()->SetTitle("y (mm)");
	
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
	
	// adcMax (left)
	canvas1->cd(1);
	mg_adcMax->Draw("AP");
	latex.DrawLatex(0+1,0+1,"no event");
	// adcMax (right)
	canvas1->cd(2);
	TGraph * gr_adcMax = new TGraph(8);
	for (int i=0;i<8;i++){
		gr_adcMax->SetPoint(i,i+1,0);
	}
	gr_adcMax->SetTitle("adcMax (adc)");
	gr_adcMax->GetXaxis()->SetTitle("layer");
	gr_adcMax->GetYaxis()->SetTitle("");
	gr_adcMax->SetMarkerStyle(1);
	gr_adcMax->SetMarkerSize(2);
	gr_adcMax->SetMarkerColor(kRed);
	gr_adcMax->Draw("AP");
	// timeOT (left)
	canvas1->cd(3);
	mg_timeOT->Draw("AP");
	latex.DrawLatex(0+1,0+1,"no event");
	// timeOT (right)
	canvas1->cd(4);
	TGraph * gr_timeOT = new TGraph(8);
	for (int i=0;i<8;i++){
		gr_timeOT->SetPoint(i,i+1,0);
	}
	gr_timeOT->SetTitle("timeOT (ns)");
	gr_timeOT->GetXaxis()->SetTitle("layer");
	gr_timeOT->GetYaxis()->SetTitle("");
	gr_timeOT->SetMarkerStyle(1);
	gr_timeOT->SetMarkerSize(2);
	gr_timeOT->SetMarkerColor(kRed);
	gr_timeOT->Draw("AP");
	// integral (left)
	canvas1->cd(5);
	mg_integral->Draw("AP");
	latex.DrawLatex(0+1,0+1,"no event");
	// integral (right)
	canvas1->cd(6);
	TGraph * gr_integral = new TGraph(8);
	gr_integral->SetTitle("integral (adc per 44 ns)");
	gr_integral->GetXaxis()->SetTitle("layer");
	gr_integral->GetYaxis()->SetTitle("");
	for (int i=0;i<8;i++){
		gr_integral->SetPoint(i,i+1,0);
	}

	gr_integral->SetMarkerStyle(1);
	gr_integral->SetMarkerSize(2);
	gr_integral->SetMarkerColor(kRed);
	gr_integral->Draw("AP");
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
		// it may have multiple hits on the same layer
		double tab_adcMax[8] = {0}; // plot the sum of adcMax "collected" each of the 8 layers 
		double tab_timeOT[8] = {0};
		double tab_integral[8] = {0};
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

			// adcMax (right plot)
			tab_adcMax[layer2number(superlayer,layer)-1] += adcMax;
			// timeOT (right plot)
			tab_timeOT[layer2number(superlayer,layer)-1] += timeOT;
			// integral (right plot)
			tab_integral[layer2number(superlayer,layer)-1] += integral;
			// deducte its geometry
			AhdcGeom ahdc;
			if (ahdc.IsAhdcWire(sector,superlayer,layer,component)){
				AhdcWire wire(sector,superlayer,layer,component);
				// adcMax (left plot)
				double fraction_adcMax = adcMax/3600; // to be improved 
				if (fraction_adcMax < 0) fraction_adcMax = 0;
				if (fraction_adcMax > 1) fraction_adcMax = 1;	
				DepositCircle * depo_adcMax = new DepositCircle(wire.start.x,wire.start.y,fraction_adcMax);
				depo_adcMax->SetFillColorAlpha(kRed,1.0);
				canvas1->cd(1);
				depo_adcMax->Draw("F");
				// timeOT (left plot)
				double fraction_timeOT = timeOT/1400; // to be improved
				if (fraction_timeOT < 0) fraction_timeOT = 0;
				if (fraction_timeOT > 1) fraction_timeOT = 1;	
				DepositCircle * depo_timeOT = new DepositCircle(wire.start.x,wire.start.y,fraction_timeOT);
				depo_timeOT->SetFillColorAlpha(kRed,1.0);
				canvas1->cd(3);
				depo_timeOT->Draw("F");
				// integral (left plot)
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
		// reset right plots
		for (int i=0; i<8; i++){
			gr_adcMax->SetPoint(i,i+1,tab_adcMax[i]);
			gr_timeOT->SetPoint(i,i+1,tab_timeOT[i]);
			gr_integral->SetPoint(i,i+1,tab_integral[i]);
		}
		// adcMax (right plot)
		canvas1->cd(2);
		gr_adcMax->Draw("AP");
		for (int i=0; i<8; i++){
			DepositBar * bar_adcMax = new DepositBar(gr_adcMax->GetPointX(i),gr_adcMax->GetPointY(i));
			bar_adcMax->SetFillColorAlpha(kRed,1.0);
			bar_adcMax->Draw("F");
		}
		// timeOT (right plot)
		canvas1->cd(4);
		gr_timeOT->Draw("AP");
		for (int i=0; i<8; i++){
			DepositBar * bar_timeOT = new DepositBar(gr_timeOT->GetPointX(i),gr_timeOT->GetPointY(i));
			bar_timeOT->SetFillColorAlpha(kRed,1.0);
			bar_timeOT->Draw("F");
		}
		// integral (right plot)
		canvas1->cd(6);
		gr_integral->Draw("AP");
		for (int i=0; i<8; i++){
			DepositBar * bar_integral = new DepositBar(gr_integral->GetPointX(i),gr_integral->GetPointY(i));
			bar_integral->SetFillColorAlpha(kRed,1.0);
			bar_integral->Draw("F");
		}
                } // end loop over columns of AHDC::wf:136
        	nEvent++;
		/**********************************  API    ******************************************/
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
		/*********************************  END API   *****************************************/
        } // end loop over events
	
	// print canvas
	//canvas1->Print("./thisEvent.pdf");
}


