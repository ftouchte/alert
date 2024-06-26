#********************************************************
# MAKEFILE for BUILDING EXAMPLES FOR HIPO4 LIBRARY
# AUTHOR: GAVALIAN DATE: 10/24/2018
#
# Version modifiée par Felix Touchte Codjo pour son 
# usage personnel. Merci GAVALIAN ! 
# Date : 04 avril 2024
#********************************************************

# ATTENTION

# Les "paths" sont en chemin relatifs (rq: les ".."). Prière de localiser le dossier *hipo/hipo4...
# Quelques options de compilations
# 	-I<path> : Ajoute le répertoire <path> au chemin de recherche des fichiers d'en-tête.
# 	-L<path> : Ajoute le répertoire <path> au chemin de recherche des bibliothèques.


HIPOCFLAGS  := -I/home/ftouchte/Desktop/hipo/hipo/hipo4 -I/home/ftouchte/Desktop/hipo/hipo/hipo4/chart   
HIPOLIBS    := -L/home/ftouchte/Desktop/hipo/hipo/lib -lhipo4 
									
LZ4LIBS     := -L/home/ftouchte/Desktop/hipo/hipo/lz4/lib -llz4
LZ4INCLUDES := -I/home/ftouchte/Desktop/hipo/hipo/lz4/lib

FELIXFLAGS := -I/home/ftouchte/Desktop/hipo/ALERT/code

# ROOT libraries 
ROOTLIBS = $(shell root-config --libs)
# ROOT include flags
ROOTCFLAGS = $(shell root-config --cflags)

#rapidCheck include directories
RAPID_CHECK_FLAGS := -I/home/ftouchte/rapidcheck/include #-I/home/ftouchte/rapidcheck/include/rapidcheck
#rapidCheck linking directories
RAPID_CHECK_LIBS := -L/home/ftouchte/rapidcheck -lrapidcheck

# Catch2
CATCH2_FLLAGS := -L/home/ftouchte/Catch2/build/src -lCatch2 -lCatch2Main

CXX       := g++
CXXFLAGS  += -Wall -fPIC -std=c++17
LD        := g++
LDFLAGS   :=


#all:  showFile histo plot benchmark simu
all: simu tmp decoder showFile histo

showFile: showFile.o
	$(CXX) -o showFile.exe $<  $(HIPOLIBS) $(LZ4LIBS)

histo: histo.o futils.o
	$(CXX) -o histo.exe $^ $(HIPOLIBS) $(LZ4LIBS) $(ROOTLIBS)

plot: plot.o futils.o
	$(CXX) -o plot.exe $^ $(HIPOLIBS) $(LZ4LIBS) $(ROOTLIBS)

benchmark: benchmark.o futils.o
	$(CXX) -o benchmark.exe $^  $(RAPID_CHECK_LIBS) $(CATCH2_FLLAGS)

simu: simu.o futils.o
	$(CXX) -o simu.exe $^  $(RAPID_CHECK_LIBS) $(CATCH2_FLLAGS) $(ROOTLIBS)

tmp: tmp.o futils.o
	$(CXX) -o tmp.exe $^  $(RAPID_CHECK_LIBS) $(CATCH2_FLLAGS) $(ROOTLIBS)

decoder: decoder.o futils.o
	$(CXX) -o decoder.exe $^  $(RAPID_CHECK_LIBS) $(CATCH2_FLLAGS) $(ROOTLIBS)

# $< représente la première de la cible, i.e histo.o
# $^ représente la liste complète des dépendances

clean:
	@echo 'Removing all build files'
	@rm -rf *.o *~ *.exe example*hipo ./output/*.pdf

%.o: %.cpp
	$(CXX) -c $< -O2 $(CXXFLAGS) $(HIPOCFLAGS) $(LZ4INCLUDES) $(ROOTCFLAGS) $(RAPID_CHECK_FLAGS) $(FELIXFLAGS)

