#include "digitalfilter.h"
#include "SFML/Audio.hpp"
#include <iostream>

DigitalFilter::DigitalFilter()
{

}

vector<complex<double>> DigitalFilter::simParallel(vector<complex<double>> x_complex)
{

    vector<int> x;

    vector<complex<double>> ysum_complex;

    //Convert from complex array to vector int
    for (int i = 0; i < x_complex.size(); i++){
        x.push_back(x_complex.at(i).real());
    }


    vector<int> ySum;
    vector<vector<int>> yList;
    for(unsigned long i = 0; i < filters.size(); i++){
        yList.push_back(simFilter(x, filters.at(i)));
    }
    for(unsigned long i = 0; i < x.size(); i++){
        int sum = 0;
        for (unsigned long u = 0; u < yList.size(); u++){
            sum += yList.at(u).at(i);
        }
        ySum.push_back(sum);
    }

    //Convert from int to complex array before output
    for (int i = 0; i < x.size(); i++){

        complex<double> number;
        number.real(x.at(i));
        ysum_complex.push_back(number);
    }


    return ysum_complex;
}

vector<int> DigitalFilter::simFilter(vector<int> x, filterCoefficients coeff)
{
    vector<double> w(coeff.b.size(), 0.0);
    vector<int> y;
    for (int n = 0; n < (int)x.size(); n++){
        double w0 = x.at(n);
        for (unsigned long i = 0; i < coeff.b.size(); i++){
            w0 -= coeff.b.at(i) * w.at(i);
        }

        w.insert(w.begin(), w0);

        double output = 0;
        for (unsigned long i = 0; i < coeff.a.size(); i++){
            output += w.at(i) * coeff.a.at(i);
        }

        y.push_back((int)output);
        w.pop_back();
    }
    return y;
}
