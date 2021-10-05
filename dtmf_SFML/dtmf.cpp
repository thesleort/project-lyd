#include "dtmf.h"
#include <fstream>

DTMF::DTMF()
{   


}


DTMF::DTMF(int duration_ms)
{
    set_duration(duration_ms);

}

void DTMF::generate(int DTMFSound)
{

    if(_DebugMode == true){
    cout << "Generating sound with frequencies: "<< endl;
    cout << "HIGH: " << _highFrequency[DTMFSound%4] << " Hz" << endl;
    cout << "LOW: " << _LowFrequency[DTMFSound/4] << " Hz" << endl;
    }

//    ofstream myfile;
//    myfile.open ("/home/peter/Desktop/LydBuffer.txt", std::ofstream::trunc);
//    for (int i = 0; i < _dtmf[DTMFSound].size(); i++){
//        myfile << to_string(_dtmf[DTMFSound].at(i)) << " ";
//    }
//    myfile.close();

    One.loadFromSamples(&_dtmf[DTMFSound][0],_dtmf[DTMFSound].size(),1, _samplerate);

    soundObj.setBuffer(One);

    soundObj.play();




}

void DTMF::generate_Wait(int DTMFSound)
{

    generate(DTMFSound);
    waitSound();

}

void DTMF::waitSound()
{
    while(soundObj.getStatus() == 2){

    }
}


void DTMF::setDebugMode(bool state)
{
    _DebugMode = state;
}

void DTMF::setSamplerate(int rate)
{
    _samplerate = rate;
}

void DTMF::set_duration(int duration_ms)
{

    _dtmf.clear();

    _duration_ms = duration_ms;

    int beebduration = _duration_ms; // enhed millisekunder

    int packetlength = beebduration*_samplerate/1000;


    for(int lowf = 0; lowf < 4; lowf++){

        for(int highf = 0; highf < 4; highf++){

            vector<sf::Int16> dtmfPart;

            for(int i = 0; i < packetlength;  i++){

                dtmfPart.push_back(10000*sin((_highFrequency[highf]*2*M_PI*i)/_samplerate) + 10000*sin((_LowFrequency[lowf]*2*M_PI*i)/_samplerate));
                //dtmfPart.push_back(10000*sin((_highFrequency[highf]*2*M_PI*i)/_samplerate));
            }

            _dtmf.push_back(dtmfPart);
        }
    }


}

