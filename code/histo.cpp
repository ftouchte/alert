#define _USE_MATH_DEFINES

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <cmath>

#include "reader.h"
#include "twig.h"
#include "futils.h"

#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TStyle.h"

int main(int argc, char const *argv[])
{
    if (argc > 1){

        hipo::reader  r(argv[1]);
        hipo::banklist list = r.getBanks({"MC::Particle","AHDC::adc","ATOF::adc"});
        long unsigned int nentries =0;

        double px,py,pz;
        double p, theta, phi;
        double vz;

        TH1D* hist_p = new TH1D("hist_p","MC::Particle #rightarrow p",100, 0,0.3);
        TH1D* hist_theta = new TH1D("hist_theta","MC::Particle #rightarrow #theta",100, 0,M_PI);
        TH1D* hist_phi = new TH1D("hist_phi","MC::Particle #rightarrow #phi",100, 0,2*M_PI);
        TH1D* hist_vz = new TH1D("hist_vz","MC::Particle #rightarrow vz",100, -20,20);

        TH1I* hist_nHits_AHDC = new TH1I("hist_nHits_AHDC","Number of Hits in AHDC",100,0,20);
        TH1I* hist_nHits_ATOF = new TH1I("hist_nHits_ATOF","Number of Hits in ATOF",100,0,20);
        TH2D* hist_p_nHits_AHDC = new TH2D("hist_p_nHits_AHDC","Correlation between p and nHits_AHDC",100,0,0.4,100,0,40);
        TH2D* hist_p_nHits_ATOF = new TH2D("hist_p_nHits_ATOF","Correlation between p and nHits_ATOF",100,0,0.4,100,0,10);

        while( r.next(list) && nentries < 15'000){
            // MC::Particle
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

                // nHits ; remark list[0].getRows() == 1, otherwise this portion of code need to be outside for {...} 
                hist_nHits_AHDC->Fill(list[1].getRows());
                hist_nHits_ATOF->Fill(list[2].getRows());
                hist_p_nHits_AHDC->Fill(p,list[1].getRows());
                hist_p_nHits_ATOF->Fill(p,list[2].getRows());
                
                nentries++;

            }
        }
        // Plots MC::Particle
        TCanvas* canvas1 = new TCanvas("c1","c1 title",1366,768);
        canvas1->Divide(2,2);
        gStyle->SetOptStat("nemruo"); 
        // hist_p
        canvas1->cd(1);
        hist_p->GetXaxis()->SetTitle("p (GeV)");
        hist_p->GetXaxis()->SetTitleSize(0.05);
        hist_p->GetYaxis()->SetTitle("#events");
        hist_p->GetYaxis()->SetTitleSize(0.05);
        hist_p->Draw();
        //hist_theta
        canvas1->cd(2);
        hist_theta->GetXaxis()->SetTitle("#theta");
        hist_theta->GetXaxis()->SetTitleSize(0.05);
        hist_theta->GetYaxis()->SetTitle("#events");
        hist_theta->GetYaxis()->SetTitleSize(0.05);
        hist_theta->Draw();
        //hist_phi
        canvas1->cd(3);
        hist_phi->GetXaxis()->SetTitle("#phi");
        hist_phi->GetXaxis()->SetTitleSize(0.05);
        hist_phi->GetYaxis()->SetTitle("#events");
        hist_phi->GetYaxis()->SetTitleSize(0.05);
        hist_phi->Draw();
        // hist_vz
        canvas1->cd(4);
        hist_vz->GetXaxis()->SetTitle("vz (cm)");
        hist_vz->GetXaxis()->SetTitleSize(0.05);
        hist_vz->GetYaxis()->SetTitle("#events");
        hist_vz->GetYaxis()->SetTitleSize(0.05);
        hist_vz->Draw();
        // SAVE
        TFile outputFile("./output/outputFile.root", "RECREATE");
        hist_p->Write();
        outputFile.Close();

        canvas1->Print("./output/hists_mc_particle.pdf");
        delete hist_p; delete hist_theta; delete hist_phi; delete hist_vz;
        delete canvas1;

        // PLOT nHits_AHDC,ATOF
        TCanvas* canvas2 = new TCanvas("c2","c2 title",1366,768);
        canvas2->Divide(2,2);
        gStyle->SetOptStat("nemruo");
        // hist_nHits_AHDC
        canvas2->cd(1);
        hist_nHits_AHDC->GetXaxis()->SetTitle("nHits");
        hist_nHits_AHDC->GetXaxis()->SetTitleSize(0.05);
        hist_nHits_AHDC->GetYaxis()->SetTitle("#events");
        hist_nHits_AHDC->GetYaxis()->SetTitleSize(0.05);
        hist_nHits_AHDC->Draw();
        // hist_nHits_ATOF
        canvas2->cd(2);
        hist_nHits_ATOF->GetXaxis()->SetTitle("nHits");
        hist_nHits_ATOF->GetXaxis()->SetTitleSize(0.05);
        hist_nHits_ATOF->GetYaxis()->SetTitle("#events");
        hist_nHits_ATOF->GetYaxis()->SetTitleSize(0.05);
        hist_nHits_ATOF->Draw();
        // hist_p_nHits_AHDC
        canvas2->cd(3);
        hist_p_nHits_AHDC->GetXaxis()->SetTitle("p");
        hist_p_nHits_AHDC->GetXaxis()->SetTitleSize(0.05);
        hist_p_nHits_AHDC->GetYaxis()->SetTitle("nHits_AHDC");
        hist_p_nHits_AHDC->GetYaxis()->SetTitleSize(0.05);
        hist_p_nHits_AHDC->SetStats(kFALSE);
        hist_p_nHits_AHDC->Draw("COLZ");
        // hist_p_nHits_ATOF
        canvas2->cd(4);
        hist_p_nHits_ATOF->GetXaxis()->SetTitle("p");
        hist_p_nHits_ATOF->GetXaxis()->SetTitleSize(0.05);
        hist_p_nHits_ATOF->GetYaxis()->SetTitle("nHits_ATOF");
        hist_p_nHits_ATOF->GetYaxis()->SetTitleSize(0.05);
        hist_p_nHits_ATOF->SetStats(kFALSE);
        hist_p_nHits_ATOF->Draw("COLZ");

        //SAVE
        canvas2->Print("./output/hists_nHists.pdf");
        delete hist_nHits_AHDC; delete hist_nHits_ATOF; delete hist_p_nHits_AHDC; delete hist_p_nHits_ATOF;
        delete canvas2;

    } 
    else {
        std::cout << " ***** please provide a filename..." << std::endl;
    }
    return 0;
}


