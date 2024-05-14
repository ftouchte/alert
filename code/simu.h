#ifndef SIMU_H
#define SIMU_H

// C++ headers
#include <iostream>
#include <cmath>

#include "futils.h"

//defined constants
#define PI 3.14159265

void PrintHelloWorld() {std::cout << "Hello World !" << std::endl;}

class GenerateGauss {
    protected:
        double mu;
        double sigma;
    public:
        GenerateGauss(double mu_, double sigma_){
            mu = mu_;
            sigma = sigma_;
        }
        ~GenerateGauss(){;}
        
        double operator()(double x){
            return (1/sqrt(2*PI*sigma*sigma))*exp(-(x-mu)*(x-mu)/(2*sigma*sigma));
        }
};




#endif
