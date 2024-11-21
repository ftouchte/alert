/************************************************
 * AhdcGeom.cpp
 *
 *
 *
 * @author Felix Touchte Codjo
 * @date November 20, 2024
 * **********************************************/

#include "AhdcGeom.h"
#include <cmath>
#include <iostream>


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
	double x_start = -R_layer*sin(alpha_layer*(component-1)); // for hit from gemc/source, component starts at 1 (until NumberOfWires)
	double y_start = -R_layer*cos(alpha_layer*(component-1)); 
	start = Point3D(x_start,y_start,0); // start point 
	double x_end = -R_layer*sin(alpha_layer*(component-1) + stereo_angle);
	double y_end = -R_layer*cos(alpha_layer*(component-1) + stereo_angle);
	end = Point3D(x_end,y_end,300); //end point
}

/** Conversion degree to radian */
double futils::toRadian(double degree){
	double pi = 3.14159265358979323846;
	return pi*degree/180.0d;
}

/** Constructor */
AhdcCell::AhdcCell(int _sector, int _superlayer, int _layer, int _component) : AhdcWire(_sector, _superlayer, _layer, _component) {

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

/** Return true if this sector exists */
bool AhdcGeom::IsAhdcSector(int sector){
	return sector == 1;
}

/** Return true if this superlayer exists */
bool AhdcGeom::IsAhdcSuperLayer(int sector, int superlayer){
	int id = sector*10 + superlayer;
	return (id == 11) || (id == 12) || (id == 13) || (id == 14) || (id == 15);
}

/** Return true if this layer exists */
bool AhdcGeom::IsAhdcLayer(int sector, int superlayer,int layer){
	int id = sector*100 + superlayer*10 + layer;
	return (id == 111) || (id == 121) || (id == 122) || (id == 131) || (id == 132) || (id == 141) || (id == 142) || (id == 151);
}

/** Return true if this wire exists */
bool AhdcGeom::IsAhdcWire(int sector, int superlayer, int layer, int component){
	bool islayer = IsAhdcLayer(sector,superlayer,layer);
	if (islayer) {
		if (superlayer == 1){
			return (component > 0) && (component <= 47);
		} else if (superlayer == 2){
			return (component > 0) && (component <= 56);
		} else if (superlayer == 3) {
			return (component > 0) && (component <= 72);
		} else if (superlayer == 4) {
			return (component > 0) && (component <= 87);
		} else { // (superlayer == 5)
			return (component > 0) && (component <= 99);
		}
	}
	return false;
}

/** Get the sector number `sector` */
AhdcSector AhdcGeom::GetAhdcSector(int sector){
	return AhdcSector(sector);
}

/** Show simplified AHDC geometry */
void AhdcGeom::Show(){
	using namespace std;
	cout << "sector superlayer layer NumberOfWires" << endl;
	cout << "1 1 1 47" << endl;
	cout << "1 2 1 56" << endl;
	cout << "1 2 2 56" << endl;
	cout << "1 3 1 72" << endl;
	cout << "1 3 2 72" << endl;
	cout << "1 4 1 87" << endl;
	cout << "1 4 2 87" << endl;
	cout << "1 5 1 99" << endl;
	//cout << "sector     :  1" << endl;
	//cout << "superlayer :  1  | 2     | 3     | 4     | 5" << endl;
	//cout << "layer      :  1  | 1 , 2 | 1 , 2 | 1 , 2 | 1" << endl;
	//cout << "Nb wires   :  47 | 56    | 72    | 87    | 99" << endl;  
}

/** Get a database entry 
 * @note return -1 if `entry` >= DataBaseSize
 */
int AhdcGeom::GetDataBaseEntry(int entry){
	if (entry < GetDataBaseSize()) 
		return db[entry];
	return -1;
}

int AhdcGeom::GetDataBaseSize(){
	return 8;
}
