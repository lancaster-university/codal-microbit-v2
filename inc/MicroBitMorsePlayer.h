#ifndef MICROBIT_MORSE_PLAYER_H
#define MICROBIT_MORSE_PLAYER_H

#include "MicroBit.h"
#include "ManagedString.h"

#define DEFAULT_DURATION 186
#define DEFAULT_FREQUENCY 2000
#define DEFAULT_RANDOMNESS 0

/*
 * The morse player takes strings of characters and converts them into morse code using a morse encoder. 
 * Then, it takes every element one by one and plays it over the speaker.
 * It also adds 'element space' farmes between consecutive dots and dashes, in order to help differentiate between audible elements. 
 * The frequency and time unit of these sounds are set in the constructor.
 *
 *
 * Durations for morse elements:
 * dot - unit
 * dash - unit*3
 * space between dots and dashes - unit
 * space between characters - unit*3
 * space between words - unit*7
 * end of transmission - unit*10
 */
class MicroBitMorsePlayer {
    
    private:
    MicroBit& uBit;
    int frequency; // frequency the sounds are played at
    int duration; // time unit for elements
    int randomness; // randomness of sounds - this is currently not settable and defaults to 0

    ManagedString dotFrame; // dot frame
    ManagedString dashFrame; // dash frame
    ManagedString pause1Frame; // element space frame
    ManagedString pause3Frame; // character space frame
    ManagedString pause7Frame; // word space frame
    ManagedString pause10Frame; // end of transmission frame

    void createFrames(); // generates frames for all the morse elements from frequency, duration and randomness

    public:

    /*
     * Constructor
     *
     * Initializes the player. 
     * 
     * @param bit is the micro:bit
     * 
     * @param freq is the frequency the sounds will be played at
     * 
     * @param dur is the time unit for playing the sounds
     */
    MicroBitMorsePlayer(MicroBit& bit, int freq, int dur);

    /*
     * Converts a string into morse and then plays it over the speaker.
     *
     * @param in is the input string
     */
    void play(const char* in);
};

#endif