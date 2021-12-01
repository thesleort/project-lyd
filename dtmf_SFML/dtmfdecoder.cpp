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


DtmfDecoder::DtmfDecoder(double sampleTime) : _sampleTime(sampleTime/1000.0)
{

//    vector<double> coeffs = {0.001168,0.001178,0.001125,0.001009,0.000840,0.000633,0.000414,0.000212,0.000062,-0.000002,0.000055,0.000262,0.000639,0.001193,0.001918,0.002791,0.003774,0.004809,0.005829,0.006753,0.007497,0.007978,0.008119,0.007859,0.007155,0.005992,0.004384,0.002378,0.000052,-0.002487,-0.005107,-0.007663,-0.009999,-0.011967,-0.013433,-0.014293,-0.014479,-0.013973,-0.012808,-0.011071,-0.008906,-0.006501,-0.004082,-0.001900,-0.000208,0.000748,0.000761,-0.000330,-0.002614,-0.006099,-0.010703,-0.016250,-0.022475,-0.029035,-0.035527,-0.041509,-0.046531,-0.050162,-0.052027,-0.051830,-0.049388,-0.044647,-0.037697,-0.028777,-0.018266,-0.006667,0.005418,0.017325,0.028370,0.037897,0.045325,0.050194,0.052204,0.051250,0.047440,0.041100,0.032767,0.023164,0.013160,0.003719,-0.004165,-0.009535,-0.011544,-0.009522,-0.003040,0.008047,0.023564,0.043004,0.065543,0.090068,0.115232,0.139534,0.161403,0.179298,0.191821,0.197807,0.196426,0.187254,0.170327,0.146165,0.115767,0.080572,0.042390,0.003298,-0.034479,-0.068713,-0.097321,-0.118519,-0.130958,-0.133846,-0.127036,-0.111084,-0.087254,-0.057487,-0.024318,0.009253,0.039921,0.064277,0.079018,0.081176,0.068336,0.038829,-0.008093,-0.072141,-0.151920,-0.244922,-0.347586,-0.455420,-0.563190,-0.665153,-0.755345,-0.827880,-0.877283,-0.898792,-0.888662,-0.844408,-0.765003,-0.651004,-0.504603,-0.329588,-0.131230,0.083918,0.308290,0.533631,0.751369,0.953011,1.130535,1.276775,1.385761,1.453009,1.475740,1.453009,1.385761,1.276775,1.130535,0.953011,0.751369,0.533631,0.308290,0.083918,-0.131230,-0.329588,-0.504603,-0.651004,-0.765003,-0.844408,-0.888662,-0.898792,-0.877283,-0.827880,-0.755345,-0.665153,-0.563190,-0.455420,-0.347586,-0.244922,-0.151920,-0.072141,-0.008093,0.038829,0.068336,0.081176,0.079018,0.064277,0.039921,0.009253,-0.024318,-0.057487,-0.087254,-0.111084,-0.127036,-0.133846,-0.130958,-0.118519,-0.097321,-0.068713,-0.034479,0.003298,0.042390,0.080572,0.115767,0.146165,0.170327,0.187254,0.196426,0.197807,0.191821,0.179298,0.161403,0.139534,0.115232,0.090068,0.065543,0.043004,0.023564,0.008047,-0.003040,-0.009522,-0.011544,-0.009535,-0.004165,0.003719,0.013160,0.023164,0.032767,0.041100,0.047440,0.051250,0.052204,0.050194,0.045325,0.037897,0.028370,0.017325,0.005418,-0.006667,-0.018266,-0.028777,-0.037697,-0.044647,-0.049388,-0.051830,-0.052027,-0.050162,-0.046531,-0.041509,-0.035527,-0.029035,-0.022475,-0.016250,-0.010703,-0.006099,-0.002614,-0.000330,0.000761,0.000748,-0.000208,-0.001900,-0.004082,-0.006501,-0.008906,-0.011071,-0.012808,-0.013973,-0.014479,-0.014293,-0.013433,-0.011967,-0.009999,-0.007663,-0.005107,-0.002487,0.000052,0.002378,0.004384,0.005992,0.007155,0.007859,0.008119,0.007978,0.007497,0.006753,0.005829,0.004809,0.003774,0.002791,0.001918,0.001193,0.000639,0.000262,0.000055,-0.000002,0.000062,0.000212,0.000414,0.000633,0.000840,0.001009,0.001125,0.001178,0.001168,};

//    _bandpass = FIRfilter(coeffs);

    fftin = fftw_alloc_complex(SAMPLERATE/_downSampling);
    fftout = fftw_alloc_complex(SAMPLERATE/_downSampling);

    //Amplitude from previous sampling
    _CurrentAmp = vector<double>{0,0};
    _prevAmp =  _CurrentAmp;


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

int DtmfDecoder::identifyDTMF(const Int16 *data, int count, double sampletime)
{
    //For knowing how many times the tone has come prior

    _sampleTime = sampletime/1000;
//    cout << "amount of samples: " << count << endl;

    vector<double> dataVec;
    for(int i = 0; i < count; i++){
        dataVec.push_back(data[i]);
    }

//   sf::Clock clock;
//   vector<double> doublevec = _bandpass.sim(dataVec);
//   cout << clock.getElapsedTime().asMilliseconds() << endl;

   //Vil give fejl indtil filteret er lavet.

   dataVec = _dFilter.simParallel(dataVec);

   vector<complex<double>> complexSoundBuffer = realToComplexVector(dataVec,count);




//   vector<double> testPrioriFFT;

//   for (int i = 0; i < complexSoundBuffer.size(); i++){
//       testPrioriFFT.push_back(complexSoundBuffer[i].real());
//   }

//   dumpDataToFile(testPrioriFFT, "/media/sf_Ubuntu_Shared_Folder", "PrioriFFT");

   //complexSoundBuffer = fft(complexSoundBuffer);


   complexSoundBuffer = fftw3(complexSoundBuffer);

   vector<DtmfDecoder::signal> frequencyData = sequenceToSignals(complexSoundBuffer);


//   vector<double> testPosterior;

//   for(int i= 0; i < frequencyData.size(); i++){
//       testPosterior.push_back(frequencyData.at(i).amplitude);
//   }

//   dumpDataToFile(testPosterior, "/media/sf_Ubuntu_Shared_Folder", "PosteriorFFT");



   vector<double> peaks = findSignalPeaks(frequencyData);
   int tone;

   if(peaks.at(0) == -1){
       //cout << "ERROR: sound to low" << endl;

       //Important to know if the last error was amp block, for tailing error check.
       _prevTone = -1;
       return -1;
   }

   tone = frequencyToDtmf(peaks[0], peaks[1]);


   if(tone == -2){
       //cout << "ERROR: error margin too great" << endl;

      return -2;
   }

   //If we begin sampling with a new tone, then delete the data from previous tone
   //Testing, disregard checking if it does not match the previous number
   if(_prevTone < 0){
       //cout << "Tailer checker restarted" << endl;
       //Initiate a double vector with values in its first vector
       restartTEC();
   }


   //If previous tone was amp block, then a tailing error is impossible.
   if(tailingErrorCheck(tone) == -3 && _prevTone != -1){
       //cout << "Tailing Tone: " << tone << endl;
       return -3;
   }

   if(_prevTone == tone){
       _comboCounterTEC++;
   }else{
       if(_prevTone < 0 && tone >= 0){
           _comboCounterTEC++;
       }

       _prevTone = tone;
   }

   return tone;

}

int DtmfDecoder::tailingErrorCheck(int tone)
{



    if(_prevAmp.at(0) == 0 || _prevAmp.at(1) == 0){

        //If tone != _prevTone, Then new tone has begun. Restart tailing error check
        _prevAmp = _CurrentAmp;
        return tone;
    }

    double localTEM = _tailingErrorMargin-(double)(_comboCounterTEC/10.0); //tailing error Margin



    //cout << "test Amp LOW: " << _prevAmp.at(0) << "  :   HIGH: " << _prevAmp.at(1) << "  :   size: " << _prevAmp.size() << "  :  TEM_value: " << localTEM << endl;


    if(_prevWasTail){
        if(((_prevAmp.at(0)-_CurrentAmp.at(0)) > 0) && (_prevAmp.at(1)-_CurrentAmp.at(1) > 0)){

            //        cout << "tail error check value low: "  << ((double)_prevAmp.at(0)-(double)_CurrentAmp.at(0))/(double)_prevAmp.at(0) << "  :  tail error check value HIGH: " << ((double)_prevAmp.at(1)-(double)_CurrentAmp.at(1))/(double)_prevAmp.at(1) << endl;
            //        cout << "Previous amp LOW: " << (double)_prevAmp[0] << "    :   previous amp HIGH: " << (double)_prevAmp[1] << endl;
            //        cout << "Current amp LOW: " << (double)_CurrentAmp.at(0)<< "   :   current amp HIGH  " << (double)_CurrentAmp[1] << endl;
            //cout << "Tailer checker restarted" << endl;
            _prevAmp = _CurrentAmp;
            _prevWasTail = false;
            restartTEC();
            return -3;

        }else{

            _prevAmp = _CurrentAmp;
            _prevWasTail = false;
        }

    }

    //Test: try
    double ratioPreviousAmpLOW = (_prevAmp.at(0)-_CurrentAmp.at(0))/_prevAmp.at(0);
    double ratioPreviousAmpHIGH = (_prevAmp.at(1)-_CurrentAmp.at(1))/_prevAmp.at(1);

    if((ratioPreviousAmpLOW > localTEM) || (ratioPreviousAmpHIGH > localTEM)){

        //        cout << "tail error check value low: "  << ((double)_prevAmp.at(0)-(double)_CurrentAmp.at(0))/(double)_prevAmp.at(0) << "  :  tail error check value HIGH: " << ((double)_prevAmp.at(1)-(double)_CurrentAmp.at(1))/(double)_prevAmp.at(1) << endl;
        //        cout << "Previous amp LOW: " << (double)_prevAmp[0] << "    :   previous amp HIGH: " << (double)_prevAmp[1] << endl;
        //        cout << "Current amp LOW: " << (double)_CurrentAmp.at(0)<< "   :   current amp HIGH  " << (double)_CurrentAmp[1] << endl;
        //cout << "Tailer checker restarted" << endl;
        _prevAmp = _CurrentAmp;
        _prevWasTail = true;
        restartTEC();
        return -3;

    }else if((ratioPreviousAmpLOW < -localTEM)||(ratioPreviousAmpHIGH < -localTEM)){
        //If the ratio was too great, the data and should be lowered by an average.
        //cout << "   spike detection" << endl;
        _CurrentAmp.at(0) = (_CurrentAmp.at(0)+_prevAmp.at(0))/2;
        _CurrentAmp.at(1) = (_CurrentAmp.at(1)+_prevAmp.at(1))/2;

        _prevAmp = _CurrentAmp;
        _prevWasTail = false;
        return tone;

    }

    _prevAmp = _CurrentAmp;
    _prevWasTail = false;
    return tone;
}

void DtmfDecoder::restartTEC()
{
    _comboCounterTEC = 0;
}



vector<complex<double>> DtmfDecoder::realToComplexVector(vector<double> reals, int count)
{
    vector<complex<double>> complexes;

    //wave amping:



    for(int p = 0; p < _repeatPadding; p++){
        //Testing different window types - Linear window sucks
        double sinusIncrement = 0;


        for (int i = 0; i < count; i += _downSampling){
            complex<double> number;
            number.real((double)reals[i]*sin(sinusIncrement)); //-> Sinusincrement

            complexes.push_back(number);


            sinusIncrement += (M_PI/(count))*_downSampling;
        }
    }


    return complexes;
}


vector<complex<double> > DtmfDecoder::fftw3(vector<complex<double> > &data)
{

     int N = data.size();


     // Zero out input array for fft
     for (int i = 0; i<N; i++) {
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

    for (int i = 0; i < N; i++){
        complex<double> complexObj;
        complexObj.real(fftout[i][0]);
        complexObj.imag(fftout[i][1]);
        result.push_back(complexObj);
    }

    return result;
}

vector<DtmfDecoder::signal> DtmfDecoder::sequenceToSignals(const vector<complex<double>> &sequence)
{
    vector<signal> sigs;


    for (int i = _cutoffLow * (_sampleTime*_repeatPadding); i < _cutoffHigh * (_sampleTime*_repeatPadding); i++){
        double real = sequence.at(i).real();
        double imag = sequence.at(i).imag();
        double amp = sqrt(real*real + imag*imag); // possible optimization by not taking square root
        double freq = i / (_sampleTime*_repeatPadding);
        signal sig(amp, freq);
        sigs.push_back(sig);
    }

    return sigs;
}


vector<double> DtmfDecoder::findSignalPeaks(const vector<signal> & signaldata, double error){

    double middleFreq = (_lowFreqs.at(_lowFreqs.size()-1)+_highFreqs.at(0))/2;

    double highestAmp = 0;
    double highestFreq;
    vector<double> peakFreqs;
    vector<double> peakAmp;
    vector<int> dtmf_Tone;

    bool searchLowFreq = true;

    for(signal i : signaldata){
        if(i.frequency-middleFreq > 0 && searchLowFreq){
            searchLowFreq = false;

            peakFreqs.push_back(highestFreq);
            peakAmp.push_back(highestAmp);

            highestAmp = 0;
        }

        if(i.amplitude > highestAmp){
            highestAmp = i.amplitude;
            highestFreq = i.frequency;
        }
    }
    peakFreqs.push_back(highestFreq);
    peakAmp.push_back(highestAmp);



    //Hurtig fix til amp begrænsning

    //cout << "AMP DATA : " << peakAmp.at(0) << "    :    HIGH    :    " << peakAmp.at(1) <<  endl;

    if(peakAmp.at(0) < _ampthreshhold || peakAmp.at(1) < _ampthreshhold){
          peakFreqs[0] = -1;
    }

    //Fjerne Tailing problem, ved at sammenligne med forrige sample

    _CurrentAmp = peakAmp;



    return peakFreqs; // Error decimal

}

int DtmfDecoder::frequencyToDtmf(double lowfreq, double highfreq){

    double lowshortestDistance = abs(_lowFreqs[0]-lowfreq);
    int lowIndex = 0;

    double highshortestDistance = abs(_highFreqs[0]-highfreq);
    int highIndex = 0;

    for(int i = 1; i < 4; i++){
        if(lowshortestDistance > abs(_lowFreqs[i]-lowfreq)){
            lowshortestDistance = abs(_lowFreqs[i]-lowfreq);
            lowIndex = i;            

        }

        if(highshortestDistance > abs(_highFreqs[i]-highfreq)){
            highshortestDistance = abs(_highFreqs[i]-highfreq);
            highIndex = i;
        }
    }

    //isDTMF_N(lowfreq,highfreq,lowIndex*4+highIndex)
    if(isDTMF_N(lowfreq,highfreq,lowIndex,highIndex)){    

        return lowIndex*4+highIndex;
    }else{
        return -2;
    }
}

bool DtmfDecoder::isDTMF_N(double lowfreq, double highfreq, int lowFreqIndex, int highFreqIndex)
{
    double errorPercentage = _errorMargin;

    //cout << "ERROR Percentage: " << (lowfreq-_lowFreqs[lowFreqIndex])/_lowFreqs[lowFreqIndex]*100.0 << endl;

    if((abs(lowfreq-_lowFreqs[lowFreqIndex])/_lowFreqs[lowFreqIndex])*100.0 > errorPercentage){
        //cout << "ERROR LOW Percentage: " << (lowfreq-_lowFreqs[lowFreqIndex])/_lowFreqs[lowFreqIndex]*100.0 << endl;
        return false;
    }

    if((abs(highfreq-_highFreqs[highFreqIndex])/_highFreqs[highFreqIndex])*100.0 > errorPercentage){
        //cout << "ERROR HIGH Percentage: " << (abs(highfreq-_highFreqs[highFreqIndex])/_highFreqs[highFreqIndex])*100.0 << endl;
        return false;
    }

    return true;


}

void DtmfDecoder::dumpDataToFile(vector<double>& data, string path, string fileName)
{
    ofstream myfile;
    myfile.open (path + "/" + fileName + ".txt", std::ofstream::trunc);
    for (unsigned long i = 0; i < data.size(); i++){
        myfile << to_string(data.at(i)) << " ";
    }
    myfile.close();
}


