/*
The MIT License (MIT)

Copyright (c) 2016 British Broadcasting Corporation.
This software is provided by Lancaster University by arrangement with the BBC.

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

#ifndef MICROBIT_AUDIO_H
#define MICROBIT_AUDIO_H

#include "NRF52PWM.h"
#include "SoundEmojiSynthesizer.h"
#include "SoundExpressions.h"
#include "Mixer2.h"
#include "SoundOutputPin.h"

namespace codal
{
    /**
     * Class definition for MicroBitAudio
     */
    class MicroBitAudio : public CodalComponent
    {
        public:
        Mixer2           mixer;      

        private:
        bool speakerEnabled;
        NRF52Pin &pin;
        NRF52Pin &speaker;
        SoundEmojiSynthesizer synth;
        MixerChannel *soundExpressionChannel;
        NRF52PWM *pwm;

        public:
        SoundExpressions soundExpressions;  
        SoundOutputPin   virtualOutputPin;

        /**
         * Constructor.
         */
        MicroBitAudio(NRF52Pin &pin, NRF52Pin &speaker);

        /**
         * Destructor.
         */
        ~MicroBitAudio();

        /**
         * post-constructor initialisation method
         */
        virtual int init() override;

        /**
         * Get the current volume.
         * @return The output volume, in the range 0..255.
         */
        int getVolume();

        /**
        * Define the volume.
        * @param volume The new output volume, in the range 0..255
        * @return DEVICE_OK on success, or DEVICE_INVALID_PARAMETER
        */
        int setVolume(int volume);

        /**
         * Enable or disable use of the on-board speaker.
         * @param on New value.
         */
        void setSpeakerEnabled(bool on);

        /**
         * Query whether the speaker is enabled.
         * @return true if enabled, false otherwise.
         */
        bool isSpeakerEnabled();

    };
}

#endif
