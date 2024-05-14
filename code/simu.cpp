#include "simu.h"
#include "futils.h"

// C++ headers
#include<iostream>
#include <cmath>

// ROOT headers
#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"

#define PI 3.14159265

double carre(double x){return x*x;}
double foo(double x){return sqrt(1-x*x);} // sommes de Riemman d'un demi-disque de rayon 1
double Landau(double x){ return (1/sqrt(2*PI))*exp(-(x+exp(-x))/2);}


int main(int argc, char const *argv[])
{
    //Integrable f = &carre;
    Integrable f(&foo);
    GenerateGauss g(0,1);
    double res = futils::integrate(f,-1,1,1'000);
    //double res2 = futils::integrate(g,-20,20,1'000);
    double res2=0;

    std::cout << "calculated value  pi/2 : " << res << std::endl;
    std::cout << "expected value    pi/2 : " << PI/2 << std::endl;
    std::cout << "calculated value intGauss : " << res2 << std::endl;
    std::cout << "expected value   intGauss : " << sqrt(PI) << std::endl;

    //********************
    //PLOT Landau
    //********************
    int Npts = 100;
    double xmin = -4;
    double xmax = 14;
    double dx = (xmax-xmin)/Npts;
    Npts++;
    double x[Npts], y[Npts];
    GenerateGauss gauss(0,1);
    for (int i=0;i<Npts;i++){
        x[i] = xmin+i*dx;
        //y[i] = Landau(x[i]);
        y[i] = gauss(x[i]);
    }

    TCanvas* canvas1 = new TCanvas("c1","c1 title",1366,768);
    TGraph* graph1 = new TGraph(Npts,x,y);
    graph1->SetTitle("Landau distribution");
    graph1->GetXaxis()->SetTitle("x");
    graph1->GetXaxis()->SetTitleSize(0.05);
    graph1->GetYaxis()->SetTitle("y");
    graph1->GetYaxis()->SetTitleSize(0.05);
    graph1->SetMarkerStyle(21);
    graph1->SetMarkerColor(kBlue);
    graph1->Draw("APL");
    
    std::string path = "./output/Landau_distribution.pdf";
    canvas1->Print(path.c_str());
    delete graph1;
    delete canvas1;


    return 0;
}
