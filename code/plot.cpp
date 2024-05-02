#include <iostream>
#include <string>

#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"


int main(int argc, char const *argv[])
{   
    if (argc > 1){

        TCanvas* canvas = new TCanvas("c","c title",1366,768);
        TGraph* signal = new TGraph(argv[1]);
        signal->SetTitle("AHDC signal");
        signal->GetXaxis()->SetTitle("time");
        signal->GetXaxis()->SetTitleSize(0.05);
        signal->GetYaxis()->SetTitle("Edep (ADC)");
        signal->GetYaxis()->SetTitleSize(0.05);
        signal->SetMarkerStyle(21);
        signal->SetMarkerColor(kRed);
        signal->Draw("APL");
        
        std::string path = "./output/ahdc_signal.pdf";
        canvas->Print(path.c_str());
        delete signal;
        delete canvas;
    }
    else {
        std::cout << " ***** please provide a filename..." << std::endl;
    }

    return 0;
}
