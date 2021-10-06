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
    return inInt;
    }

vector<bool> bytestuffer::unstuff(vector<int> in){
for(int i=0;i<in.size();i++){
    if(in.at(i)==_etcI){
        in.erase(in.begin()+i);//erase decrements vector size, which means we skip the 2nd etc in a series
    }
}
return intToBool(in);

}






