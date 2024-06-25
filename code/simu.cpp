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
    canvas1->Print("./output/signal_simu1.pdf");
    //delete gr1; 
    delete ox1; delete oy1; 
    delete canvas1;

    // ****************************************************
    // Change all point by a distributon in the time
    // Change Edep by d/dt• {Edep}
    // ****************************************************
    double tmin = -1, tmax = 11;
    int Npts = 1000; // number of points 
    double dt = (tmax-tmin)/Npts; // time résolution of the electronic (ALERT : 44 ns)
    double sigma = 0.3;
    double Kg = 1/sqrt(2*PI*pow(sigma,2)); 

    TCanvas* canvas2 = new TCanvas("c2","c2 title",1366,768);
    canvas2->Range(-2,-2,12,(int) floor(3*Kg) + 3);
    std::cout << "========>  Hi Felix ! " << std::endl;
    // for each step
    for (int s=0;s<nsteps;s++){ 
        // Draw graph
        TGraph* gr2 = new TGraph(Npts);
        double xRange[Npts], yRange[Npts];
        futils::Gauss gauss(stepTime.at(s),sigma); // gaussian centered in stepTime.at(s)
        for (int i=0;i<Npts;i++){
            xRange[i] = tmin + i*dt;
            yRange[i] = Edep.at(s)*gauss(xRange[i]);
            gr2->SetPoint(i,xRange[i],yRange[i]);
        }
        gr2->SetLineColor(kBlue);
        gr2->SetFillColorAlpha(2+s,0.35);
        gr2->SetFillStyle(3001);
        gr2->Draw("LF"); 
        // Draw stem
        gr1->Draw("P");
        TLine* line = new TLine(stepTime.at(s),0,stepTime.at(s),Edep.at(s));
        line->SetLineWidth(1);
        line->SetLineColor(kBlack);
        line->Draw(); 
    }
    // Draw axis
    TGaxis* ox2 = new TGaxis(-1,0,11,0,-1,11,510,"+-S>");
    ox2->SetTickSize(0.009);
    ox2->SetLabelFont(42);
    ox2->SetLabelSize(0.025);
    ox2->SetTitle("time [ns]");
    ox2->SetTitleSize(0.03);
    ox2->Draw();
    TGaxis* oy2 = new TGaxis(0,-1,0,(int) 3*Kg + 1,-1,(int) floor(3*Kg) +2,510,"+-S>");
    oy2->SetTickSize(0.009);
    oy2->SetLabelFont(42);
    oy2->SetLabelSize(0.025);
    oy2->SetTitle("#frac{d (Edep)}{dt} [keV/ns]");
    oy2->SetTitleSize(0.03); //oy->SetTitleOffset(0.5);
    oy2->Draw();
    canvas2->Print("./output/signal_simu2.pdf");
    delete ox2; delete oy2; delete gr1;
    delete canvas2;

    // ****************************************************
    // Sum all distributions
    // ****************************************************
    TGraph* gr3 = new TGraph(Npts);
    double xRange[Npts], yRange[Npts];
    double ymax = 0;
    for (int i=0;i<Npts;i++){
        xRange[i] = tmin + i*dt;
        yRange[i] = 0;
        for (int s=0;s<nsteps;s++){ // for each step, compute gauss(i-th points)
            futils::Gauss gauss(stepTime.at(s),sigma); // gaussian centered in stepTime.at(s)
            yRange[i] += Edep.at(s)*gauss(xRange[i]);
        }
        if (ymax < yRange[i]) ymax = yRange[i];
        gr3->SetPoint(i,xRange[i],yRange[i]);
    }
    TCanvas* canvas3 = new TCanvas("c3","c3 title",1366,768);
    canvas3->Range(-2,-2,12,(int) ymax + 2);
    // Draw graph
    gr3->SetLineColor(kBlue);
    gr3->SetFillColorAlpha(38,0.35);
    gr3->SetFillStyle(3001);
    gr3->Draw("LF");
    // Draw axis
    TGaxis* ox3 = new TGaxis(-1,0,11,0,-1,11,510,"+-S>");
    ox3->SetTickSize(0.009);
    ox3->SetLabelFont(42);
    ox3->SetLabelSize(0.025);
    ox3->SetTitle("time [ns]");
    ox3->SetTitleSize(0.03);
    ox3->Draw();
    TGaxis* oy3 = new TGaxis(0,-1,0,(int) 3*Kg + 1,-1,(int) floor(3*Kg) +1,510,"+-S>");
    oy3->SetTickSize(0.009);
    oy3->SetLabelFont(42);
    oy3->SetLabelSize(0.025);
    oy3->SetTitle("#Sigma_{s} #frac{d (Edep)}{dt}|_{s} [keV/ns]");
    oy3->SetTitleSize(0.03); //oy->SetTitleOffset(0.5);
    oy3->Draw();
    // Draw title
    TLatex latex3;
    latex3.SetTextSize(0.04);
    latex3.SetTextAlign(13);
    latex3.DrawLatex(4,ymax+1,"#bf{#Sigma_{s} #frac{d (Edep)}{dt}|_{s} [keV/ns]}");

    canvas3->Print("./output/signal_simu3.pdf");
    delete ox3; delete oy3; delete gr3;
    delete canvas3;

    // ****************************************************
    // Sum all distributions 
    // >>>>>>   test the class GenerateSignal
    // ****************************************************
    futils::GenerateSignal Signal;
    for (int s=0;s<nsteps;s++){
        //double stdev = 0.1*(1 + rand() % 3);
        int shape_type = rand () % 2;
        Signal.Add(stepTime.at(s),Edep.at(s),0.3, shape_type);
    }
    std::cout << "Signal(tmin) : " << Signal(tmin) << std::endl;
    std::cout << "Signal(tmax) : " << Signal(tmax) << std::endl;

    TGraph* gr4 = new TGraph(Npts);
    //double ymax = 0;
    ymax = 0;
    for (int i=0;i<Npts;i++){
        double x_ = tmin + i*dt;
        double y_ = Signal(x_);
        if (ymax < y_) ymax = y_;
        gr4->SetPoint(i,x_,y_);
    }
    TCanvas* canvas4 = new TCanvas("c3","c3 title",1366,768);
    canvas4->Range(-2,-2,12,(int) ymax + 2);
    // Draw graph
    gr4->SetLineColor(kBlue);
    gr4->SetFillColorAlpha(38,0.35);
    gr4->SetFillStyle(3001);
    gr4->Draw("LF");
    // Draw axis
    TGaxis* ox4 = new TGaxis(-1,0,11,0,-1,11,510,"+-S>");
    ox4->SetTickSize(0.009);
    ox4->SetLabelFont(42);
    ox4->SetLabelSize(0.025);
    ox4->SetTitle("time [ns]");
    ox4->SetTitleSize(0.03);
    ox4->Draw();
    TGaxis* oy4 = new TGaxis(0,-1,0,(int) 3*Kg + 1,-1,(int) floor(3*Kg) +1,510,"+-S>");
    oy4->SetTickSize(0.009);
    oy4->SetLabelFont(42);
    oy4->SetLabelSize(0.025);
    oy4->SetTitle("#Sigma_{s} #frac{d (Edep)}{dt}|_{s} [keV/ns]");
    oy4->SetTitleSize(0.03); //oy->SetTitleOffset(0.5);
    oy4->Draw();
    // Draw title
    TLatex latex4;
    latex4.SetTextSize(0.04);
    latex4.SetTextAlign(13);
    latex4.DrawLatex(4,ymax+1,"#bf{#Sigma_{s} #frac{d (Edep)}{dt}|_{s} [keV/ns]}");

    canvas4->Print("./output/signal_simu4.pdf");
    delete ox4; delete oy4; delete gr4;
    delete canvas4;

    // ***************************************
    // Test functions Print___()
    // **************************************
    Signal.PrintBeforeProcessing();
    Signal.PrintAllShapes(-2,12,1000);
    Signal.PrintAfterProcessing(-2,12,1000);

    return 0;
}



