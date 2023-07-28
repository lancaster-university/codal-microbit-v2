#include "MicroBit.h"
#include "MorseEncoder.h"

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

// Initialize map from characters to encodings
std::map<char, ManagedString> MorseEncoder::toStr = {
    {'A', ".-"}, {'B', "-..."}, {'C', "-.-."}, {'D', "-.."}, {'E', "."}, {'F', "..-."}, {'G', "--."}, {'H', "...."}, {'I', ".."}, {'J', ".---"},
    {'K', "-.-"}, {'L', ".-.."}, {'M', "--"}, {'N', "-."}, {'O', "---"}, {'P', ".--."}, {'Q', "--.-"}, {'R', ".-."}, {'S', "..."}, {'T', "-"},
    {'U', "..-"}, {'V', "...-"}, {'W', ".--"}, {'X', "-..-"}, {'Y', "-.--"}, {'Z', "--.."}, {'1', ".----"}, {'2', "..---"}, {'3', "...--"},
    {'4', "....-"}, {'5', "....."}, {'6', "-...."}, {'7', "--..."}, {'8', "---.."}, {'9', "----."}, {'0', "-----"}, {'&', ".-..."}};

/*
* Constructor
*
* Initializes the encoder.
*/
MorseEncoder::MorseEncoder() {
    // Generate toChar from toStr    
    for(std::map<char, ManagedString>::iterator it = toStr.begin(); it != toStr.end(); ++it) {
        toChar[it->second] = it->first;
    }
}

/*
* Encodes a string into morse code.
* 
* @param in is the input string to be encoded
* 
* @param out is the output string that the encoding is written into
* 
* @note out should be at least 6 times the size of in
*/
void MorseEncoder::encode(const char* in, char* out) {
    int i = 0; // index over in
    int j = 0; // index over out
    char c;
    while (in[i]!=0) {
        c = in[i];
        // capitalize letters
        if (('a' <= c) && (c <= 'z')) c -= ('a' - 'A');
    
        //  turn unknown characters into &
        if (c != ' ' && (c < 'A' || c > 'Z') && (c < '0' || c > '9')) c = '&';

        if (c == ' '){
            if (j>0 && out[j-1] == ' ') out[j-1] = ';';
            else {out[j] = ';'; j++;}
        }
        else{

            ManagedString s = toStr[c];
            int k = 0; // index over s
            while (s.charAt(k)!=0) {
                out[j] = s.charAt(k);
                j++;k++;
            }
            out[j] = ' ';j++;
        }
        i++;
    }
    if (j>0 && out[j-1] == ' ') out[j-1] = '#';
            else {out[j] = '#'; j++;}
    out[j] = 0;
}

/*
* Decodes morse code into a string.
* 
* @param in is the input string containing morse code
* 
* @param out is the output string that the morse code is decoded into
* 
* @note out should be at least the same size as in
*/
void MorseEncoder::decode(const char* in, char* out){ 
    int i = 0; // index over in
    int j = 0; // index over out   
    int k = 0; // index over s
    char s[10]; // current character buffer
    char c;

    while (in[i]!=0){
        c = in[i];
        if (c == '.' || c == '-'){
            s[k] = c; k++;
        }
        else{
            if (k!=0){
                s[k] = 0;
                k = 0;
                out[j] = toChar[s];         
                j++;
            }
            if (c == ';'){
                out[j] = ' ';
                j++;
            }
        }
        i++;
    }
    out[j] = 0;
}
