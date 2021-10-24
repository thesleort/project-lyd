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
return frame;
}

vector<bool> FrameGenerator::generateACKFrame(vector<vector<bool>> parts){
vector<bool> frame={parts.at(TYPE).at(0),parts.at(TYPE).at(1),parts.at(SEQ).at(0),parts.at(SEQ).at(1)};
return frame;

}

vector<vector<bool>> FrameGenerator::splitFrame(vector<bool> frame){
vector<bool> type={frame.at(0),frame.at(1)};
vector<bool> seq={frame.at(2),frame.at(3)};
vector<bool> crc={frame.at(4),frame.at(7)};
vector<bool> data;
for(int i=8;i<frame.size();i++){
    data.push_back(frame.at(i));
}
vector<vector<bool>> parts={data,type,seq,crc};
return parts;
}

