// futils : f stands for felix

#ifndef FUTILS_H
#define FUTILS_H

#include "reader.h"
#include "dictionary.h"
#include <vector>


namespace futils {

    // Statistical analysis
    double mean(const std::vector<double> & data);
    double variance(const std::vector<double> & data);
    double std_dev(const std::vector<double> & data);
    void cart2polar(double x, double y, double z, double & rho, double & theta, double & phi);
}


#endif