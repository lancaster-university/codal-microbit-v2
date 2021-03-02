/*
The MIT License (MIT)

Copyright (c) 2017 Lancaster University.

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
#include "StreamNormalizer.h"
#include "StreamSplitter.h"
#include "LevelDetector.h"
#include "LevelDetectorSPL.h"

namespace codal
{
    /**
     * Class definition for MicroBitAudio
     */
    class MicroBitAudio : public CodalComponent
    {
        public:
        static MicroBitAudio    *instance;      // Primary instance of MicroBitAudio, on demand activated.
        Mixer2                  mixer;          // Multi channel audio mixer
        StreamNormalizer        *processor;     // Stream Normaliser instance
        StreamSplitter          *splitter;      // Stream Splitter instance
        LevelDetector           *level;         // Level Detector instance
        LevelDetectorSPL        *levelSPL;      // Level Detector SPL instance

        private:
        bool speakerEnabled;                    // State of on board speaker
        bool pinEnabled;                        // State of on auxiliary output pin
        NRF52Pin &pin;                          // Auxiliary pin to route audio to
        NRF52Pin &speaker;                      // Primary pin for onboard speaker
        SoundEmojiSynthesizer synth;            // Synthesizer used bfor SoundExpressions
        MixerChannel *soundExpressionChannel;   // Mixer channel associated with sound expression audio
        NRF52PWM *pwm;                          // PWM driver used for sound generation (mixer output)
        NRF52ADC &adc;                          // ADC from MicroBitConstructor
        NRF52ADCChannel *mic;                   // Microphone ADC Channel from uBit.IO
        NRF52Pin &microphone;                   // Microphone pin passed from MicroBit constructor
        NRF52Pin &runmic;                       // Runmic pin passed from MicroBit constructor

        public:
        SoundExpressions soundExpressions;      // SoundExpression intepreter
        SoundOutputPin   virtualOutputPin;      // Virtual PWM channel (backward compatibility).

        /**
         * Constructor.
         */
        MicroBitAudio(NRF52Pin &pin, NRF52Pin &speaker, NRF52ADC &adc, NRF52Pin &microphone, NRF52Pin &runmic);

        /**
         * Destructor.
         */
        ~MicroBitAudio();

        /**
         * Demand request from a component to enable the default instance of this audio pipeline
         */
        static void requestActivation();

        /**
          * Catch events from the splitter
          * @param MicroBitEvent
          */
        void onSplitterEvent(MicroBitEvent);

        /**
          * Activate Mic
          */
        void activateMic();

        /**
          * Dectivate Mic
          */
        void deactivateMic();

        /**
         * post-constructor initialisation method
         */
        int enable();

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

        /**
         * Define which pin on the edge connector is used for audio.
         * @param pin The pin to use for auxiliary audio.
         */
        void setPin(NRF52Pin &pin);

        /**
         * Define if audio is enabled on the edge connector pin.
         * @param on New value.
         */
        void setPinEnabled(bool on);

        /**
         * Query whether the audio is enabled on the edge connector.
         * @return true if enabled, false otherwise.
         */
        bool isPinEnabled();
    };
}

#endif
