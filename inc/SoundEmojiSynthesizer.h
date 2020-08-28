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

#ifndef SOUND_EMOJI_SYNTHESIZER_H
#define SOUND_EMOJI_SYNTHESIZER_H

#include "DataStream.h"

#define EMOJI_SYNTHESIZER_SAMPLE_RATE         44100
#define EMOJI_SYNTHESIZER_TONE_WIDTH          1024
#define EMOJI_SYNTHESIZER_TONE_WIDTH_F        1024.0f
#define EMOJI_SYNTHESIZER_BUFFER_SIZE         512

//
// Status flags
#define EMOJI_SYNTHESIZER_STATUS_ACTIVE       0x01
//

namespace codal
{
    typedef uint16_t (*TonePrintFunction)(void *arg, int position);
    typedef float    (*ToneInterpolator)(float startFrequency, int endFrequency, int stepNumber, int stepCount);

    /**
     * Definition of a sound effect that can be synthesized.
     */
    typedef struct
    {
        float       startvolume;            // Initial volume of the effect (0..1)
        float       endVolume;              // Final volume of the effect (0..1)
        float       startFrequency;         // Initial frequency of the effect (Hz)
        float       endFrequency;           // Initial frequency of the effect (Hz)
        float       duration;               // milliseconds
        uint16_t    tone;                   // Tone identifier
        uint16_t    effect;                 // Effect identifier
        uint32_t    effectParameter1;        // Optional context sensitive effect parameter. 
        uint32_t    effectParameter2;        // Optional context sensitive effect parameter. 
        uint16_t    effect2;                 // Effect identifier
        uint32_t    effect2Parameter1;        // Optional context sensitive effect parameter. 
        uint32_t    effect2Parameter2;        // Optional context sensitive effect parameter. 

    } SoundEffect;


    /**
      * Class definition for the micro:bit Sound Emoji Synthesizer.
      * Generates synthesized sound effects based on a set of parameterised inputs.
      */
    class SoundEmojiSynthesizer : public DataSource, public CodalComponent
    {
        DataSink*               downStream;             // Our downstream component.
        FiberLock               lock;                   // Ingress queue to handle concurrent playback requests.
        ManagedBuffer           buffer;                 // Current playout buffer.
        ManagedBuffer           effectBuffer;           // Current sound effect sequence being generated.

        int                     sampleRate;             // The sample rate of our output, measure in samples per second (e.g. 44000).
        uint16_t                sampleRange;            // The maximum sample value that can be output.
        uint16_t                orMask;                 // A bitmask that is logically OR'd with each output sample.
        int                     bufferSize;             // The number of samples to create in a single buffer before scheduling it for playback
        int                     bytesWritten;           // Number of bytes written to the current output buffer.

        int                     samplesToWrite;         // The number of samples needed from the current sound effect block.
        int                     samplesWritten;         // The number of samples written from the current sound effect block.
        float                   samplesPerStep;         // The number of samples to render for each interpolation step in an effect.
        float                   volumeDeltaPerStep;     // The volume change to apply for each interpolation step.
        float                   frequency;              // The instantaneous frequency currently being generated within an effect.
        float                   volume;                 // The instantaneous volume currently being generated within an effect.
        SoundEffect*            effect;                 // The effect within the current EffectBuffer that's being generated.

        void*                   tonePrintArg;
        TonePrintFunction       tonePrint;              // The currently selected playout tone (always unsigned).
        float                   position;               // Position within the tonePrint.

        ToneInterpolator        interpolator;           // The currently selected interpolation effect.
        int                     step;                   // the currnet step being rendered/
        int                     steps;                  // the number of discrete steps in an effect (e.g. an appregio)

        public:

        /**
         * Frequency Interpolation Effect Functions
         */
        static float noInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount);
        static float logarithmicInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount);
        static float linearInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount);
        static float curveInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount);
        static float slowVibratoInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount);
        static float warbleInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount);
        static float vibratoInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount);
        static float exponentialRisingInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount);
        static float exponentialFallingInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount);
        static float majAppregrioAscendInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount);
        static float majAppregrioDescendInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount);
        static float minAppregrioAscendInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount);
        static float minAppregrioDescendInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount);
        static float dimAppregrioAscendInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount);
        static float dimAppregrioDescendInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount);
        static float chromaticAppregrioAscendInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount);
        static float chromaticAppregrioDescendInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount);
        static float toneAppregrioAscendInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount);
        static float toneAppregrioDescendInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount);


        /**
          * Default Constructor.
          * Creates an empty DataStream.
          *
          * @param sampleRate The sample rate at which this synthesizer will produce data.
          */
        SoundEmojiSynthesizer(int sampleRate = EMOJI_SYNTHESIZER_SAMPLE_RATE);

        /**
          * Destructor.
          * Removes all resources held by the instance.
          */
        ~SoundEmojiSynthesizer();

        /**
         * Define a downstream component for data stream.
         *
         * @sink The component that data will be delivered to, when it is availiable
         */
        virtual void connect(DataSink &sink) override;

        /**
         *  Determine the data format of the buffers streamed out of this component.
         */
        virtual int getFormat() override;

        /**
         * Provide the next available ManagedBuffer to our downstream caller, if available.
         */
        virtual ManagedBuffer pull() override;

        /**
         * Schedules the next sound effect as defined in the effectBuffer, if available.
         */
        void nextSoundEffect();

        /**
        * Schedules playout of the given sound effect.
        * @param sound A buffer containing an array of one or more SoundEffects.
        * @return DEVICE_OK on success, or DEVICE_INVALID_PARAMETER
        */
        int play(ManagedBuffer sound);

        /**
        * Define the size of the audio buffer to hold. The larger the buffer, the lower the CPU overhead, but the longer the delay.
        * @param size The new bufer size to use.
        * @return DEVICE_OK on success, or DEVICE_INVALID_PARAMETER
        */
        int setBufferSize(int size);

        /**
         * Determine the sample rate currently in use by this Synthesizer.
         * @return the current sample rate, in Hz.
         */
        int getSampleRate();

        /**
         * Change the sample rate used by this Synthesizer,
         * @param sampleRate The new sample rate, in Hz.
         * @return DEVICE_OK on success.
         */
        int setSampleRate(int sampleRate);

        /**
         * Determines the sample range used by this Synthesizer,
         * @return the maximum sample value that will be output by the Synthesizer.
         */
        uint16_t getSampleRange();

        /**
         * Change the sample range used by this Synthesizer,
         * @param sampleRange The new sample range, that defines by the maximum sample value that will be output by the Synthesizer.
         * @return DEVICE_OK on success.
         */
        int setSampleRange(uint16_t sampleRange);

        /**
         * Defines an optional bit mask to logical OR with each sample.
         * Useful if the downstream component encodes control data within its samples.
         *
         * @param mask The bitmask to to apply to each sample.
         * @return DEVICE_OK on success.
         */
        int setOrMask(uint16_t mask);


        private:

        /**
         * Determine the number of samples required for the given playout time.
         *
         * @param playoutTimeUs The playout time (in microseconds)
         * @return The number if samples required to play for the given amount fo time
         * (at the currently defined sample rate)
         */
        int determineSampleCount(int playoutTimeUs);

    };
}

#endif
