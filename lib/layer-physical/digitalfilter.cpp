#include "digitalfilter.h"
#include "SFML/Audio.hpp"
#include <iostream>

DigitalFilter::DigitalFilter() {
}

vector<double> DigitalFilter::simParallel(vector<double> x) {
  vector<double> ySum(x.size(), 0.0);
  for (unsigned long i = 0; i < filters.size(); i++) {
    vector<double> y = simFilter(x, filters.at(i));
    for (unsigned int n = 0; n < y.size(); n++) {
      ySum.at(n) += y.at(n);
    }
  }
  return ySum;
}

vector<double> DigitalFilter::simFilter(vector<double> x, filterCoefficients coeff) {
  vector<double> y;
  for (unsigned int n = 0; n < x.size(); n++) {
    if (x.at(n) != 0) {
    }
    double val = 0;
    for (unsigned int i = 0; i < coeff.a.size(); i++) {
      val += x.at(n) * coeff.a.at(i);
    }

    for (unsigned int i = 0; i < min(coeff.b.size(), y.size()); i++) {
      val -= y.at(y.size() - 1 - i) * coeff.b.at(i);
    }
    y.push_back(val);
  }
  return y;
}
