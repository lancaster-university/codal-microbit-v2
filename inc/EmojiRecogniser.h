
#ifndef EMOJI_RECOGNISER_H
#define EMOJI_RECOGNISER_H

#include "MicroBitSoundRecogniser.h"

class EmojiRecogniser : public MicroBitSoundRecogniser
{
    void addHappySound();
    void addHelloSound();
    void addSadSound();
    void addSoaringSound();
    void addTwinkleSound();

    public:
    EmojiRecogniser(MicroBitAudioProcessor& processor);
};

#endif