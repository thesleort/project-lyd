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




int main()
{


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
    PhysicalLayer soundObj(60,60,15);
    vector<int> mega;

    for(int i = 0; i < 100; i++){
        mega.push_back(rand()%15);
    }

    for (int i = 0; i < mega.size(); i++){

        soundObj.writeOutBuffer(mega.at(i));
    }

    int tone;
    vector<int> tones;

    while(true){


        if(soundObj.readInBuffer(tone)){
            tones.push_back(tone);
            cout << "tones vector size: " << tones.size() << endl;

            if(tones.size() == mega.size()){

                if(tones == mega){
                    cout << "very gut" << endl;
                }else{
                    cout << "ERROR: two vectors not the same" << endl;
                }

               for(int i = 0; i < tones.size(); i++){
                   cout << "played tone:Decoded tone  :   " << mega.at(i) << " : " << tones.at(i) << endl;
               }

            }
        }


    }

    return 0;
}
