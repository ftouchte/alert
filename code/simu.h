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
    // the integral of this signal from -inf to +inf in time is equal to Amplitude for each steps
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
        std::vector<double> Location;
        std::vector<double> Amplitude;
        std::vector<double> Width;
        std::vector<int> Shape; // allow to have a specific shape associated to each step (ex: gaussians with different std_dev )
        
    public:
        GenerateSignal(std::vector<double> Location_, std::vector<double> Amplitude_, std::vector<double> Width_, std::vector<int> Shape_){
            Location = Location_;
            Amplitude = Amplitude_;
            Width = Width_;
            Shape = Shape_;
        }
        //GenerateSignal(MHit* aHit);
        GenerateSignal() = default;
        GenerateSignal(const GenerateSignal & obj){
            Location = obj.Location;
            Amplitude = obj.Amplitude;
            Width = obj.Width;
            Shape = obj.Shape;
        }
        ~GenerateSignal(){;}
        void Add(double time, double amplitude, double width, int shape){
            Location.push_back(time);
            Amplitude.push_back(amplitude);
            Width.push_back(width);
            Shape.push_back(shape);
        }
        std::vector<double>                     GetAmplitude(){return Amplitude;}
        std::vector<double>                     GetLocation() {return Location;}
        std::vector<double>                     GetWidth(){return Width;}
        std::vector<int>                        GetShape(){return Shape;}
        void SetAmplitude(std::vector<double> Amplitude_) {Amplitude = Amplitude_;}
        void SetLocation(std::vector<double> Location_){Location = Location_;}
        void SetWidth(std::vector<double> Width_){Width = Width_;}
        void SetShape(std::vector<int> Shape_){Shape = Shape_;}
        bool is_safe(){
            int n1 = Location.size();
            int n2 = Amplitude.size();
            int n3 = Width.size();
            int n4 = Shape.size();
            return (n1 == n2) && (n2 == n3) && (n3 == n4);
        }
        // The reason why this class has been created
        double operator()(double x){
            double res = 0;
            int nLoc = Location.size();
            for (int l=0;l<nLoc;l++){
                if (Shape.at(l) == 0) {
                    res += Amplitude.at(l)*ROOT::Math::gaussian_pdf(x,Width.at(l),Location.at(l));
                }
                else if (Shape.at(l) == 1){
                    res += Amplitude.at(l)*ROOT::Math::landau_pdf(x,Width.at(l),Location.at(l));
                }
            }
            return res;
        }

        void PrintBeforeProcessing();
        void PrintAllShapes(double tmin, double tmax, int Npts);
        void PrintAfterProcessing(double tmin, double tmax, int Npts);
        
};


}



#endif
