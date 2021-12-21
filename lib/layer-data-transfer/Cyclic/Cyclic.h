#ifndef H_CYCLIC
#define H_CYCLIC
#include <vector>

using namespace std;
class Cyclic {
  public:
  Cyclic();
  Cyclic(vector<bool>);
  void addDivisor(vector<bool>);

  vector<bool> Encode(vector<bool>);
  bool Decode(vector<bool>, vector<bool>);
  vector<bool> remainder(vector<bool>, vector<bool>);

  ~Cyclic();

  private:
  vector<bool> _divisor;
  vector<bool> _encodertail;
  int _length;
};

#endif
