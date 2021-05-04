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

    callback(ManagedString(out));
    
    return DEVICE_OK;
}

void MicroBitMorseInterpreter::setCallback(void (*_callback)(ManagedString)){
    callback = _callback;
}

void MicroBitMorseInterpreter::startInterpreting(void (*_callback)(ManagedString)){
    setCallback(_callback);
    interpreting = true;
    recogniser.startAnalisying();
}

void MicroBitMorseInterpreter::stopInterpreting(){
    interpreting = false;
    recogniser.stopAnalisying();
}
