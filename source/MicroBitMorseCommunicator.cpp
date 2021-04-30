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
#include "MicroBitMorseCommunicator.h"


MicroBitMorseCommunicator::MicroBitMorseCommunicator(MicroBit* bit){
    uBit = bit;
    duration = DEFAULT_DURATION;
    frequency = DEFAULT_FREQUENCY;
    randomness = DEFAULT_RANDOMNESS;

    createFrames();
}

// turns n into a managed string of size 4
ManagedString fourString(int n){
    if (n < 10)
        return ManagedString("000") + ManagedString(n);
    if (n < 100)
        return ManagedString("00") + ManagedString(n);
    if (n < 1000)
        return ManagedString("0") + ManagedString(n);
    if (n > 9999)
        return ManagedString("9999");
    return ManagedString(n);
}

void MicroBitMorseCommunicator::createFrames(){
    ManagedString freqString = fourString(frequency);
    ManagedString dotDurationString = fourString(duration);
    ManagedString dashDurationString = fourString(duration*3);
    ManagedString randomString = fourString(randomness);


    dotFrame = ManagedString("01023")+
                freqString+
                dotDurationString+
                ManagedString("02440")+
                freqString+
                ManagedString("0888102301280002000024")+
                randomString+
                ManagedString("000000000000000000000000");

    dashFrame = ManagedString("01023")+
                freqString+
                dashDurationString+
                ManagedString("02440")+
                freqString+
                ManagedString("0888102301280002000024")+
                randomString+
                ManagedString("000000000000000000000000");
}


MicroBitMorseCommunicator::~MicroBitMorseCommunicator(){
    return;
}

void MicroBitMorseCommunicator::play(Symbol s){
    // uBit->serial.printf("%d\n", s);
    switch(s){
        case Unit_Gap:
            uBit->sleep(duration);
            break;
        case Letter_Gap:
            uBit->sleep(duration * 3);
            break;
        case Word_Gap:
            uBit->sleep(duration * 7);
            break;
        case Dot:
            uBit->audio.soundExpressions.play(dotFrame);
            break;
        case Dash:
            uBit->audio.soundExpressions.play(dashFrame);
            break;
    }
}

void MicroBitMorseCommunicator::send(MicroBitMorseMessage* mess){
    while (!mess->isEmpty()){
        this->play(mess->getSymbol());
    }
}

void MicroBitMorseCommunicator::set_duration(int d){
    duration = d;
    createFrames();
}

void MicroBitMorseCommunicator::set_frequency(int f){
    frequency = f;
    createFrames();
}

void MicroBitMorseCommunicator::set_randomness(int r){
    randomness = r;
    createFrames();
}