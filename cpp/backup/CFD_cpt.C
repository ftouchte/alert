//________________________________________________________________________________________________
// 
// Date : 4 juillet 2024
// Auteur : gabriel.charles@ijclab.in2p3.fr
//
// Ce code ouvre le fichier root créé par Noise_cpt.C (CMN + zero suppress)
// Il cherche le jeux de paramètres le plus adaptés pour appliquer un discriminateur à fraction constante
//
// Testé pour le moment uniquement sur des données acquisent avec un détecteur équipé de fils de carbone
// le résultat n'est pas convaincant. Probablement parce que dans le cas du carbone résistif, il existe
// justement une corrélation entre amplitude et temps d'arrivée
//
//________________________________________________________________________________________________





#include <iostream>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TMath.h>
#include <TProfile.h>
#include <algorithm>



void CFD_cpt() {

//________________________________________________________________________________________________
//  __________________________________________ Variables ___________________________________________
//________________________________________________________________________________________________

 bool debug = false;
 const int Nlim_start = 4; // limande du fil à regarder
 const int Wire_selec = 17;// fil à regarder

 TCanvas *canvas = new TCanvas("canvas", "Amplitude maximale vs Temps du maximum", 1200, 800);
 canvas->Divide(2,1);

 // Amplitude maximale en fonction du temps du maximum
 TH2D *h2 = new TH2D("h2", "Amplitude maximale vs Temps du maximum;Temps du maximum;Amplitude maximale", 21, 0, 20, 100, 0, 4096);
 TProfile *prof = new TProfile("prof", "Amplitude maximale vs Temps du maximum;Temps du maximum;Amplitude maximale", 21, 0, 20, 0, 4096);

 TF1 *FitExpo = new TF1("FitExpo", "TMath::Exp(TMath::Exp(x*[0]+[1]))", 4, 13);

//________________________________________________________________________________________________
//  ___________________________________________ Files ______________________________________________
//________________________________________________________________________________________________

    // Ouvrir le fichier ROOT contenant l'arbre ToutFiltered
    TFile *file = TFile::Open("Am_1700_CMN.root");
    if (!file || file->IsZombie()) {
        std::cerr << "Erreur en ouvrant le fichier Am_1700_CMN.root" << std::endl;
        return;
    }

    // Obtenir l'arbre ToutFiltered
    TTree *tree = (TTree*)file->Get("ToutFiltered");
    if (!tree) {
        std::cerr << "Erreur : Arbre 'ToutFiltered' non trouvé dans le fichier" << std::endl;
        return;
    }

    // Déclarer les variables pour stocker les valeurs des branches
    int eveN;
    std::vector<int> *WireN = nullptr;
    std::vector<int> *LimN = nullptr;
    std::vector<std::vector<double>> *q_cmn = nullptr;
    std::vector<int> *Time = nullptr;

    // Fixer les adresses des branches
    tree->SetBranchAddress("eveN", &eveN);
    tree->SetBranchAddress("WireN", &WireN);
    tree->SetBranchAddress("LimN", &LimN);
    tree->SetBranchAddress("q_cmn", &q_cmn);
    tree->SetBranchAddress("Time", &Time);

//________________________________________________________________________________________________
//  ________________________________________ Event loop ____________________________________________
//________________________________________________________________________________________________

    // Définir la gamme de valeurs pour le délai et l'atténuation
    std::vector<int> delays = {1, 2, 3, 5, 6}; // Exemple de valeurs de délai en indices de temps
    std::vector<double> fractions = {0.1, 0.2, 0.4, 0.6, 0.8}; // Exemple de valeurs d'atténuation en fraction

    // Stocker les résultats des ajustements
    std::vector<std::tuple<int, double, double>> results; // Stockera (délai, fraction, exposant de l'ajustement)
    
    // Collection pour stocker les TProfiles
    std::vector<TProfile*> profiles;

    Long64_t nEntries = tree->GetEntries();

   bool gr1_IsUsed = false;
    // Boucler sur les combinaisons de délai et d'atténuation
    for (int delay : delays) {
        for (double fraction : fractions) {
            
cout << delay << "  " << fraction << endl;

            // Créer un TProfile pour cette combinaison de paramètres
            TProfile *prof_CFD = new TProfile(Form("prof_CFD_d%d_f%.1f", delay, fraction), "Amplitude maximale vs Temps du maximum (CFD);Temps du maximum;Amplitude maximale", 25, -4, 20, 0, 4096);
            profiles.push_back(prof_CFD); // Ajouter à la collection

            // Boucler sur les événements
            for(Long64_t eve = 0; eve < nEntries; ++eve){

               tree->GetEntry(eve);

               // Boucler sur les canaux
               for(size_t j = 0; j < WireN->size(); ++j){

                  if(LimN->at(j)!=Nlim_start){
                    break;
                  }

                  int wire = WireN->at(j);

                  if(wire!=Wire_selec){continue;}
                  std::vector<double> &signal = q_cmn->at(j);

                  // Trouver l'amplitude maximale et son temps
                  double max_amp = -1;
                  int max_time = -1;
                  TGraph *gr1 = new TGraph(signal.size()); 
                  for(int k = 0; k < signal.size(); ++k){
                     if(signal[k] > max_amp) {
                        max_amp = signal[k];
                        max_time = k;
                     }
                     gr1->SetPoint(k,k,signal.at(k));
                  }
                  if (eve == 1) {
                     if (!gr1_IsUsed){
                        TCanvas* canvas1 = new TCanvas("c1","c1 title",1366,768);
                        gr1->Draw("APL");
                        canvas1->Print("./output/test_CFD.pdf");
                        gr1_IsUsed = true;
                     }
                  }
                  // Ajuste une gaussienne autour du maximum
                  int start = std::max(0, max_time - 3);
                  int end = std::min((int)signal.size(), max_time + 4);
                  if(end - start < 7) continue; // S'assurer qu'il y a au moins 7 points

                  TGraph *graph = new TGraph();
                  for(int k = start; k < end; ++k){
                     graph->SetPoint(graph->GetN(), k, signal[k]);
                  }

                  TF1 *FitGaus = new TF1("FitGaus", "gaus", start, end);
                  graph->Fit(FitGaus, "Q");
                  double fitted_max_amp = FitGaus->Eval(FitGaus->GetParameter(1));

                  if(fitted_max_amp<3700){
                     h2->Fill(FitGaus->GetParameter(1), fitted_max_amp);
                     if(delay==delays.at(0) && fraction==fractions.at(0)) prof->Fill(FitGaus->GetParameter(1), fitted_max_amp);
                  }
                  else continue;

                  // Générer les deux signaux fractionnés
                  std::vector<double> signal1(signal.size());
                  std::vector<double> signal2(signal.size(), 0.0);

                  for(size_t k = 0; k < signal.size(); ++k) {
                     signal1[k] = -fraction * signal[k];
                     if(k >= delay) {
                        signal2[k] = /*(1 - fraction) **/ signal[k - delay];
                     }
                  }

                  // Ajouter les deux signaux
                  std::vector<double> combined_signal(signal.size());
                  for(size_t k = 0; k < signal.size(); ++k) {
                     combined_signal[k] = signal1[k] + signal2[k];
                  }

                  // Trouver le point de croisement
                  double crossing_time = -1;
                  for(size_t k = delay; k < signal.size(); ++k) {
                     if(combined_signal[k] > 0 && combined_signal[k - 1] < 0) {
                        crossing_time = (combined_signal[k] * (k - 1) - combined_signal[k - 1] * k) / (combined_signal[k] - combined_signal[k - 1]); // t0 = y2x1-y1x2 / (y2-y1)
                        break;
                     }
                  }

                  // Ajouter au TProfile
                  if(1/*crossing_time >= 0 && crossing_time < signal.size()*/) {
                     prof_CFD->Fill(crossing_time, fitted_max_amp);
                  }

                  delete graph;
                  delete FitGaus;
              }    
           }

           // Ajuster le TProfile avec une exponentielle d'exponentielle
           TF1 *FitExpo_CFD = new TF1("FitExpo_CFD", "TMath::Exp(TMath::Exp(x*[0]+[1]))", 4, 13);
           prof_CFD->Fit(FitExpo_CFD, "EQR");
           double exponent = FitExpo_CFD->GetParameter(0);

           // Stocker le résultat
           results.emplace_back(delay, fraction, exponent);

           // Ajouter les coefficients au titre du TProfile
           prof_CFD->SetTitle(Form("CFD (delay=%d, fraction=%.1f) - Expo: %.2f", delay, fraction, exponent));

           delete FitExpo_CFD;
        }
     }

    // Afficher les résultats
    for (const auto& result : results) {
        int delay;
        double fraction, exponent;
        std::tie(delay, fraction, exponent) = result;
        std::cout << "Délai: " << delay << ", Fraction: " << fraction << ", Exposant: " << exponent << std::endl;
    }

    // Dessiner l'histogramme 2D et le TProfile original
    canvas->cd(1);
    h2->Draw("colz");

    canvas->cd(2);
    prof->Draw("");
    prof->Fit(FitExpo,"EQR");
    std::cout << FitExpo->GetParameter(0) << "  " << FitExpo->GetParameter(1) << std::endl;

    // Créer un canvas pour afficher tous les TProfiles
    TCanvas *multiCanvas = new TCanvas("multiCanvas", "TProfiles for Different Delays and Fractions", 1600, 1200);
    multiCanvas->Divide(5, 5);

    for (size_t i = 0; i < profiles.size(); ++i) {
        multiCanvas->cd(i + 1);
        profiles[i]->Draw();
    }

    multiCanvas->Update();
}

