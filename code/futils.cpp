// futils : f stands for felix

#define _USE_MATH_DEFINES

#include "./futils.h"
#include <iostream>
#include <cmath>


namespace futils {

    double mean(const std::vector<double> & data){
        long unsigned int N = data.size();
        double res=0;
        for (long unsigned int i=0; i<N;++i){
            res+= data.at(i);
        }
        return res/N;
    }

    double variance(const std::vector<double> & data){
        long unsigned int N = data.size();
        double res=0;
        double mean = futils::mean(data);
        for (long unsigned int i=0; i<N;++i){
            res+= pow(data.at(i) - mean,2);
        }
        return res/(N-1);
    }

    double std_dev(const std::vector<double> & data){
        return sqrt(futils::variance(data));
    }

    void cart2polar(double x, double y, double z, double & rho, double & theta, double & phi){
        rho = sqrt(x*x+y*y+z*z);
        theta = acos(z/rho);
        if (y >= 0){
            phi = acos(x/(rho*sin(theta)));
        }
        else {
            phi = 2*M_PI - acos(x/(rho*sin(theta)));
        }
        
    } 
    

}
