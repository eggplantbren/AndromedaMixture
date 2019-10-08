#include <iostream>
#include "AndromedaMixture/AndromedaMixture.hpp"

using namespace AndromedaMixture;


int main(int argc, char** argv)
{
    // Load the data
    TheModel::load_data("Data/data.txt");

    // Set the rotation model
    constexpr RotationModel rotation_model = RotationModel::S;
    constexpr int num_components = 2;
    static_assert(num_components == 1 || num_components == 2);
    TheModel::set_rotation_model(rotation_model, num_components);

    // Run DNest4
    DNest4::RNG::randh_is_randh2 = true;
    DNest4::start<TheModel>(argc, argv);

    return 0;
}

