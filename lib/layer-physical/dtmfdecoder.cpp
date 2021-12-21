#include "dtmfdecoder.h"
#include "dtmfEncoder.h"
#include <complex>
#include <SFML/Audio.hpp>
#include <cmath>
#include <iostream>
#include <math.h>
#include <fstream>
#include "firfilter.h"

#include <unistd.h>

using namespace std;

DtmfDecoder::DtmfDecoder(double sampleTime) : _sampleTime(sampleTime / 1000.0) {

  fftin = fftw_alloc_complex(SAMPLERATE / _downSampling);
  fftout = fftw_alloc_complex(SAMPLERATE / _downSampling);

  //Amplitude from previous sampling
  _CurrentAmp = vector<double>{0, 0};

  _cummulativeAmp = vector<double>{0, 0};
  _prevAmp = _CurrentAmp;

  // filter keofficienter
  vector<double> a1 = {0, 0.00759724};
  vector<double> b1 = {-0.976123};
  filterCoefficients filter1(a1, b1);

  vector<double> a2 = {0, -0.0716137};
  vector<double> b2 = {-0.930117};
  filterCoefficients filter2(a2, b2);

  vector<double> a3 = {0, 0.0810964, -0.0387499};
  vector<double> b3 = {-1.77176, 0.831691};
  filterCoefficients filter3(a3, b3);

  // opret filter
  _dFilter = DigitalFilter(vector<filterCoefficients>{filter1, filter2, filter3});
}

//Normal sampling: 50 ms, 10 Repeat padding, downsample factor 10, amp threshold 5000, Errormargin = 2

//Speed sampling 10 ms, 60 repeat padding, downsample factor 10, amp threshold 5000, ErrorMargin = 4

int DtmfDecoder::identifyDTMF(const Int16 *data, int count, double sampletime) {
  //For knowing how many times the tone has come prior

  _sampleTime = sampletime / 1000;

  vector<double> dataVec;
  for (int i = 0; i < count; i++) {
    dataVec.push_back(data[i]);
  }

  //Vil give fejl indtil filteret er lavet.

  dataVec = _dFilter.simParallel(dataVec);

  vector<complex<double>> complexSoundBuffer = realToComplexVector(dataVec, count);

  complexSoundBuffer = fftw3(complexSoundBuffer);

  vector<DtmfDecoder::signal> frequencyData = sequenceToSignals(complexSoundBuffer);

  vector<double> peaks = findSignalPeaks(frequencyData);
  int tone;

  if (peaks.at(0) == -1) {
    //Important to know if the last error was amp block, for tailing error check.
    _prevTone = -1;
    _cummulativeAmp = vector<double>{0, 0};
    return -1;
  }

  tone = frequencyToDtmf(peaks[0], peaks[1]);

  if (tone == -2) {
    _cummulativeAmp = vector<double>{0, 0};
    return -2;
  }

  //If we begin sampling with a new tone, then delete the data from previous tone
  //Testing, disregard checking if it does not match the previous number
  if (_prevTone < 0) {
    //cout << "Tailer checker restarted" << endl;
    //Initiate a double vector with values in its first vector
    _cummulativeAmp = vector<double>{0, 0};
    restartTEC();
  }

  //If previous tone was amp block, then a tailing error is impossible.
  if (tailingErrorCheck(tone) == -3 && _prevTone != -1) {
    _cummulativeAmp = vector<double>{0, 0};
    return -3;
  }

  if (_prevTone == tone) {
    _comboCounterTEC++;
    _cummulativeAmp.at(0) += _CurrentAmp.at(0);
    _cummulativeAmp.at(1) += _CurrentAmp.at(1);
  } else {
    // If tone is not equal to tone prior, then restart the _cummulativeAmp
    _cummulativeAmp = vector<double>{0, 0};
    if (_prevTone < 0 && tone >= 0) {
      _comboCounterTEC++;
      _cummulativeAmp.at(0) += _CurrentAmp.at(0);
      _cummulativeAmp.at(1) += _CurrentAmp.at(1);
    }

    _prevTone = tone;
  }

  return tone;
}

