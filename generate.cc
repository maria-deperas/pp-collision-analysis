// generate.cc: minimum-bias pp events (Pythia 8, SoftQCD:inelastic) -> ROOT TTree "events".
// One entry per event; vectors id, charge, pt, eta, phi of final-state particles.
// Run ./generate --help for options.

#include "Pythia8/Pythia.h"
#include "TFile.h"
#include "TTree.h"
#include <vector>
#include <iostream>
#include <string>
#include <stdexcept>
using namespace Pythia8;

// Default values
const int defaultEvents = 100000;
const int defaultSeed = 12345;
const double defaultEcm = 13600.0;
const std::string defaultOutput = "events.root";

void printUsage() {
    std::cout << "Usage: ./generate [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -n, --events <int>     number of events to generate   (default: " << defaultEvents << ")\n";
    std::cout << "  -s, --seed <int>       random seed, 0 = time-based    (default: " << defaultSeed << ")\n";
    std::cout << "  -e, --ecm <float>      collision energy sqrt(s) [GeV] (default: " << defaultEcm << ")\n";
    std::cout << "  -o, --output <path>    output ROOT file               (default: " << defaultOutput << ")\n";
    std::cout << "  -h, --help             show this help and exit\n";
}

int main(int argc, char* argv[]) {
    int nEvents = defaultEvents;
    int seed = defaultSeed;
    double eCM = defaultEcm;
    std::string outName = defaultOutput;

    // Parse command line arguments
    try {
        for (int i = 1; i < argc; i++) {
            const std::string arg = argv[i];
            if (arg == "-h" || arg == "--help") { printUsage(); return 0; }
            else if ((arg == "-n" || arg == "--events") && i + 1 < argc) nEvents = std::stoi(argv[++i]);
            else if ((arg == "-s" || arg == "--seed") && i + 1 < argc) seed = std::stoi(argv[++i]);
            else if ((arg == "-e" || arg == "--ecm") && i + 1 < argc) eCM = std::stod(argv[++i]);
            else if ((arg == "-o" || arg == "--output") && i + 1 < argc) outName = argv[++i];
            else { 
                std::cerr << "Unknown option: " << arg << std::endl; 
                printUsage(); 
                return 1; 
            }
        }
    } catch (const std::exception&) {
        std::cerr << "Invalid value for option" << std::endl;
        printUsage();
        return 1;
    }

    if (nEvents <= 0 || eCM <= 0 || seed < 0 || seed > 900000000) {
        std::cerr << "Invalid settings: events > 0, ecm > 0, seed in 0-900000000" << std::endl;
        return 1;
    }

    std::cout << "Events: " << nEvents << ", seed: " << seed
              << ", sqrt(s): " << eCM << " GeV, output: " << outName << std::endl;

    Pythia pythia;
    pythia.readString("Beams:eCM = " + std::to_string(eCM));
    pythia.readString("SoftQCD:inelastic = on");
    pythia.readString("Random:setSeed = on");
    pythia.readString("Random:seed = " + std::to_string(seed));
    pythia.readString("Next:numberShowEvent = 0");
    pythia.readString("Next:numberCount = 10000");
    
    if (!pythia.init()) {
        std::cerr << "Error initializing Pythia" << std::endl;
        return 1;
    }

    TFile file(outName.c_str(), "RECREATE");

    if (file.IsZombie()) {
        std::cerr << "Error opening " << outName << std::endl;
        return 1;
    }

    TTree tree("events", "Generated events");

    std::vector<int> id;
    std::vector<float> charge, pt, eta, phi;

    tree.Branch("id", &id);
    tree.Branch("charge", &charge);
    tree.Branch("pt", &pt);
    tree.Branch("eta", &eta);
    tree.Branch("phi", &phi);

    int nFailed = 0;

    for (int i = 0; i < nEvents; ++i) {
        if (!pythia.next()) { 
            nFailed++; 
            continue; 
        }

        id.clear();
        charge.clear();
        pt.clear();
        eta.clear();
        phi.clear();

        for (int j = 0; j < pythia.event.size(); ++j) {
            const Particle& p = pythia.event[j];
            if (!p.isFinal()) continue;
            id.push_back(p.id());
            charge.push_back(p.charge());
            pt.push_back(p.pT());
            eta.push_back(p.eta());
            phi.push_back(p.phi());
        }

        tree.Fill();
    }

    pythia.stat();

    std::cout << "Saved " << tree.GetEntries() << " events, failed: " << nFailed << std::endl;

    tree.Write();
    file.Close();

    return 0;
}