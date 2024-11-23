/*************************************************
 * This code generates a plot of all ahdc wires.
 * It also plots some wire lines for superlayer
 * 3 and 4 to be able to see the stereo angles.
 * 
 * View parameters :
 * - alpha angle to simulate the z axis in a 2D plan
 * - scale factor
 *
 * Remark : some cuts can be done to plot specific 
 * superlayer, layer, component (wire)
 *
 * This code is an example of use of AhdcGeom.h
 *
 * @author Felix Touchte Codjo
 * @date November 20, 2024
 * **********************************************/

#include "ahdcExtractor.h"
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

#include "AhdcGeom.h"

int main(int argc, char const *argv[]){
	using namespace std;
	AhdcGeom ahdc;
	TCanvas* canvas1 = new TCanvas("c1","c1 title",3000,3000);
	TLegend* legend = new TLegend();
	
	// 3D view
	// we assume an angle alpha between the z and x axis
	// 3D(x,y,z) is represented by 2D(x,y) + z* scale * 2D(ux,uy) 
	// where (ux,uy) is a unitary director vector the z-axis
	// (ux,uy) = (cons(alpha),sin(alpha))
	// scale is a number between 0 and 1 or bigger
	double alpha = futils::toRadian(20);
	double ux = cos(alpha);
	double uy = sin(alpha);
	double z = 300;
	double scale = 200/z; // scale*z >= 2*R_max >= 136

	TMultiGraph  *mg  = new TMultiGraph();
	// loop over (super)layer
	for (int entry = 0; entry < ahdc.GetDataBaseSize(); entry++) {
		//if (entry != 0) {break;} // can be commented
		int info = ahdc.GetDataBaseEntry(entry);
		int sector = info/10000;
		int superlayer = (info-10000*sector)/1000;
		int layer = (info-10000*sector-1000*superlayer)/100;
		int NumberOfWires = info-10000*sector-1000*superlayer-100*layer;
		TGraph* gr1t = new TGraph(NumberOfWires); // top face of this layer
		TGraph* gr1b = new TGraph(NumberOfWires); // bottom face of this layer
		// loop over layer wires
		for (int component = 1; component <= NumberOfWires; component++){
			AhdcWire wire(sector,superlayer,layer,component);
			// wire top face
			gr1t->SetPoint(component-1,wire.start.x,wire.start.y);
			// wire bottom face
			wire.end.Translate(z*scale*ux,z*scale*uy,0);
			gr1b->SetPoint(component-1,wire.end.x,wire.end.y);
			// wire line
			if (((superlayer == 4) || superlayer == 3) && (layer == 1) && (wire.start.x > 0 ) && (wire.start.y > 0)){
				TGraph* gr1tb = new TGraph(2);
				gr1tb->SetPoint(0,wire.start.x,wire.start.y);
				gr1tb->SetPoint(1,wire.end.x,wire.end.y);
				gr1tb->SetLineColor(superlayer);
				mg->Add(gr1tb);
			}
		}
		// setting top face
		gr1t->SetMarkerStyle(20);
		gr1t->SetMarkerColor(40+superlayer);
		gr1t->SetMarkerSize(2);
		gr1t->SetLineColor(40+superlayer);
		mg->Add(gr1t);
		// setting bottom face
		gr1b->SetMarkerStyle(4);
		gr1b->SetMarkerColor(40+superlayer);
		gr1b->SetMarkerSize(2);
		gr1b->SetLineColor(40+superlayer);
		mg->Add(gr1b);
		legend->AddEntry(gr1t,TString::Format("layer %d%d",superlayer,layer).Data(),"p");
	}
	// top face center
	TGraph* gr2t = new TGraph(1);
	gr2t->SetPoint(0,0,0);
	gr2t->SetMarkerStyle(2);
	gr2t->SetMarkerSize(2);
	gr2t->SetMarkerColor(kBlack);
	mg->Add(gr2t);
	// bettom face center
	TGraph* gr2b = new TGraph(1);
	gr2b->SetPoint(0,0+z*scale*ux,0+z*scale*uy);
	gr2b->SetMarkerStyle(2);
	gr2b->SetMarkerSize(2);
	gr2b->SetMarkerColor(kBlack);
	mg->Add(gr2b);
	// Draw multigraph
	mg->Draw("APL");
	// Draw legend
	legend->Draw();
	// Draw text
	TLatex latex;
	latex.SetTextSize(0.015);
	latex.DrawLatex(0+1,0+1,"top face");
	latex.DrawLatex(0+z*scale*ux+1,0+z*scale*uy+1,"bot face");
	// print canvas
	canvas1->Print("./ahdcWires.pdf");
}
