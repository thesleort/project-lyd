#include <iostream>
#include "bytestuffer.h"

using namespace std;
int main(int argc, char const *argv[])
{
    bytestuffer stuffer1({1,1,1,1},{0,0,0,0});
    vector<bool> bools={1,0,1,1,1,0,0,0,0,1,1,1};
    for(int i:stuffer1.boolToInt(bools)){
        cout << i;
    }
    cout << endl;
   // vector<bool> stuffedframe=stuffer1.stuff({})
    return 0;
}
