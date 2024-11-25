/*************************************************
 * Test de color palette in RGB space
 *
 * @author Felix Touchte Codjo
 * @date November 25, 2024
 * **********************************************/

#include <cmath>
#include <iostream>

#include "TGraph.h"
#include "TCanvas.h"
#include "TColor.h"
#include "TMultiGraph.h"

int main(int argc, const char * argv[]){
	using namespace std;
	double blue[3] = {0.,0.,1.};
	double green[3] = {0.,1.,0.};
	double yellow[3] = {1.,1.,0.};
	int Npts = 15;
	double palette[2*Npts+1][3];
	for (int i=0;i<=Npts;i++){
		double t = i/(1.*Npts);
		palette[i][0] = (1-t)*blue[0] + t*green[0];
		palette[i][1] = (1-t)*blue[1] + t*green[1];
		palette[i][2] = (1-t)*blue[2] + t*green[2];
	}
	for (int i=0;i<=Npts;i++){
		double t = i/(1.*Npts);
		palette[i+Npts][0] = (1-t)*green[0] + t*yellow[0];
		palette[i+Npts][1] = (1-t)*green[1] + t*yellow[1];
		palette[i+Npts][2] = (1-t)*green[2] + t*yellow[2];
	}
	/*int palette[(int) Npts+1][3]; 
	for (int i=0;i<=Npts;i++){
		palette[i][0] = (int) color[i][0];
		palette[i][1] = (int) color[i][1];
		palette[i][2] = (int) color[i][2];
		cout << palette[i][0] << " " << palette[i][1] << " " << palette[i][2] << endl;
	}*/

	// new color definitions
	TCanvas* canvas1 = new TCanvas("c1","c1 title",2000,2000);
	TMultiGraph  *mg  = new TMultiGraph();	
	for (int i=0;i<=2*Npts;i++){
		TGraph * carre = new TGraph(5);
		carre->SetPoint(0,i,0);
		carre->SetPoint(1,i+1,0);
		carre->SetPoint(2,i+1,1);
		carre->SetPoint(3,i,1);
		carre->SetPoint(4,i,0);
		int ci = TColor::GetFreeColorIndex();
		auto color = new TColor(ci, palette[i][0], palette[i][1], palette[i][2]);
		carre->SetFillColorAlpha(ci,1.0);
		mg->Add(carre);
	}
	mg->Draw("APLF");
	canvas1->Print("color.pdf");
	
}
