/****************************************************
 * AhdcDetector.h
 * 
 * Enhanced version of AhdcGeom.h
 *
 * @author Felix Touchte Codjo
 * @date November 28, 2024
 * *************************************************/

#ifndef AHDC_DETECTOR
#define AHDC_DETECTOR

#include "Point3D.h"
#include "TMultiGraph.h"

namespace futils {
	double toRadian(double);
}


/** 
 * @class AhdcWire
 * Is typically a segment characterised by 
 * its two end points : top and bot (bottom)
 */
class AhdcWire {
	Point3D top; ///< center point of the top the wire (z == -150 mm)
	Point3D bot; ///< center point of the bottom the wire (z == +150 mm)
public :
	AhdcWire(Point3D _top, Point3D _bot);
	AhdcWire(double x1 = 0, double y1 = 0, double z1 = 0, double x2 = 0, double y2 = 0, double z2 = 0);
};


/**
 * @class AhdcLayer
 * Constitued of a set of AHDC wires
 * Characterised by :
 * - a number of wires
 * - a radius
 * - a stereo angle (those of the wires) 
 */
class AhdcLayer {
	int nwires; ///< number of wires
	double rlayer; ///< radius of this layer
	double stereoangle; ///< stereo angle of the wires of this layer by comparaison to one of another (super)layer (can be -20° or +20°)
	AhdcWire * ptrWires; ///< list of wires
public :
	AhdcLayer(int _nwires = 0, double _rlayer = 0, double _stereoangle = 0);
	AhdcLayer(const AhdcLayer & obj);
	AhdcLayer & operator=(const AhdcLayer & obj);
	~AhdcLayer();
	AhdcWire * GetAhdcWire(int id);
	int GetNumberOfWires();
};


/**
 * @class AhdcSuperLayer
 * Modelise an AHDC super layer
 * Contains 1 or 2 layer
 */
class AhdcSuperLayer {
        int nlayers; ///< number of layer
	int nwires;  ///< number of wires, it is the same for each of its layers
	double rsuperlayer; ///< radius of this superlayer
	int orientation; ///< can be -1 or +1; define the sign of the stereo angle of the wires
	AhdcLayer * ptrLayers; ///< list of layers
public:
	AhdcSuperLayer(int _nlayers = 0, int _nwires = 0, double _rsuperlayer = 0, int _orientation = 0);
	AhdcSuperLayer(const AhdcSuperLayer & obj);
	AhdcSuperLayer & operator=(const AhdcSuperLayer & obj);
	~AhdcSuperLayer();
	AhdcLayer * GetLayer(int id);
	int GetNumberOfLayers();
};


/**
 * @class AhdcSector
 * Modelise an AHDC sector (the unique one)
 * Contains 5 superlayer
 */
class AhdcSector {
	int nsuperlayers; ///< number of superlayer 
	AhdcSuperLayer * ptrSuperLayers; ///< list of superlayer
	public:
		AhdcSector(int _nsuperlayers = 5);
		AhdcSector(const AhdcSector & obj);
		AhdcSector & operator=(const AhdcSector & obj);
		~AhdcSector();
		AhdcSuperLayer* GetSuperLayer(int id);
		int GetNumberOfSuperLayers();
};


/**
 * @class AhdcDetector
 * Modelises the AHDC detector
 * It contains :
 * - 1 sector
 * - 5 superlayer (from 1 to 5)
 * - each superlayer is composed of 1 or 2 layer (1 for sl = 1 et 5, 2 for sl = 2,3,4)
 * - each layer of the same superlayer is composed of a constant number of components (in that case : wires)
 */
class AhdcDetector {
	int nsectors; ///< number of sector
	AhdcSector * ptrSectors; ///< list of sector
	public:
		AhdcDetector();
		AhdcDetector(const AhdcDetector & obj);
		AhdcDetector & operator=(const AhdcDetector & obj);
		~AhdcDetector();
		AhdcSector* GetSector(int sector);
		int GetNumberOfSectors();
};



#endif