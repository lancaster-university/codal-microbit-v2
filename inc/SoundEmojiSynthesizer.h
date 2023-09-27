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

#ifndef SOUND_EMOJI_SYNTHESIZER_H
#define SOUND_EMOJI_SYNTHESIZER_H

#include "DataStream.h"

#ifndef CONFIG_EMOJI_SYNTHESIZER_OUTPUT_BUFFER_DEPTH
#define CONFIG_EMOJI_SYNTHESIZER_OUTPUT_BUFFER_DEPTH  3
#endif

#define EMOJI_SYNTHESIZER_SAMPLE_RATE         44100
#define EMOJI_SYNTHESIZER_TONE_WIDTH          1024
#define EMOJI_SYNTHESIZER_TONE_WIDTH_F        1024.0f
#define EMOJI_SYNTHESIZER_BUFFER_SIZE         512

#define EMOJI_SYNTHESIZER_TONE_EFFECT_PARAMETERS        2
#define EMOJI_SYNTHESIZER_TONE_EFFECTS                  3

//
// Status flags
//
#define EMOJI_SYNTHESIZER_STATUS_ACTIVE                         0x01
#define EMOJI_SYNTHESIZER_STATUS_OUTPUT_SILENCE_AS_EMPTY        0x02
#define EMOJI_SYNTHESIZER_STATUS_STOPPING                       0x04


#define DEVICE_ID_SOUND_EMOJI_SYNTHESIZER_0 3010
#define DEVICE_ID_SOUND_EMOJI_SYNTHESIZER_1 3011
#define DEVICE_ID_SOUND_EMOJI_SYNTHESIZER_2 3012
#define DEVICE_ID_SOUND_EMOJI_SYNTHESIZER_3 3013
#define DEVICE_ID_SOUND_EMOJI_SYNTHESIZER_4 3014
#define DEVICE_ID_SOUND_EMOJI_SYNTHESIZER_5 3015
#define DEVICE_ID_SOUND_EMOJI_SYNTHESIZER_6 3016
#define DEVICE_ID_SOUND_EMOJI_SYNTHESIZER_7 3017
#define DEVICE_ID_SOUND_EMOJI_SYNTHESIZER_8 3018
#define DEVICE_ID_SOUND_EMOJI_SYNTHESIZER_9 3019

#define DEVICE_SOUND_EMOJI_SYNTHESIZER_EVT_DONE 1
#define DEVICE_SOUND_EMOJI_SYNTHESIZER_EVT_PLAYBACK_COMPLETE 2


namespace codal
{

    /**
     * Tone Generator and Effect function prototypes
     */
    class SoundEmojiSynthesizer;
    typedef struct ToneEffect ToneEffect;
    typedef uint16_t (*TonePrintFunction)(void *arg, int position);
    typedef void     (*ToneEffectFunction)(SoundEmojiSynthesizer *synth, ToneEffect *context);

    /**
     * Definition of a parameterised Toneprint (e.g.SquareWave, SinWave etc)
     */
    typedef struct
    {
        TonePrintFunction       tonePrint;
        void *                  parameter;
    } TonePrint;

    /**
     * Definition of a parameterised Tone Effect (e.g. vibrato, chromatic interpolator etc)
     */
    struct ToneEffect
    {
        ToneEffectFunction      effect;                                                 // Effect function to invoke
        int                     step;                                                   // Current step being executed
        int                     steps;                                                  // Number of time-steps when to apply this effect
        union {
            float               parameter[EMOJI_SYNTHESIZER_TONE_EFFECT_PARAMETERS];    // Effect specific parameters
            uint32_t            parameter_i[EMOJI_SYNTHESIZER_TONE_EFFECT_PARAMETERS];  // Effect specific parameters
            const void *        parameter_p[EMOJI_SYNTHESIZER_TONE_EFFECT_PARAMETERS];  // Effect specific parameters     
        };
    };

