#include <iostream>
#include "AndromedaMixture/AndromedaMixture.hpp"

using namespace AndromedaMixture;

int main(int argc, char** argv)
{
    // Load the data
    TheModel::load_data("Data/data.txt");

    // Run DNest4
    DNest4::RNG::randh_is_randh2 = true;
    DNest4::start<TheModel>(argc, argv);

    return 0;
}

