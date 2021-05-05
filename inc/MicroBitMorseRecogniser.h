
#ifndef MICROBIT_MORSE_CODE_RECOGNISER_H
#define MICROBIT_MORSE_CODE_RECOGNISER_H

#include "DataStream.h"
#include "MicroBitAudioProcessor.h"
#include "arm_math.h"

/*
 * The sound recogniser takes in data from the audio processor - in 
 * form of AudioFrameAnalysis. It then tries to identify whether 
 * the listening frequency was played during that frame. Based on 
 * the last frames it sends dots ('.'), dashes ('-'), letter spaces 
 * (' '), word spaces (';'), and end-of-transmission ('#') to the 
 * data sink connected.
 * 
 * Algorithm
 * 
 * The recogniser first converts the AudioFrameAnalysis data in 
 * booleans which represent whether or not the listening frequency was 
 * being played in that frame. A timeUnit is calculated based on the 
 * sampling frequency and the number of samples taken before analysis 
 * by the audio processor - it represents the number of frames a dot 
 * should take.
 * 
 * It has 2 values: zeros and ones which counts for how many timeUnits 
 * the frequency wasn't being played/ was being played. The value 
 * synchronised keeps track of the last transition - true if the last 
 * transition was from the frequency not being played to it being played, 
 * false otherwise. At each transition, the values of zeros or ones are 
 * being interpreted into the right character and then reset.
 * 
 * All characters are being accumulated into a buffer and send together 
 * when the end-of-transmission character ('#') is found.
 */

// Configuration vairables
#define MORSE_FRAME_TRUE_RATE_THRESHOLD 0.8
#define DETECTION_THRESHOLD 130
#define MAX_TIME_UNIT       500
#define MAX_MESSAGE         500

class MicroBitMorseRecogniser : public DataSink, public DataSource
{   
        MicroBitAudioProcessor& audio_proceesor;    // the audio processor it takes data from
        DataSink*               interpreter;        // the interpreter where it sends the data when ready

        uint16_t timeUnit;      // the number of frames for one dot
        uint16_t frequency;     // the frequency it uses for communication

        bool analysing;         // whether it should analyse the data coming in
        bool synchronised;      // whether it synchrnosied with a stream on ones - should be the status of last transition

        uint16_t zeros;         // the number of zero frames it received since last transition
        uint16_t ones;          // the number on one frames it received since last transition

        uint8_t output[MAX_MESSAGE];        // the ouput buffer
        bool buffer[2 * MAX_TIME_UNIT];     // the input buffer 
        uint16_t output_len;                // the length of the output buffer
        uint16_t buffer_len;                // the length of the input buffer


        /*
         * Processes a incoming frame.
         *
         * @param frame the frame it should process
         */
        void processFrame(MicroBitAudioProcessor::AudioFrameAnalysis* frame);

        /*
         * Checks if the number of ones in the last timeUnit frames up to 
         * a given position in buffer fit in a given threshold.
         * 
         * @param to specifies the interval [to - timeUnit, to) which is 
         *           analysed 
         * 
         * @param threshold the minimum number of ones in the interval
         * 
         * @return number of trues in buffer[to - timeUnit, to) >= threshold
         * 
         * @note if threshold is 255 thet it will use 
         *       timeUnit * MORSE_FRAME_TRUE_RATE_THRESHOLD as the threshold
         */
        bool recogniseLastMorseFrame(uint16_t to, uint16_t threshold );


        /*
         * Adds a character to the output buffer. If it adds the 
         * end-of-transmission ('#') character, then it sends the buffer 
         * to the next component in the pipeline - 'interpreter'.
         * 
         * @param c the character to be added to the out buffer
         */
        void pushOut(char c);

    public:

        /*
         * Constructor. 
         * 
         * Initializes the MicroBitMorseRecogniser.
         * 
         * @param the audio processor it should take data from
         * 
         * @param freq the frequency it should listen for
         * 
         * @param timeUnit the amount of time in ms it takes a dot
         *                 to be transmitted
         */
        MicroBitMorseRecogniser(MicroBitAudioProcessor& processor, uint16_t freq, uint16_t timeUnit) ;
        
        /*
         * Destructor.
         *
         * Deallocates all the memory allocated dynamically.
         */
        ~MicroBitMorseRecogniser();

        /*
         * A callback for when the data is ready.
         */
        virtual int pullRequest();
        
        /*
         * Starts analysing the data that comes in.
         */
        void startAnalisying();
        
        /*
         * Stops analysing the data and also stops the audio processor
         * from receiving.
         */
        void stopAnalisying();

        /*
         * Provides the next available data to the downstream caller.
         */
        virtual ManagedBuffer pull();

        /*
         * Allow out downstream component to register itself with us
         */
    	virtual void connect(DataSink *sink);
};


#endif