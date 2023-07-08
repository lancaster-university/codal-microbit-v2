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

#define DEVICE_ID_MIXER 3030

#define DEVICE_MIXER_EVT_SILENCE 1
#define DEVICE_MIXER_EVT_SOUND   2


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

    /**
     * @brief Changes the volume between 0 and CONFIG_MIXER_INTERNAL_RANGE
     * 
     * @param volume A floating point value between 0 and CONFIG_MIXER_INTERNAL_RANGE
     */
    void setVolume( float volume ) { this->volume = volume; }

    /**
     * @brief Gets the volume for this channel (not the overall mixer volume)
     * 
     * @return float A floating point value between 0 and CONFIG_MIXER_INTERNAL_RANGE
     */
    float getVolume() { return this->volume; }

    /**
     * @brief Sets the sample rate of this channel.
     * 
     * @param rate 
     */
    void setSampleRate( float rate ) {
        this->rate = rate;
        this->skip = 0.0f;
        if( this->rate == DATASTREAM_SAMPLE_RATE_UNKNOWN )
            this->rate = stream->getSampleRate();
    }

    /**
     * @brief Gets the sample rate for this channel.
     * 
     * @note This may be set to DATASTREAM_SAMPLE_RATE_UNKNOWN if the upstream is unable to determine a correct rate
     * 
     * @return float 
     */
    float getSampleRate() { return this->rate; }
};

class Mixer2 : public DataSource
{
    MixerChannel    *channels;
    DataSink        *downStream;
    float           mix[CONFIG_MIXER_BUFFER_SIZE];
    float           outputRange;
    float           outputRate;
    int             outputFormat;
    int             bytesPerSampleOut;
    float           volume;
    uint32_t        orMask;
    float           silenceLevel;
    bool            silent;
    CODAL_TIMESTAMP silenceStartTime;
    CODAL_TIMESTAMP silenceEndTime;

public:
    /**
     * Constructor.
     * Creates an empty Mixer.
     * @param sampleRate (samples per second) of the mixer output
     * @param sampleRange (quantization levels) the difference between the maximum and minimum sample level on the output channel
     * @param format The format the mixer will output (DATASTREAM_FORMAT_16BIT_UNSIGNED or DATASTREAM_FORMAT_16BIT_SIGNED)
     */
    Mixer2(float sampleRate = CONFIG_MIXER_DEFAULT_SAMPLERATE, int sampleRange = CONFIG_MIXER_INTERNAL_RANGE, int format = DATASTREAM_FORMAT_16BIT_UNSIGNED);

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
    MixerChannel *addChannel(DataSource &stream, float sampleRate = 0, int sampleRange = CONFIG_MIXER_INTERNAL_RANGE);

    /**
     * Removes a channel from the mixer
     * 
     * @param channel The channel pointer to remove
     * @return int DEVICE_OK if completed successfully
     */
    int removeChannel( MixerChannel * channel );

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
     * Determines if this source is connected to a downstream component
     * 
     * @return true If a downstream is connected
     * @return false If a downstream is not connected
     */
    bool isConnected();

    /**
     * Determines the output format for the Mixer.
     */
    virtual int getFormat();

    /**
     * Defines the output format for the Mixer.
     * @param format The fomrat to output. Valid values are:
     * DATASTREAM_FORMAT_16BIT_UNSIGNED
     * DATASTREAM_FORMAT_16BIT_SIGNED
     * DATASTREAM_FORMAT_8BIT_UNSIGNED
     * DATASTREAM_FORMAT_8BIT_SIGNED
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
    int setSampleRate(float sampleRate);

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
    float getSampleRate();

    /**
     * Defines an optional bit mask to logical OR with each sample.
     * Useful if the downstream component encodes control data within its samples.
     *
     * @param mask The bitmask to to apply to each sample.
     * @return DEVICE_OK on success.
     */
    int setOrMask(uint32_t mask);

    /**
     * Defines an optional sample level to generate during periods of silence.
     * If undefined, the mixer defaults to a normalized level of 0.0f...1024.0f
     *
     * @param level The output level to apply for silence, in the range 0.0f...1024.0f
     * @return DEVICE_OK on success or DEVICE_INVALID_PARAMETER.
     */
    int setSilenceLevel(float level);
    
    /**
     * Determines if the mixer is silent
     * @return true if the mixer is silent
     */
    bool isSilent();

    /**
     * Determines the time at which the mixer has most recently been generating silence 
     *
     * @return the system time in microseconds at which the mixer has been continuously 
     * producing silence on its output, or zero if the mixer is not producing silence.
     */
    CODAL_TIMESTAMP getSilenceStartTime();

    /**
     * Determines the time at which the mixer stopped continuously generating silence 
     *
     * @return the system time in microseconds at which the mixer stopped continuously 
     * producing silence on its output, or zero if the mixer is not producing silence.
     */
    CODAL_TIMESTAMP getSilenceEndTime();


    private:
    void configureChannel(MixerChannel *c);
};

} // namespace codal

#endif
