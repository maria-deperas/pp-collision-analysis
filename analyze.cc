#include <iostream>
#include "TFile.h"
#include "TTree.h"

int main() {
    TFile inFile("events.root", "READ");

    TTree* tree = (TTree*)inFile.Get("events");

    if (!tree) {
        std::cerr << "Nie mozna znalezc drzewa 'events' w pliku events.root" << std::endl;
        return 1;
    }

    if (inFile.IsZombie()) {
        std::cerr << "Nie mozna otworzyc events.root" << std::endl;
        return 1;
    }

    std::cout << "Liczba zdarzen w drzewie: " << tree->GetEntries() << std::endl;

    return 0;
}