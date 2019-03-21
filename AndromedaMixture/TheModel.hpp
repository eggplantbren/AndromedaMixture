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


// Represent the rotation model chosen
enum class RotationModel
{
    V, S, F
};


class TheModel
{
    private:

        // The dataset
        static Data data;

        // A Cauchy distribution
        static DNest4::TruncatedCauchy cauchy;

        // Rotation model and number of components
        static RotationModel rotation_model;
        static int num_components;

    public:

        // Load the dataset
        static void load_data(const char* filename);

        // Set the rotation model
        static void set_rotation_model(const RotationModel& rm, int nc);

    private:

        // Amplitudes for rotation
        std::vector<double> A;

        // Axis for the rotation
        std::vector<double> phi;

        // A length scale parameter
        std::vector<double> L;

        // Velocity dispersion parameters
        std::vector<double> sigma;
        std::vector<double> gamma;

        // Uniform latent variables for ambiguous clusters
        std::vector<double> us;

        // Threshold value that counts it as a substructure
        double p_subs;

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

// Initial values of static variables
Data TheModel::data;
DNest4::TruncatedCauchy TheModel::cauchy(0.0, 5.0, -100.0, 100.0);
RotationModel TheModel::rotation_model = RotationModel::V;
int TheModel::num_components = 2;

void TheModel::set_rotation_model(const RotationModel& rm, int nc)
{
    rotation_model = rm;
    num_components = nc;
    assert(num_components == 1 || num_components == 2);
}

TheModel::TheModel()
:A(2)
,phi(2)
,L(2)
,sigma(2)
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

    phi[0] = -M_PI + 2.0*M_PI*rng.rand();
    phi[1] = -M_PI + 2.0*M_PI*rng.rand();

    L[0] = exp(log(1E-3*R0) + log(1E3)*rng.rand());
    L[1] = exp(log(1E-3*R0) + log(1E3)*rng.rand());

    // The two sigmas are within
    // an order of magnitude or so of each other
    sigma[0] = cauchy.generate(rng);
    sigma[1] = sigma[0] + rng.randn();
    for(int i=0; i<2; ++i)
        sigma[i] = exp(sigma[i]);

    gamma[0] = -3.0*rng.rand();
    gamma[1] = -3.0*rng.rand();

    for(double& u: us)
        u = rng.rand();
    p_subs = rng.rand();
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
        phi[k] += 2.0*M_PI*rng.randh();
        DNest4::wrap(phi[k], -M_PI, M_PI);
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
        sigma[0] = log(sigma[0]);
        sigma[1] = log(sigma[1]);
        double old = sigma[0];
        logH += cauchy.perturb(sigma[0], rng);
        sigma[1] += sigma[0] - old;
        sigma[0] = exp(sigma[0]);
        sigma[1] = exp(sigma[1]);
    }
    else if(which == 5)
    {
        sigma[1] = log(sigma[1]);
        logH -= -0.5*pow(sigma[1] - log(sigma[0]), 2);
        sigma[1] += rng.randh();
        logH += -0.5*pow(sigma[1] - log(sigma[0]), 2);
        sigma[1] = exp(sigma[1]);
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
        p_subs += rng.randh();
        DNest4::wrap(p_subs, 0.0, 1.0);
    }

    return logH;
}

double TheModel::log_likelihood() const
{
    double logL = 0.0;

    for(size_t i=0; i<data.xs.size(); ++i)
    {
        // Flag - use component zero or component one for this GC?
        int k = 0;
        if(num_components == 2)
        {
            if(data.classifications[i] == Classification::substructure)
                k = 1;
            if(data.classifications[i] == Classification::no_substructure)
                k = 0;
            if((data.classifications[i] == Classification::ambiguous) &&
                (us[i] < p_subs))
            {
                k = 1;
            }
        }

	    double sth = sin(phi[k]);
	    double cth = cos(phi[k]);
	    double dist = data.xs[i]*sth - data.ys[i]*cth;

        double mu_v = 0.0;

        if(rotation_model == RotationModel::V)
        {
            // Veljanoski's model
            double theta = atan2(data.ys[i], data.xs[i]);
            mu_v = A[k]*sin(theta - phi[k]);
        }
        else if(rotation_model == RotationModel::S)
        {
            // Expected velocity based on distance from the line
            // Geraint's model
            mu_v = A[k]*dist;
        }
        else
        {
            // Geraint and Brendon's sigmoid model
            mu_v = A[k]*tanh(dist/L[k]);
        }

        // Distance from centre
        double Rsq = pow(data.xs[i], 2) + pow(data.ys[i], 2);

        // Velocity dispersion (+) Prior SD of measurement error
        double var_tot = data.sig_vs[i]*data.sig_vs[i]
                          + sigma[k]*sigma[k]*pow(Rsq/(R0*R0), gamma[k]);

        logL += -0.5*log(2.0*M_PI*var_tot)
                        -0.5*pow(data.vs[i] - mu_v, 2)/var_tot;

    }

    return logL;
}


void TheModel::print(std::ostream& out) const
{
    out << std::setprecision(10);
    out << A[0] << ' ' << A[1] << ' ';
    out << phi[0]*180.0/M_PI << ' ' << phi[1]*180.0/M_PI << ' ';
    out << L[0] << ' ' << L[1] << ' ';
    out << sigma[0] << ' ' << sigma[1] << ' ';
    out << gamma[0] << ' ' << gamma[1] << ' ' << p_subs;
}


std::string TheModel::description() const
{
    std::stringstream ss;
    ss << "A[0], A[1], phi[0], phi[1], L[0], L[1], sigma[0], sigma[1], ";
    ss << "gamma[0], gamma[1], p_subs";
    return ss.str();
}


} // namespace

#endif

