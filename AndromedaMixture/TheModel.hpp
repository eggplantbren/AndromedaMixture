#ifndef AndromedaMixture_TheModel_hpp
#define AndromedaMixture_TheModel_hpp

#include "Data.hpp"
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

    


};

/* IMPLEMENTATIONS FOLLOW */

Data TheModel::data;

void TheModel::load_data(const char* filename)
{
    data = Data(filename);
}



} // namespace

#endif

