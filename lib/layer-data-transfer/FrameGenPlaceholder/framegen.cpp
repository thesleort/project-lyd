#include <iostream>

#include "data-transfer-config.h"
#include "framegen.h"

using namespace std;
FrameGenerator::FrameGenerator(){};
//format: datac,type,seq,crc
vector<bool> FrameGenerator::generateFrame(vector<vector<bool>> parts) {
  vector<bool> frame;

  for (int j = 0; j < parts.at(TYPE).size(); j++) {
    frame.push_back(parts.at(TYPE).at(j));
  }
  for (int j = 0; j < parts.at(SEQ).size(); j++) {
    frame.push_back(parts.at(SEQ).at(j));
  }
  for (int j = 0; j < parts.at(CRC).size(); j++) {
    frame.push_back(parts.at(CRC).at(j));
  }
  for (int j = 0; j < parts.at(DATA).size(); j++) {
    frame.push_back(parts.at(DATA).at(j));
  }
  cout << "generated frame" << endl;
  for (bool n : frame) {
    std::cout << n;
  }
  std::cout << endl;
  cout << endl
       << " ------------" << endl;

  return frame;
}

vector<bool> FrameGenerator::generateACKFrame(vector<vector<bool>> parts) {
  vector<bool> frame = {parts.at(TYPE).at(0), parts.at(TYPE).at(1), parts.at(SEQ).at(0), parts.at(SEQ).at(1)};
  return frame;
}

vector<vector<bool>> FrameGenerator::splitFrame(vector<bool> frame) {
  //incomingframe is unstuffed bool frame

  vector<vector<bool>> parts;

  if (frame.size() < 4) {
    parts = {{0}, {1, 1}, {0, 0}, {0}}; //if frame is too small we set a type that discards the message
    return parts;
  }
  vector<bool> type = {frame.at(0), frame.at(1)};
  vector<bool> _msgType = {1, 0};
  vector<bool> _ackType = {0, 1};
  if (type == _msgType) {

    vector<bool> seq = {frame.at(2), frame.at(3)};
    vector<bool> crc = {frame.at(4), frame.at(5), frame.at(6), frame.at(7)};
    vector<bool> data;
    for (int i = 8; i < frame.size(); i++) {
      data.push_back(frame.at(i));
    }
    parts = {data, type, seq, crc};

  } else if (type == _ackType) {
    vector<bool> seq = {frame.at(2), frame.at(3)};
    parts = {{0}, type, seq, {0}};
  }
  return parts;
}
