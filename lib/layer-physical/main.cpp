#include <iostream>
#include <SFML/Audio.hpp>
#include <cmath>
#include <vector>
#include <complex>
#include <string>
#include <fstream>

#include "dtmfEncoder.h"
#include "dtmfdecoder.h"

#include "physicallayer.h"

#include "digitalfilter.h"

#include <unistd.h>

int main() {

  //    vector<double> data;
  //    ifstream in("Sigurd.txt");
  //      double data1;
  //      while(in >> data1)
  //      {
  //          data.push_back(data1);
  //      }
  //      in.close();
  //      cout << "eh" <<endl;

  //      ofstream myfile;
  //      myfile.open ("SigurdV2.txt");
  //      for (unsigned long i = 0; i < data.size(); i++){
  //          myfile << to_string(data.at(i)) << ",";
  //      }
  //      myfile.close();

  //      return 0;

  //    for(int i = 0; i< 1000; i++){
  //    SoundBufferRecorder recorder;

  //    sf::Clock clock;
  //    double sampletime = 100.0;
  //    usleep(100);
  //    recorder.start();

  //    while(clock.getElapsedTime().asMilliseconds() <=  sampletime){}
  //    recorder.stop();

  //    const sf::SoundBuffer& buffer = recorder.getBuffer();

  //    const sf::Int16* samples = buffer.getSamples();

  //    const long count = buffer.getSampleCount();

  //    double fractionSampleTime = 10.0;

  //    double timefraction = sampletime/fractionSampleTime;

  //Testning af encoder

  //    DtmfEncoder encoder(70, OUTPUT_SAMPLERATE, 10000);

  //    for(int i = 0;i < 16; i++){
  //        encoder.playDtmfTone(i);
  //    }

  //    return 0;

  //       DtmfDecoder testSampling((count/(44100.0*timefraction))*1000.0);
  //       int tone = testSampling.identifyDTMF(samples, count/timefraction);

  //           cout << tone << endl;

  //    }

  srand((unsigned)time(0));

  int decodetime = 15;
  PhysicalLayer soundObj(decodetime * 4, decodetime * 4, decodetime);
  vector<int> mega{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};

  int duplicatorFactor = 2;

  //    for(int i = 0; i < 10; i++){
  //        mega.push_back(rand()%15);
  //    }

  for (int i = 0; i < mega.size(); i++) {

    soundObj.writeOutBuffer(mega.at(i));
  }

  int tone;
  vector<int> tones;

  while (true) {

    //        if(soundObj.readInBuffer(tone)){
    //            tones.push_back(tone);

    //            if(tones.size() == mega.size()){

    //                if(tones == mega){

    //                    cout << "very gut" << endl;
    //                    return 0;
    //                }else{

    //                    cout << "ERROR: two vectors not the same" << endl;
    //                    return 0;
    //                }

    //               for(int i = 0; i < tones.size(); i++){
    //                   cout << "played tone:Decoded tone  :   " << mega.at(i) << " : " << tones.at(i) << endl;
    //               }

    //            }
    //        }
  }

  //    int fails = 0;
  //        int success = 0;
  //        srand((unsigned)time(0));
  //        PhysicalLayer soundObj(60,60,15);

  //        for(int i = 0; i < 10; i++){
  //            int trueTone = rand()%15;
  //            soundObj.writeOutBuffer(trueTone);
  //            int tone;
  //            sf::Clock clock;
  //            while(true){
  //                if (clock.getElapsedTime().asMilliseconds() > 12                              00){
  //                    fails++;
  //                    cout << "timeout" << endl;
  //                    break;
  //                }
  //                if (soundObj.readInBuffer(tone)){
  //                    if (trueTone == tone){
  //                        success++;
  //                        cout << "success" << endl;
  //                        break;
  //                    }
  //                    else{
  //                        fails++;
  //                        cout << "wrong tone" << endl;
  //                        break;
  //                    }
  //                }
  //            }
  //        }
  //        cout << "fails: " << fails << "     succeses: " << success << endl;
  //        cout << "fail ratio + you're white + didn't ask: " << (((double) fails)/((double) fails + (double) success))*100 << "%" << endl;

  return 0;
}
