#include <vector>

using namespace std;


class FrameGenerator{
public:
    FrameGenerator();
    vector<bool> generateFrame(vector<vector<bool>>);
    vector<bool> generateACKFrame(vector<vector<bool>>);
    
    vector<vector<bool>> partFrame(vector<bool>);

};

//FRAME FORMAT:
//type,seq,crc,data
//VEKTOR FORMAT:
//{data,type,seq,crc}