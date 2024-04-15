#define _USE_MATH_DEFINES

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <cmath>

#include "reader.h"
#include "twig.h"
#include "futils.h"

#include "TH1.h"
#include "TCanvas.h"
#include "TFile.h"


int main(int argc, char const *argv[])
{
    if (argc > 1){

        hipo::reader  r(argv[1]);
        hipo::banklist list = r.getBanks({"MC::Particle"});
        long unsigned int nentries =0;

        double px,py,pz;
        //double vx,vy,vz;
        double p, theta, phi;
        double vz;

        TH1D* hist_p = new TH1D("hist_p","MC::Particle #rightarrow p",100, 0,0.4);
        TH1D* hist_theta = new TH1D("hist_theta","MC::Particle #rightarrow #theta",100, 0,M_PI);
        TH1D* hist_phi = new TH1D("hist_phi","MC::Particle #rightarrow #phi",100, 0,2*M_PI);
        TH1D* hist_vz = new TH1D("hist_vz","MC::Particle #rightarrow vz",100, -20,20);
        
        while( r.next(list) && nentries < 15'000){
            for(int itr = 0; itr < list[0].getRows(); itr++){
                px = list[0].getFloat("px",itr);
                py = list[0].getFloat("py",itr);
                pz = list[0].getFloat("pz",itr);
                vz = list[0].getFloat("vz",itr);
                /*p*/
                futils::cart2polar(px,py,pz,p,theta,phi);
                hist_p->Fill(p);
                hist_theta->Fill(theta);
                hist_phi->Fill(phi);
                hist_vz->Fill(vz);
                nentries++;
            }
        }
        // Plots
        TCanvas* canvas = new TCanvas("c","c title",1366,768);
        canvas->Divide(2,2);
        // hist_p
        canvas->cd(1);
        hist_p->GetXaxis()->SetTitle("p (GeV)");
        hist_p->GetXaxis()->SetTitleSize(0.05);
        hist_p->GetYaxis()->SetTitle("#events");
        hist_p->GetYaxis()->SetTitleSize(0.05);
        hist_p->Draw();
        //hist_theta
        canvas->cd(2);
        hist_theta->GetXaxis()->SetTitle("#theta");
        hist_theta->GetXaxis()->SetTitleSize(0.05);
        hist_theta->GetYaxis()->SetTitle("#events");
        hist_theta->GetYaxis()->SetTitleSize(0.05);
        hist_theta->Draw();
        //hist_phi
        canvas->cd(3);
        hist_phi->GetXaxis()->SetTitle("#phi");
        hist_phi->GetXaxis()->SetTitleSize(0.05);
        hist_phi->GetYaxis()->SetTitle("#events");
        hist_phi->GetYaxis()->SetTitleSize(0.05);
        hist_phi->Draw();
        // hist_vz
        canvas->cd(4);
        hist_vz->GetXaxis()->SetTitle("vz (cm)");
        hist_vz->GetXaxis()->SetTitleSize(0.05);
        hist_vz->GetYaxis()->SetTitle("#events");
        hist_vz->GetYaxis()->SetTitleSize(0.05);
        hist_vz->Draw();
        // SAVE

        TFile outputFile("./output/outputFile.root", "RECREATE");
        hist_p->Write();
        outputFile.Close();

        canvas->Print("./output/hists.pdf");
        delete hist_p; delete hist_theta; delete hist_phi; delete hist_vz;
        delete canvas;
    } 
    else {
        std::cout << " ***** please provide a filename..." << std::endl;
    }
    return 0;
}


