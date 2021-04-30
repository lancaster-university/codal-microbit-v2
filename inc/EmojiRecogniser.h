
#ifndef EMOJI_RECOGNISER_H
#define EMOJI_RECOGNISER_H

#include "MicroBitSoundRecogniser.h"

class EmojiRecogniser : public MicroBitSoundRecogniser
{
    void addHappySound( MicroBit& ubit);
    void addHelloSound( MicroBit& ubit);
    void addSadSound( MicroBit& ubit);
    void addSoaringSound( MicroBit& ubit);
    void addTwinkleSound( MicroBit& ubit);

    public:
    EmojiRecogniser(MicroBitAudioProcessor& processor, 
                    MicroBit& uBit);
};

#endif