#include "MicroBit.h"
#include "DataStream.h"
#include "MicroBitMorseCodeRecogniser.h"
#include "MorseEncoder.h"

class MicroBitMorseInterpreter: public DataSink {
    
    private:

    MicroBitMorseCodeRecogniser& recogniser;
    MicroBit& uBit;
    MorseEncoder encoder;
    bool interpreting;

    void (*callback)(ManagedString) = NULL;


    public:
    MicroBitMorseInterpreter(MicroBitMorseCodeRecogniser& rec, MicroBit& bit);
    virtual int pullRequest();
    void setCallback(void (*_callback)(ManagedString));
    void startInterpreting(void (*_callback)(ManagedString));
    void stopInterpreting();
};
