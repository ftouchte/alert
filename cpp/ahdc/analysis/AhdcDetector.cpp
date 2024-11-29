/************************************************
 * AhdcDetector.cpp
 *
 * @author Felix Touchte Codjo
 * @date November 28, 2024
 * **********************************************/

#include "AhdcDetector.h"
#include <cmath>
#include <iostream>


/*******************************
 * Ahdcwire
 * ****************************/

/** Constructor */
AhdcWire::AhdcWire(Point3D _top, Point3D _bot) : top(_top), bot(_bot) {}

/** (Default) Constructor */
AhdcWire::AhdcWire(double x1, double y1, double z1, double x2, double y2, double z2) { 
	top = Point3D(x1,y1,z1); 
	bot = Point3D(x2,y2,z2);
}


/*******************************
 * AhdcLayer
 * ****************************/

/** (Default) Constructor */
AhdcLayer::AhdcLayer(int _nwires, double _rlayer, double _stereoangle) : nwires(_nwires), rlayer(_rlayer), stereoangle(_stereoangle) {
	if (nwires > 0) {
		ptrWires = new AhdcWire[nwires];
		for (int id = 0; id < nwires; id++){
			// All distance is in mm !
			Point3D top, bot;
			double alpha = futils::toRadian(360.0/nwires);
			top.x = -rlayer*sin(alpha*(id-1)); // id numerotation goes from 1 to nwires
			top.y = -rlayer*cos(alpha*(id-1));
			top.z = 0; // sometimes set to -150
			bot.x = -rlayer*sin(alpha*(id-1)+futils::toRadian(stereoangle));
			bot.y = -rlayer*cos(alpha*(id-1)+stereoangle);
			bot.z = 300; // sometimes set to +150
			ptrWires[id] = AhdcWire(top,bot);
		}
	}
}

/** Copy constructor */
AhdcLayer::AhdcLayer(const AhdcLayer & obj){
	nwires = obj.nwires;
	rlayer = obj.rlayer;
	stereoangle = obj.stereoangle;
	ptrWires = new AhdcWire[nwires];
	for (int id = 0; id < nwires; id++){
		ptrWires[id] = obj.ptrWires[id];
	}
}


/** Affectation operator */
AhdcLayer & AhdcLayer::operator=(const AhdcLayer & obj){
	nwires = obj.nwires;
	rlayer = obj.rlayer;
	stereoangle = obj.stereoangle;
	if (ptrWires != nullptr)
		delete[] ptrWires;
	ptrWires = new AhdcWire[nwires];
	for (int id = 0; id < nwires; id++){
		ptrWires[id] = obj.ptrWires[id];
	}
	return *this;
}

/** Destructor */
AhdcLayer::~AhdcLayer(){
	if (ptrWires != nullptr)
		delete[] ptrWires;
}

/** Return a pointer to the wire number id */
AhdcWire* AhdcLayer::GetAhdcWire(int id) {return ptrWires + id;} // arithmétiques des pointeurs !

/** Return the number of wires */
int AhdcLayer::GetNumberOfWires() {return nwires;}


/*******************************
 * AhdcSuperLayer
 * ****************************/

/** (Default) Constructor */
AhdcSuperLayer::AhdcSuperLayer(int _nlayers, int _nwires, double _rsuperlayer, int _orientation) : nlayers(_nlayers), nwires(_nwires), rsuperlayer(_rsuperlayer), orientation(_orientation) {
	if (nlayers > 0) {
		ptrLayers = new AhdcLayer[nlayers];
		for (int id = 0; id < nlayers; id++){
			ptrLayers[id] = AhdcLayer(nwires,rsuperlayer+4*(id-1),orientation*20);
		}
	}
}

/** Copy constructor */
AhdcSuperLayer::AhdcSuperLayer(const AhdcSuperLayer & obj){
	nlayers = obj.nlayers;
	nwires = obj.nwires;
	rsuperlayer = obj.rsuperlayer;
	orientation = obj.orientation;
	ptrLayers = new AhdcLayer[nlayers];
	for (int id = 0; id < nlayers; id++){
		ptrLayers[id] = obj.ptrLayers[id];
	}
}

