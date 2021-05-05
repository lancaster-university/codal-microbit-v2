#include "MicroBit.h"
#include "ManagedString.h"

#define DEFAULT_DURATION 186
#define DEFAULT_FREQUENCY 2000
#define DEFAULT_RANDOMNESS 0

class MicroBitMorsePlayer {
    
    private:
    MicroBit& uBit;
    int frequency;
    int duration;
    int randomness;

    ManagedString dotFrame;
    ManagedString dashFrame;
    ManagedString pause1Frame;
    ManagedString pause3Frame;
    ManagedString pause7Frame;
    ManagedString pause10Frame;

    void createFrames();

    public:
    MicroBitMorsePlayer(MicroBit& bit, int freq, int dur);
    void play(const char* in);
};
