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
        static void noInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context);
        static void logarithmicInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context);
        static void linearInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context);
        static void curveInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context);
        static void slowVibratoInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context);
        static void warbleInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context);
        static void vibratoInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context);
        static void exponentialRisingInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context);
        static void exponentialFallingInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context);// SOUNDS_REALLY ODD!

        // TODO:
        static void majAppregrioAscendInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context);// UNTESTED
        static void majAppregrioDescendInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context);// UNTESTED
        static void minAppregrioAscendInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context);// UNTESTED
        static void minAppregrioDescendInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context);// UNTESTED
        static void dimAppregrioAscendInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context);// UNTESTED
        static void dimAppregrioDescendInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context);// UNTESTED
        static void chromaticAppregrioAscendInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context);// UNTESTED
        static void chromaticAppregrioDescendInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context);// UNTESTED
        static void toneAppregrioAscendInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context);// UNTESTED
        static void toneAppregrioDescendInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context);// UNTESTED

        /**
         * Frequency Delta effects
         */
        static void vibratoEffect(SoundEmojiSynthesizer *synth, ToneEffect *context);      // UNTESTED
        static void slowVibratoEffect(SoundEmojiSynthesizer *synth, ToneEffect *context);  // UNTESTED

        /**
         * Volume Delta effects
         */
        static void adsrVolumeEffect(SoundEmojiSynthesizer *synth, ToneEffect *context);
        static void volumeRampEffect(SoundEmojiSynthesizer *synth, ToneEffect *context);
    };
}

#endif
