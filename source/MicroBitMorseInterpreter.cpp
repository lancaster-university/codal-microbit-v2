#include "MicroBitMorseInterpreter.h"

/*
 * This class takes morse data from a MicroBitMorseCodeRecogniser and uses a MorseEncoder to decode it.
 * It then calls an event signalling the fact that it is done processing some data.
 * The last processed data can then be taken from lastMessage. 
 */

/* 
 * Constructor.
 * 
 * Initializes the interpreter.
 * 
 * @param rec is the recogniser this will receive data from
 * 
 * @param bit is the micro:bit
 */
MicroBitMorseInterpreter::MicroBitMorseInterpreter(MicroBitMorseRecogniser& rec, MicroBit& bit)
                                : recogniser(rec), uBit(bit){
    recogniser.connect(this);
    interpreting = false;
    encoder = MorseEncoder();
}

/*
 * Callback for when the data is ready.
 */
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


/*
 * Starts interpreting and also starts the associated recogniser.
 */
void MicroBitMorseInterpreter::startInterpreting() {
    interpreting = true;
    recogniser.startAnalysing();
}

/*
 * Stops interpreting and also stops the associated recogniser.
 */
void MicroBitMorseInterpreter::stopInterpreting() {
    interpreting = false;
    recogniser.stopAnalysing();
}
