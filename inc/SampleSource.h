#ifndef SAMPLE_SOURCE_H
#define SAMPLE_SOURCE_H

#include "MemorySource.h"
#include "DataStream.h"
#include "Mixer2.h"

namespace codal
{
    class SampleSource : public MemorySource
    {
        private:
        Mixer2 &mixer;
        float sampleRate;
        float sampleRange;
        MixerChannel *channel;

        public:
            /**
             * Constructor.
             * Creates an empty sample source with a Mixer.
             * = CONFIG_MIXER_DEFAULT_SAMPLERATE
             */
            SampleSource(Mixer2 &mixer, float sampleRate, float sampleRange);

            /**
             * Destructor
             * Removes all resources held by the instance
             */
            ~SampleSource();

            void setSampleRate(float sampleRate);

            void setVolume(float volume);
    };
}
#endif