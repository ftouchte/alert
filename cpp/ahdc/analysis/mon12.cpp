/**********************************************************************
 * mon12 
 * 
 * Principle 
 * ---
 * Like monitor.cpp but using AhdcDetector.h instead AhdcGeom.h 
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
#include "TColor.h"

int main(int argc, char const *argv[]){
	using namespace std;
	
	AhdcDetector * ahdc = new AhdcDetector();

	Point3D top = ahdc->GetSector(0)->GetSuperLayer(0)->GetLayer(0)->GetWire(0)->top;
	Point3D bot = ahdc->GetSector(0)->GetSuperLayer(0)->GetLayer(0)->GetWire(0)->bot;
	cout << "x : " << (top.x + bot.x)/2 << endl;
	cout << "y : " << (top.y + bot.y)/2 << endl;
	cout << "z : " << (top.z + bot.z)/2 << endl;
	
	TCanvas* canvas1 = new TCanvas("c1","c1 title",2400,2000);
	canvas1->Range(-80,-80,120,80);
	ahdc->Draw("P");
	// Color palette
	double blue[3] = {0.,0.,1.};
	double green[3] = {0.,1.,0.};
	double yellow[3] = {1.,1.,0.};
	int Npts = 12;
	double palette[2*Npts][3]; // 24 color range
	for (int i=0;i<Npts;i++){ // from blue to green
		double t = i/(1.*Npts);
		palette[i][0] = (1-t)*blue[0] + t*green[0];
		palette[i][1] = (1-t)*blue[1] + t*green[1];
		palette[i][2] = (1-t)*blue[2] + t*green[2];
	}
	for (int i=0;i<Npts;i++){ // from green to yellow
		double t = i/(1.*Npts);
		palette[i+Npts][0] = (1-t)*green[0] + t*yellow[0];
		palette[i+Npts][1] = (1-t)*green[1] + t*yellow[1];
		palette[i+Npts][2] = (1-t)*green[2] + t*yellow[2];
	}
	// bar axis goes from -65 to 65 in y coord
	// bar aixs is located at x = 100
	// each color range is represented by a rectangle
	// rectangle size in x 100 -> 80
	// rectangle size in y -65*(1-i/24) + (i/24)*63 
	// where i goes from 0 to 23 (24 points)
	TMultiGraph  *mg_palette  = new TMultiGraph();	
	for (int i = 0; i<2*Npts; i++) {
		TGraph * box = new TGraph(5);
		double t = (1.0*i)/(2*Npts);
		double y1 = -65*(1-t) + t*65;
		double tplus1 = t + 1.0/(2*Npts);
		double y2 = -65*(1-tplus1) + tplus1*65;
		box->SetPoint(0,100,y1);
		box->SetPoint(1,100,y2);
		box->SetPoint(2,80,y2);
		box->SetPoint(3,80,y1);
		box->SetPoint(4,100,y1);
		int ci = TColor::GetFreeColorIndex();
		auto color = new TColor(ci, palette[i][0], palette[i][1], palette[i][2]);
		box->SetFillColorAlpha(ci,1.0);
		mg_palette->Add(box);
	}
	mg_palette->Draw("F");
	// axis
	TGaxis * baraxis = new TGaxis(100,-65,100,65,0,4000,10,"+L");
	//baraxis->SetTickSize(0.009);
	baraxis->SetLabelSize(0.025);
	baraxis->Draw();
	canvas1->Print("./mon12.pdf");
	delete canvas1;
	return 0;	
}


