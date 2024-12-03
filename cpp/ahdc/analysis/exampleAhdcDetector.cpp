/**********************************************************************
 * exampleAhdcDetector 
 * 
 * Simple code showing an example of use of the AhdcDetector class.
 *
 * @author Felix Touchte Codjo
 * @date December 2, 2024
 * **********************************************************************/

#include "ahdcExtractor.h"
#include "AhdcDetector.h"
#include "reader.h"

#include <string>
#include <iostream>
#include <cmath>
#include <ctime>

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

int main(int argc, char const *argv[]){
	using namespace std;
	
	AhdcDetector * ahdc = new AhdcDetector();
	
	TCanvas* canvas1 = new TCanvas("c1","c1 title",2000,2000);
	cout << "Try to plot" << endl;
	ahdc->Draw("AP");
	//AhdcSector *sector = ahdc->GetSector(0);
	//sector->Draw("AP");
	AhdcSuperLayer *slayer = ahdc->GetSector(0)->GetSuperLayer(2);
	slayer->SetMarkerColor(1);
	slayer->Draw("P");
	AhdcLayer *layer2 = ahdc->GetSector(0)->GetSuperLayer(1)->GetLayer(1);
	AhdcLayer *layer1 = ahdc->GetSector(0)->GetSuperLayer(1)->GetLayer(0);
	layer2->SetMarkerColor(2);
	layer1->SetMarkerColor(2);
	layer2->Draw("P");
	layer1->Draw("P");
	AhdcWire *wire = ahdc->GetSector(0)->GetSuperLayer(1)->GetLayer(0)->GetWire(20);	
	//wire->SetMarkerSize(2);
	//wire->SetMarkerStyle(2);
	//wire->SetMarkerColor(kRed);
	wire->Draw("P");
	canvas1->Print("./exampleAhdcDetector.pdf");
	delete canvas1;
	return 0;	
}


