#ifndef MICROBIT_MORSE_INTERPRETER_H
#define MICROBIT_MORSE_INTERPRETER_H

#include "MicroBit.h"
#include "DataStream.h"
#include "MicroBitMorseRecogniser.h"
#include "MorseEncoder.h"
/*
 * This class takes morse data from a MicroBitMorseCodeRecogniser and turns it into characters.
 */
class MicroBitMorseInterpreter: public DataSink {
    
    private:

    MicroBitMorseRecogniser& recogniser;
    MicroBit& uBit;
    MorseEncoder encoder;
    bool interpreting;

    void (*callback)(ManagedString) = NULL;


    public:
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

    /*
     * Set callback function that is called when the interpreter is done interpreting a string.
     */
    void setCallback(void (*_callback)(ManagedString));
    
    /*
     * Starts listening and interpreting.
     */
    void startInterpreting(void (*_callback)(ManagedString));
    
    /*
     * Stops listening and interpreting.
     */
    void stopInterpreting();
};

#endif