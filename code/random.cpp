// From chatGPT


#include <iostream>
#include <random>

int main() {
    // Define the range for random double numbers
    double lowerBound = 0.0;
    double upperBound = 1.0;

    // Create a random number generator engine
    std::random_device rd; // Seed for the random number engine
    std::mt19937 rng(rd()); // Mersenne Twister pseudo-random number generator

    // Create a uniform real distribution for double numbers
    std::uniform_real_distribution<double> distribution(lowerBound, upperBound);

    // Generate and print 5 random double numbers within the range [lowerBound, upperBound]
    for (int i = 0; i < 5; ++i) {
        double randomDouble = distribution(rng);
        std::cout << "Random Double " << i+1 << ": " << randomDouble << std::endl;
    }

    return 0;
}