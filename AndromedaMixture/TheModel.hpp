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

        // Velocity dispersion parameters
        double sigma0, gamma;

        // Characteristic radius for velocity dispersion
        static constexpr double R0 = 30.0;

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

    sigma0 = cauchy.generate(rng);
    sigma0 = exp(sigma0);

    gamma = 3.0*rng.rand();
}

double TheModel::perturb(DNest4::RNG& rng)
{
    double logH = 0.0;

    int which = rng.rand_int(4);

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
    else if(which == 2)
    {
        sigma0 = log(sigma0);
        logH += cauchy.perturb(sigma0, rng);
        sigma0 = exp(sigma0);
    }
    else
    {
        gamma += 3.0*rng.randh();
        DNest4::wrap(gamma, 0.0, 3.0);
    }

    return logH;
}

double TheModel::log_likelihood() const
{
    double logL = 0.0;

    double tan_theta0 = tan(theta0);

    for(size_t i=0; i<data.xs.size(); ++i)
    {
        // Closest point on the line
        double x_line = (data.xs[i] + data.ys[i]*tan_theta0) /
                                        (1.0 + tan_theta0*tan_theta0);
        double y_line = x_line*tan_theta0;

        // Distance to the closest point on the line
        double dist_to_line = sqrt(pow(x_line - data.xs[i], 2) +
                                   pow(y_line - data.ys[i], 2));

        // Expected velocity based on distance from the line
        double mu_v = A*dist_to_line;
        
        // Distance from centre
        double Rsq = pow(data.xs[i], 2) + pow(data.ys[i], 2);

        // Velocity dispersion (+) Prior SD of measurement error
        double var_tot = data.sig_vs[i]*data.sig_vs[i]
                            + sigma0*sigma0*pow(Rsq/(R0*R0), 2.0*gamma);

        logL += -0.5*log(2.0*M_PI*var_tot)
                        -0.5*pow(data.vs[i] - mu_v, 2)/var_tot;

    }

    return logL;
}


void TheModel::print(std::ostream& out) const
{
    out << A << ' ' << theta0 << ' ' << sigma0 << ' ' << gamma;
}


std::string TheModel::description() const
{
    return "A, theta0, sigma0, gamma";
}


} // namespace

#endif

