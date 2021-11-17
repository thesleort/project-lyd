#ifndef DIGITALFILTER_H
#define DIGITALFILTER_H

#include <vector>

using namespace std;

class DigitalFilter
{
public:
    struct filterCoefficients{
        filterCoefficients(vector<double> a, vector<double> b) : a(a), b(b) {}
        vector<double> a;
        vector<double> b;
    };
    DigitalFilter(vector<filterCoefficients> filters) : filters(filters) {};
    vector<int> simParallel(vector<int> x);
private:
    vector<filterCoefficients> filters;
    vector<int> simFilter(vector<int> x, filterCoefficients coeff);
};

#endif // DIGITALFILTER_H
