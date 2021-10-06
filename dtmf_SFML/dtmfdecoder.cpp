#include "dtmfdecoder.h"
#include <complex>
#include <SFML/Audio.hpp>
#include <cmath>

DtmfDecoder::DtmfDecoder(){

}

int DtmfDecoder::doSample(int msDuration)
{

}

vector<complex<double>> DtmfDecoder::recordData(int msDuration)
{
    sf::Clock clock;
    _recorder.start();

    while(clock.getElapsedTime().asMilliseconds() < msDuration){}

    _recorder.stop();

    const sf::SoundBuffer& buffer = _recorder.getBuffer();

    const sf::Int16* samples = buffer.getSamples();
    const long count = buffer.getSampleCount();

    vector<complex<double>> data;
    for (int i = 0; i < count; i++){
        complex<double> number;
        number.real((double)samples[i]);
        data.push_back(number);
    }
    return data;
}

vector<complex<double>> DtmfDecoder::fft(vector<complex<double> > &a)
{
    // https://www.geeksforgeeks.org/fast-fourier-transformation-poynomial-multiplication/

    using cd = complex<double>;
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

vector<double> DtmfDecoder::modulus(const vector<complex<double>>& data)
{
    vector<double> ampVector;

    for (int i = _cutoffLow; i < _cutoffHigh; i++){
        double real = data.at(i).real();
        double imag = data.at(i).imag();
        double amp = sqrt(real*real + imag*imag);
        ampVector.push_back(amp);
    }
    return ampVector;
}
