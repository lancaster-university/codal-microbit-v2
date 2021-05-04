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

    void createFrames();

    public:
    MicroBitMorsePlayer(MicroBit& bit, int freq, int dur);
    void play(char* in);
};
