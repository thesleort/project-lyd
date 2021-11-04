#include <vector>
#include <iostream>
#include "bytestuffer.h"

using namespace std;
bytestuffer::bytestuffer(vector<bool> flag,vector<bool> etc){
    _etc=etc;
    _flag=flag;
    _etcI=boolToInt(etc).at(0);
    _flagI=boolToInt(flag).at(0);
}

bytestuffer::~bytestuffer(){};

vector<int> bytestuffer::boolToInt(vector<bool> in) {
    vector<int> out;
    while(in.size()>0){
        out.push_back(in.at(0)*8+in.at(1)*4+in.at(2)*2+in.at(3)*1); //4 bits til "hex" som int
        in.erase(in.begin());//4 bits deleted
        in.erase(in.begin());
        in.erase(in.begin());
        in.erase(in.begin());
    }

    return out;
}

vector<bool> bytestuffer::intToBool(vector<int> in){
    vector<bool> out;
    for(int i=0;i<in.size();i++){
        int testInt=in.at(i);
        if(testInt>=8){ //if int>8, first bit needs to be set
            testInt=testInt%8;//modulo gives the remainder
            out.push_back(1);
        } else {
            out.push_back(0); //if int<8, bit is not set
        }
        if(testInt>=4){
            testInt=testInt%4;
            out.push_back(1);
        } else {
            out.push_back(0);
        }
        if(testInt>=2){
            testInt=testInt%2;
            out.push_back(1);
        } else {
            out.push_back(0);
        }
        if(testInt>=1){
            out.push_back(1);
        } else {
            out.push_back(0);
        }
    }
    return out;
}    

vector<int> bytestuffer::stuff(vector<bool> in){
    vector<int> inInt=boolToInt(in);
    for(int i=0;i<inInt.size();i++){
        if(inInt.at(i)==_etcI){
            inInt.insert(inInt.begin()+i,_etcI);
            i++;//i is iterated, since i+1 is now the same as the old i, which would mean etc would be added
        }
    }
    for(int i=0;i<inInt.size();i++){
        if(inInt.at(i)==_flagI){
            inInt.insert(inInt.begin()+i,_etcI);
            i++;
        }
    }
    //Add flags
    inInt.insert(inInt.begin(),_flagI);
    inInt.insert(inInt.begin()+inInt.size(),_flagI);

    return inInt;
    }

vector<bool> bytestuffer::unstuff(vector<int> in){
if(in.size()<3){//if empty vector is unstuffed(2 flags: size<3) we return a vector that is too small, which will be handled by framesplitter 
    vector<bool> z={0};
    return z;
}


for(int i=0;i<in.size();i++){
    if(in.at(i)==_etcI){
        in.erase(in.begin()+i);//erase decrements vector size, which means we skip the 2nd etc in a series
    
    }
}

//remove flags
vector<int> noflags;
for(int i=1;i<in.size()-1;i++){
    noflags.push_back(in.at(i));
}

return intToBool(noflags);
}


//note to self: this does not use etc->etc-> flag, to detect flags that are naturally next to natural etc's
// we just assume that the part in the frame before the flag can never be the etc, since the CRC check will never be 0000

int bytestuffer::getEtc(){
    return _etcI;
}

int bytestuffer::getFlag(){
    return _flagI;
}








