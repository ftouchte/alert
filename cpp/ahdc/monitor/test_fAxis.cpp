/***************************************
 * Test the class : fAxis
 *
 * @author Felix Touchte Codjo
 * @date January 26, 2025
 * ************************************/

#include "fAxis.h"
#include <vector>
#include <cstdio>

int main (int argc, const char* argv[]) {
	printf("In main\n");
	std::vector<fAxis> Axis = {fAxis(1,10), fAxis(0.57,0.973), fAxis(1.5,1.7)};
	for (fAxis axis : Axis) {
		axis.print();
	}
	return 0;	
}
