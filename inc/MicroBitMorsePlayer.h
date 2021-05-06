#ifndef MICROBIT_MORSE_PLAYER_H
#define MICROBIT_MORSE_PLAYER_H

#include "MicroBit.h"
#include "ManagedString.h"

#define DEFAULT_DURATION 186
#define DEFAULT_FREQUENCY 2000
#define DEFAULT_RANDOMNESS 0

/*
 * Class that is able to play character arrays encoded as morse over the speaker. 
 *
 */
 
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

    /*
     * Generate dotFrame and dashFrame from frequency, duration and randomness.
     */
    void createFrames();

    public:

    /*
     * Constructor
     *
     * Initializes the player. 
     */
    MicroBitMorsePlayer(MicroBit& bit, int freq, int dur);

    /*
     * Plays a char array in morse.
     */
    void play(const char* in);
};

#endif