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
#define CONFIG_MIXER_INTERNAL_RANGE 1024.0f
#endif

namespace codal
{

class MixerChannel : public DataSink
{
private:
    MixerChannel    *next;
    DataSource      *stream;
    ManagedBuffer   buffer;
    int             pullRequests;
    uint8_t         *in;
    uint8_t         *end;

    float           range;
    float           offset;
    float           gain;

    float           volume;
    int             format;
    int             bytesPerSample;    

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
    int             outputFormat;
    int             bytesPerSampleOut;
    float           volume;
    uint32_t        orMask;

public:
    /**
     * Default Constructor.
     * Creates an empty Mixer.
     * 
     * @param format The format the mixer will output.
     */
    Mixer2(int format = DATASTREAM_FORMAT_16BIT_UNSIGNED);

    /**
     * Destructor.
     * Removes all resources held by the instance.
     */
    ~Mixer2();

    MixerChannel *addChannel(DataSource &stream, int range = 1023);
    void configureChannel(MixerChannel *c);

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
     * @param sampleRange The new sample range, that defines by the maximum sample value that will be output by the mixer.
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
    int setOrMask(uint32_t mask);

};

} // namespace codal

#endif
