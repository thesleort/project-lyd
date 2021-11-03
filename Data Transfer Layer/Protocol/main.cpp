#include <iostream>
#include "bytestuffer.h"

using namespace std;
int main(int argc, char const *argv[])
{

    bytestuffer stuffer1({1,1,1,1},{0,0,0,0});
    vector<bool> bools={1,0,1,1,0,0,0,0,1,1,1,0,1,1,1,1,0,0,0,0,0,0,0,0};
    cout <<" bools" << endl;
    for(bool i:bools){
        cout<<i;
    }
    cout << endl;
    cout << "ints" << endl;
    vector<int> inter;
       for(int i:stuffer1.boolToInt(bools)){
        cout <<" "<< i;
    }
    cout << endl << "Stuffed" << endl;
    for(int i:stuffer1.stuff(bools)){
        inter.push_back(i);
        cout <<" "<< i;
    }
    cout << endl << "unStuffed" << endl;
    for(int i:stuffer1.boolToInt(stuffer1.unstuff(inter))){
        inter.push_back(i);
        cout <<" "<< i;
    }
   // vector<bool> stuffedframe=stuffer1.stuff({})
    return 0;
}
