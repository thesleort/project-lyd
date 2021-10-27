#ifndef PHYSICALLAYER_H
#define PHYSICALLAYER_H
#include <vector>

using namespace std;

class PhysicalLayer
{
public:
    PhysicalLayer(double duration, double spacing);
    bool readInBuffer(int& dtmf);
    bool writeOutBuffer(int dtmf);

private:
    vector<int> _outBuffer;
    vector<int> _inBuffer;
    double _duration;
    double _spacing;

    void encoding(vector<int>& _outBuffer);
    void decoding(vector<int>& _inBuffer);
};

#endif // PHYSICALLAYER_H
