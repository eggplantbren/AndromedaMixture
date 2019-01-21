#ifndef AndromedaMixture_Data_hpp
#define AndromedaMixture_Data_hpp

#include <fstream>
#include <vector>

namespace AndromedaMixture
{

// Classifications of the substructures
enum class Classification
{
    substructure, no_substructure, ambiguous
};

// Output for classifications
std::ostream& operator << (std::ostream& out,
                           const Classification& c);

// Store the data in one of these
class Data
{
    private:
        std::vector<double> rs, thetas;
        std::vector<double> vs, sig_vs;
        std::vector<Classification> classifications;

    public:
        Data(const char* filename);
};


/* IMPLEMENTATIONS FOLLOW */

Data::Data(const char* filename)
{
    std::fstream fin(filename, std::ios::in);
    if(!fin)
    {
        std::cerr << "Couldn't open data file " << filename << "." << std::endl;
        return;
    }

    double a, b, c, d, e;
    while(fin >> a && fin >> b && fin >> c && fin >> d && fin >> e)
    {
        rs.push_back(a);
        thetas.push_back(b);
        vs.push_back(c);
        sig_vs.push_back(d);
        if(e == 1.0)
            classifications.push_back(Classification::substructure);
        else if(e == 0.0)
            classifications.push_back(Classification::no_substructure);
        else
            classifications.push_back(Classification::ambiguous);
    }
    fin.close();

    std::cout << "# Loaded " << rs.size() << " data points from file ";
    std::cout << filename << '.' << std::endl;
}


// Output for classifications
std::ostream& operator << (std::ostream& out,
                           const Classification& c)
{
    if(c == Classification::substructure)
        out << "substructure";
    if(c == Classification::no_substructure)
        out << "no_substructure";
    if(c == Classification::ambiguous)
        out << "ambiguous";
    return out;
}

} // namespace

#endif

