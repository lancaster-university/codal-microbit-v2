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

#ifndef CODAL_MIXER2_H
#define CODAL_MIXER2_H

#include "DataStream.h"

#ifndef CONFIG_MIXER_BUFFER_SIZE
#define CONFIG_MIXER_BUFFER_SIZE 512
#endif

#ifndef CONFIG_MIXER_INTERNAL_RANGE
#define CONFIG_MIXER_INTERNAL_RANGE 1023
#endif

#ifndef CONFIG_MIXER_DEFAULT_SAMPLERATE
#define CONFIG_MIXER_DEFAULT_SAMPLERATE 44100
#endif


namespace codal
{

class MixerChannel : public DataSink
{
private:
    DataSource      *stream;                    // The DataSource feeding this mixer channel.
    ManagedBuffer   buffer;                     // The last buffer received from the DataSource.
    int             pullRequests;               // The number of buffers ready to be read from the DataSource.

    uint8_t         *in;                        // Pointer to the next sample that should be read from the current buffer.
    uint8_t         *end;                       // Pointer to the end of the current buffer (optimisation).

    float           range;                      // The number of quantization levels in the input data.
    float           rate;                       // The sample rate of the input data.
    float           offset;                     // Offset applied to every sample before mixing (for unsigned samples)
    float           gain;                       // Input gain to applied ot each sample to normalise (optimisation)
    float           skip;                       // Number of input samples to progress for each output sample (when sub/super sampling)
    float           position;                   // fractional position within the buffer of next sample (sub/super sampling) 

    float           volume;                     // Volume leve of channel, in the range 0..CONFIG_MIXER_INTERNAL_RANGE
    int             format;                     // Format of the data recieved on this channel (e.g. DATASTREAM_FORMAT_16BIT_UNSIGNED...)
    int             bytesPerSample;             // The number of bytes used in the input stream for each sample (optimisation)

    MixerChannel    *next;                      // Internal Linkage - list of all mixer channels

    friend class    Mixer2;

public:

    /**
     * Deliver the next available ManagedBuffer to our downstream caller.
     */
    virtual int pullRequest();
    virtual ~MixerChannel() {};
};

class Mixer2 : public DataSource
{
    MixerChannel    *channels;
    DataSink        *downStream;
    float           mix[CONFIG_MIXER_BUFFER_SIZE/2];
    float           outputRange;
    float           outputRate;
    int             outputFormat;
    int             bytesPerSampleOut;
    float           volume;
    uint32_t        orMask;

public:
    /**
     * Constructor.
     * Creates an empty Mixer.
     * @param sampleRate (samples per second) of the mixer output
     * @param sampleRange (quantization levels) the difference between the maximum and minimum sample level on the output channel
     * @param format The format the mixer will output (DATASTREAM_FORMAT_16BIT_UNSIGNED or DATASTREAM_FORMAT_16BIT_SIGNED)
     */
    Mixer2(int sampleRate = CONFIG_MIXER_DEFAULT_SAMPLERATE, int sampleRange = CONFIG_MIXER_INTERNAL_RANGE, int format = DATASTREAM_FORMAT_16BIT_UNSIGNED);

    /**
     * Destructor.
     * Removes all resources held by the instance.
     */
    ~Mixer2();

    /**
     * Add a new channel to the mixer.
     * 
     * @oaram stream DataSource to connect to the new input channel
     * @param sampleRate (samples per second) - if set to zero, defaults to the output sample rate of the Mixer
     * @param sampleRange (quantization levels) the difference between the maximum and minimum sample level on the input channel
     */
    MixerChannel *addChannel(DataSource &stream, int sampleRate = 0, int sampleRange = CONFIG_MIXER_INTERNAL_RANGE);

    /**
     * Provide the next available ManagedBuffer to our downstream caller, if available.
     */
    virtual ManagedBuffer pull();

    /**
     * Define a downstream component for data stream.
     *
     * @sink The component that data will be delivered to, when it is availiable
     */
    virtual void connect(DataSink &sink);

    /**
     * Determines the output format for the Mixer.
     */
    virtual int getFormat();

    /**
     * Defines the output format for the Mixer.
     * @param format The fomrat to output. Valid values are:
     * DATASTREAM_FORMAT_16BIT_UNSIGNED
     * DATASTREAM_FORMAT_16BIT_SIGNED
     */
    virtual int setFormat(int format);


    /**
     * Determines the overall master volume of the mixer.
     * @return the voluem level in the range 0..1023.
     */
    int getVolume();

    /**
     * Defines the overall master volume of the mixer.
     *
     * @param volume The master volume of this mixer, in the range 0..1023.
     * @return DEVICE_OK on success.
     */
    int setVolume(int volume);

    /**
     * Change the sample range used by this Synthesizer,
     * @param sampleRange (quantization levels) the difference between the maximum and minimum sample level on the output channel
     * @return DEVICE_OK on success.
     */
    int setSampleRange(uint16_t sampleRange);

    /**
     * Change the sample rate output of this Mixer.
     * @param sampleRate The new sample rate (samples per second) of the mixer output
     * @return DEVICE_OK on success.
     */
    int setSampleRate(int sampleRate);

    /**
     * Determine the sample range used by this Synthesizer,
     * @param sampleRange The new sample range, that defines by the maximum sample value that will be output by the mixer.
     * @return the sample range (quantization levels) the difference between the maximum and minimum sample level on the output channel.
     */
    int getSampleRange();

    /**
     * Determine the sample rate output of this Mixer.
     * @return The sample rate (samples per second) of the mixer output.
     */
    int getSampleRate();

    /**
     * Defines an optional bit mask to logical OR with each sample.
     * Useful if the downstream component encodes control data within its samples.
     *
     * @param mask The bitmask to to apply to each sample.
     * @return DEVICE_OK on success.
     */
    int setOrMask(uint32_t mask);

    private:
    void configureChannel(MixerChannel *c);
};

} // namespace codal

#endif
