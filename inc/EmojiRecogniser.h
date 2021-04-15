
#ifndef EMOJI_RECOGNISER_H
#define EMOJI_RECOGNISER_H

#include "MicroBitSoundRecogniser.h"

class EmojiRecogniser : public MicroBitSoundRecogniser
{
    void addHappySound();

    public:
    EmojiRecogniser(MicroBitAudioProcessor& processor, 
                    MicroBit& uBit);
};

#endif