void futils::GenerateSignal::PrintBeforeProcessing(){
    // Parameters : std::vector<double or int> Location, Amplitude, Width, Shape
    int nLoc = Location.size();
    // Determine extrema values
    double lMax = Location.at(0), aMax = Amplitude.at(0), wMax = Width.at(0);
    double lMin = lMax, aMin = aMax, wMin = wMax;
    for (int l=0;l<nLoc;l++){
        if (lMax < Location.at(l)) lMax = Location.at(l);
        if (aMax < Amplitude.at(l)) aMax = Amplitude.at(l);
        if (wMax < Width.at(l)) wMax = Width.at(l);
        if (lMin > Location.at(l)) lMin = Location.at(l);
        if (aMin > Amplitude.at(l)) aMin = Amplitude.at(l);
        if (wMin > Width.at(l)) wMin = Width.at(l);
    }
    aMin = 0;
    // Define canvas
    double Dl = lMax - lMin;
    double Da = aMax - aMin;
    //double Dw = wMax - wMin;
    double xmargin = 0.1*Dl;
    double ymargin = 0.2*Da;
    TCanvas* canvas1 = new TCanvas("c1","c1 title",1366,768);
    canvas1->Range(lMin-2*xmargin,0-2*ymargin,lMax+2*xmargin,aMax+3*ymargin); //canvas1->Range(-2,-2,12,6);
    // Define graph 
    TGraph* gr1 = new TGraph(nLoc);
    for (int l=0;l<nLoc;l++){
        gr1->SetPoint(l,Location.at(l),Amplitude.at(l));
    }
    // Draw graph
    gr1->SetMarkerStyle(20);
    gr1->SetMarkerColor(kRed);
    gr1->SetMarkerSize(2);
    gr1->Draw("P");
    // Draw lines
    for (int l=0;l<nLoc;l++){
        TLine* line = new TLine(Location.at(l),0,Location.at(l),Amplitude.at(l));
        line->SetLineWidth(1);
        line->SetLineColor(kBlack);
        line->Draw();
    }
    // Draw axis
    TGaxis* ox1 = new TGaxis(lMin-1*xmargin, 0, lMax+1*xmargin, 0, lMin-1*xmargin, lMax+1*xmargin,510,"+-S>");
    ox1->SetTickSize(0.009);
    ox1->SetLabelFont(42);
    ox1->SetLabelSize(0.025);
    ox1->SetTitle("time [ns]");
    ox1->SetTitleSize(0.03);
    ox1->Draw();
    TGaxis* oy1 = new TGaxis(lMin-0.5*xmargin, aMin-1*ymargin, lMin-0.5*xmargin, aMax+2*ymargin, aMin-1*ymargin, aMax+2*ymargin,505,"+-S>");
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
    latex1.DrawLatex(lMin + (lMax-lMin)/3, aMax+2.2*ymargin,"#bf{Deposited energy in each steps}");

    //canvas1->Update();
    canvas1->Print("./output/TestPrintBeforeProcessing.pdf");
    delete gr1; 
    delete ox1; delete oy1; 
    delete canvas1;
}


