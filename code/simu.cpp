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

double carre(double x){return x*x;}
double foo(double x){return sqrt(1-x*x);} // sommes de Riemman d'un demi-disque de rayon 1
double Landau(double x){ return (1/sqrt(2*PI))*exp(-(x+exp(-x))/2);}


int main(int argc, char const *argv[])
{
    // **************************
    // Generate data
    // **************************
    std::vector<double> stepTime;
    std::vector<double> Edep ;
    std::cout << "initial vector size : " << stepTime.size() << std::endl;
    int nsteps = 10;
    srand(time(NULL));
    std::cout << "stepTime Edep" << std::endl;
    for (int i=0;i<nsteps;i++){
        stepTime.push_back(i+1);
        Edep.push_back(1 + rand() % 3);
        std::cout << std::setw(8) << stepTime.at(i) << " " << std::setw(4) << Edep.at(i) << std::endl;
    }
    std::cout << "final vector size : " << stepTime.size() << std::endl;

    // **************************
    // View of data
    // **************************
    TCanvas* canvas1 = new TCanvas("c1","c1 title",1366,768);
    canvas1->Range(-2,-2,12,6);

    TGraph* gr1 = new TGraph(nsteps);
    for (int i=0;i<nsteps;i++){
        gr1->SetPoint(i,stepTime.at(i),Edep.at(i));
    }
    // Draw graph
    gr1->SetTitle("Deposited energy");
    gr1->SetMarkerStyle(20);
    gr1->SetMarkerColor(kRed);
    gr1->SetMarkerSize(2);
    gr1->Draw("P");
    // Draw lines
    for (int i=0;i<nsteps;i++){
        TLine* line = new TLine(stepTime.at(i),0,stepTime.at(i),Edep.at(i));
        line->SetLineWidth(1);
        line->SetLineColor(kBlack);
        line->Draw();
    }
    // Draw the axis
    TGaxis* ox1 = new TGaxis(-1,0,11,0,-1,11,510,"+-S>");
    ox1->SetTickSize(0.009);
    ox1->SetLabelFont(42);
    ox1->SetLabelSize(0.025);
    ox1->SetTitle("time [ns]");
    ox1->SetTitleSize(0.03);
    ox1->Draw();
    TGaxis* oy1 = new TGaxis(0,-1,0,5,-1,5,510,"+-S>");
    oy1->SetTickSize(0.009);
    oy1->SetLabelFont(42);
    oy1->SetLabelSize(0.025);
    oy1->SetTitle("Edep [keV]");
    oy1->SetTitleSize(0.03); //oy->SetTitleOffset(0.5);
    oy1->Draw();
    // Draw title
    TLatex latex1;
    latex1.SetTextSize(0.04);
    latex1.SetTextAlign(13);
    latex1.DrawLatex(3,5.5,"#bf{Deposited energy in each steps}");

    //canvas1->Update();
    canvas1->Print("./output/signal_simu.pdf");
    delete gr1; delete ox1; delete oy1; 
    delete canvas1;

    // ****************************************************
    // Change all point by a distributon in the time
    // Change Edep by d/dt• {Edep}
    // ****************************************************
    double tmin = 0, tmax = 11;
    int N = 100; // number of points 
    double dt = (tmax-tmin)/N; // time résolution of the electronic (ALERT : 44 ns)
    double tRange[N];
    double eRange[N];
    for (int i=0;i<N;i++){
        tRange[i] = tmin + i*dt;
        eRange[i] = 0;
        for (int s=0;s<nsteps;s++){
            futils::Gauss gauss(stepTime.at(s),0.25);
            eRange[i] += gauss(tRange[i]);
        }
    }
    // plot tRange, eRange

    
    return 0;
}


