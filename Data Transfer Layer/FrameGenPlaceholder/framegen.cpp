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

}
}