    /**
     * Definition of a sound effect that can be synthesized.
     */
    typedef struct
    {
        float               frequency;                                      // Central frequency of this sound effect
        float               volume;                                         // Central volume of this sound effect
        float               duration;                                       // Duration of the sound in milliseconds. Negative values are interpreted as infinite.
        TonePrint           tone;                                           // TonePrint function and parameters
        ToneEffect          effects[EMOJI_SYNTHESIZER_TONE_EFFECTS];        // Optional Effects to apply to the SoundEffect
    } SoundEffect;

    /**
      * Class definition for the micro:bit Sound Emoji Synthesizer.
      * Generates synthesized sound effects based on a set of parameterised inputs.
      */
    class SoundEmojiSynthesizer : public DataSource, public CodalComponent
    {
        public:

        DataSink*               downStream;             // Our downstream component.
        FiberLock               lock;                   // Ingress queue to handle concurrent playback requests.
        ManagedBuffer           buffer;                 // Current playout buffer.
        ManagedBuffer           buffer2;                // Current playout buffer.
        ManagedBuffer           effectBuffer;           // Current sound effect sequence being generated.
        ManagedBuffer           emptyBuffer;            // Zero length buffer.
        SoundEffect*            effect;                 // The effect within the current EffectBuffer that's being generated.
        uint16_t*               partialBuffer;          // Reference to a position within a DMA buffer, if a SFX completed mid buffer.

        int                     sampleRate;             // The sample rate of our output, measure in samples per second (e.g. 44000).
        float                   sampleRange;            // The maximum sample value that can be output.
        uint16_t                orMask;                 // A bitmask that is logically OR'd with each output sample.
        int                     bufferSize;             // The number of samples to create in a single buffer before scheduling it for playback
        int                     playbackCompleteIn;     // The number of DMA buffers to generate befor next issuing a DEVICE_SOUND_EMOJI_SYNTHESIZER_EVT_PLAYBACK_COMPLETE event

        float                   frequency;              // The instantaneous frequency currently being generated within an effect.
        float                   volume;                 // The instantaneous volume currently being generated within an effect.
        int                     samplesToWrite;         // The number of samples needed from the current sound effect block.
        int                     samplesWritten;         // The number of samples written from the current sound effect block.
        float                   position;               // Position within the tonePrint.
        float                   samplesPerStep[EMOJI_SYNTHESIZER_TONE_EFFECTS];     // The number of samples to render per step for each effect.
        /**
          * Default Constructor.
          * Creates an empty DataStream.
          *
          * @param id The ID of this synthesizer.
          * @param sampleRate The sample rate at which this synthesizer will produce data.
          */
        SoundEmojiSynthesizer(uint16_t id, int sampleRate = EMOJI_SYNTHESIZER_SAMPLE_RATE);

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
         * Determines if this source is connected to a downstream component
         * 
         * @return true If a downstream is connected
         * @return false If a downstream is not connected
         */
        virtual bool isConnected() override;

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
         * @return true if we've just completed a buffer of effects, false otherwise.
         */
        bool nextSoundEffect();

        /**
        * Schedules playout of the given sound effect.
        * @param sound A buffer containing an array of one or more SoundEffects.
        * @return DEVICE_OK on success, or DEVICE_INVALID_PARAMETER
        */
        int play(ManagedBuffer sound);

        /**
        * Stops play of the current buffer of SoundEffects and discards it.
        */
        void stop();

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
        virtual float getSampleRate() override;

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

        /**
         * Define how silence is treated on this components output.
         * 
         * @param mode if set to true, this component will return empty buffers when there is no data to send.
         * Otherwise, a full buffer containing silence will be genersated.
         */
        void allowEmptyBuffers(bool mode);


        private:

        /**
         * Determine the number of samples required for the given playout time.
         *
         * @param playoutTime The playout time (in milliseconds)
         * @return The number if samples required to play for the given amount fo time
         * (at the currently defined sample rate)
         */
        int determineSampleCount(float playoutTime);

        /**
        * Provide the next available ManagedBuffer to our downstream caller, if available.
        */
        ManagedBuffer fillOutputBuffer();

    };
}

#endif
