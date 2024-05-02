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


all:  showFile histo plot prog 

showFile: showFile.o
	$(CXX) -o showFile.exe $<  $(HIPOLIBS) $(LZ4LIBS)

histo: histo.o futils.o
	$(CXX) -o histo.exe $^ $(HIPOLIBS) $(LZ4LIBS) $(ROOTLIBS)

plot: plot.o futils.o
	$(CXX) -o plot.exe $^ $(HIPOLIBS) $(LZ4LIBS) $(ROOTLIBS)

prog: prog.o futils.o
	$(CXX) -o prog.exe $^  $(RAPID_CHECK_LIBS) $(CATCH2_FLLAGS)

# $< représente la première de la cible, i.e histo.o
# $^ représente la liste complète des dépendances

clean:
	@echo 'Removing all build files'
	@rm -rf *.o *~ *.exe example*hipo 

%.o: %.cpp
	$(CXX) -c $< -O2 $(CXXFLAGS) $(HIPOCFLAGS) $(LZ4INCLUDES) $(ROOTCFLAGS) $(RAPID_CHECK_FLAGS) $(FELIXFLAGS)
