/************************************************* 
 * Class for axis (labels and sticks)
 * 
 * @author Felix Touchte Codjo 
 * @date January 26,2025 
 * **********************************************/

#include "fAxis.h"
#include <cmath>
#include <cstdio>


// Utility
// int sprintf( char* buffer, const char* format, ... );
//

/**
 * @brief Constructor
 *
 * @note If `start` > `end`, it is an error, we define `start` and `end` at -1.
 */
fAxis::fAxis(double _start, double _end, int _n2, int _n3) : start(_start), end(_end), n2(_n2), n3(_n3) {
	if (start > end) {
		start = -1;
		end = -1;
	}
	length = end - start;
	int digit = ceil(log10(length)) - 1;
	eps1 = pow(10.0,digit);
	eps2 = eps1/n2;
	eps3 = eps2/n3;
	int n_inf = floor(start/eps1);
	int n_sup = ceil(end/eps1);
	n1 = n_sup - n_inf;
	// First divisions
	for (int k = n_inf; k <= n_sup; k++) {
		Div1.push_back(k*eps1);
		char buffer[50];
		sprintf(buffer, "%.*lf", digit <= 0 ? -digit : 0 , k*eps1);
		Labels1.push_back(buffer);
	}
	printf("n_inf : %d, n_sup : %d\n", n_inf, n_sup);
	// Second divisions
	for (int k = 1; k < (int) Div1.size(); k++) {
		for (int i = 1; i < n2; i++) {
			Div2.push_back(Div1[k-1] + i*eps2);
			char buffer[50];
			sprintf(buffer, "%.*lf", digit <= 0 ? -digit+1 : 0 , Div1[k-1] + i*eps2);
			Labels2.push_back(buffer);
		}
	}
	// Third divisions
	for (int k = 1; k < (int) Div2.size(); k++) {
		for (int i = 1; i < n3; i++) {
			Div2.push_back(Div2[k-1] + i*eps3);
			char buffer[50];
			sprintf(buffer, "%.*lf", digit <= 0 ? -digit+2 : 0 , Div2[k-1] + i*eps3);
			Labels3.push_back(buffer);
		}
	}
}

fAxis::~fAxis(){} ///< Destructor

int  fAxis::get_n1() {return n1;} ///< get the number of first divisions
int  fAxis::get_n2() {return n2;} ///< get the number of second divisions
int  fAxis::get_n3() {return n3;} ///< get the number of third divisions
int  fAxis::get_eps1() {return eps1;} ///< get the space between the first divisions
int  fAxis::get_eps2() {return eps2;} ///< get the space between the second divisions
int  fAxis::get_eps3() {return eps3;} ///< get the space between the third divisions

void fAxis::print() {
	printf(">>> start : %lf, end : %lf, eps1 : %lf\n",start,end,eps1);
	printf("1st divisions [ ");
	for (std::string s : Labels1) {
		printf("%s ", s.c_str());
	}
	printf("]\n");

	printf("2nd divisions [ ");
	for (std::string s : Labels2) {
		printf("%s ", s.c_str());
	}
	printf("]\n");

	printf("3rd divisions [ ");
	for (std::string s : Labels3) {
		printf("%s ", s.c_str());
	}
	printf("]\n");
}


std::vector<std::string> fAxis::get_labels1() {
	return Labels1;
}


std::vector<std::string> fAxis::get_labels2() {
	return Labels2;
}

std::vector<std::string> fAxis::get_labels3() {
	return Labels3;
}