void DtmfDecoder::UpdateAmpBlock() {

  _ampBlockMarginLow = (_cummulativeAmp.at(0) / 3.0) * AmpBlockPercent;
  _ampBlockMarginHigh = (_cummulativeAmp.at(1) / 3.0) * AmpBlockPercent;
  _cummulativeAmp = vector<double>{0, 0};
}

int DtmfDecoder::tailingErrorCheck(int tone) {

  if (_prevAmp.at(0) == 0 || _prevAmp.at(1) == 0) {
    _prevAmp = _CurrentAmp;
    return tone;
  }

  double localTEM = _tailingErrorMargin - (double)(_comboCounterTEC / 10.0); //tailing error Margin

  if (_prevWasTail) {
    if (((_prevAmp.at(0) - _CurrentAmp.at(0)) > 0) && (_prevAmp.at(1) - _CurrentAmp.at(1) > 0)) {
      _prevAmp = _CurrentAmp;
      _prevWasTail = false;
      restartTEC();
      return -3;

    } else {

      _prevAmp = _CurrentAmp;
      _prevWasTail = false;
    }
  }

  //Test: try
  double ratioPreviousAmpLOW = (_prevAmp.at(0) - _CurrentAmp.at(0)) / _prevAmp.at(0);
  double ratioPreviousAmpHIGH = (_prevAmp.at(1) - _CurrentAmp.at(1)) / _prevAmp.at(1);

  if ((ratioPreviousAmpLOW > localTEM) || (ratioPreviousAmpHIGH > localTEM)) {
    _prevAmp = _CurrentAmp;
    _prevWasTail = true;
    restartTEC();
    return -3;

  } else if ((ratioPreviousAmpLOW < -localTEM) || (ratioPreviousAmpHIGH < -localTEM)) {
    //If the ratio was too great, the data and should be lowered by an average.
    _prevAmp.at(0) = (_CurrentAmp.at(0) + _prevAmp.at(0)) / 2;
    _prevAmp.at(1) = (_CurrentAmp.at(1) + _prevAmp.at(1)) / 2;

    _prevWasTail = false;
    return tone;
  }

  _prevAmp = _CurrentAmp;
  _prevWasTail = false;
  return tone;
}

void DtmfDecoder::restartTEC() {
  _comboCounterTEC = 0;
}

vector<complex<double>> DtmfDecoder::realToComplexVector(vector<double> reals, int count) {
  vector<complex<double>> complexes;

  //wave amping:

  for (int p = 0; p < _repeatPadding; p++) {
    //Testing different window types - Linear window sucks
    double sinusIncrement = 0;

    for (int i = 0; i < count; i += _downSampling) {
      complex<double> number;
      number.real((double)reals[i] * sin(sinusIncrement)); //-> Sinusincrement

      complexes.push_back(number);

      sinusIncrement += (M_PI / (count)) * _downSampling;
    }
  }

  return complexes;
}

vector<complex<double>> DtmfDecoder::fftw3(vector<complex<double>> &data) {

  int N = data.size();

  // Zero out input array for fft
  for (int i = 0; i < N; i++) {
    fftin[i][0] = 0;
    fftin[i][1] = 0;
  }

  // Setup forward fft
  my_plan = fftw_plan_dft_1d(N, fftin, fftout, FFTW_FORWARD, FFTW_ESTIMATE);

  for (int i = 0; i < data.size(); i++) {
    fftin[i][0] = data[i].real();
    fftin[i][1] = 0; // complex part of the array
  }

  fftw_execute(my_plan);

  vector<complex<double>> result;

  for (int i = 0; i < N; i++) {
    complex<double> complexObj;
    complexObj.real(fftout[i][0]);
    complexObj.imag(fftout[i][1]);
    result.push_back(complexObj);
  }

  return result;
}

