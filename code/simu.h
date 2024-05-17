#ifndef SIMU_H
#define SIMU_H

// C++ headers
#include <iostream>
#include <cmath>

#include "futils.h"
#include "Math/PdfFuncMathCore.h"

//defined constants
#define PI 3.14159265

void PrintHelloWorld() {std::cout << "Hello World !" << std::endl;}

namespace futils {

class ElectronicShape {
    // class to represent the form of the electronic 
    // the integral of this signal from -inf to +inf in time is equal to Edep for each steps
    public:
        virtual double operator()(double x) {return 0;};
        virtual void Print(){
            std::cout << "not defined electronic shape" << std::endl;
        };
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
        virtual double operator()(double x){
            return (1/sqrt(2*PI*sigma*sigma))*exp(-(x-mu)*(x-mu)/(2*sigma*sigma));
        }
        virtual Gauss & operator=(const Gauss & obj){
            mu = obj.mu;
            sigma = obj.sigma;
            return *this;
        }
        void Print(){
            std::cout << "Gauss { mu = " << mu << " , " << sigma << " }" << std::endl;
        }
};

class GenerateSignal {
    private:
        std::vector<double> stepTime;
        std::vector<double> Edep;
        std::vector<int> thisShape; // allow to have a specific shape associated to each step (ex: gaussians with different std_dev )
    public:
        GenerateSignal(std::vector<double> stepTime_, std::vector<double> Edep_, std::vector<int> thisShape_){
            stepTime = stepTime_;
            Edep = Edep_;
            thisShape = thisShape_;
        }
        GenerateSignal() = default;
        GenerateSignal(const GenerateSignal & obj){
            stepTime = obj.stepTime;
            Edep = obj.Edep;
            thisShape = obj.thisShape;
        }
        ~GenerateSignal(){;}
        void Add(double time, double edep, int shape){
            stepTime.push_back(time);
            Edep.push_back(edep);
            thisShape.push_back(shape);
        }
        std::vector<double>                     GetEdep(){return Edep;}
        std::vector<double>                     GetStepTime(){return stepTime;}
        std::vector<int>    GetShape(){return thisShape;}
        void SetEdep(std::vector<double> Edep_) {Edep = Edep_;}
        void SetStepTime(std::vector<double> stepTime_){stepTime = stepTime_;}
        void SetShape(std::vector<int> thisShape_){thisShape = thisShape_;}
        bool is_safe(){
            int n1 = stepTime.size();
            int n2 = Edep.size();
            int n3 = thisShape.size();
            return (n1 == n2) && (n2 == n3);
        }
        // The reason why this class has been created
        double operator()(double x){
            double res = 0;
            int nsteps = stepTime.size();
            for (int s=0;s<nsteps;s++){
                if (thisShape.at(s) == 0) {
                    res += Edep.at(s)*ROOT::Math::gaussian_pdf(x,0.3,stepTime.at(s));
                }
                else if (thisShape.at(s) == 1){
                    res += Edep.at(s)*ROOT::Math::landau_pdf(x,0.3,stepTime.at(s));
                }
            }
            return res;
        }
        
};


}



#endif
