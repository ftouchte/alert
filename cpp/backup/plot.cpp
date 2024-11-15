#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <cmath>

// ROOT class
#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TH1.h"
#include "TStyle.h"
#include "TScatter.h"
#include "TText.h"
#include "TGraphPolar.h"
#include "TGaxis.h"
#include "TLatex.h"
#include "TPaveText.h"

#include "futils.h"

#define PI 3.14159265


int main(int argc, char const *argv[])
{   
    if (argc > 1){

        std::ifstream file_nsteps(argv[1],std::ios::in);
        int nsteps;
        int Nrows=0;
        std::vector<int> vec;
        int max_elt=0;
        std::cout << "initial vec size : " << vec.size() << std::endl;
        if (file_nsteps){
            while (!file_nsteps.eof()){
                file_nsteps >> nsteps;
                vec.push_back(nsteps);
                if (max_elt < nsteps) {max_elt = nsteps;}
                Nrows++;
                std::cout << nsteps << std::endl;
            }
            vec.pop_back();
            std::cout << "Number of line : " << Nrows << std::endl;
            std::cout << "final vec size : " << vec.size() << std::endl;
            std::cout << "2nd last elt : " << vec.at(vec.size()-2) << std::endl;
            std::cout << "1st last elt : " << vec.at(vec.size()-1) << std::endl;
            std::cout << "max nsteps value : " << max_elt << std::endl;
        }
        else {
            std::cout << "Impossible d'ouvrir le fichier" << std::endl;
            return 0;
        }
        file_nsteps.close();

        std::cout << "> Plot : Hitogram of nsteps" << std::endl;
        Nrows = vec.size();
        TH1D * hist_nsteps = new TH1D("hist_nsteps","Number of steps / MHit",100,0,30);
        for (int i=0;i<Nrows;i++){
            hist_nsteps->Fill(vec.at(i));
        }
        TCanvas* canvas1 = new TCanvas("c1","c1 title",1366,768);
        //canvas1->Divide(2,2);
        gStyle->SetOptStat("nemruo"); 
        canvas1->cd(1);
        hist_nsteps->GetXaxis()->SetTitle("nsteps");
        hist_nsteps->GetXaxis()->SetTitleSize(0.05);
        hist_nsteps->GetYaxis()->SetTitle("Number of MHits");
        hist_nsteps->GetYaxis()->SetTitleSize(0.05);
        hist_nsteps->Draw();

        canvas1->Print("./output/hist_nsteps.pdf");
        delete hist_nsteps; delete canvas1;
    }
    else {
        std::cout << " ***** please provide a filename..." << std::endl;
    }
   
   // *********************************
   // TEST
   // *********************************

    
    //vector<G4ThreeVector> Lpos        = aHit->GetLPos();
    //int nsteps = Lpos.size();
    int nsteps = 10;
    //double xmin = Lpos[0].x();
    //double xmax = xmin;
    //double ymin = Lpos[0].y();
    //double ymax = ymin;
    double xmin = 0, ymin =0;
    double xmax = xmin, ymax = ymin;

    srand(time(NULL));

    double x1[nsteps]; 
    double y1[nsteps];

    TGraph* graph1 = new TGraph(nsteps);
    for (int i=0;i<nsteps;i++) {
            //double x_tmp = Lpos[i].x();
            //double y_tmp = Lpos[i].y();
            double x_tmp = rand() % 10 - 5;
            double y_tmp = rand() % 12 - 6;
            x1[i] = x_tmp;
            y1[i] = y_tmp;
            graph1->SetPoint(i,x_tmp,y_tmp);
            if (xmax < x_tmp) xmax = x_tmp;
            if (xmin > x_tmp) xmin = x_tmp;
            if (ymax < y_tmp) ymax = y_tmp;
            if (ymin > y_tmp) ymin = y_tmp;
    }

    double xlim = abs(xmax);
    if (xlim < abs(xmin)) xlim = abs(xmin);
    double ylim = abs(ymax);
    if (ylim < abs(ymin)) ylim = abs(ymin);
    double xpad = xlim;
    if (xpad < ylim) xpad = ylim;

    std::cout << "xmax : " << xmax << std::endl;
    std::cout << "xmin : " << xmin << std::endl;
    std::cout << "ymax : " << ymax << std::endl;
    std::cout << "ymin : " << ymin << std::endl;
    std::cout << "xlim : " << xlim << std::endl;
    std::cout << "ylim : " << ylim << std::endl;
    std::cout << "xpad : " << xpad << std::endl;

    // Draw canvas
    TCanvas* canvas3 = new TCanvas("c3","c3 title",0,0,1500,1500);
    canvas3->Range(-xpad-2,-xpad-2,xpad+2,xpad+2);
    
    
    // Draw gragh
    graph1->SetTitle("");
    graph1->SetMarkerStyle(21);
    graph1->SetMarkerColor(kRed);
    graph1->SetMarkerSize(2);
    graph1->Draw("P");

    // Draw the axis
    TGaxis* ox = new TGaxis(-xpad,0,xpad,0,-xpad,xpad,510,"+-S>");
    ox->SetTickSize(0.009);
    ox->SetLabelFont(42);
    ox->SetLabelSize(0.025);
    ox->SetTitle("x (mm)");
    ox->SetTitleSize(0.03);
    ox->Draw();
    TGaxis* oy = new TGaxis(0,-xpad,0,xpad,-xpad,xpad,510,"+-S>");
    oy->SetTickSize(0.009);
    oy->SetLabelFont(42);
    oy->SetLabelSize(0.025);
    oy->SetTitle("y (mm)");
    oy->SetTitleSize(0.03);
    oy->Draw();

    // Draw wire
    /*
    double x[1] = {0}, y[1] = {0};
    TScatter* scatter = new TScatter(1,x,y);
    scatter->SetMarkerStyle(20);
    scatter->SetMarkerColor(kBlack);
    scatter->SetMarkerSize(2);
    scatter->Draw("SAME");
    for (int i=0;i<nsteps;i++){
        TText *text = new TText(x1[i],y1[i],TString::Format("%d",i));
        text->SetTextFont(42);
        text->SetTextSize(40);
        text->Draw("SAME");
    }*/

    canvas3->Print("./output/Lpos.pdf");

    // **********************************
    // Draw polar graph (rho,theta) plan
    // **********************************

    TCanvas* canvas4 = new TCanvas("c4","c4 title",1500,1500);

    double theta[nsteps];
    double radius[nsteps];
    

    for (int i=0;i<nsteps;i++){
        /*double x_=x1[i], y_=y1[i];
        double rho_=0, theta_=0;
        futils::cart2polar(x_,y_,rho_,theta_);
        radius[i] = rho_; theta[i]=theta_;*/
        if (!futils::cart2polar(x1[i],y1[i],radius[i],theta[i])){
            radius[i]=0; theta[i]=0;
        }
        
        
        /*radius[i] = sqrt(x1[i]*x1[i] + y1[i]*y1[i]);
        
        if (y1[i] > 0) {theta[i] = acos(x1[i]/radius[i]); }
        else {theta[i] = 2*PI - acos(x1[i]/radius[i]); }*/
        std::cout << "theta[" << i << "] : " << theta[i] << std::endl;
        

    }

    TGraphPolar* grP = new TGraphPolar(nsteps,theta,radius,0,0);
    grP->SetTitle("");
    grP->SetMarkerStyle(20);
    grP->SetMarkerSize(4);
    grP->SetMarkerColor(kRed);
    grP->Draw("PE");
    canvas4->Update();
    grP->GetPolargram()->SetToRadian();
    grP->GetPolargram()->SetRadialLabelSize(0.03);
    grP->GetPolargram()->SetPolarLabelSize(0.04);
    grP->SetMinRadial(0);

    

    std::cout << "double epsilon : " << __DBL_EPSILON__ << std::endl;
    canvas4->Print("./output/Lpos2222.pdf");


    // ********************************************
    // Draw polar graph (rho,theta,phi=pi/2) plan
    // ********************************************

    //etc...

    return 0;
}
