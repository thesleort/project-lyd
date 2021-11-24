#ifndef DIGITALFILTER_H
#define DIGITALFILTER_H

#include <vector>

//For at kunne gøre brug af sf objekter
#include <SFML/Audio.hpp>
#include <complex>

using namespace std;


struct filterCoefficients{
    filterCoefficients(vector<double> a, vector<double> b) : a(a), b(b) {}
    vector<double> a;
    vector<double> b;
};


class DigitalFilter
{
public:

    DigitalFilter();
    DigitalFilter(vector<filterCoefficients> filters) : filters(filters) {};
    vector<complex<double>> simParallel(vector<complex<double>> x_complex);

 private:
    vector<filterCoefficients> filters;
    vector<int> simFilter(vector<int> x, filterCoefficients coeff);
};

#endif // DIGITALFILTER_H
