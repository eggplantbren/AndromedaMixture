#ifndef AndromedaMixture_TheModel_hpp
#define AndromedaMixture_TheModel_hpp

#include "Data.hpp"
#include "DNest4/code/DNest4.h"
#include <iomanip>
#include <memory>
#include <sstream>
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

        // A length scale parameter
        std::vector<double> L;

        // Velocity dispersion parameters
        std::vector<double> sigma0;
        std::vector<double> gamma;

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
,L(2)
,sigma0(2)
,gamma(2)
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

    L[0] = exp(log(1E-3*R0) + log(1E3)*rng.rand());
    L[1] = exp(log(1E-3*R0) + log(1E3)*rng.rand());

    // The two sigmas are within
    // an order of magnitude or so of each other
    sigma0[0] = cauchy.generate(rng);
    sigma0[1] = sigma0[0] + rng.randn();
    for(int i=0; i<2; ++i)
        sigma0[i] = exp(sigma0[i]);

    gamma[0] = -3.0*rng.rand();
    gamma[1] = -3.0*rng.rand();

    for(double& u: us)
        u = rng.rand();
    substructure_threshold = rng.rand();
}

double TheModel::perturb(DNest4::RNG& rng)
{
    double logH = 0.0;

    int which = rng.rand_int(9);

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
        int k = rng.rand_int(A.size());
        L[k] = log(L[k]);
        L[k] += log(1E3)*rng.randh();
        DNest4::wrap(L[k], log(1E-3*R0), log(R0));
        L[k] = exp(L[k]);
    }
    else if(which == 4)
    {
        sigma0[0] = log(sigma0[0]);
        sigma0[1] = log(sigma0[1]);
        double old = sigma0[0];
        logH += cauchy.perturb(sigma0[0], rng);
        sigma0[1] += sigma0[0] - old;
        sigma0[0] = exp(sigma0[0]);
        sigma0[1] = exp(sigma0[1]);
    }
    else if(which == 5)
    {
        sigma0[1] = log(sigma0[1]);
        logH -= -0.5*pow(sigma0[1] - log(sigma0[0]), 2);
        sigma0[1] += rng.randh();
        logH += -0.5*pow(sigma0[1] - log(sigma0[0]), 2);
        sigma0[1] = exp(sigma0[1]);
    }
    else if(which == 6)
    {
        int k = rng.rand_int(A.size());
        gamma[k] += 3.0*rng.randh();
        DNest4::wrap(gamma[k], -3.0, 0.0);
    }
    else if(which == 7)
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
        // Geraint's model
        double mu_v = A[k]*dist;

        // Veljanoski's model
//        double theta = atan2(data.ys[i], data.xs[i]);
//        double mu_v = A[k]*sin(theta - theta0[k]);

        // Geraint and Brendon's sigmoid model
//        double mu_v = A[k]*tanh(dist/L[k]);

        // Distance from centre
        double Rsq = pow(data.xs[i], 2) + pow(data.ys[i], 2);

        // Velocity dispersion (+) Prior SD of measurement error
        double var_tot = data.sig_vs[i]*data.sig_vs[i]
                          + sigma0[k]*sigma0[k]*pow(Rsq/(R0*R0), 2.0*gamma[k]);

        logL += -0.5*log(2.0*M_PI*var_tot)
                        -0.5*pow(data.vs[i] - mu_v, 2)/var_tot;

    }

    return logL;
}


void TheModel::print(std::ostream& out) const
{
    out << std::setprecision(10);
    out << A[0] << ' ' << A[1] << ' ';
    out << theta0[0]*180.0/M_PI << ' ' << theta0[1]*180.0/M_PI << ' ';
    out << L[0] << ' ' << L[1] << ' ';
    out << sigma0[0] << ' ' << sigma0[1] << ' ';
    out << gamma[0] << ' ' << gamma[1] << ' ' << substructure_threshold;
}


std::string TheModel::description() const
{
    std::stringstream ss;
    ss << "A[0], A[1], theta0[0], theta0[1], L[0], L[1], sigma0[0], sigma0[1], ";
    ss << "gamma[0], gamma[1], substructure_threshold";
    return ss.str();
}


} // namespace

#endif

