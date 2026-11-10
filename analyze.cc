#include <iostream>
#include <vector>
#include <string>
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TMath.h"

int main() {
    TFile inFile("events.root", "READ");
    if (inFile.IsZombie()) {
        std::cerr << "Nie mozna otworzyc events.root" << std::endl;
        return 1;
    }

    TTree* tree = inFile.Get<TTree>("events");
    if (!tree) {
        std::cerr << "Nie mozna znalezc drzewa 'events' w pliku events.root" << std::endl;
        return 1;
    }

    std::cout << "Liczba zdarzen w drzewie: " << tree->GetEntries() << std::endl;

    std::vector<int>* id = nullptr;
    std::vector<float>* charge = nullptr;
    std::vector<float>* pt = nullptr;
    std::vector<float>* eta = nullptr;
    std::vector<float>* phi = nullptr;

    tree->SetBranchAddress("id", &id);
    tree->SetBranchAddress("charge", &charge);
    tree->SetBranchAddress("pt", &pt);
    tree->SetBranchAddress("eta", &eta);
    tree->SetBranchAddress("phi", &phi);

    tree->GetEntry(0);
    std::cout << "Liczba czastek w pierwszym zdarzeniu: " << id->size() << std::endl;
    std::cout << (*id)[0] << " " << (*charge)[0] << " " << (*pt)[0] << " " << (*eta)[0] << " " << (*phi)[0] << std::endl;

    TFile outFile("analysis.root", "RECREATE");
    if (outFile.IsZombie()) {
        std::cerr << "Nie mozna otworzyc analysis.root" << std::endl;
        return 1;
    }

        const std::vector<std::string> categories = {"all", "ch", "pip", "pim", "Kp", "Km", "p", "pbar"};
    std::vector<TH1D*> hN, hPt, hEta, hPhi;

    for (const std::string& cat : categories) {
        hN.push_back(new TH1D(("hN_" + cat).c_str(),
                              ("Multiplicity (" + cat + ");Multiplicity N;Events").c_str(),
                              400, 0, 400));
        hPt.push_back(new TH1D(("hPt_" + cat).c_str(),
                               ("p_{T} (" + cat + ");p_{T} [GeV];Particles").c_str(),
                               100, 0, 10));
        hEta.push_back(new TH1D(("hEta_" + cat).c_str(),
                                ("#eta (" + cat + ");#eta;Particles").c_str(),
                                100, -5, 5));
        hPhi.push_back(new TH1D(("hPhi_" + cat).c_str(),
                                ("#phi (" + cat + ");#phi [rad];Particles").c_str(),
                                64, 0, TMath::Pi()));
    }

    std::cout << "Utworzono " << 4 * hN.size() << " histogramow, np. " << hPt[2]->GetName() << std::endl;

    return 0;
}