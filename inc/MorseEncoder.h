#ifndef MORSE_ENCODER_H
#define MORSE_ENCODER_H

#include "ManagedString.h"
#include <map>


/*
 * Comparator for ManagedString.
 */
struct cmpString {
    bool operator()(ManagedString a, ManagedString b) const {
        return (a < b);
    }
};

/*
 * The morse encoder has two uses. 
 * Firstly, it can encode a string of characters into its morse representation.
 * This is done by transforming every character using a map and adding spaces between characters and words when appropriate.
 * At the end, the `end of transmission` character is added.
 * Whenever a lowercase letter is encountered, it is converted to uppercase.
 * Whenever an unknown character is encountered, it is converted to '&'.
 * 
 * Secondly, it can decode a series of morse elements into the corresponding string.
 * Decoding takes every continuous sequence of dots and dashes and transforms it into the appropriate character using another map.
 * This is done by building up every continuous sequence of dots and dashes into a small buffer and, when another element is encountered,
 * transforming it into the appropriate character using a map. The buffer is reset every time this happens.
 * All of the generated chracyters are added to the output string.
 * 
 * 
 * This uses the following elements for morse code:
 * dot - '.'
 * dash - '-'
 * element gap - not represented
 * letter gap - ' '
 * word gap - ';'
 * end of transmission - '#'
 * 
 * The reason element gap is not represented is that it can be easily added appropriately at the time that the morse code is played over the speaker. 
 * When interpreting morse played by another source, it can just be ignored.
*/
class MorseEncoder {
    
    private:
    static std::map<char, ManagedString> toStr; // map from characters to their morse encoding
    std::map<ManagedString, char, cmpString> toChar; // map from morse encodings to their characters

    public:
    /*
     * Constructor
     *
     * Initializes the encoder.
     */
    MorseEncoder();

    /*
     * Encodes a string into morse code.
     * 
     * @param in is the input string to be encoded
     * 
     * @param out is the output string that the encoding is written into
     * 
     * @note out should be at least 6 times the size of in
     */
    void encode(const char* in, char* out);
    
    /*
     * Decodes morse code into a string.
     * 
     * @param in is the input string containing morse code
     * 
     * @param out is the output string that the morse code is decoded into
     * 
     * @note out should be at least the same size as in
     */
    void decode(const char* in, char* out);
};

#endif