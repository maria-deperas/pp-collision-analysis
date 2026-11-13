CXX = g++
CXXFLAGS = -O2 -Wall -Wextra

ROOTFLAGS = $(shell root-config --cflags)
ROOTLIBS = $(shell root-config --libs)
PYTHIAFLAGS = -I$(shell pythia8-config --includedir)
PYTHIALIBS = $(shell pythia8-config --libs)

all: generate analyze

generate: generate.cc
	$(CXX) $(CXXFLAGS) $(ROOTFLAGS) $(PYTHIAFLAGS) $< -o $@ $(ROOTLIBS) $(PYTHIALIBS)

analyze: analyze.cc
	$(CXX) $(CXXFLAGS) $(ROOTFLAGS) $< -o $@ $(ROOTLIBS)

run: all
	./generate
	./analyze
	root -l -b -q draw_plots.C

plots:
	root -l -b -q draw_plots.C

clean:
	rm -f generate analyze

.PHONY: all run plots clean
