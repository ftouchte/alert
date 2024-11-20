/************************************************
 * GeomAhdc.cpp
 *
 *
 *
 * @author Felix Touchte Codjo
 * @date November 20, 2024
 * **********************************************/

#include "GeomAhdc.h"
#include <cmath>

/** Constructor */
AhdcWire::AhdcWire(int _sector, int _superlayer, int _layer, int _component) 
	: sector(_sector), superlayer(_superlayer), layer(_layer), component(_component) 
{
	// All distance is in mm !
	double round = 360; // 360 degrees
	double numWires; // number of wires in this superlayer
	double R_layer; // radius of the 1st layer of this superlayer 
	double DR_layer = 4.0d; // space between 2 layers of a given superlayer
	if (superlayer == 1) {
		numWires = 47.0d; //47
		R_layer  = 32.0d;
	} else if (superlayer == 2) {
		numWires = 56.0d; //56
		R_layer  = 38.0d;
	} else if (superlayer == 3) {
		numWires = 72.0d; //72
		R_layer  = 48.0d;
	} else if (superlayer == 4) {
		numWires = 87.0d;
		R_layer  = 58.0d;
	} else { // (superlayer == 5)
		numWires = 99.0d;
		R_layer  = 68.0d;
	}
	R_layer = R_layer + (layer - 1)*DR_layer; // radius this layer
	double alpha_layer = futils::toRadian(round/numWires); // angle between 2 consecutive wires (components) of a layer
	double parity_angle = pow(-1.0d,superlayer-1); 
	double stereo_angle = parity_angle*futils::toRadian(20.0d); // assuming superlayer numerotation starts at 1
	double x_start = -R_layer*sin(alpha_layer*component);  
	double y_start = -R_layer*cos(alpha_layer*component); 
	start = Point3D(x_start,y_start,0); // start point 
	double x_end = -R_layer*sin(alpha_layer*component + stereo_angle);
	double y_end = -R_layer*cos(alpha_layer*component + stereo_angle);
	end = Point3D(x_end,y_end,300); //end point
}

/** Conversion degree to radian */
double futils::toRadian(double degree){
	double pi = 3.14159265358979323846;
	return pi*degree/180.0d;
}

/** Constructor */
AhdcLayer::AhdcLayer(int _sector, int _superlayer, int _layer)
	: sector(_sector), superlayer(_superlayer), layer(_layer) 
{	
}

/** Return the wire number `component` */
AhdcWire AhdcLayer::GetAhdcWire(int component){
	return AhdcWire(sector,superlayer,layer,component);
}

/** Constructor */
AhdcSuperLayer::AhdcSuperLayer(int _sector, int _superlayer) 
	: sector(_sector), superlayer(_superlayer) 
{
}

/** Return the layer number`layer` */
AhdcLayer AhdcSuperLayer::GetAhdcLayer(int layer){
	return AhdcLayer(sector,superlayer,layer);
}

/** Constructor */
AhdcSector::AhdcSector(int _sector)
	: sector(_sector)
{
}

/** Return the layer number`superlayer` */
AhdcSuperLayer AhdcSector::GetAhdcSuperLayer(int superlayer){
	return AhdcSuperLayer(sector,superlayer);
}

/** Constructor */
AhdcCell::AhdcCell(int _sector, int _superlayer, int _layer, int _component) : AhdcWire(_sector, _superlayer, _layer, _component) {

}


