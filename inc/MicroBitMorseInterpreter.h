#ifndef MICROBIT_MORSE_INTERPRETER_H
#define MICROBIT_MORSE_INTERPRETER_H

#include "MicroBit.h"
#include "DataStream.h"
#include "MicroBitMorseRecogniser.h"
#include "MorseEncoder.h"

#define DEVICE_MORSE_INTERPRETER_EVT_NEW_MESSAGE     1

// It might be better for this to be in CodalComponent.
#define DEVICE_ID_MORSE_INTERPRETER    38

/*
 * This class takes morse data from a MicroBitMorseCodeRecogniser and turns it into characters.
 */
class MicroBitMorseInterpreter: public DataSink {
    
    private:

    MicroBitMorseRecogniser& recogniser;
    MicroBit& uBit;
    MorseEncoder encoder;
    bool interpreting;

    public:
    
    ManagedString lastMessage;
    
    /* 
     * Constructor.
     * 
     * Initializes the interpreter.
     */
    MicroBitMorseInterpreter(MicroBitMorseRecogniser& rec, MicroBit& bit);

    /*
     * Callback for when the data from the source is ready.
     */
    virtual int pullRequest();
    
    void startInterpreting();
    void stopInterpreting();
};

#endif