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
#include "MicroBitMorseMessage.h"

std::map<char, ManagedString> MicroBitMorseMessage::toStr = {
    {'A', ".-"}, {'B', "-..."}, {'C', "-.-."}, {'D', "-.."}, {'E', "."}, {'F', "..-."}, {'G', "--."}, {'H', "...."}, {'I', ".."}, {'J', ".---"},
    {'K', "-.-"}, {'L', ".-.."}, {'M', "--"}, {'N', "-."}, {'O', "---"}, {'P', ".--."}, {'Q', "--.-"}, {'R', ".-."}, {'S', "..."}, {'T', "-"},
    {'U', "..-"}, {'V', "...-"}, {'W', ".--"}, {'X', "-..-"}, {'Y', "-.--"}, {'Z', "--.."}, {'1', ".----"}, {'2', "..---"}, {'3', "...--"},
    {'4', "....-"}, {'5', "....."}, {'6', "-...."}, {'7', "--..."}, {'8', "---.."}, {'9', "----."}, {'0', "-----"}};

MicroBitMorseMessage::MicroBitMorseMessage(MicroBit* bit) {
    head = new Node();
    end = head;
    curr = head;

    uBit = bit;

    for(std::map<char, ManagedString>::iterator it = toStr.begin(); it != toStr.end(); ++it) {
        toChar[it->second] = it->first;
    }
}

MicroBitMorseMessage::~MicroBitMorseMessage() {
    Node* i = head;
    Node* j = 0;
    while (i != 0) {
        j = i->next;
        free(i);
        i = j;
    }
}

Symbol MicroBitMorseMessage::getSymbol() {
    curr = curr->next;
    return curr->data;
}

void MicroBitMorseMessage::get(char* out){    
    int i = 0, si = 0;
    char s[10];
    Symbol x;

    while (!isEmpty()){
        x = getSymbol();
        switch(x){
            case Dot:
                s[si] = '.';
                si++;
                break;
            case Dash:
                s[si] = '-';
                si++;
                break;
            case Unit_Gap:
                break;
            default:
                if (si!=0){
                    s[si] = 0;
                    si = 0;
                    out[i] = toChar[s];            
                    i++;
                }
                if (x == Word_Gap){
                    out[i] = ' ';
                    i++;
                }
        }
    }
    if (si!=0){
        s[si] = 0;
        si = 0;
        out[i] = toChar[s];            
        i++;
    }
    out[i] = 0;
}

bool MicroBitMorseMessage::isEmpty() {
    return (curr->next == 0);
}

void MicroBitMorseMessage::push(Symbol s) {
    Node* aux =  new Node();
    aux->data = s;
    end->next = aux;
    end = aux;
}

void MicroBitMorseMessage::push(char c) {
    // capitalize letters
    if (('a' <= c) && (c <= 'z')) c -= ('a' - 'A');
    
    //  turn unknown characters into E
    if (c != ' ' && (c < 'A' || c > 'Z') && (c < '0' || c > '9')) c = 'E';

    if (c == ' ') push(Word_Gap);
    
    // add space before every char except the first of every word
    if ( head->next != 0 && (end->data == Dot || end->data == Dash)) push(Letter_Gap);
    
    ManagedString s = toStr[c];
    int i = 0;
    while (s.charAt(i)!=0) {
        // add space before every dot or dash except the first
        if (i!=0) push(Unit_Gap);
        
        if (s.charAt(i) == '.') push(Dot);
        else push(Dash);
        i++;
    }
    i = 0;
    while (s.charAt(i)!=0) {
        // uBit->serial.printf("%c", s.charAt(i));
        i++;
    }
    
}

void MicroBitMorseMessage::push(const char* c) {
    int i = 0;
    while (c[i]!=0) {
        push(c[i]);
        i++;
    }
}
