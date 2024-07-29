// ------------------------------------------------------------
//
// Date : July 29, 2024
// Author : Felix Touchte Codjo
// 
// This code is an analysis of the value obtained during the 
// decoding of the AHDC signal. 
//
// ------------------------------------------------------------

#include <iostream>
#include <vector>
#include <cmath>

#include "reader.h"
#include "futils.h"



int main(int argc, char const *argv[]){
	const char* filename = "./data/simu.hipo";
	hipo::reader  r(filename);
	hipo::banklist list = r.getBanks({"MC::Particle","AHDC::adc","ATOF::adc"});
	long unsigned int nEvents =0;
	
	double px, py, pz;
	double p, theta, phi; 
	while( r.next(list)){
		// MC informations
		px = list[0].getFloat("px",0);
		py = list[0].getFloat("py",0);
		pz = list[0].getFloat("pz",0);
		futils::cart2polar(px,py,pz,p,theta,phi); // theta and phi in radians
		// 

		nEvents++;
	}
	std::cout << "Number of events : " << nEvents << std::endl;
	return 0;
}
