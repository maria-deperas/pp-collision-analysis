#include "Pythia8/Pythia.h"
#include "TFile.h"
#include "TTree.h"
#include <vector>
#include <iostream>
#include <string>
using namespace Pythia8;

int main() {
    const int nEvents = 100000;
    const int seed = 12345;

    Pythia pythia;
    pythia.readString("Beams:eCM = 13600");
    pythia.readString("SoftQCD:inelastic = on");
    pythia.readString("Random:setSeed = on");
    pythia.readString("Random:seed = " + std::to_string(seed));
    pythia.readString("Next:numberShowEvent = 0");
    
    if (!pythia.init()) {
        std::cerr << "Blad inicjalizacji Pythii" << std::endl;
        return 1;
    }

    TFile file("events.root", "RECREATE");

    if (file.IsZombie()) {
        std::cerr << "Nie mozna otworzyc events.root" << std::endl;
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

    for (int i = 0; i < nEvents; ++i) {
        if (!pythia.next()) continue;

        id.clear();
        charge.clear();
        pt.clear();
        eta.clear();
        phi.clear();

        for (int j = 0; j < pythia.event.size(); ++j) {
            if (pythia.event[j].isFinal()) {
                id.push_back(pythia.event[j].id());
                charge.push_back(pythia.event[j].charge());
                pt.push_back(pythia.event[j].pT());
                eta.push_back(pythia.event[j].eta());
                phi.push_back(pythia.event[j].phi());
            }
        }

        tree.Fill();
    }

    pythia.stat(); // podsumowanie

    tree.Write();
    file.Close();

    return 0;
}