/**********************************************
 * Test de la classe fhist.h
 *
 *
 * @author Felix Touchte COdjo
 * @date November 14, 2024
 * *******************************************/

#include "fhist.h"
#include <random>
#include <string>
#include <iostream>
#include <cmath>

/** Distribution uniforme */
void test1(){
	using namespace std::__cxx11; // for the to_string() method
	double a = 2;
	double b = 6;
	std::string title = "distribution uniforme dans [" + to_string(a) + "," + to_string(b) + "]"; 
	fhist1D * hist = new fhist1D(title.c_str(), 1000, 0, 10);
	std::default_random_engine generator;
	std::uniform_real_distribution<double> distribution(a,b);
	for (unsigned long int entry = 0; entry < 1'000'000; entry++){
		hist->Fill(distribution(generator));
	}
	hist->Print();
	hist->DrawContour("distribution_uniforme.pdf");
}

void test2() {
	using namespace std::__cxx11; // for the to_string() method
	double mean = 0;
	double stdev = 1;
	std::string title = "distribution normale N(" + to_string(mean) + "," + to_string(stdev) + ")";
	double start = 0;
	double end = 73;
	double width = end - start;
	fhist1D * hist = new fhist1D(title.c_str(), 1000, start, end);
	hist->SetFlagStatOverFlows(false);
	std::default_random_engine generator;
	std::normal_distribution<double> distribution(mean,stdev);
	for (unsigned long int entry = 0; entry < 1'000'000; entry++){
		hist->Fill(distribution(generator));
	}
	hist->Print();
	hist->DrawContour("distribution_normale.pdf");
	
	double beta  = log10(width/2);
	double gamma = log10(width/5);
	double eta   = log10(width/1);
	std::cout << "width : " << width << std::endl;
	std::cout << "beta  : " << beta << std::endl;
	std::cout << "gamma : " << gamma << std::endl;
	std::cout << "eta   : " << eta << std::endl;
}

int main(int argc, char const *argv[]){
	test1();
	std::cout << std::endl;
	test2();
	return 0;
}