void futils::GenerateSignal::PrintAllShapes(double tmin, double tmax, int Npts){
    int nLoc = Location.size();
    // Determine extrema values
    double lMax = Location.at(0), aMax = Amplitude.at(0), wMax = Width.at(0);
    double lMin = lMax, aMin = aMax, wMin = wMax;
    for (int l=0;l<nLoc;l++){
        if (lMax < Location.at(l)) lMax = Location.at(l);
        if (aMax < Amplitude.at(l)) aMax = Amplitude.at(l);
        if (wMax < Width.at(l)) wMax = Width.at(l);
        if (lMin > Location.at(l)) lMin = Location.at(l);
        if (aMin > Amplitude.at(l)) aMin = Amplitude.at(l);
        if (wMin > Width.at(l)) wMin = Width.at(l);
    }
    aMin = 0;
    // Define canvas
    double Dl = lMax - lMin;
    double Da = aMax - aMin;
    //double Dw = wMax - wMin;
    double xmargin = 0.1*Dl;
    double ymargin = 0.2*Da;
    // Define canvas
    TCanvas* canvas1 = new TCanvas("c1","c1 title",1366,768); // canvas1->Range() will set later
    // Define graph 1
    TGraph* gr1 = new TGraph(nLoc);
    for (int l=0;l<nLoc;l++){
        gr1->SetPoint(l,Location.at(l),Amplitude.at(l));
    }
    gr1->SetMarkerStyle(20);
    gr1->SetMarkerColor(kRed);
    gr1->SetMarkerSize(2);
    gr1->Draw("P");
    // Draw lines
    for (int l=0;l<nLoc;l++){
        TLine* line = new TLine(Location.at(l),0,Location.at(l),Amplitude.at(l));
        line->SetLineWidth(1);
        line->SetLineColor(kBlack);
        line->Draw();
    }
    // Draw all shape
    double ymax = aMax;
    for (int l=0;l<nLoc;l++){ 
        TGraph* gr2 = new TGraph(Npts);
        double xRange[Npts], yRange[Npts];
        for (int i=0;i<Npts;i++){
            xRange[i] = tmin + i*(tmax-tmin)/Npts;
            if (Shape.at(l) == 0) {
                yRange[i] = Amplitude.at(l)*ROOT::Math::gaussian_pdf(xRange[i],Width.at(l),Location.at(l));
                if (ymax < yRange[i]) ymax = yRange[i];
            }
            if (Shape.at(l) == 1){
                yRange[i] =  Amplitude.at(l)*ROOT::Math::landau_pdf(xRange[i],Width.at(l),Location.at(l));
                if (ymax < yRange[i]) ymax = yRange[i];
            }
            gr2->SetPoint(i,xRange[i],yRange[i]);
        }
        gr2->SetLineColor(kBlue);
        gr2->SetFillColorAlpha(2+l,0.35);
        gr2->SetFillStyle(3001);
        gr2->Draw("LF"); 
    }
    // Draw axis
    TGaxis* ox1 = new TGaxis(lMin-1*xmargin, 0, lMax+1*xmargin, 0, lMin-1*xmargin, lMax+1*xmargin,510,"+-S>");
    ox1->SetTickSize(0.009);
    ox1->SetLabelFont(42);
    ox1->SetLabelSize(0.025);
    ox1->SetTitle("time [ns]");
    ox1->SetTitleSize(0.03);
    ox1->Draw();
    TGaxis* oy1 = new TGaxis(lMin-0.5*xmargin, aMin-1*ymargin, lMin-0.5*xmargin, ymax+2*ymargin, aMin-1*ymargin, ymax+2*ymargin,505,"+-S>");
    oy1->SetTickSize(0.009);
    oy1->SetLabelFont(42);
    oy1->SetLabelSize(0.025);
    oy1->SetTitle("#frac{d (Edep)}{dt} [keV/ns]");
    oy1->SetTitleSize(0.03); //oy->SetTitleOffset(0.5);
    oy1->Draw();
    // Draw title
    TLatex latex1;
    latex1.SetTextSize(0.04);
    latex1.SetTextAlign(13);
    latex1.DrawLatex(lMin + (lMax-lMin)/4, ymax+2.2*ymargin,"#bf{Distribution of deposited energy in each steps}");
    // Print file
    canvas1->Range(lMin-2*xmargin,0-2*ymargin,lMax+2*xmargin,ymax+3*ymargin);
    canvas1->Print("./output/TestPrintAllShapes.pdf");
    delete gr1; 
    delete ox1; delete oy1; 
    delete canvas1;
}


