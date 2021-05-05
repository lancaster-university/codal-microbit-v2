#include "MicroBit.h"
#include "DataStream.h"
#include "MicroBitMorseRecogniser.h"
#include "MorseEncoder.h"

#define DEVICE_MORSE_INTERPRETER_EVT_NEW_MESSAGE     1

// It might be better for this to be in CodalComponent.
#define DEVICE_ID_MORSE_INTERPRETER    38

class MicroBitMorseInterpreter: public DataSink {
    
    private:

    MicroBitMorseRecogniser& recogniser;
    MicroBit& uBit;
    MorseEncoder encoder;
    bool interpreting;

    public:
    ManagedString lastMessage;

    MicroBitMorseInterpreter(MicroBitMorseRecogniser& rec, MicroBit& bit);
    virtual int pullRequest();
    
    void startInterpreting();
    void stopInterpreting();
};
