#include "digitalfilter.h"
#include "SFML/Audio.hpp"
#include <iostream>

DigitalFilter::DigitalFilter()
{

}

vector<double> DigitalFilter::simParallel(vector<double> x)
{
    vector<double> ySum(x.size(), 0.0);
    for(unsigned long i = 0; i < filters.size(); i++){
        vector<double> y = simFilter(x, filters.at(i));
        for (unsigned int n = 0; n < y.size(); n++){
            ySum.at(n) += y.at(n);
        }
    }
    return ySum;
}

//vector<double> DigitalFilter::simFilter(vector<double> x, filterCoefficients coeff)
//{
//    vector<double> w(coeff.b.size(), 0.0);
//    vector<double> y;
//    for (int n = 0; n < (int)x.size(); n++){
//        double w0 = x.at(n);
//        for (unsigned long i = 0; i < coeff.b.size(); i++){
//            w0 -= coeff.b.at(i) * w.at(i);
//        }

//        w.insert(w.begin(), w0);

//        double output = 0;
//        for (unsigned long i = 0; i < coeff.a.size(); i++){
//            output += w.at(i) * coeff.a.at(i);
//        }
//        y.push_back(output);
//        w.pop_back();
//    }
//    return y;
//}

vector<double> DigitalFilter::simFilter(vector<double> x, filterCoefficients coeff)
{
    vector<double> y;
    for(unsigned int n = 0; n < x.size(); n++){
        if (x.at(n) != 0){
        }
        double val = 0;
        for(unsigned int i = 0; i < coeff.a.size(); i++){
            val += x.at(n) * coeff.a.at(i);
        }

        for(unsigned int i = 0; i < min(coeff.b.size(), y.size()); i++){
            val -= y.at(y.size()-1-i) * coeff.b.at(i);
        }
        y.push_back(val);
    }
    return y;
}
