#include "framegen.h"
#define DATA 0
#define TYPE 1
#define SEQ 2
#define CRC 3
FrameGenerator::FrameGenerator(){};
//format: datac,type,seq,crc
vector<bool> FrameGenerator::generateFrame(vector<vector<bool>> parts){
vector<bool> frame;
for(int i=0;i<parts.size();i++){
    for(int j=0;j<parts.at(TYPE).size();j++){
        frame.push_back(parts.at(TYPE).at(j));
    }
    for(int j=0;j<parts.at(SEQ).size();j++){
        frame.push_back(parts.at(SEQ).at(j));
    }
    for(int j=0;j<parts.at(CRC).size();j++){
        frame.push_back(parts.at(CRC).at(j));
    }
    for(int j=0;j<parts.at(DATA).size();j++){
        frame.push_back(parts.at(DATA).at(j));
    }
}
}


