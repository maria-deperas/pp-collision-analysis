// draw_plots.C
// Draws histograms from analysis.root and saves them as PNG and PDF in plots/:
//   - one plot per histogram (32 plots)
//   - comparison plots of identified hadrons (compare_*)
// Run: root -l -b -q draw_plots.C
//      root -l -b -q 'draw_plots.C("other.root")'

#include <iostream>
#include <string>
#include <vector>
#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TLegend.h"

void draw_plots(const char* inName = "analysis.root") {
    gSystem->mkdir("plots", true);

    TFile file(inName, "READ");
    if (file.IsZombie()) {
        std::cerr << "Error: cannot open " << inName << std::endl;
        return;
    }

    // Plot style, applies to every canvas created below
    gStyle->SetPadLeftMargin(0.13);
    gStyle->SetPadRightMargin(0.05);
    gStyle->SetPadBottomMargin(0.12);
    gStyle->SetPadTopMargin(0.12);
    gStyle->SetTitleY(0.97);
    gStyle->SetTitleFontSize(0.04);
    gStyle->SetTitleOffset(1.4, "Y");

    // Stats box: name, entries, mean, std dev, overflow
    gStyle->SetOptStat("nemro");

    // Histogram names from analyze.cc and readable labels for the titles
    const std::vector<std::string> categories = {"all", "ch", "pip", "pim", "Kp", "Km", "p", "pbar"};
    const std::vector<std::string> labels = {"all final-state particles", "charged particles",
                                             "#pi^{+}", "#pi^{-}", "K^{+}", "K^{-}",
                                             "protons", "antiprotons"};

    // Line colour per category
    const std::vector<int> colors = {kBlack, kGray + 2, // all, charged
                                     kRed + 1, kRed - 7, // pi+, pi-
                                     kBlue + 1, kAzure + 6, // K+, K-
                                     kGreen + 2, kSpring + 5}; // p, pbar

    const std::vector<std::string> variables = {"N", "Pt", "Eta", "Phi"};
    const std::vector<std::string> titleStart = {"Multiplicity N of ",
                                                 "Transverse momentum p_{T} of ",
                                                 "Pseudorapidity #eta of ",
                                                 "Azimuthal angle |#phi| of "};
    const std::vector<std::string> axes = {";N;Events",
                                           ";p_{T} [GeV];Particles",
                                           ";#eta;Particles",
                                           ";|#phi| [rad];Particles"};

    for (size_t v = 0; v < variables.size(); v++) {
        const std::string& var = variables[v];
        const bool logY = (var == "N" || var == "Pt");

        for (size_t k = 0; k < categories.size(); k++) {
            const std::string name = "h" + var + "_" + categories[k];
            TH1D* h = file.Get<TH1D>(name.c_str());
            if (!h) {
                std::cerr << "Missing histogram " << name << std::endl;
                continue;
            }

            TCanvas c("c", "c", 800, 600);
            if (logY) c.SetLogy();

            const double maxY = h->GetBinContent(h->GetMaximumBin());
            if (logY) {
                h->SetMinimum(0.5);
                h->SetMaximum(maxY * 10);
            } else {
                h->SetMinimum(0);
                h->SetMaximum(maxY * 1.4);
            }

            h->SetTitle((titleStart[v] + labels[k] + axes[v]).c_str());
            h->SetLineWidth(2);
            h->SetLineColor(colors[k]);
            h->Draw("HIST");

            c.SaveAs(("plots/" + name + ".png").c_str());
            c.SaveAs(("plots/" + name + ".pdf").c_str());
        }
    }

    // Comparison plots for identified hadrons
    for (size_t v = 0; v < variables.size(); v++) {
        const std::string& var = variables[v];
        const bool logY = (var == "N" || var == "Pt");
 
        TCanvas c("c", "c", 800, 600);
        if (logY) c.SetLogy();
 
        TLegend leg(0.68, 0.66, 0.92, 0.86);
        leg.SetBorderSize(0);
        leg.SetFillStyle(0);
        leg.SetNColumns(2);

        std::vector<TH1D*> hists;
        double maxY = 0;
        for (size_t k = 2; k < categories.size(); k++) {
            const std::string name = "h" + var + "_" + categories[k];
            TH1D* h = file.Get<TH1D>(name.c_str());
            if (!h) {
                std::cerr << "Missing histogram " << name << std::endl;
                continue;
            }
            h->SetStats(false);
            h->SetLineColor(colors[k]);
            h->SetLineWidth(2);
            h->SetLineStyle(k % 2 == 0 ? 1 : 2); // particle solid, antiparticle dashed
            leg.AddEntry(h, labels[k].c_str(), "l");
            hists.push_back(h);
 
            const double hMax = h->GetBinContent(h->GetMaximumBin());
            if (hMax > maxY) maxY = hMax;
        }
        if (hists.empty()) continue;
 
        hists[0]->SetTitle((titleStart[v] + "identified hadrons" + axes[v]).c_str());
        if (logY) {
            hists[0]->SetMinimum(0.5);
            hists[0]->SetMaximum(maxY * 10);
        } else {
            hists[0]->SetMinimum(0);
            hists[0]->SetMaximum(maxY * 1.6);
        }
 
        hists[0]->Draw("HIST");
        for (size_t i = 1; i < hists.size(); i++) hists[i]->Draw("HIST SAME");
        leg.Draw();
 
        c.SaveAs(("plots/compare_" + var + ".png").c_str());
        c.SaveAs(("plots/compare_" + var + ".pdf").c_str());
    }

    std::cout << "Done: plots saved in plots/" << std::endl;
}
