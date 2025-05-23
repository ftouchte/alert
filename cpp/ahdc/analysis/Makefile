#********************************************************
# MAKEFILE for BUILDING EXAMPLES FOR HIPO4 LIBRARY
# AUTHOR: GAVALIAN DATE: 10/24/2018
#
# Version modifiée par Felix Touchte Codjo pour son 
# usage personnel. Merci GAVALIAN ! 
# Date : 12 avril 2024
#********************************************************

# ATTENTION

# Les "paths" sont en chemin relatifs (rq: les ".."). Prière de localiser le dossier *hipo/hipo4...
# Quelques options de compilations
# 	-I<path> : Ajoute le répertoire <path> au chemin de recherche des fichiers d'en-tête.
# 	-L<path> : Ajoute le répertoire <path> au chemin de recherche des bibliothèques.

#PATH2HIPO := /homeijclab/touchte-codjo/hipo
#PATH2HIPO := /home/ftouchte/hipo
PATH2HIPO := /home/touchte-codjo/framework/hipo

HIPOCFLAGS  := -I$(PATH2HIPO)/hipo4 -I$(PATH2HIPO)/hipo4/chart   
HIPOLIBS    := -L$(PATH2HIPO)/lib -lhipo4 
									
LZ4LIBS     := -L$(PATH2HIPO)/lz4/lib -llz4
LZ4INCLUDES := -I$(PATH2HIPO)/lz4/lib

FELIXFLAGS := -I/homeijclab/touchte-codjo/Bureau/alert/cpp/ahdc

# ROOT libraries 
ROOTLIBS = $(shell root-config --libs)
# ROOT include flags
ROOTCFLAGS = $(shell root-config --cflags)

GTKLIBS = $(shell pkg-config --libs gtkmm-4.0)
GTKFLAGS = $(shell pkg-config --cflags gtkmm-4.0)

CXX       := g++
CXXFLAGS  += -Wall -fPIC -std=c++17
LD        := g++
LDFLAGS   :=


#all:  showFile histo plot benchmark simu
all: gui mon12 exampleAhdcDetector monitor color ahdcView histAhdcAdc corrTimeAmp simpleTest

#ahdcExtractor: ahdcExtractor.o 
#	$(CXX) -o ahdcExtractor.exe $^ $(HIPOLIBS) $(LZ4LIBS) $(ROOTLIBS)
gui: gui.o ahdcExtractor.o
	$(CXX) -o gui.exe $^ $(HIPOLIBS) $(LZ4LIBS) $(ROOTLIBS) $(GTKLIBS)

mon12: mon12.o ahdcExtractor.o Point3D.o AhdcDetector.o
	$(CXX) -o mon12.exe $^ $(HIPOLIBS) $(LZ4LIBS) $(ROOTLIBS)

exampleAhdcDetector: exampleAhdcDetector.o ahdcExtractor.o Point3D.o AhdcDetector.o
	$(CXX) -o exampleAhdcDetector.exe $^ $(HIPOLIBS) $(LZ4LIBS) $(ROOTLIBS)

monitor: monitor.o ahdcExtractor.o Point3D.o AhdcGeom.o 
	$(CXX) -o monitor.exe $^ $(HIPOLIBS) $(LZ4LIBS) $(ROOTLIBS)

color: color.o 
	$(CXX) -o color.exe $^ $(HIPOLIBS) $(LZ4LIBS) $(ROOTLIBS)

ahdcView: ahdcView.o ahdcExtractor.o Point3D.o AhdcGeom.o
	$(CXX) -o ahdcView.exe $^ $(HIPOLIBS) $(LZ4LIBS) $(ROOTLIBS)

simpleTest: simpleTest.o ahdcExtractor.o
	$(CXX) -o simpleTest.exe $^ $(HIPOLIBS) $(LZ4LIBS) $(ROOTLIBS)

histAhdcAdc: histAhdcAdc.o ahdcExtractor.o
	$(CXX) -o histAhdcAdc.exe $^ $(HIPOLIBS) $(LZ4LIBS) $(ROOTLIBS)

corrTimeAmp: corrTimeAmp.o ahdcExtractor.o
	$(CXX) -o corrTimeAmp.exe $^ $(HIPOLIBS) $(LZ4LIBS) $(ROOTLIBS)

#histo: histo.o futils.o
#	$(CXX) -o histo.exe $^ $(HIPOLIBS) $(LZ4LIBS) $(ROOTLIBS) 

#benchmark: benchmark.o futils.o
#	$(CXX) -o benchmark.exe $^  $(RAPID_CHECK_LIBS) $(CATCH2_FLLAGS)


# $< représente la première de la cible, i.e histo.o
# $^ représente la liste complète des dépendances

clean:
	@echo 'Removing all build files'
	@rm -rf *.o *~ *.exe example*hipo *.pdf

%.o: %.cpp
	$(CXX) -c $< -O2 $(CXXFLAGS) $(HIPOCFLAGS) $(LZ4INCLUDES) $(ROOTCFLAGS) $(GTKFLAGS) 
	
#	$(CXX) -c $< -O2 $(CXXFLAGS) $(HIPOCFLAGS) $(LZ4INCLUDES) $(ROOTCFLAGS) $(RAPID_CHECK_FLAGS) $(FELIXFLAGS)

