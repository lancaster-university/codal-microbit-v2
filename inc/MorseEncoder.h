#include "ManagedString.h"
#include <map>

/*
International Morse code is composed of five elements:

    short mark, dot or "dit": "dot duration" is one time unit long
    longer mark, dash or "dah": three time units long
    inter-element gap between the dots and dashes within a character: one unit long
    short gap (between letters): three time units long
    medium gap (between words): seven time units long

*/

struct cmpString {
    bool operator()(ManagedString a, ManagedString b) const {
        return (a < b);
    }
};

class MorseEncoder {
    
    private:
    static std::map<char, ManagedString> toStr;
    std::map<ManagedString, char, cmpString> toChar;

    public:
    MorseEncoder();
    void encode(const char* in, char* out);
    void decode(const char* in, char* out);
};
