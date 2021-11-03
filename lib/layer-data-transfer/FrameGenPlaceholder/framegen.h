#ifndef H_FRAMEGEN
#define H_FRAMEGEN
#include <vector>

using namespace std;

class FrameGenerator {
  public:
  FrameGenerator();
  vector<bool> generateFrame(vector<vector<bool>>);
  vector<bool> generateACKFrame(vector<vector<bool>>);

  vector<vector<bool>> splitFrame(vector<bool>);
};

//FRAME FORMAT:
//type,seq,crc,data
//VEKTOR FORMAT:
//{data,type,seq,crc}

#endif