#include "simu.h"
#include "futils.h"

// C++ headers
#include <iostream>
#include <cmath>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <iomanip> // std::setw() and std::setfill(' ')

// ROOT headers
#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TArrow.h"
#include "TGaxis.h"
#include "TLatex.h"

#define PI 3.14159265

double maFonction(double x) {return ROOT::Math::landau_pdf(x,2,10);}

int main(int argc, char const *argv[])
{
    double xmin = -5;
    double xmax = 70;
    int Npts = 1000;
    double location = 10;
    double width = 3;
    double amplitude = 1000;
    double dx = (xmax-xmin)/Npts;
    double x[Npts], y[Npts];
    double ymin = ROOT::Math::landau_pdf(xmin,width,location);
    double ymax = ymin;
    for (int i=0;i<Npts;i++){
        x[i] = xmin + i*dx;
        y[i] = amplitude*ROOT::Math::landau_pdf(x[i],width,location);
        if (ymax < y[i]) ymax = y[i];
    }

    // Plot graph
    double Dx = xmax-xmin;
    double Dy = ymax-ymin;
    double xmargin = 0.1*Dx;
    double ymargin = 0.2*Dy;
    // Draw canvas
    TCanvas* canvas1 = new TCanvas("c1","c1 title",1366,768);
    canvas1->Range(xmin-2*xmargin,ymin-2*ymargin,xmax+2*xmargin,ymax+2*ymargin);
    // Draw graph
    TGraph* gr = new TGraph(Npts,x,y);
    gr->Draw("L");
    gr->SetLineColor(kRed);
    // Draw the axis
    TGaxis* ox1 = new TGaxis(xmin-1*xmargin,0,xmax+1*xmargin,0,xmin-1*xmargin,xmax+1*xmargin,510,"+-S>");
    ox1->SetTickSize(0.009);
    ox1->SetLabelFont(42);
    ox1->SetLabelSize(0.025);
    ox1->SetTitle("x");
    ox1->SetTitleSize(0.03);
    ox1->Draw();
    TGaxis* oy1 = new TGaxis(0,-1,0,ymax+1*ymargin,-1,ymax+1*ymargin,510,"+-S>");
    oy1->SetTickSize(0.009);
    oy1->SetLabelFont(42);
    oy1->SetLabelSize(0.025);
    oy1->SetTitle("y");
    oy1->SetTitleSize(0.03); //oy->SetTitleOffset(0.5);
    oy1->Draw();
    // Draw title
    TLatex latex1;
    latex1.SetTextSize(0.04);
    latex1.SetTextAlign(13);
    latex1.DrawLatex((xmax+xmin)/2,ymax+1.3*ymargin, "#bf{Title...}");
    // Print
    canvas1->Print("./output/decoder.pdf");
    //delete gr1; 
    delete ox1; delete oy1; 
    delete canvas1;

    return 0;
}