void futils::GenerateSignal::PrintAfterProcessing(double tmin, double tmax, int Npts){
    int nLoc = Location.size();
    // Determine extrema values
    double lMax = Location.at(0), aMax = Amplitude.at(0), wMax = Width.at(0);
    double lMin = lMax, aMin = aMax, wMin = wMax;
    for (int l=0;l<nLoc;l++){
        if (lMax < Location.at(l)) lMax = Location.at(l);
        if (aMax < Amplitude.at(l)) aMax = Amplitude.at(l);
        if (wMax < Width.at(l)) wMax = Width.at(l);
        if (lMin > Location.at(l)) lMin = Location.at(l);
        if (aMin > Amplitude.at(l)) aMin = Amplitude.at(l);
        if (wMin > Width.at(l)) wMin = Width.at(l);
    }
    aMin = 0;
    // Define canvas
    double Dl = lMax - lMin;
    double Da = aMax - aMin;
    //double Dw = wMax - wMin;
    double xmargin = 0.1*Dl;
    double ymargin = 0.2*Da;
    // Define canvas
    TCanvas* canvas1 = new TCanvas("c1","c1 title",1366,768); // canvas1->Range() will set later
    // Draw graph
    double ymax = 0;
    TGraph* gr1 = new TGraph(Npts);
    for (int i=0;i<Npts;i++){
        double x_ = tmin + i*(tmax-tmin)/Npts;
        double y_ = this->operator()(x_);
        if (ymax < y_) ymax = y_;
        gr1->SetPoint(i,x_,y_);
    }
    gr1->SetLineColor(kBlue);
    gr1->SetFillColorAlpha(kRed,0.35);
    gr1->SetFillStyle(3001);
    gr1->Draw("LF"); 
    // Draw axis
    TGaxis* ox1 = new TGaxis(lMin-1*xmargin, 0, lMax+1*xmargin, 0, lMin-1*xmargin, lMax+1*xmargin,510,"+-S>");
    ox1->SetTickSize(0.009);
    ox1->SetLabelFont(42);
    ox1->SetLabelSize(0.025);
    ox1->SetTitle("time [ns]");
    ox1->SetTitleSize(0.03);
    ox1->Draw();
    TGaxis* oy1 = new TGaxis(lMin-0.5*xmargin, aMin-1*ymargin, lMin-0.5*xmargin, ymax+2*ymargin, aMin-1*ymargin, ymax+2*ymargin,505,"+-S>");
    oy1->SetTickSize(0.009);
    oy1->SetLabelFont(42);
    oy1->SetLabelSize(0.025);
    oy1->SetTitle("#frac{d (Edep)}{dt} |_{s} [keV/ns]");
    oy1->SetTitleSize(0.03); //oy->SetTitleOffset(0.5);
    oy1->Draw();
    // Draw title
    TLatex latex1;
    latex1.SetTextSize(0.04);
    latex1.SetTextAlign(13);
    latex1.DrawLatex(lMin + (lMax-lMin)/3, ymax+2.2*ymargin,"#bf{AHDC signal}");
    // Print file
    canvas1->Range(lMin-2*xmargin,0-2*ymargin,lMax+2*xmargin,ymax+3*ymargin);
    canvas1->Print("./output/TestPrintAfterProcessing.pdf");
    delete gr1; 
    delete ox1; delete oy1; 
    delete canvas1;
}

