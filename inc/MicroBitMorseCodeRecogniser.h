
#ifndef MICROBIT_MORSE_CODE_RECOGNISER_H
#define MICROBIT_MORSE_CODE_RECOGNISER_H

#include "DataStream.h"
#include "MicroBitAudioProcessor.h"
#include "arm_math.h"

#define DETECTION_THRESHOLD 150
#define MORSE_FRAME_TRUE_RATE_THRESHOLD 0.8
#define MAX_TIME_UNIT 500

class MicroBitMorseCodeRecogniser : public DataSink
{   
        MicroBitAudioProcessor& audio_proceesor;
        MicroBit&               uBit;

        uint16_t timeUnit;
        uint16_t frequency;

        bool analysing;
        bool syncronised;
        unsigned int pauses;

        bool buffer[2 * MAX_TIME_UNIT];
        bool normalised_buffer[6];
        uint16_t buffer_len;
        uint16_t normalised_buffer_len;
        
        void (*callback)(ManagedString) = NULL;

        void processFrame(MicroBitAudioProcessor::AudioFrameAnalysis* frame);

        bool recogniseLastMorseFrame(uint16_t to, uint16_t threshold );

    public:
        MicroBitMorseCodeRecogniser(MicroBitAudioProcessor& processor, MicroBit& uBit, uint16_t freq, uint16_t timeUnit) ;
        
        ~MicroBitMorseCodeRecogniser();

        virtual int pullRequest();
        
        MicroBitAudioProcessor* getAudioProcessor();

        void setCallback    (void (*_callback)(ManagedString));
        void startAnalisying(void (*_callback)(ManagedString));
        void stopAnalisying();

};


#endif