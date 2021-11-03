#include "Cyclic.h"
#include <iostream>
using namespace std;
int main(int argc, char const *argv[])
{
    vector<bool> div={1,0,1,1,1}; //create divisor
    Cyclic Cyclic1(div); //cyclic object
    vector<bool> ins={1,0,1,0,0,1,1,1,1}; //dataword
    vector<bool> tail=Cyclic1.Encode(ins); //tail/remainder obtained from encoding
    bool k = Cyclic1.Decode(ins,tail); //decoding with dataword and tail yields 1 for no errors
    cout << k << endl;

    return 0;
}
