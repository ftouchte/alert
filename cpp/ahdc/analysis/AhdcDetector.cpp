/************************************************
 * AhdcDetector.cpp
 *
 * @author Felix Touchte Codjo
 * @date November 28, 2024
 * **********************************************/

#include "AhdcDetector.h"
#include <cmath>
#include <iostream>

/** Default constructor */
AhdcDetector::AhdcDetector() : nsector(0) {
	ptrSector = new AhdcSector();
}