/** Affectation operator */
AhdcSuperLayer & AhdcSuperLayer::operator=(const AhdcSuperLayer & obj){
	nlayers = obj.nlayers;
	nwires = obj.nwires;
	rsuperlayer = obj.rsuperlayer;
	orientation = obj.orientation;
	if (ptrLayers != nullptr) 
		delete[] ptrLayers;
	ptrLayers = new AhdcLayer[nlayers];
	for (int id = 0; id < nlayers; id++){
		ptrLayers[id] = obj.ptrLayers[id];
	}
	return *this;
}
/** Destructor */
AhdcSuperLayer::~AhdcSuperLayer(){
	if (ptrLayers != nullptr)
		delete[] ptrLayers;
}

/** Return a pointer to the layer number id */
AhdcLayer* AhdcSuperLayer::GetLayer(int id) {return ptrLayers + id;}

/** Return the number of layer */
int AhdcSuperLayer::GetNumberOfLayers(){ return nlayers;}



/*******************************
 * AhdcSector
 * ****************************/

AhdcSector::AhdcSector(int _nsuperlayers) : nsuperlayers(_nsuperlayers) {
	ptrSuperLayers = new AhdcSuperLayer[nsuperlayers];
	for (int id = 0; id < nsuperlayers; id++) {
		int nlayers; 
		int nwires;
		double rsuperlayer;
		int orientation = pow(-1.0,id-1);
		if (id == 1) {
			nlayers     = 1;
			nwires      = 47;
			rsuperlayer = 32.0;
		} else if (id == 2) {
			nlayers     = 2;
			nwires      = 56;
			rsuperlayer = 38.0;
		} else if (id == 3) {
			nlayers     = 2;
			nwires      = 72;
			rsuperlayer = 48;
		} else if (id == 4) {
			nlayers     = 2;
			nwires      = 87;
			rsuperlayer = 58.0;
		} else { // (id == 5)
			nlayers     = 1;
			nwires      = 99;
			rsuperlayer = 68;
		}
		ptrSuperLayers[id] = AhdcSuperLayer(nlayers,nwires,rsuperlayer,orientation);
	}
}

/** Copy constructor */
AhdcSector::AhdcSector(const AhdcSector & obj){
	nsuperlayers = obj.nsuperlayers;
	ptrSuperLayers = new AhdcSuperLayer[nsuperlayers];
	for (int id = 0; id < nsuperlayers; id++){
		ptrSuperLayers[id] = obj.ptrSuperLayers[id];
	}
}

/** Affectation operator */
AhdcSector & AhdcSector::operator=(const AhdcSector & obj){
	nsuperlayers = obj.nsuperlayers;
	if (ptrSuperLayers != nullptr)
		delete[] ptrSuperLayers;
	ptrSuperLayers = new AhdcSuperLayer[nsuperlayers];
	for (int id = 0; id < nsuperlayers; id++){
		ptrSuperLayers[id] = obj.ptrSuperLayers[id];
	}
	return *this;
}

/** Destructor */
AhdcSector::~AhdcSector(){
	if (ptrSuperLayers != nullptr)
		delete[] ptrSuperLayers;
}

/** Return a pointer to the super layer number id */
AhdcSuperLayer* AhdcSector::GetSuperLayer(int id){return ptrSuperLayers + id;}

/** Return the number of super layer */
int AhdcSector::GetNumberOfSuperLayers() {return nsuperlayers;}



/*******************************
 * AhdcDetector
 * ****************************/

/** (Default) constructor */
AhdcDetector::AhdcDetector() : nsectors(1) {
	ptrSectors = new AhdcSector[nsectors];
	for (int id = 0; id < nsectors; id++) {
		ptrSectors[id] = AhdcSector(5);
	}
}

/** Copy constructor */
AhdcDetector::AhdcDetector(const AhdcDetector & obj){
	nsectors = obj.nsectors;
	ptrSectors = new AhdcSector[nsectors];
	for (int id = 0; id < nsectors; id++) {
		ptrSectors[id] = obj.ptrSectors[id];
	}
}

/** Affectation operator */
AhdcDetector & AhdcDetector::operator=(const AhdcDetector & obj){
	nsectors = obj.nsectors;
	if (ptrSectors != nullptr)
		delete[] ptrSectors;
	ptrSectors = new AhdcSector[nsectors];
	for (int id = 0; id < nsectors; id++) {
		ptrSectors[id] = obj.ptrSectors[id];
	}
	return *this;
}

/** Destructor */
AhdcDetector::~AhdcDetector() {
	if (ptrSectors != nullptr)
		delete[] ptrSectors;
}

/** Return a pointer to the super layer number id */
AhdcSector * AhdcDetector::GetSector(int id) {return ptrSectors + id;}

/** Return the number of super layer */
int AhdcDetector::GetNumberOfSectors() {return nsectors;}



