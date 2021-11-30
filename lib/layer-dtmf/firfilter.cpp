#include "firfilter.h"

FIRfilter::FIRfilter()
{

}

vector<double> FIRfilter::sim(vector<int> samples)
{
    vector<double> output;
    for(unsigned long i = 0; i < _coeff.size(); i++){
        samples.insert(samples.begin(), 0);
    }
    for(unsigned long n = _coeff.size(); n < samples.size(); n++){
        double value = 0;
        for(unsigned long a = 0; a < _coeff.size(); a++){
            value += _coeff.at(a) * samples.at(n-a);
        }
        output.push_back(value);
    }
    return output;
}
