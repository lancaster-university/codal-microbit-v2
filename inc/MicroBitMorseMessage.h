/*
The MIT License (MIT)
Copyright (c) 2020 Arm Limited.
Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include "MicroBit.h"
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

enum Symbol {Dot, Dash, Unit_Gap, Letter_Gap, Word_Gap};

class Node{
public:
    Symbol data;
    Node* next;
};

struct cmpString {
    bool operator()(ManagedString a, ManagedString b) const {
        return (a < b);
    }
};


class MicroBitMorseMessage {
    
    private:
    Node* head;
    Node* end;
    Node* curr;
    MicroBit* uBit;

    static std::map<char, ManagedString> toStr;
    std::map<ManagedString, char, cmpString> toChar;

    public:
    MicroBitMorseMessage(MicroBit* bit);
    ~MicroBitMorseMessage(); 
    bool isEmpty();
    void push(Symbol s);
    void push(char c);
    void push(const char* c);
    Symbol getSymbol();
    void get(char* out);
};
