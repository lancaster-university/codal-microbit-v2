#include "MicroBit.h"
#include "DataStream.h"
#include "MicroBitMorseRecogniser.h"
#include "MorseEncoder.h"

class MicroBitMorseInterpreter: public DataSink {
    
    private:

    MicroBitMorseRecogniser& recogniser;
    MicroBit& uBit;
    MorseEncoder encoder;
    bool interpreting;

    void (*callback)(ManagedString) = NULL;


    public:
    MicroBitMorseInterpreter(MicroBitMorseRecogniser& rec, MicroBit& bit);
    virtual int pullRequest();
    void setCallback(void (*_callback)(ManagedString));
    void startInterpreting(void (*_callback)(ManagedString));
    void stopInterpreting();
};
