#include <iostream>
#include <SFML/Audio.hpp>
#include <cmath>
#include "dtmfSigurd.h"
#include <vector>
#include <complex>
#include <string>
#include <fstream>
#include "dtmf.h"


#include <unistd.h>

using namespace std;
using namespace sf;

using cd = complex<double>;
// Recursive function of FFT
vector<cd> fft(vector<cd>& a)
{
    int n = a.size();

    // if input contains just one element
    if (n == 1)
        return vector<cd>(1, a[0]);

    // For storing n complex nth roots of unity
    vector<cd> w(n);
    for (int i = 0; i < n; i++) {
        double alpha = -2 * M_PI * i / n;
        w[i] = cd(cos(alpha), sin(alpha));
    }

    vector<cd> A0(n / 2), A1(n / 2);
    for (int i = 0; i < n / 2; i++) {

        // even indexed coefficients
        A0[i] = a[i * 2];

        // odd indexed coefficients
        A1[i] = a[i * 2 + 1];
    }

    // Recursive call for even indexed coefficients
    vector<cd> y0 = fft(A0);

    // Recursive call for odd indexed coefficients
    vector<cd> y1 = fft(A1);

    // for storing values of y0, y1, y2, ..., yn-1.
    vector<cd> y(n);

    for (int k = 0; k < n / 2; k++) {
        y[k] = y0[k] + w[k] * y1[k];
        y[k + n / 2] = y0[k] - w[k] * y1[k];
    }
    return y;
}

int main()
{

    // first check if an input audio device is available on the system
    if (!sf::SoundBufferRecorder::isAvailable())
    {
        cout << "no recording device" << endl;
    }

    // create the recorder
    sf::SoundBufferRecorder recorder;

    DTMF peterLyde(3000);

    // start the capture
    peterLyde.generate(15);
    cout << "Wait sound start" << endl;
    sleep(2); //Sleep 2 således at den kan recorde i lang nok tid
    cout << "sound finished" << endl;
    recorder.start();
    peterLyde.waitSound();

//    DtmfSigurd generator(1000, 44100, 0);
//    generator.playDualTone(697, 1609);

    // stop the capture
    recorder.stop();

    // retrieve the buffer that contains the captured audio data
    const sf::SoundBuffer& buffer = recorder.getBuffer();

    const Int16* samples = buffer.getSamples();
    const long count = buffer.getSampleCount();

    cout << log2(count) << endl;

    vector<cd> data;
    for (int i = 0; i < count; i++){
        cd number;
        number.real((double)samples[i]);
        data.push_back(number);
    }

    int cutoffLow = 600;
    int cutoffHigh = 1800;

    vector<cd> dataT = fft(data);
    vector<double> ampVector;

    for (int i = cutoffLow; i < cutoffHigh; i++){
        double real = dataT.at(i).real();
        double imag = dataT.at(i).imag();
        double amp = sqrt(real*real + imag*imag);
        ampVector.push_back(amp);
    }

    ofstream myfile2;
    myfile2.open ("/home/peter/Desktop/ProjektLyd.txt", std::ofstream::trunc);
    for (int i = 0; i < ampVector.size(); i++){
        myfile2 << to_string(ampVector.at(i)) << " ";
    }
    myfile2.close();


    // Need some way to indentify peaks.

    // Some values at frequencies are randomly 0.
    // Maybe average out the values and then find the frequency and amplitude of every peak.
    // peaks could then be sorted by amplitude and the largest two could be chosen and compared to dtmf tones.
    // The tones that are closest could be chosen.

    return 0;
}
