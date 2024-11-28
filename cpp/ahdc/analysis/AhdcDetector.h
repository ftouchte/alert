/****************************************************
 * AhdcDetector.h
 * 
 * Enhanced version of AhdcGeom.h
 *
 * @author Felix Touchte Codjo
 * @date November 28, 2024
 * *************************************************/

#ifndef AHDC_DETECTOR
#define GEOM_DETECTOR

#include "Point3D.h"

namespace futils {
	double toRadian(double);
}

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
	const int nsector; ///< number of sector
	AhdcSector * ptrSector; ///< pointer of AhdcSector
	public:
		AhdcDetector();
		~AhdcDetector();
		int GetNumberOfSector() {return nsector;}
		AhdcSector* GetSector(int sector);
};

/**
 * @class AhdcSector
 * Modelise an AHDC sector (the unique one)
 * Contains 5 superlayer
 */
class AhdcSector {
	const int nsuperlayer; 
	public:
		AhdcSector() : nsuperlayer(5) {;}
		~AhdcSector(){;}
		int GetNumberOfSuperLayer() {return nsuperlayer;}
		AhdcSuperLayer* GetSuperLayer(int superlayer);
};

/**
 * @class AhdcSuperLayer
 * Modelise an AHDC super layer
 * Contains 1 or 2 layer
 */
class AhdcSuperLayer {
        const int nlayer;
        public:
                AhdcSuperLayer(int _nlayer) : nlayer(_nlayer) {;}
                ~AhdcSuperLayer(){;}
                int GetNumberOfLayer() {return nlayer;}
                AhdcLayer* GetLayer(int layer);
};

#endif
