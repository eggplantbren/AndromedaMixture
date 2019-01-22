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

        // A Cauchy distribution
        static DNest4::TruncatedCauchy cauchy;

    public:

        // Load the dataset
        static void load_data(const char* filename);

    private:

        // Amplitude for rotation
        double A;

        // Axis for the rotation
        double theta0;

        // Velocity dispersion (constant for now)
        double sig;

        // Characteristic radius for velocity dispersion
//        double R0;

    public:

        void from_prior(DNest4::RNG& rng);
        double perturb(DNest4::RNG& rng);
        double log_likelihood() const;
        void print(std::ostream& out) const;
        std::string description() const;

};

/* IMPLEMENTATIONS FOLLOW */

Data TheModel::data;
DNest4::TruncatedCauchy TheModel::cauchy(0.0, 5.0, -100.0, 100.0);

void TheModel::load_data(const char* filename)
{
    data = Data(filename);
}

void TheModel::from_prior(DNest4::RNG& rng)
{
    A = cauchy.generate(rng);
    A = exp(A);

    theta0 = 2.0*M_PI*rng.rand();

    sig = cauchy.generate(rng);
    sig = exp(sig);
}

double TheModel::perturb(DNest4::RNG& rng)
{
    double logH = 0.0;

    int which = rng.rand_int(3);

    if(which == 0)
    {
        A = log(A);
        logH += cauchy.perturb(A, rng);
        A = exp(A);
    }
    else if(which == 1)
    {
        theta0 += 2.0*M_PI*rng.randh();
        DNest4::wrap(theta0, 0.0, 2.0*M_PI);
    }
    else
    {
        sig = log(sig);
        logH += cauchy.perturb(sig, rng);
        sig = exp(sig);
    }

    return logH;
}

double TheModel::log_likelihood() const
{
    double logL = 0.0;

    for(size_t i=0; i<data.xs.size(); ++i)
    {
        // Distance from centre
        double Rsq = pow(data.xs[i], 2) + pow(data.ys[i], 2);

        // Total variance
        double var_tot = sig*sig + data.sig_vs[i]*data.sig_vs[i];

        logL += -0.5*log(2.0*M_PI*var_tot)
                -0.5*pow(data.vs[i], 2)/var_tot;

        // Distance from rotation axis
//        double mu_v = A*sqrt(Rsq);
    }

    return logL;
}


void TheModel::print(std::ostream& out) const
{
    out << A << ' ' << theta0 << ' ' << sig;
}


std::string TheModel::description() const
{
    return "A, theta0, sig";
}


} // namespace

#endif

