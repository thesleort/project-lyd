#include <vector>

using namespace std;
class Cyclic
{
public:
    Cyclic(vector<bool>);
    //vector<int> IntToVect(int);
    vector<bool> Encode(vector<bool>);
    bool Decode(vector<bool>,vector<bool>);
    vector<bool> remainder(vector<bool>,vector<bool>);
       
    ~Cyclic();
private:
    vector<bool> _divisor;
    vector<bool> _encodertail;
    int _length;
};



