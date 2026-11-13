// analyze.cc: reads the "events" tree, fills N, pT, eta, phi histograms
// for 8 particle categories and saves them to a ROOT file.
// Run ./analyze --help for options.

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TMath.h"

// Default values
const std::string defaultInput  = "events.root";
const std::string defaultOutput = "analysis.root";

void printUsage() {
    std::cout << "Usage: ./analyze [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -i, --input <path>     input file with events         (default: " << defaultInput << ")\n";
    std::cout << "  -o, --output <path>    output file with histograms    (default: " << defaultOutput << ")\n";
    std::cout << "  -h, --help             show this help and exit\n";
}

int main(int argc, char* argv[]) {
    std::string inName  = defaultInput;
    std::string outName = defaultOutput;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        const std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") { printUsage(); return 0; }
        else if ((arg == "-i" || arg == "--input") && i + 1 < argc) inName = argv[++i];
        else if ((arg == "-o" || arg == "--output") && i + 1 < argc) outName = argv[++i];
        else { 
            std::cerr << "Unknown option: " << arg << std::endl; 
            printUsage(); 
            return 1; 
        }
    }

    std::cout << "Input: " << inName << ", output: " << outName << std::endl;

    TFile inFile(inName.c_str(), "READ");
    if (inFile.IsZombie()) {
        std::cerr << "Error opening " << inName << std::endl;
        return 1;
    }

    TTree* tree = inFile.Get<TTree>("events");
    if (!tree) {
        std::cerr << "Error: could not find tree 'events' in file " << inName << std::endl;
        return 1;
    }

    std::cout << "Number of events in tree: " << tree->GetEntries() << std::endl;

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

    TFile outFile(outName.c_str(), "RECREATE");
    if (outFile.IsZombie()) {
        std::cerr << "Error opening " << outName << std::endl;
        return 1;
    }

    // Particle categories (index k = 0..7): all, charged, pi+, pi-, K+, K-, p, pbar
    const std::vector<std::string> categories = {"all", "ch", "pip", "pim", "Kp", "Km", "p", "pbar"};
    // One histogram per category for each variable: N, pT, eta, phi
    std::vector<TH1D*> hN, hPt, hEta, hPhi;
    // Labels for plot titles
    const std::vector<std::string> labels = {"all final-state", "charged", "#pi^{+}", "#pi^{-}", "K^{+}", "K^{-}", "p", "#bar{p}"};

    for (size_t k = 0; k < categories.size(); k++) {
        const std::string name = categories[k];
        const std::string lab  = labels[k];
        hN.push_back(new TH1D(("hN_" + name).c_str(),
                              ("Multiplicity: " + lab + ";Multiplicity N;Events").c_str(),
                              400, 0, 400));
        hPt.push_back(new TH1D(("hPt_" + name).c_str(),
                               ("p_{T}: " + lab + ";p_{T} [GeV];Particles").c_str(),
                               100, 0, 10));
        hEta.push_back(new TH1D(("hEta_" + name).c_str(),
                                ("#eta: " + lab + ";#eta;Particles").c_str(),
                                100, -5, 5));
        hPhi.push_back(new TH1D(("hPhi_" + name).c_str(),
                                ("#phi: " + lab + ";#phi [rad];Particles").c_str(),
                                64, 0, TMath::Pi()));
    }

    const Long64_t nEvents = tree->GetEntries();

    for (Long64_t i = 0; i < nEvents; i++) {
        tree->GetEntry(i);
        
        if (i % 10000 == 0) std::cout << "Processing event " << i << " / " << nEvents << std::endl;

        // Count particles in each category
        std::vector<int> counts(8, 0);

        for (size_t j = 0; j < id->size(); j++) {
            const int pdg = (*id)[j];
            const float q = (*charge)[j];
            const float pT = (*pt)[j];
            const float etaJ = (*eta)[j];
            const float phiJ = std::abs((*phi)[j]); // fold phi from (-pi, pi] into [0, pi]
            
            std::vector<int> particleCategories = {0}; // all: every final-state particle
            if (q != 0) particleCategories.push_back(1); // ch: charged

            if (pdg == 211) particleCategories.push_back(2); // pi+
            else if (pdg == -211) particleCategories.push_back(3); // pi-
            else if (pdg == 321) particleCategories.push_back(4); // K+
            else if (pdg == -321) particleCategories.push_back(5); // K-
            else if (pdg == 2212) particleCategories.push_back(6); // p
            else if (pdg == -2212) particleCategories.push_back(7); // pbar

            for (size_t c = 0; c < particleCategories.size(); c++) {
                const int k = particleCategories[c];
                counts[k]++;
                hPt[k]->Fill(pT);
                hEta[k]->Fill(etaJ);
                hPhi[k]->Fill(phiJ);
            }
        }

        for (int k = 0; k < 8; k++) hN[k]->Fill(counts[k]);
    }

    outFile.Write();
    outFile.Close();

    std::cout << "Saved histograms to " << outName << std::endl;

    return 0;
}