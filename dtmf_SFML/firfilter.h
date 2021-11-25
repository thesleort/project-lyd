#ifndef FIRFILTER_H
#define FIRFILTER_H

#include <vector>
#include "SFML/Audio.hpp"

using namespace std;

class FIRfilter
{
public:
    FIRfilter(vector<double> coeff) : _coeff(coeff) {};
    vector<double> sim(vector<int> samples);
private:
    vector<double> _coeff;
};

#endif // FIRFILTER_H