vector<DtmfDecoder::signal> DtmfDecoder::sequenceToSignals(const vector<complex<double>> &sequence) {
  vector<signal> sigs;

  for (int i = _cutoffLow * (_sampleTime * _repeatPadding); i < _cutoffHigh * (_sampleTime * _repeatPadding); i++) {
    double real = sequence.at(i).real();
    double imag = sequence.at(i).imag();
    double amp = sqrt(real * real + imag * imag); // possible optimization by not taking square root
    double freq = i / (_sampleTime * _repeatPadding);
    signal sig(amp, freq);
    sigs.push_back(sig);
  }

  return sigs;
}

vector<double> DtmfDecoder::findSignalPeaks(const vector<signal> &signaldata, double error) {

  double middleFreq = (_lowFreqs.at(_lowFreqs.size() - 1) + _highFreqs.at(0)) / 2;

  double highestAmp = 0;
  double highestFreq;
  vector<double> peakFreqs;
  vector<double> peakAmp;
  vector<int> dtmf_Tone;

  bool searchLowFreq = true;

  for (signal i : signaldata) {
    if (i.frequency - middleFreq > 0 && searchLowFreq) {
      searchLowFreq = false;

      peakFreqs.push_back(highestFreq);
      peakAmp.push_back(highestAmp);

      highestAmp = 0;
    }

    if (i.amplitude > highestAmp) {
      highestAmp = i.amplitude;
      highestFreq = i.frequency;
    }
  }
  peakFreqs.push_back(highestFreq);
  peakAmp.push_back(highestAmp);

  //Hurtig fix til amp begrænsning

  if (peakAmp.at(0) < _ampBlockMarginLow || peakAmp.at(1) < _ampBlockMarginHigh) {
    peakFreqs[0] = -1;
  }

  //Fjerne Tailing problem, ved at sammenligne med forrige sample

  _CurrentAmp = peakAmp;

  return peakFreqs; // Error decimal
}

int DtmfDecoder::frequencyToDtmf(double lowfreq, double highfreq) {

  double lowshortestDistance = abs(_lowFreqs[0] - lowfreq);
  int lowIndex = 0;

  double highshortestDistance = abs(_highFreqs[0] - highfreq);
  int highIndex = 0;

  for (int i = 1; i < 4; i++) {
    if (lowshortestDistance > abs(_lowFreqs[i] - lowfreq)) {
      lowshortestDistance = abs(_lowFreqs[i] - lowfreq);
      lowIndex = i;
    }

    if (highshortestDistance > abs(_highFreqs[i] - highfreq)) {
      highshortestDistance = abs(_highFreqs[i] - highfreq);
      highIndex = i;
    }
  }

  //isDTMF_N(lowfreq,highfreq,lowIndex*4+highIndex)
  if (isDTMF_N(lowfreq, highfreq, lowIndex, highIndex)) {

    return lowIndex * 4 + highIndex;
  } else {
    return -2;
  }
}

bool DtmfDecoder::isDTMF_N(double lowfreq, double highfreq, int lowFreqIndex, int highFreqIndex) {
  double errorPercentage = _errorMargin;

  if ((abs(lowfreq - _lowFreqs[lowFreqIndex]) / _lowFreqs[lowFreqIndex]) * 100.0 > errorPercentage) {
    return false;
  }

  if ((abs(highfreq - _highFreqs[highFreqIndex]) / _highFreqs[highFreqIndex]) * 100.0 > errorPercentage) {
    return false;
  }

  return true;
}

void DtmfDecoder::dumpDataToFile(vector<double> &data, string path, string fileName) {
  ofstream myfile;
  myfile.open(path + "/" + fileName + ".txt", std::ofstream::trunc);
  for (unsigned long i = 0; i < data.size(); i++) {
    myfile << to_string(data.at(i)) << " ";
  }
  myfile.close();
}
