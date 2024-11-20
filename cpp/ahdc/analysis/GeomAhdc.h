/****************************************************
 * GeomAhdc.h
 *
 *
 * @author Felix Touchte Codjo
 * @date November 20, 2024
 * *************************************************/

#ifndef GEOM_AHDC
#define GEOM_AHDC

#include "Point3D.h"

//#define PI 3.14159265358979323846


/**
 * @class GeomAhdc
 * Endoces all the sense wires of the AHDC
 */
class GeomAhdc {

};

/**
 * @class AhdcWire
 * An AHDC (sense) wire is entirely determined by 
 * 1 sector id, 1 superlayer id, 1 layer id and
 * 1 component id
 */
class AhdcWire {
	int sector; ///< always set to 1
	int superlayer; ///< from 1 to 5
	int layer; ///< set to 1 if superlayer == 1,5 and set to 2 if superlayer == 2,3,4
	int component; ///< from 0 to Total number of wires in this layer
public :
	Point3D start; ///< coordonates of the top face (z = 0 mm)
	Point3D end; ///< coordonates of the bottom face (z = 300 mm)
	AhdcWire(int _sector, int _superlayer, int _layer, int _component);
};

/**
 * @class AhdcLayer
 * Is composed of set of AhdcWire
 */
class AhdcLayer {
	int sector;
	int superlayer;
	int layer;
public :
	AhdcLayer(int _sector, int _superlayer, int _layer);
	AhdcWire GetAhdcWire(int component);
};

/** 
 * @class AhdcSuperLayer 
 * Is composed of 1 or 2 AhdcLayer
 */
class AhdcSuperLayer {
	int sector;
	int superlayer;
public :
	AhdcSuperLayer(int _sector, int _superlayer);
	AhdcLayer GetAhdcLayer(int layer);
};

/** 
 * @class AhdcSuperLayer 
 * Is composed of five AhdcSuperLayer
 */
class AhdcSector {
	int sector;
public :
	AhdcSector(int _sector);
	AhdcSuperLayer GetAhdcSuperLayer(int superlayer);
};

/** 
 *@class AhdcCell
 * An AHDC cell is composed of six field wires 
 * surrounding one AHDC sense wires*/
class AhdcCell : public AhdcWire {
	Point3D topFaces[6];
	Point3D bottomfaces[6];
public :
	AhdcCell(int _sector, int _superlayer, int _layer, int _component);	
};

namespace futils {
	double toRadian(double);
}
#endif
