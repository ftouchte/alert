#include <iostream>
#include <fstream>
#include <vector>

#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TGaxis.h"
//#include "TString.h"

int main(int argc, char const *argv[])
{
    std::cout << "argc : " << argc << std::endl;
    if (argc >1){
        std::vector<double> Dgtz;
        int Npts;
        double value;
        std::ifstream fichier(argv[1]);
        std::cout << "file processed : " << argv[1] << std::endl;
        if (fichier.is_open()) {
            fichier >> Npts;
            std::cout << "Npts : " << Npts << std::endl;
            for (int i=0;i<Npts;i++){
                fichier >> value;
                Dgtz.push_back(value);
                std::cout << value << " ";
            }
            std::cout << std::endl;
            std::cout << "Dgtz size : " << Dgtz.size() << std::endl;

            // --------------------------------
            // Constant Fraction Discriminator
            // --------------------------------
            double fraction = 0.3;
            int delay = 60;
            std::vector<double> signal(Npts,0.0);
            std::cout << "signal : " << std::endl;

            // Remove noise
            double noise=0;
            for (int i=0;i<5;i++){
                noise += Dgtz.at(i);
            }
            noise = noise/5;
            for (int i=0;i<Npts;i++){
                Dgtz[i] = Dgtz.at(i) - noise;
            }
            // End remove noise
            for (int i=0;i<Npts;i++){
                signal[i] += -1*fraction*Dgtz.at(i);
                if (i >= delay) {
                    signal[i] += Dgtz.at(i-delay);
                }
                std::cout << signal.at(i) << " "; 
            }
            std::cout << std::endl;
            // Determine t_cfd
            // Convention : dernier passage en bas de zéro
            int i_ref = 0;
            for (int i=0;i<Npts;i++){
                if (signal.at(i) < 0){
                    i_ref = i;
                }
            } // dernier passage en bas de zéro
            int i1 = i_ref; // 1 index below 
            int i2 = i_ref+1; // 1 index above
            if (i1 < 0) {i1 = 0; } 
            if (i2 >= Npts) {i2 = Npts-1;}
            double slope = (signal.at(i1) - signal.at(i2))/(i1-i2); 
            double t_cfd;
            t_cfd = i1 + (0-signal.at(i1))/slope; // DONE
            std::cout << "--------------------------------------" << std::endl;
            std::cout << "|       t_CFD  :  "  << t_cfd << std::endl;
            std::cout << "--------------------------------------" << std::endl;

            TCanvas* canvas1 = new TCanvas("c1","c1 title",1366,768);
            TGraph* gr1 = new TGraph(Npts);
            TGraph* gr2 = new TGraph(Npts);
            for (int i=0;i<Npts;i++){
                gr1->SetPoint(i,i,Dgtz.at(i));
                gr2->SetPoint(i,i,signal.at(i));
            }
            gr1->SetTitle(TString::Format("CFD,  fraction = %lf, delay = %d unité d'indice",fraction,delay));
            gr1->GetXaxis()->SetTitle("Time (ns)");
            gr1->GetXaxis()->SetTitleSize(0.05);
            gr1->GetYaxis()->SetTitle("Charge (adc)");
            gr1->GetYaxis()->SetTitleSize(0.05);
            gr1->SetMarkerColor(kBlue);
            gr1->SetMarkerSize(5);
            gr1->SetLineColor(kBlue);
            //gr1->SetLineStyle(2);
            
            
            //gr2->SetLineStyle(1);
            gr2->SetLineColor(kRed);
            gr2->SetMarkerColor(kRed);
            gr2->SetMarkerSize(5);
            gr2->Draw("APL");
            gr1->Draw("PL");

            TGaxis* axis1 = new TGaxis(0,0,136,0,0,136,510,"");
            axis1->SetLineColor(kGreen);
            axis1->SetLabelColor(kGreen);
            axis1->Draw();


            //canvas1->Print(TString::Format("./output/CFD_%s.pdf",argv[1]));
            canvas1->Print("./output/CFD.pdf");
            delete gr1; delete gr2; delete canvas1;
            delete axis1;

        }
        else {
            std::cout << "this file cannot be opened" << std::endl;
        }
    }
    else {
        std::cout << "please, provide a filename" << std::endl;
        return -1;
    }
    

    return 0;
}
