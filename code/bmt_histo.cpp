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
        hipo::banklist list = r.getBanks({"MC::Particle","BMT::adc","FMT::adc"});
        long unsigned int nentries =0;

        double px,py,pz;
        double p, theta, phi;
        double vz;

        TH1D* hist_p = new TH1D("hist_p","MC::Particle #rightarrow p",100, 0,12);
        TH1D* hist_theta = new TH1D("hist_theta","MC::Particle #rightarrow #theta",100, 0,M_PI);
        TH1D* hist_phi = new TH1D("hist_phi","MC::Particle #rightarrow #phi",100, 0,2*M_PI);
        TH1D* hist_vz = new TH1D("hist_vz","MC::Particle #rightarrow vz",100, -20,20);

        TH1I* hist_nHits_BMT = new TH1I("hist_nHits_BMT","Number of Hits in BMT",100,0,830);
        TH1I* hist_nHits_FMT = new TH1I("hist_nHits_FMT","Number of Hits in FMT",100,0,8);
        TH2D* hist_p_nHits_BMT = new TH2D("hist_p_nHits_BMT","Correlation between p and nHits_BMT",100,0,12,100,0,830);
        TH2D* hist_p_nHits_FMT = new TH2D("hist_p_nHits_FMT","Correlation between p and nHits_FMT",100,0,0.4,100,0,10);

        double max_p=0;
        int max_nHits=0; 

        while( r.next(list)){
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

                if (max_p < p) {max_p = p;}

                // nHits ; remark list[0].getRows() == 1, otherwise this portion of code need to be outside for {...} 
                hist_nHits_BMT->Fill(list[1].getRows());
                hist_nHits_FMT->Fill(list[2].getRows());
                hist_p_nHits_BMT->Fill(p,list[1].getRows());
                hist_p_nHits_FMT->Fill(p,list[2].getRows());
                
                if (max_nHits < list[1].getRows()) { max_nHits = list[1].getRows();}
                nentries++;

            }
        }
        std::cout << "Max_p : " << max_p << std::endl;
        std::cout << "Max_nHits : " << max_nHits << std::endl; 

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

        canvas1->Print("./output/bmt_mc_particle.pdf");
        delete hist_p; delete hist_theta; delete hist_phi; delete hist_vz;
        delete canvas1;

        // PLOT nHits_BMT,FMT
        TCanvas* canvas2 = new TCanvas("c2","c2 title",1366,1366);
        canvas2->Divide(1,2);
        gStyle->SetOptStat("nemruo");
        // hist_nHits_BMT
        canvas2->cd(1);
        hist_nHits_BMT->GetXaxis()->SetTitle("nHits");
        hist_nHits_BMT->GetXaxis()->SetTitleSize(0.05);
        hist_nHits_BMT->GetYaxis()->SetTitle("#events");
        hist_nHits_BMT->GetYaxis()->SetTitleSize(0.05);
        hist_nHits_BMT->Draw();
        // hist_nHits_FMT
        /*canvas2->cd(2);
        hist_nHits_FMT->GetXaxis()->SetTitle("nHits");
        hist_nHits_FMT->GetXaxis()->SetTitleSize(0.05);
        hist_nHits_FMT->GetYaxis()->SetTitle("#events");
        hist_nHits_FMT->GetYaxis()->SetTitleSize(0.05);
        hist_nHits_FMT->Draw();*/
        // hist_p_nHits_BMT
        canvas2->cd(2);
        hist_p_nHits_BMT->GetXaxis()->SetTitle("p");
        hist_p_nHits_BMT->GetXaxis()->SetTitleSize(0.05);
        hist_p_nHits_BMT->GetYaxis()->SetTitle("nHits_BMT");
        hist_p_nHits_BMT->GetYaxis()->SetTitleSize(0.05);
        hist_p_nHits_BMT->SetStats(kFALSE);
        hist_p_nHits_BMT->Draw("COLZ");
        // hist_p_nHits_FMT
        /*canvas2->cd(4);
        hist_p_nHits_FMT->GetXaxis()->SetTitle("p");
        hist_p_nHits_FMT->GetXaxis()->SetTitleSize(0.05);
        hist_p_nHits_FMT->GetYaxis()->SetTitle("nHits_FMT");
        hist_p_nHits_FMT->GetYaxis()->SetTitleSize(0.05);
        hist_p_nHits_FMT->SetStats(kFALSE);
        hist_p_nHits_FMT->Draw("COLZ");*/

        //SAVE
        canvas2->Print("./output/bmt_nHits.pdf");
        delete hist_nHits_BMT; delete hist_nHits_FMT; delete hist_p_nHits_BMT; delete hist_p_nHits_FMT;
        delete canvas2;

    } 
    else {
        std::cout << " ***** please provide a filename..." << std::endl;
    }
    return 0;
}


