#ifndef AndromedaMixture_TheModel_hpp
#define AndromedaMixture_TheModel_hpp

#include "Data.hpp"
#include "DNest4/code/DNest4.h"
#include <memory>

namespace AndromedaMixture
{

class TheModel
{
    private:

        // The dataset
        static Data data;

    public:

        // Load the dataset
        static void load_data(const char* filename);

    private:

        // Amplitude for rotation
        double A;

        // Axis
        double theta0;

        // Velocity dispersion
        double sig;

        // Characteristic radius for velocity dispersion
        double R0;

    public:

        void from_prior(DNest4::RNG& rng);
        double perturb(DNest4::RNG& rng);
        double log_likelihood() const;
        void print(std::ostream& out) const;
        std::string description() const;

};

/* IMPLEMENTATIONS FOLLOW */

Data TheModel::data;

void TheModel::load_data(const char* filename)
{
    data = Data(filename);
}

void TheModel::from_prior(DNest4::RNG& rng)
{

}


} // namespace

#endif

