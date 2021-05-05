#include "MicroBitMorseInterpreter.h"

MicroBitMorseInterpreter::MicroBitMorseInterpreter(MicroBitMorseRecogniser& rec, MicroBit& bit)
                                : recogniser(rec), uBit(bit){
    recogniser.connect(this);
    interpreting = false;
    encoder = MorseEncoder();
}

int MicroBitMorseInterpreter::pullRequest() {

    ManagedBuffer data = recogniser.pull();

    if(!interpreting) return DEVICE_OK;

    char* in = (char*)data.getBytes();
    char* out = new char[data.length()];
    encoder.decode(in, out);

    lastMessage = out;

    Event evnt(DEVICE_ID_MORSE_INTERPRETER, DEVICE_MORSE_INTERPRETER_EVT_NEW_MESSAGE);

    return DEVICE_OK;
}

void MicroBitMorseInterpreter::startInterpreting() {
    interpreting = true;
    recogniser.startAnalisying();
}

void MicroBitMorseInterpreter::stopInterpreting() {
    interpreting = false;
    recogniser.stopAnalisying();
}
