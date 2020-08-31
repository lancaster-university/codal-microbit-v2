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

#ifndef SOUND_SYNTHESIZER_EFFECTS_H
#define SOUND_SYNTHESIZER_EFFECTS_H

#include "SoundEmojiSynthesizer.h"

namespace codal
{
    /**
     * Utility class, containing a library of sound effect functions.
     */
    class SoundSynthesizerEffects
    {
        public:

        /**
         * Root Frequency Interpolation Effect Functions
         */
        static void noInterpolation(SoundEmojiSynthesizer *synth, float *parameter);
        static void logarithmicInterpolation(SoundEmojiSynthesizer *synth, float *parameter);
        static void linearInterpolation(SoundEmojiSynthesizer *synth, float *parameter);
        static void curveInterpolation(SoundEmojiSynthesizer *synth, float *parameter);
        static void slowVibratoInterpolation(SoundEmojiSynthesizer *synth, float *parameter);
        static void warbleInterpolation(SoundEmojiSynthesizer *synth, float *parameter);
        static void vibratoInterpolation(SoundEmojiSynthesizer *synth, float *parameter);
        static void exponentialRisingInterpolation(SoundEmojiSynthesizer *synth, float *parameter);
        static void exponentialFallingInterpolation(SoundEmojiSynthesizer *synth, float *parameter);// SOUNDS_REALLY ODD!

        // TODO:
        static void majAppregrioAscendInterpolation(SoundEmojiSynthesizer *synth, float *parameter);// UNTESTED
        static void majAppregrioDescendInterpolation(SoundEmojiSynthesizer *synth, float *parameter);// UNTESTED
        static void minAppregrioAscendInterpolation(SoundEmojiSynthesizer *synth, float *parameter);// UNTESTED
        static void minAppregrioDescendInterpolation(SoundEmojiSynthesizer *synth, float *parameter);// UNTESTED
        static void dimAppregrioAscendInterpolation(SoundEmojiSynthesizer *synth, float *parameter);// UNTESTED
        static void dimAppregrioDescendInterpolation(SoundEmojiSynthesizer *synth, float *parameter);// UNTESTED
        static void chromaticAppregrioAscendInterpolation(SoundEmojiSynthesizer *synth, float *parameter);// UNTESTED
        static void chromaticAppregrioDescendInterpolation(SoundEmojiSynthesizer *synth, float *parameter);// UNTESTED
        static void toneAppregrioAscendInterpolation(SoundEmojiSynthesizer *synth, float *parameter);// UNTESTED
        static void toneAppregrioDescendInterpolation(SoundEmojiSynthesizer *synth, float *parameter);// UNTESTED

        /**
         * Frequency Delta effects
         */
        static void vibratoEffect(SoundEmojiSynthesizer *synth, float *parameter);      // UNTESTED
        static void slowVibratoEffect(SoundEmojiSynthesizer *synth, float *parameter);  // UNTESTED

        /**
         * Volume Delta effects
         */
        static void adsrVolumeEffect(SoundEmojiSynthesizer *synth, float *parameter);
        static void volumeRampEffect(SoundEmojiSynthesizer *synth, float *parameter);
    };
}

#endif
