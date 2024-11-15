// ------------------------------------------------------------
//
// Date : July 29, 2024
// Author : Felix Touchte Codjo
// 
// This code is an analysis of the value obtained during the 
// decoding of the AHDC signal. 
//
// ------------------------------------------------------------

#include <iostream>
#include <vector>
#include <cmath>

#include "reader.h"
#include "futils.h"

#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TStyle.h"



int main(int argc, char const *argv[]){
	//const char* filename = "./data/simu.hipo";
	//const char* filename = "./data/simu_100k.hipo";
	const char* filename = "./data/new_simu_100k.hipo";
	hipo::reader  r(filename);
	hipo::banklist list = r.getBanks({"MC::Particle","AHDC::adc","ATOF::adc"});
	long unsigned int nEvents =0;

	// AHDC::adc
	double t_start, t_cfd, mctime, t_ovr, mcEtot;
	int max_value, integral, noise, nsteps;
	double delta_start, delta_cfd;

	TH1D* hist_t_start = new TH1D("hist_t_start","#bf{a)} t_start",600, 400,1000);
	TH1D* hist_t_cfd = new TH1D("hist_t_cfd","#bf{b)} t_cfd",600, 400,1000);
	TH1D* hist_mctime = new TH1D("hist_mctime","#bf{e)} mctime",400, 0,400);
	TH1D* hist_t_ovr = new TH1D("hist_t_ovr","#bf{f)} t_ovr",2000, 0,2000);

	TH1D* hist_delta_start = new TH1D("hist_delta_start","#bf{c)} #Delta t_{start}",600, 400,1000);
	TH1D* hist_delta_cfd = new TH1D("hist_delta_cfd","#bf{d)} #Delta t_{cfd}",600, 400,1000);

	TH1D* hist_mcEtot = new TH1D("hist_mcEtot","#bf{c)} hist_mcEtot",1000, 0,30);



	TH1I* hist_nsteps = new TH1I("hist_nsteps","#bf{j)} nsteps",100, 0,50);
	TH1I* hist_max_value = new TH1I("hist_max_value","#bf{g)} max_value",4095, 0,4095);
	TH1I* hist_integral = new TH1I("hist_integral","#bf{h)} integral",1000, 0,60000);
	TH1I* hist_noise = new TH1I("hist_noise","#bf{i)} noise",600, 0,600);

	TH2D* hist2d_start_cfd = new TH2D("hist2d_start_cfd","#bf{a)} Correlation between #bf{t_start} and #bf{t_cfd}",100,500,1000,100,500,1000);
	TH2D* hist2d_start_mctime = new TH2D("hist2d_start_mctime","#bf{b)} Correlation between #bf{t_start} and #bf{mctime}",100,500,1000,100,0,400);
	TH2D* hist2d_cfd_mctime = new TH2D("hist2d_cfd_mctime","#bf{c)} Correlation between #bf{t_cfd} and #bf{mctime}",100,500,1000,100,0,400);

	TH2D* hist2d_start_magnitude = new TH2D("hist2d_start_magnitude","#bf{a)} Correlation between #Delta t_{start} and amplitude",600,400,1000,1000,0,4095);
	TH2D* hist2d_cfd_magnitude = new TH2D("hist2d_cfd_magnitude","#bf{b)} Correlation between #Delta t_{cfd} and amplitude",600,400,1000,1000,0,4095);

	TH2D* hist2d_adc_mcEtot = new TH2D("hist2d_adc_mcEtot","#bf{a)} Correlation between max_value and mcEtot",4095,0,4095,30,0,30);
	TH2D* hist2d_integral_mcEtot = new TH2D("hist2d_integral_mcEtot","#bf{b)} Correlation between integral and mcEtot",1000,0,8000,30,0,30);

	// MC::Particle
	double px, py, pz;
	double p, theta, phi;
	double vz;
	TH1D* hist_p = new TH1D("hist_p","MC::Particle #rightarrow p",100, 0,300); // Mev
	TH1D* hist_theta = new TH1D("hist_theta","MC::Particle #rightarrow #theta",100, 0,180); // deg
	TH1D* hist_phi = new TH1D("hist_phi","MC::Particle #rightarrow #phi",100, 0,360); // phi
	TH1D* hist_vz = new TH1D("hist_vz","MC::Particle #rightarrow vz",100, -20,20);

	// nHits
	TH1I* hist_nHits_AHDC = new TH1I("hist_nHits_AHDC","Number of Hits in AHDC",100,0,35);
	TH1I* hist_nHits_ATOF = new TH1I("hist_nHits_ATOF","Number of Hits in ATOF",100,0,8);
	TH2D* hist_p_nHits_AHDC = new TH2D("hist_p_nHits_AHDC","Correlation between p and nHits_AHDC",100,0,0.4,100,0,40);
	TH2D* hist_p_nHits_ATOF = new TH2D("hist_p_nHits_ATOF","Correlation between p and nHits_ATOF",100,0,0.4,100,0,10);

	while( r.next(list)){ // loop over events
		// MC informations
		px = list[0].getFloat("px",0);
		py = list[0].getFloat("py",0);
		pz = list[0].getFloat("pz",0);
		vz = list[0].getFloat("vz",0);
		futils::cart2polar(px,py,pz,p,theta,phi); // p (GeV); theta and phi (rad)
		hist_p->Fill(p*1000); // MeV
		hist_theta->Fill(theta*180/M_PI); // deg
		hist_phi->Fill(phi*180/M_PI); // deg
		hist_vz->Fill(vz); // cm
		// nHits
		hist_nHits_AHDC->Fill(list[1].getRows());
		hist_nHits_ATOF->Fill(list[2].getRows());
		hist_p_nHits_AHDC->Fill(p,list[1].getRows());
		hist_p_nHits_ATOF->Fill(p,list[2].getRows());
		
		// AHDC::adc
		for(int itr = 0; itr < list[1].getRows(); itr++){ // loop over Hits for the case of AHDC::adc or ATOF::adc
			t_start = list[1].getFloat("timestamp",itr);
			t_cfd = list[1].getFloat("t_cfd",itr);
			mctime = list[1].getFloat("mctime",itr);
			t_ovr = list[1].getFloat("time",itr);
			nsteps = list[1].getInt("nsteps",itr);
			max_value = list[1].getInt("ADC",itr);
			integral = list[1].getInt("integral",itr);
			noise = list[1].getInt("ped",itr);
			mcEtot = list[1].getFloat("mcEtot",itr);
			delta_start = t_start-mctime;
			delta_cfd = t_cfd-mctime;
			//delta_start = delta_start - 625.6; // centered at 0
			//delta_cfd = delta_cfd - 589.1; // centerd at 0 

			if (true){
			//if ((mctime > 10) and (mctime < 400)){ // Define your cut
			//if (nsteps > 0){
			//if ((nsteps > 0) and (1000*p > 70) and (1000*p < 80)){
				// Fill histograms
				hist_t_start->Fill(t_start);
				hist_t_cfd->Fill(t_cfd);
				hist_mctime->Fill(mctime);
				hist_t_ovr->Fill(t_ovr);
				hist_nsteps->Fill(nsteps);
				hist_max_value->Fill(max_value);
				hist_integral->Fill(integral);
				hist_noise->Fill(noise);
				hist_mcEtot->Fill(mcEtot);

				hist_delta_start->Fill(delta_start);
				hist_delta_cfd->Fill(delta_cfd);

				hist2d_start_cfd->Fill(t_start,t_cfd);
				hist2d_start_mctime->Fill(t_start,mctime);
				hist2d_cfd_mctime->Fill(t_cfd,mctime);

				hist2d_start_magnitude->Fill(delta_start,max_value);
				hist2d_cfd_magnitude->Fill(delta_cfd,max_value);
				hist2d_adc_mcEtot->Fill(max_value,mcEtot);
				hist2d_integral_mcEtot->Fill(integral,mcEtot);
			}

		}

		nEvents++;
	}
	std::cout << "Number of events : " << nEvents << std::endl;


	
	// *******************************
	//          AHDC::adc
	// *******************************
	TCanvas* canvas1 = new TCanvas("c1","c1 title",2000,3000);
	canvas1->Divide(2,5);
	gStyle->SetOptStat("nemruo"); 

	// hist_t_start
	canvas1->cd(1);
	hist_t_start->GetXaxis()->SetTitle("t_start (ns)");
	hist_t_start->GetXaxis()->SetTitleSize(0.05);
	hist_t_start->GetYaxis()->SetTitle("#events");
	hist_t_start->GetYaxis()->SetTitleSize(0.05);
	hist_t_start->Draw();
	// hist_t_cfd
	canvas1->cd(2);
	hist_t_cfd->GetXaxis()->SetTitle("t_cfd (ns)");
	hist_t_cfd->GetXaxis()->SetTitleSize(0.05);
	hist_t_cfd->GetYaxis()->SetTitle("#events");
	hist_t_cfd->GetYaxis()->SetTitleSize(0.05);
	hist_t_cfd->Draw();
	// hist_mctime
	canvas1->cd(5);
	hist_mctime->GetXaxis()->SetTitle("mctime (ns)");
	hist_mctime->GetXaxis()->SetTitleSize(0.05);
	hist_mctime->GetYaxis()->SetTitle("#events");
	hist_mctime->GetYaxis()->SetTitleSize(0.05);
	hist_mctime->Draw();
	// hist_t_ovr
	canvas1->cd(6);
	hist_t_ovr->GetXaxis()->SetTitle("t_ovr (ns)");
	hist_t_ovr->GetXaxis()->SetTitleSize(0.05);
	hist_t_ovr->GetYaxis()->SetTitle("#events");
	hist_t_ovr->GetYaxis()->SetTitleSize(0.05);
	hist_t_ovr->Draw();
	// nsteps
	canvas1->cd(10);
	hist_nsteps->GetXaxis()->SetTitle("nsteps");
	hist_nsteps->GetXaxis()->SetTitleSize(0.05);
	hist_nsteps->GetYaxis()->SetTitle("#events");
	hist_nsteps->GetYaxis()->SetTitleSize(0.05);
	hist_nsteps->Draw();
	// max_value
	canvas1->cd(7);
	hist_max_value->GetXaxis()->SetTitle("max_value (adc)");
	hist_max_value->GetXaxis()->SetTitleSize(0.05);
	hist_max_value->GetYaxis()->SetTitle("#events");
	hist_max_value->GetYaxis()->SetTitleSize(0.05);
	hist_max_value->Draw();
	// integral
	canvas1->cd(8);
	hist_integral->GetXaxis()->SetTitle("integral (adc per 44 ns)");
	hist_integral->GetXaxis()->SetTitleSize(0.05);
	hist_integral->GetYaxis()->SetTitle("#events");
	hist_integral->GetYaxis()->SetTitleSize(0.05);
	hist_integral->Draw();
	// noise
	canvas1->cd(9);
	hist_noise->GetXaxis()->SetTitle("noise (adc)");
	hist_noise->GetXaxis()->SetTitleSize(0.05);
	hist_noise->GetYaxis()->SetTitle("#events");
	hist_noise->GetYaxis()->SetTitleSize(0.05);
	hist_noise->Draw();
	// hist_delta_start
	canvas1->cd(3);
	hist_delta_start->GetXaxis()->SetTitle("t_start - mctime (ns)");
	hist_delta_start->GetXaxis()->SetTitleSize(0.05);
	hist_delta_start->GetYaxis()->SetTitle("#events");
	hist_delta_start->GetYaxis()->SetTitleSize(0.05);
		// analysis
		//std::cout << "maximum     : " << hist_delta_start->GetMaximum() << std::endl;
		//std::cout << "maximum bin : " << hist_delta_start->GetMaximumBin() << std::endl;
	hist_delta_start->Draw();
	// hist_delta_cfd
	canvas1->cd(4);
	hist_delta_cfd->GetXaxis()->SetTitle("t_cfd - mctime (ns)");
	hist_delta_cfd->GetXaxis()->SetTitleSize(0.05);
	hist_delta_cfd->GetYaxis()->SetTitle("#events");
	hist_delta_cfd->GetYaxis()->SetTitleSize(0.05);
	hist_delta_cfd->Draw();

	// SAVE
	canvas1->Print("./output/Decoding_analysis.pdf");
	delete hist_t_start;
	delete hist_t_cfd;
	delete hist_mctime;
	delete hist_t_ovr;
	delete hist_nsteps;
	delete hist_max_value;
	delete hist_integral;
	delete hist_noise;
	delete hist_delta_cfd;
	delete hist_delta_start;
	delete canvas1;

	// **************************************
	//     Study of correlation in AHDC::adc
	// **************************************

	TCanvas* canvas2 = new TCanvas("c2","c2 title",2400,1400);
	canvas2->Divide(2,2);
	gStyle->SetOptStat("nemruo");
	
	// hist2d_start_cfd
	canvas2->cd(1);
	hist2d_start_cfd->GetXaxis()->SetTitle("t_start");
	hist2d_start_cfd->GetXaxis()->SetTitleSize(0.05);
	hist2d_start_cfd->GetYaxis()->SetTitle("t_cfd");
	hist2d_start_cfd->GetYaxis()->SetTitleSize(0.05);
	//hist2d_start_cfd->SetStats(kFALSE);
	hist2d_start_cfd->Draw("COLZ");
	// hist2d_start_mctime
	canvas2->cd(3);
	hist2d_start_mctime->GetXaxis()->SetTitle("t_start");
	hist2d_start_mctime->GetXaxis()->SetTitleSize(0.05);
	hist2d_start_mctime->GetYaxis()->SetTitle("mctime");
	hist2d_start_mctime->GetYaxis()->SetTitleSize(0.05);
		//hist2d_start_mctime->SetStats(kFALSE);
	hist2d_start_mctime->Draw("COLZ");
	// hist2d_cfd_mctime
	canvas2->cd(4);
	hist2d_cfd_mctime->GetXaxis()->SetTitle("t_cfd");
	hist2d_cfd_mctime->GetXaxis()->SetTitleSize(0.05);
	hist2d_cfd_mctime->GetYaxis()->SetTitle("mctime");
	hist2d_cfd_mctime->GetYaxis()->SetTitleSize(0.05);
		//hist2d_cfd_mctime->SetStats(kFALSE);
	hist2d_cfd_mctime->Draw("COLZ");
	// SAVE
	canvas2->Print("./output/Decoding2d1_analysis.pdf");

	TCanvas* canvas22 = new TCanvas("c22","c22 title",2200,2000);
	canvas22->Divide(2,3);
	// hist2d_start_magnitude
	canvas22->cd(1);
	hist2d_start_magnitude->GetXaxis()->SetTitle("#Delta t_{start}");
	hist2d_start_magnitude->GetXaxis()->SetTitleSize(0.05);
	hist2d_start_magnitude->GetYaxis()->SetTitle("amplitude");
	hist2d_start_magnitude->GetYaxis()->SetTitleSize(0.05);
		//hist2d_start_magnitude->SetStats(kFALSE);
	hist2d_start_magnitude->Draw("COLZ");
	// hist2d_cfd_magnitude
	canvas22->cd(2);
	hist2d_cfd_magnitude->GetXaxis()->SetTitle("#Delta t_{cfd}");
	hist2d_cfd_magnitude->GetXaxis()->SetTitleSize(0.05);
	hist2d_cfd_magnitude->GetYaxis()->SetTitle("amplitude");
	hist2d_cfd_magnitude->GetYaxis()->SetTitleSize(0.05);
		//hist2d_cfd_magnitude->SetStats(kFALSE);
	hist2d_cfd_magnitude->Draw("COLZ");
	// time error as a function of magnitude 
	// error t_start
	std::cout << "nbins : " << hist2d_start_magnitude->GetYaxis()->GetNbins() << std::endl;
	int Nbins = hist2d_start_magnitude->GetYaxis()->GetNbins();
	double wbins = hist2d_start_magnitude->GetYaxis()->GetBinWidth(0);
	TGraph* gr_start = new TGraph(Nbins);
	TH1D* hist_tmp = hist2d_start_magnitude->ProjectionX("_px20",Nbins/5,Nbins/5,"[20,21]"); canvas22->cd(3); hist_tmp->SetTitle(TString::Format("#bf{c)} projection of #Delta t_{start} for amp = %.0lf", 400 + wbins*Nbins/5)); hist_tmp->Draw();
	double stdev;
	for (int k=0;k<Nbins;k++){
		stdev = hist2d_start_magnitude->ProjectionX("_px",k,k,"")->GetStdDev();
		gr_start->SetPoint(k,k*wbins,stdev);
	}
	canvas22->cd(5); 
	gr_start->SetTitle("#bf{e)} StdDev #Delta t_{start} vs amplitude");
	gr_start->GetXaxis()->SetTitle("amplitude (adc)");
	gr_start->GetXaxis()->SetTitleSize(0.05);
	gr_start->GetYaxis()->SetTitle("StdDev (ns)");
	gr_start->GetYaxis()->SetTitleSize(0.05);
	//gr2->SetLineStyle(1);
	gr_start->SetLineColor(kBlue+2);
	gr_start->SetMarkerColor(kRed);
	gr_start->SetMarkerSize(5);
	gr_start->Draw("ALP");

	// error t_cfd
	Nbins = hist2d_cfd_magnitude->GetYaxis()->GetNbins();
	wbins = hist2d_cfd_magnitude->GetYaxis()->GetBinWidth(0);
	TGraph* gr_cfd = new TGraph(Nbins);
	TH1D* hist_tmp2 = hist2d_cfd_magnitude->ProjectionX("_px21",Nbins/5,Nbins/5,"[20,21]"); canvas22->cd(4); hist_tmp2->SetTitle(TString::Format("#bf{d)} projection of #Delta t_{cfd} for amp = %.0lf", 400 + wbins*Nbins/5)); hist_tmp2->Draw();
	for (int k=0;k<Nbins;k++){
		stdev = hist2d_cfd_magnitude->ProjectionX("_pxx",k,k,"")->GetStdDev();
		gr_cfd->SetPoint(k,k*wbins,stdev);
	}
	canvas22->cd(6); 
	gr_cfd->SetTitle("#bf{f)} StdDev #Delta t_{cfd} vs amplitude");
	gr_cfd->GetXaxis()->SetTitle("amplitude (adc)");
	gr_cfd->GetXaxis()->SetTitleSize(0.05);
	gr_cfd->GetYaxis()->SetTitle("StdDev (ns)");
	gr_cfd->GetYaxis()->SetTitleSize(0.05);
	//gr2_cfd->SetLineStyle(1);
	gr_cfd->SetLineColor(kBlue+2);
	gr_cfd->SetMarkerColor(kRed);
	gr_cfd->SetMarkerSize(5);
	gr_cfd->Draw("ALP");

	// SAVE
	canvas22->Print("./output/Decoding2d2_analysis.pdf");
	delete hist2d_start_cfd;
	delete hist2d_start_mctime;
	delete hist2d_cfd_mctime;
	delete hist2d_cfd_magnitude;
	delete hist2d_start_magnitude;
	delete canvas2;

	TCanvas* canvas222 = new TCanvas("c22","c22 title",2200,1200);
	canvas222->Divide(2,2);
	// hist2d_adc_mcEtot
	canvas222->cd(1);
	hist2d_adc_mcEtot->GetXaxis()->SetTitle("max_value");
	hist2d_adc_mcEtot->GetXaxis()->SetTitleSize(0.05);
	hist2d_adc_mcEtot->GetYaxis()->SetTitle("mcEtot");
	hist2d_adc_mcEtot->GetYaxis()->SetTitleSize(0.05);
		//hist2d_adc_mcEtot->SetStats(kFALSE);
	hist2d_adc_mcEtot->Draw("COLZ");
	// hist2d_integral_mcEtot
	canvas222->cd(2);
	hist2d_integral_mcEtot->GetXaxis()->SetTitle("integral");
	hist2d_integral_mcEtot->GetXaxis()->SetTitleSize(0.05);
	hist2d_integral_mcEtot->GetYaxis()->SetTitle("mcEtot");
	hist2d_integral_mcEtot->GetYaxis()->SetTitleSize(0.05);
		//hist2d_integral_mcEtot->SetStats(kFALSE);
	hist2d_integral_mcEtot->Draw("COLZ");
	// hist_mcEtot
	canvas222->cd(3);
	hist_mcEtot->GetXaxis()->SetTitle("mcEtot");
	hist_mcEtot->GetXaxis()->SetTitleSize(0.05);
	hist_mcEtot->GetYaxis()->SetTitle("#events");
	hist_mcEtot->GetYaxis()->SetTitleSize(0.05);
		//hist_mcEtot->SetStats(kFALSE);
	hist_mcEtot->Draw();


	// SAVE
	canvas222->Print("./output/Decoding2d3_analysis.pdf");



	// *****************************************
	//          PlOT MC::Particle
	// *****************************************

	TCanvas* canvas3 = new TCanvas("c1","c1 title",1366,768);
	canvas3->Divide(2,2);
	gStyle->SetOptStat("nemruo"); 
	// hist_p
	canvas3->cd(1);
	hist_p->GetXaxis()->SetTitle("p (MeV)");
	hist_p->GetXaxis()->SetTitleSize(0.05);
	hist_p->GetYaxis()->SetTitle("#events");
	hist_p->GetYaxis()->SetTitleSize(0.05);
	hist_p->Draw();
	//hist_theta
	canvas3->cd(2);
	hist_theta->GetXaxis()->SetTitle("#theta (deg)");
	hist_theta->GetXaxis()->SetTitleSize(0.05);
	hist_theta->GetYaxis()->SetTitle("#events");
	hist_theta->GetYaxis()->SetTitleSize(0.05);
	hist_theta->Draw();
	//hist_phi
	canvas3->cd(3);
	hist_phi->GetXaxis()->SetTitle("#phi (deg)");
	hist_phi->GetXaxis()->SetTitleSize(0.05);
	hist_phi->GetYaxis()->SetTitle("#events");
	hist_phi->GetYaxis()->SetTitleSize(0.05);
	hist_phi->Draw();
	// hist_vz
	canvas3->cd(4);
	hist_vz->GetXaxis()->SetTitle("vz (cm)");
	hist_vz->GetXaxis()->SetTitleSize(0.05);
	hist_vz->GetYaxis()->SetTitle("#events");
	hist_vz->GetYaxis()->SetTitleSize(0.05);
	hist_vz->Draw();
	// SAVE
	canvas3->Print("./output/mc_particle.pdf");
	delete hist_p; 
	delete hist_theta; 
	delete hist_phi; 
	delete hist_vz;
	delete canvas3;

	// *****************************************
	//          PLOT nHits_AHDC,ATOF
	// *****************************************
	TCanvas* canvas4 = new TCanvas("c2","c2 title",1366,768);
	canvas4->Divide(2,2);
	gStyle->SetOptStat("nemruo");
	// hist_nHits_AHDC
	canvas4->cd(1);
	hist_nHits_AHDC->GetXaxis()->SetTitle("nHits");
	hist_nHits_AHDC->GetXaxis()->SetTitleSize(0.05);
	hist_nHits_AHDC->GetYaxis()->SetTitle("#events");
	hist_nHits_AHDC->GetYaxis()->SetTitleSize(0.05);
	hist_nHits_AHDC->Draw();
	// hist_nHits_ATOF
	canvas4->cd(2);
	hist_nHits_ATOF->GetXaxis()->SetTitle("nHits");
	hist_nHits_ATOF->GetXaxis()->SetTitleSize(0.05);
	hist_nHits_ATOF->GetYaxis()->SetTitle("#events");
	hist_nHits_ATOF->GetYaxis()->SetTitleSize(0.05);
	hist_nHits_ATOF->Draw();
	// hist_p_nHits_AHDC
	canvas4->cd(3);
	hist_p_nHits_AHDC->GetXaxis()->SetTitle("p");
	hist_p_nHits_AHDC->GetXaxis()->SetTitleSize(0.05);
	hist_p_nHits_AHDC->GetYaxis()->SetTitle("nHits_AHDC");
	hist_p_nHits_AHDC->GetYaxis()->SetTitleSize(0.05);
	hist_p_nHits_AHDC->SetStats(kFALSE);
	hist_p_nHits_AHDC->Draw("COLZ");
	// hist_p_nHits_ATOF
	canvas4->cd(4);
	hist_p_nHits_ATOF->GetXaxis()->SetTitle("p");
	hist_p_nHits_ATOF->GetXaxis()->SetTitleSize(0.05);
	hist_p_nHits_ATOF->GetYaxis()->SetTitle("nHits_ATOF");
	hist_p_nHits_ATOF->GetYaxis()->SetTitleSize(0.05);
	hist_p_nHits_ATOF->SetStats(kFALSE);
	hist_p_nHits_ATOF->Draw("COLZ");
	//SAVE
	canvas4->Print("./output/nHits.pdf");
	delete hist_nHits_AHDC; 
	delete hist_nHits_ATOF; 
	delete hist_p_nHits_AHDC; 
	delete hist_p_nHits_ATOF;
	delete canvas4;


	return 0;
}
