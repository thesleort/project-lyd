#include <sstream>
#include <vector>
#include <string>
#include <math.h>
#include <iostream>

#include "Cyclic.h"

using namespace std;

Cyclic::Cyclic(vector<bool> div) {
  _divisor = div;
  _length = div.size();
  for (int i = 0; i < _length - 1; i++) { //encodertail is divisorlength-1
    _encodertail.push_back(0);
  }
}
Cyclic::Cyclic(){};

Cyclic::~Cyclic() {
  //deallocate heap variables
}

void Cyclic::addDivisor(vector<bool> div) {
  _divisor = div;
  _length = div.size();
  for (int i = 0; i < _length - 1; i++) { //encodertail is divisorlength-1
    _encodertail.push_back(0);
  }
}

vector<bool> Cyclic::remainder(vector<bool> dataword, vector<bool> tail) {

  for (int i = 0; i < tail.size(); i++) { //divisor -1 length tail added to in based on bool. 0 to encode
    dataword.push_back(tail.at(i));
  }

  int divL = dataword.size() - _length; //divisionlength=dataword-divisor=> remainder is divisorlength -1
  vector<bool> longdivisor;
  for (int i = 0; i <= divL; i++) { //following operation is repeated untill the correctly sized remainder remains
    if (dataword.at(0)) {           //if first=1, divisor is used for division, if 0, zeros are used
      longdivisor = _divisor;
    } else {
      vector<bool> zerodiv(_divisor.size(), 0);
      longdivisor = zerodiv;
    }

    int startsize = longdivisor.size();
    for (int j = startsize; j < dataword.size(); j++) { //longdivisor filled with 0's till length matches dataword, this will copy the old dataword onto 0's during XOR
      longdivisor.push_back(0);
    }

    vector<bool> newword;
    for (int j = 0; j < dataword.size(); j++) { //new word = XOR between longdivisor and dataword
      newword.push_back(dataword.at(j) != longdivisor.at(j));
    }
    if (!newword.at(0)) { //if the first bool in newword =0, we erase the front and dataword gets set to newword, which will be used for the next iteration
      newword.erase(newword.begin());
    }
    dataword = newword;
  }

  //after "divL" operations, dataword will be the remainder
  return dataword;
}

vector<bool> Cyclic::Encode(vector<bool> dataword) {
  vector<bool> remain = remainder(dataword, _encodertail); //remainder is dataword with added encodertail(zeros)
  return remain;
}

bool Cyclic::Decode(vector<bool> dataword, vector<bool> tail) {
  vector<bool> remains = remainder(dataword, tail); //remainder is dataword with tail from encoding
  for (bool i : remains) {
    if (i) {
      return 0; //if remainder is 1 at any point there has been an error
    }
  }
  return 1;
}
