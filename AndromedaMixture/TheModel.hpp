#ifndef AndromedaMixture_TheModel_hpp
#define AndromedaMixture_TheModel_hpp

#include "Data.hpp"
#include "DNest4/code/DNest4.h"
#include <memory>
#include <vector>

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

        // Amplitudes for rotation
        std::vector<double> A;

        // Axis for the rotation
        std::vector<double> theta0;

        // Velocity dispersion parameters
        double sigma0, gamma;

        // Uniform latent variables for ambiguous clusters
        std::vector<double> us;

        // Threshold value that counts it as a substructure
        double substructure_threshold;

        // Characteristic radius for velocity dispersion
        static constexpr double R0 = 30.0;

    public:

        TheModel();
        void from_prior(DNest4::RNG& rng);
        double perturb(DNest4::RNG& rng);
        double log_likelihood() const;
        void print(std::ostream& out) const;
        std::string description() const;

};

/* IMPLEMENTATIONS FOLLOW */

Data TheModel::data;
DNest4::TruncatedCauchy TheModel::cauchy(0.0, 5.0, -100.0, 100.0);

TheModel::TheModel()
:A(2)
,theta0(2)
,us(data.xs.size())
{

}

void TheModel::load_data(const char* filename)
{
    data = Data(filename);
}

void TheModel::from_prior(DNest4::RNG& rng)
{
    // The two As are within
    // an order of magnitude or so of each other
    A[0] = cauchy.generate(rng);
    A[1] = A[0] + rng.randn();
    for(int i=0; i<2; ++i)
        A[i] = exp(A[i]);

    theta0[0] = -M_PI + 2.0*M_PI*rng.rand();
    theta0[1] = -M_PI + 2.0*M_PI*rng.rand();

    sigma0 = cauchy.generate(rng);
    sigma0 = exp(sigma0);

    gamma = -3.0*rng.rand();

    for(double& u: us)
        u = rng.rand();
    substructure_threshold = rng.rand();
}

double TheModel::perturb(DNest4::RNG& rng)
{
    double logH = 0.0;

    int which = rng.rand_int(7);

    if(which == 0)
    {
        A[0] = log(A[0]);
        A[1] = log(A[1]);
        double old = A[0];
        logH += cauchy.perturb(A[0], rng);
        A[1] += A[0] - old;
        A[0] = exp(A[0]);
        A[1] = exp(A[1]);
    }
    else if(which == 1)
    {
        A[1] = log(A[1]);
        logH -= -0.5*pow(A[1] - log(A[0]), 2);
        A[1] += rng.randh();
        logH += -0.5*pow(A[1] - log(A[0]), 2);
        A[1] = exp(A[1]);
    }
    else if(which == 2)
    {
        int k = rng.rand_int(A.size());
        theta0[k] += 2.0*M_PI*rng.randh();
        DNest4::wrap(theta0[k], -M_PI, M_PI);
    }
    else if(which == 3)
    {
        sigma0 = log(sigma0);
        logH += cauchy.perturb(sigma0, rng);
        sigma0 = exp(sigma0);
    }
    else if(which == 4)
    {
        gamma += 3.0*rng.randh();
        DNest4::wrap(gamma, -3.0, 0.0);
    }
    else if(which == 5)
    {
        int k = rng.rand_int(us.size());
        us[k] += rng.randh();
        DNest4::wrap(us[k], 0.0, 1.0);
    }
    else
    {
        substructure_threshold += rng.randh();
        DNest4::wrap(substructure_threshold, 0.0, 1.0);
    }

    return logH;
}

double TheModel::log_likelihood() const
{
    double logL = 0.0;

    for(size_t i=0; i<data.xs.size(); ++i)
    {
        int k = 0;
        if(data.classifications[i] == Classification::substructure)
            k = 1;
        if(data.classifications[i] == Classification::no_substructure)
            k = 0;
        if((data.classifications[i] == Classification::ambiguous) &&
            (us[i] < substructure_threshold))
        {
            k = 1;
        }

	    double sth = sin(theta0[k]);
	    double cth = cos(theta0[k]);
	    double dist = data.xs[i]*sth - data.ys[i]*cth;

        // Expected velocity based on distance from the line
        double mu_v = A[k]*dist;

        // Distance from centre
        double Rsq = pow(data.xs[i], 2) + pow(data.ys[i], 2);

        // Velocity dispersion (+) Prior SD of measurement error
        double var_tot = data.sig_vs[i]*data.sig_vs[i]
                            + sigma0*sigma0*pow(Rsq/(R0*R0), gamma);

        logL += -0.5*log(2.0*M_PI*var_tot)
                        -0.5*pow(data.vs[i] - mu_v, 2)/var_tot;

    }

    return logL;
}


void TheModel::print(std::ostream& out) const
{
    out << A[0] << ' ' << A[1] << ' ';
    out << theta0[0]*180.0/M_PI << ' ' << theta0[1]*180.0/M_PI << ' ';
    out << sigma0 << ' ' << gamma << ' ' << substructure_threshold;
}


std::string TheModel::description() const
{
    return "A[0], A[1], theta0[0], theta0[1], sigma0, gamma, substructure_threshold";
}


} // namespace

#endif

