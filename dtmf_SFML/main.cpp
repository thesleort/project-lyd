#include <iostream>
#include "dtmf.h"

using namespace std;

int main()
{

    DTMF soundObj(10000); //input er genvej til at indsætte beep tid.

    soundObj.setDebugMode(true); //aktiverer i øjeblikket terminal beskeder ved input

    soundObj.generate_Wait(7);





    return 0;


}
