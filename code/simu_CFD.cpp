#include <iostream>
#include <fstream>
#include <vector>

#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TGaxis.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TLatex.h"
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
            std::vector<double> Fraction = {0.1,0.3,0.5,0.7,0.9};
            std::vector<double> Delay = {5,10,15,20,25,30,35,40,45,50,55,60};
            std::vector<double> time; // vector for saving t_cfd
            for (double fraction : Fraction) {
                for (int delay : Delay){
                    std::vector<double> signal(Npts,0.0);
                    std::cout << "signal : " << std::endl;

                    // Remove noise
                    double noise=0;
                    for (int i=0;i<5;i++){
                        noise += Dgtz.at(i);
                    }
                    noise = noise/5;
                    double ymax = 0; // for plotting
                    for (int i=0;i<Npts;i++){
                        if (ymax < Dgtz.at(i)) ymax = Dgtz.at(i);
                        Dgtz[i] = Dgtz.at(i) - noise;
                    }
                    double delta_y = ymax*(1+fraction); // for plotting
                    // End remove noise
                    for (int i=0;i<Npts;i++){
                        //signal[i] += -1*fraction*Dgtz.at(i);
                        //if (i >= delay) {
                        //    signal[i] += Dgtz.at(i-delay);
                        //}
                        signal[i] += Dgtz.at(i);
                        if (i < Npts-delay){
                            signal[i] += -1*fraction*Dgtz.at(i+delay);
                        }
                        std::cout << signal.at(i) << " "; 
                    }
                    std::cout << std::endl;
                    // Determine t_cfd
                    // Convention : dernier passage en bas de zéro et compris entre les min et max absolues
                    int i_min=0, i_max=0;
                    for (int i=0;i<Npts;i++){
                        if (signal.at(i_max) < signal.at(i)) i_max = i;
                        if (signal.at(i_min) > signal.at(i)) i_min = i;
                    }
                    int i_ref = 0;
                    for (int i=i_min;i<=i_max;i++){
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
                    time.push_back(t_cfd);
                    std::cout << "--------------------------------------" << std::endl;
                    std::cout << "|       i_min  :  "  << i_min << std::endl;
                    std::cout << "|       i_max  :  "  << i_max << std::endl;
                    std::cout << "|       i_ref  :  "  << i_ref << std::endl;
                    std::cout << "|       t_CFD  :  "  << t_cfd << std::endl;
                    std::cout << "--------------------------------------" << std::endl;

                    TCanvas* canvas1 = new TCanvas("c1","c1 title",1366,768);
                    TGraph* gr1 = new TGraph(Npts);
                    TGraph* gr2 = new TGraph(Npts);
                    for (int i=0;i<Npts;i++){
                        gr1->SetPoint(i,i,Dgtz.at(i));
                        gr2->SetPoint(i,i,signal.at(i));
                    }
                    gr2->SetTitle(TString::Format("CFD,  fraction = %.1lf, delay = %d index units",fraction,delay));
                    gr2->GetXaxis()->SetTitle("Time (index unit)");
                    gr2->GetXaxis()->SetTitleSize(0.05);
                    gr2->GetYaxis()->SetTitle("Charge (adc)");
                    gr2->GetYaxis()->SetTitleSize(0.05);
                    //gr2->SetLineStyle(1);
                    gr2->SetLineColor(kRed);
                    gr2->SetMarkerColor(kRed);
                    gr2->SetMarkerSize(5);
                    gr2->GetYaxis()->SetRangeUser(-ymax*fraction-delta_y*0.1,ymax+delta_y*0.1);
                    gr2->Draw("APL");

                    gr1->SetMarkerColor(kBlue);
                    gr1->SetMarkerSize(5);
                    gr1->SetLineColor(kBlue);
                    //gr1->SetLineStyle(2);
                    gr1->Draw("PL");

                    TGaxis* axis1 = new TGaxis(0,0,136,0,0,136,510,"");
                    axis1->SetLineColor(kGreen);
                    axis1->SetLabelColor(kGreen);
                    axis1->Draw();

                    TLegend* legend = new TLegend(0.7,0.8,0.9,0.9);
                    legend->AddEntry(gr1,"Digitized signal","l");
                    legend->AddEntry(gr2,"CFD signal","l");
                    legend->Draw();

                    TLatex data;
                    data.SetTextSize(0.03);
                    data.SetTextAlign(13);
                    data.DrawLatexNDC(0.7,0.6,TString::Format("#bf{#bf{i_cfd} =  %.2lf}",t_cfd).Data());
                    data.DrawLatexNDC(0.7,0.6-1*0.05,TString::Format("#bf{#bf{t_cfd} =  %.2lf ns}",t_cfd*44).Data());

                    canvas1->Print(TString::Format("./output/CFD_%.1lf_%d.pdf",fraction,delay));
                    //canvas1->Print("./output/CFD.pdf");
                    delete gr1; delete gr2; delete canvas1;
                    delete axis1;
                }
            }
            // -----------------------
            // Bilan
            // -----------------------
            TCanvas* canvas2 = new TCanvas("c1","c1 title",1366,768);
            TGraph* gr_cfd = new TGraph(time.size());
            int k=0;
            for (double fraction : Fraction){
                for (int delay : Delay){
                    gr_cfd->SetPoint(k,delay,fraction); 
                    //data.DrawLatex(delay,fraction,TString::Format("#bf{%.2lf}",time.at(k)));
                    k++;
                }
            }
            gr_cfd->SetTitle("Time from CFD");
            gr_cfd->GetXaxis()->SetTitle("delay (index units)");
            gr_cfd->GetXaxis()->SetTitleSize(0.05);
            gr_cfd->GetYaxis()->SetTitle("fraction");
            gr_cfd->GetYaxis()->SetTitleSize(0.05);
            //gr2->SetLineStyle(1);
            //gr_cfd->SetLineColor(kRed);
            gr_cfd->SetMarkerColor(kRed);
            gr_cfd->SetMarkerStyle(47);
            //gr_cfd->SetMarkerSize(5);
            gr_cfd->Draw("AP");

            TLatex data;
            data.SetTextSize(0.02);
            data.SetTextAlign(13);
            k = 0;
            for (double fraction : Fraction){
                for (int delay : Delay){
                    data.DrawLatex(delay,fraction,TString::Format("#bf{%.2lf}",44*time.at(k)));
                    k++;
                }
            }
            canvas2->Print("./output/Bilan_CFD.pdf");
            delete gr_cfd;
            delete canvas2;
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
