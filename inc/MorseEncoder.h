#ifndef MORSE_ENCODER_H
#define MORSE_ENCODER_H

#include "ManagedString.h"
#include <map>

/*
  Characters used for encodings:
  '.' - dot
  '-' - dash
  ' ' - space between characters
  ';' - space between words
  '#' - end of transmission
  we don't mark the space between dots and dashes because it can be added automatically when the morse code is played.
*/


/*
 * Comparator for ManagedString in order for the toChar map to work.
 */
struct cmpString {
    bool operator()(ManagedString a, ManagedString b) const {
        return (a < b);
    }
};

/*
 * Class that encodes characters into morse and decodes morse into characters.
 * 
 */
class MorseEncoder {
    
    private:
    /*
     * Maps from characters to their encodings and the other way around.
     */
    static std::map<char, ManagedString> toStr;
    std::map<ManagedString, char, cmpString> toChar;

    public:
    /*
     * Constructor
     *
     * Initializes the encoder.
     */
    MorseEncoder();

    /*
     * Writes encoding of in into out.
     * Out should be at least 6 times larger than in. 
     */
    void encode(const char* in, char* out);
    
    /*
     * Writes decoding of in into out.
     * Out should be the same size as in.
     */
    void decode(const char* in, char* out);
};

#endif