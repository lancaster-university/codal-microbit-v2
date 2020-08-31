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
     * Utility structure, used to hold definitions of scales and other progressions.
     */
    typedef struct
    {
        const float*      interval;
        const int         length;
    } Progression;

    /**
     * Utility class, holds common progressions and intervals.
     */

    class MusicalIntervals
    {
        public:

        static const float chromaticInterval[];
        static const float majorScaleInterval[];
        static const float minorScaleInterval[];
        static const float pentatonicScaleInterval[];
        static const float majorTriadInterval[];
        static const float minorTriadInterval[];
        static const float diminishedInterval[];
        static const float wholeToneInterval[];
    };

    class MusicalProgressions
    {
        public:

        static const Progression *chromatic;
        static const Progression *majorScale;
        static const Progression *minorScale;
        static const Progression *pentatonic;
        static const Progression *majorTriad;
        static const Progression *minorTriad;
        static const Progression *diminished;
        static const Progression *wholeTone;
    };

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
        static void exponentialFallingInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context);   // SOUNDS_REALLY ODD...
        static void appregrioAscending(SoundEmojiSynthesizer *synth, ToneEffect *context);
        static void appregrioDescending(SoundEmojiSynthesizer *synth, ToneEffect *context);

        /**
         * Frequency Delta effects
         */
        static void frequencyVibratoEffect(SoundEmojiSynthesizer *synth, ToneEffect *context);
        static void volumeVibratoEffect(SoundEmojiSynthesizer *synth, ToneEffect *context);

        /**
         * Volume Delta effects
         */
        static void adsrVolumeEffect(SoundEmojiSynthesizer *synth, ToneEffect *context);
        static void volumeRampEffect(SoundEmojiSynthesizer *synth, ToneEffect *context);
    };
}

#endif
