#ifndef DTMF_H
#define DTMF_H

#include <iostream>
using namespace std;

#include <SFML/Audio.hpp>
#include <math.h>


class DTMF
{
public:
    DTMF();

    DTMF(int duration_ms);

    void generate(int DTMFSound);

    void generate_Wait(int DTMFSound);

    void waitSound();

    void set_duration(int duration_ms); //Recontrurere hele dtmf bufferens samples til den rigtige tid

    void setDebugMode(bool state);

    void setSamplerate(int rate);


private:

    bool _DebugMode = false;

    int _duration_ms;

    vector<vector<sf::Int16>> _dtmf;

    int _samplerate = 100000;

    vector<int> _highFrequency = {1209,1336,1477,1633};

    vector<int> _LowFrequency = {697,770,852,941};

    sf::SoundBuffer One;

    sf::Sound soundObj;
};

#endif // DTMF_H
