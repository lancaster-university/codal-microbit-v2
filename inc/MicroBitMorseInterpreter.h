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
 * This class takes morse data from a MicroBitMorseCodeRecogniser and uses a MorseEncoder to decode it.
 * It then calls an event signalling the fact that it is done processing some data.
 * The last processed data can then be taken from lastMessage. 
 */
class MicroBitMorseInterpreter: public DataSink {
    
    private:

    MicroBitMorseRecogniser& recogniser; // recogniser that this takes data from
    MicroBit& uBit; // the microbit - used in order to send an event in the message bus
    MorseEncoder encoder; // encoder used for decoding received data
    bool interpreting; // wether the Interpreter is currently interpreting or not

    public:
    
    /*
     * Last processed message
     */
    ManagedString lastMessage; 
    
    /* 
     * Constructor.
     * 
     * Initializes the interpreter.
     * 
     * @param rec is the recogniser this will receive data from
     * 
     * @param bit is the micro:bit
     */
    MicroBitMorseInterpreter(MicroBitMorseRecogniser& rec, MicroBit& bit);

    /*
     * Callback for when the data is ready.
     */
    virtual int pullRequest();
    
    /*
     * Starts interpreting and also starts the associated recogniser.
     */
    void startInterpreting();
    
    /*
     * Stops interpreting and also stops the associated recogniser.
     */
    void stopInterpreting();
};

#endif