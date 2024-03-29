/*
The MIT License (MIT)

Copyright (c) 2020 Lancaster University.

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

#ifndef SOUND_EXPRESSIONS_SYNTHESIZER_H
#define SOUND_EXPRESSIONS_SYNTHESIZER_H

#include "ManagedString.h"
#include "SoundEmojiSynthesizer.h"

namespace codal
{

    class SoundExpressions
    {
        public:

        /**
          * Default Constructor.
          */
        SoundExpressions(SoundEmojiSynthesizer &synth);

        /**
          * Destructor.
          * Removes all resources held by the instance.
          */
        ~SoundExpressions();

        /**
         * Plays a sound encoded as a series of decimal encoded effects or specified by name.
         * Blocks until the sound generation is complete, and the synthesizer is ready to accept new requests.
         * @param sound a string representing the sound effect to play, in the form descripbed by parseSoundExperession().
         * @param event the event to wait for - either DEVICE_SOUND_EMOJI_SYNTHESIZER_EVT_DONE (default) or DEVICE_SOUND_EMOJI_SYNTHESIZER_EVT_PLAYBACK_COMPLETE.
         */
        void play(ManagedString sound, uint16_t event = DEVICE_SOUND_EMOJI_SYNTHESIZER_EVT_DONE);

        /**
         * Plays a sound encoded as a series of decimal encoded effects or specified by name.
         * Does not block.
         * @param a string representing the sound effect to play, in the form descripbed by parseSoundExperession().
         */
        void playAsync(ManagedString sound);

        /**
         * Plays a sound encoded as a series of decimal encoded effects or specified by name.
         * Blocks until the sound generation is complete, and the synthesizer is ready to accept new requests.
         * @param sound a string representing the sound effect to play, in the form descripbed by parseSoundExperession().
         * @param event the event to wait for - either DEVICE_SOUND_EMOJI_SYNTHESIZER_EVT_DONE (default) or DEVICE_SOUND_EMOJI_SYNTHESIZER_EVT_PLAYBACK_COMPLETE.
         */
        void play(ManagedBuffer sound, uint16_t event = DEVICE_SOUND_EMOJI_SYNTHESIZER_EVT_DONE);

        /**
         * Plays a sound encoded as an array of one or more SoundEffect structures.
         * Does not block unless a sound effect is already queued.
         */
        void playAsync(ManagedBuffer sound);
        
        /**
         * Stops the currently playing sound.
         */
        void stop();

        private:
        SoundEmojiSynthesizer &synth;

        static int parseDigits(const char *input, const int digits);
        static int applyRandom(int value, int rand);
        static ManagedString lookupBuiltIn(ManagedString sound);
        bool parseSoundExpression(const char *soundChars, SoundEffect *fx);

    };
}

#endif