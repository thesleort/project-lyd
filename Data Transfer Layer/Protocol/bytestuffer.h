#ifndef H_BYTESTUFFER
#define H_BYTESTUFFER
#include <vector>
using namespace std;

//converts boolean frames into integer frames with stuffing and vice versa
//splits buffer into frames
class bytestuffer{
public:
    bytestuffer(vector<bool>,vector<bool>); //creates stuffer with etc and flag
    ~bytestuffer();

    vector<int> boolToInt(vector<bool>); //converts a boolean vector into a "hexidecinal" int vector
    vector<bool> intToBool(vector<int>); //converts an integer vector into a vector of booleans

    vector<int> stuff(vector<bool>); //stuffs boolean vector with etcs ahead of flags and etcs, adds flags
    
    vector<bool> unstuff(vector<int>); //removes 1 etc and skips what is behind it(requires etc stuffing into other etcs)
    vector<int> splitbuffer(vector<int>&); //creates a frame from the buffer(and deletes what is taken(still needs implementing))

    int getEtc();
    int getFlag();

private:
    vector<bool> _flag; //flag as bits
    vector<bool> _etc; //etc as bits
    int _etcI; //etc as integer
    int _flagI; //flag as integer
};

#endif