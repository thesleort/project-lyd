#include <iostream>
#include "bytestuffer.h"

using namespace std;
int main(int argc, char const *argv[])
{

    bytestuffer stuffer1({1,1,1,1},{0,0,0,0});
    vector<bool> bools={1,0,1,1,1,0,0,0,0,1,1,1};
    cout <<" bools" << endl;
    for(bool i:bools){
        cout<<i;
    }
    cout << endl;
    cout << "ints" << endl;
    vector<int> inter;
    for(int i:stuffer1.boolToInt(bools)){
        inter.push_back(i);
        cout <<" "<< i;
    }
    cout << endl;
        cout << "bools" << endl;
    for(int i:stuffer1.intToBool(inter)){
        cout << i;
    }
    cout << endl;
   // vector<bool> stuffedframe=stuffer1.stuff({})
    return 0;
}
