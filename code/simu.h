#ifndef SIMU_H
#define SIMU_H

// C++ headers
#include <iostream>
#include <cmath>

#include "futils.h"

//defined constants
#define PI 3.14159265

void PrintHelloWorld() {std::cout << "Hello World !" << std::endl;}

namespace futils {

class ElectronicShape {
    // class to represent the form of the electronic 
    // the integral of this signal from -inf to +inf in time is equal to Edep for each steps
    public:
        virtual double operator()(double)=0;        
};

class Gauss : public futils::ElectronicShape {
    // Class to use gaussian distribution
    // These distributions are normalised
    private:
        double mu;
        double sigma;
    public:
        Gauss(double mu_, double sigma_){
            mu = mu_;
            sigma = sigma_;
        }
        Gauss(const Gauss & obj){
            mu = obj.mu;
            sigma = obj.sigma;
        }
        ~Gauss(){;}
        double GetMu(){return mu;}
        double GetSigma(){return sigma;}
        void SetMu(double mu_){ mu = mu_;}
        void SetSigma(double sigma_) {sigma = sigma_;}
        double operator()(double x){
            return (1/sqrt(2*PI*sigma*sigma))*exp(-(x-mu)*(x-mu)/(2*sigma*sigma));
        }
};

}



#endif
