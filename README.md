# Symulacja i analiza rozkładów cząstek dla pp @ √s = 13.6 TeV (Pythia 8 + ROOT)

Symulacja 100 000 zderzeń proton–proton typu *minimum bias* przy √s = 13.6 TeV
w generatorze Pythia 8 oraz analiza powstających w nich cząstek w ROOT.
Dla wszystkich cząstek, cząstek naładowanych oraz π⁺, π⁻, K⁺, K⁻, p i p̄ wyznaczane są
cztery rozkłady: liczba cząstek w zdarzeniu (krotność N), pęd poprzeczny p_T,
pseudorapidity η i kąt azymutalny φ.

## Struktura

```
pp-collision-analysis/
├── generate.cc        # generacja zdarzeń w Pythii 8 → events.root
├── analyze.cc         # histogramy: events.root → analysis.root
├── draw_plots.C       # makro ROOT, wykresy: analysis.root → plots/
├── Makefile           # kompilacja i uruchomienie całości
├── environment.yml    # środowisko conda z ROOT-em i Pythią
├── analysis.root      # gotowe histogramy (100 000 zdarzeń)
├── plots/
│   ├── png/           # 36 wykresów (32 histogramy + 4 porównawcze)
│   └── pdf/           # te same wykresy w PDF
└── slides/
    └── slides.pdf     # prezentacja
```

`events.root` (~225 MB) nie jest dołączony, tworzy go `./generate`.

## Wymagania

- ROOT 6 (`root-config` w `PATH`)
- Pythia 8 (`pythia8-config` w `PATH`)
- kompilator C++ i `make`

Testowane na Ubuntu (WSL2) z ROOT 6.40.04 i Pythią 8.312.

Kod używa tylko C++17. Makefile przejmuje jednak standard z `root-config` (C++20 w ROOT-cie
z conda-forge), bo program i ROOT muszą być skompilowane w tym samym standardzie.

Instalacja przez condę (dokładne wersje są w `environment.yml`):

```bash
conda env create -f environment.yml
```

## Uruchomienie

```bash
conda activate pp-analysis    # 0) środowisko (w każdym nowym terminalu)
make                          # 1) kompilacja: tworzy ./generate i ./analyze
./generate                    # 2) 100 000 zdarzeń -> events.root (~4.5 min)
./analyze                     # 3) histogramy: events.root -> analysis.root
root -l -b -q draw_plots.C    # 4) wykresy: analysis.root -> plots/
```

Całość można też uruchomić jednym poleceniem:

```bash
make run      # kroki 1-4: kompilacja, generacja, analiza i wykresy
make plots    # tylko wykresy, np. po zmianie ich wyglądu
```

Programy mają opcje, np. liczbę zdarzeń, seed, energię i nazwy plików
(wszystkie z wartościami domyślnymi). Lista: `./generate -h` i `./analyze -h`.

## Ustawienia i uwagi

- Proces `SoftQCD:inelastic` (zderzenia nieelastyczne), seed 12345: ten sam seed daje identyczne wyniki.
- Analizowane są tylko cząstki stanu końcowego (`isFinal()`).
- Zadanie wymaga φ w zakresie 0–π, a Pythia podaje (−π, π], więc kąty ujemne są odbijane na dodatnie (|φ|).
- W 12,5% zdarzeń liczba wszystkich cząstek przekracza 400, czyli zakres `hN_all`;
  takie zdarzenia trafiają do overflow (widocznego w ramce statystyk).