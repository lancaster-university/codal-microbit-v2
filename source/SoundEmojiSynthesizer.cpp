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

#include "SoundEmojiSynthesizer.h"
#include "Synthesizer.h"
#include "CodalFiber.h"
#include "CodalUtil.h"
#include "ErrorNo.h"

using namespace codal;

static const KeyValueTableEntry soundEmojiTonePrintData[] = {
    {0, (const uint32_t) Synthesizer::SineTone},
    {1, (const uint32_t) Synthesizer::SawtoothTone},
    {2, (const uint32_t) Synthesizer::TriangleTone},
    {3, (const uint32_t) Synthesizer::SquareWaveTone},
    {4, (const uint32_t) Synthesizer::NoiseTone},
    {5, (const uint32_t) Synthesizer::SquareWaveToneExt}
};
CREATE_KEY_VALUE_TABLE(soundEmojiTonePrint, soundEmojiTonePrintData);

static const KeyValueTableEntry soundEmojiInterpolatorsData[] = {
    {0, (const uint32_t) SoundEmojiSynthesizer::noInterpolation},
    {1, (const uint32_t) SoundEmojiSynthesizer::linearInterpolation},
    {2, (const uint32_t) SoundEmojiSynthesizer::curveInterpolation},
    {3, (const uint32_t) SoundEmojiSynthesizer::slowVibratoInterpolation},
    {4, (const uint32_t) SoundEmojiSynthesizer::warbleInterpolation},
    {5, (const uint32_t) SoundEmojiSynthesizer::exponentialRisingInterpolation},
    {6, (const uint32_t) SoundEmojiSynthesizer::exponentialFallingInterpolation},
    {7, (const uint32_t) SoundEmojiSynthesizer::vibratoInterpolation},
    {8, (const uint32_t) SoundEmojiSynthesizer::majAppregrioAscendInterpolation},
    {9, (const uint32_t) SoundEmojiSynthesizer::majAppregrioDescendInterpolation},
    {10, (const uint32_t) SoundEmojiSynthesizer::minAppregrioAscendInterpolation},
    {11, (const uint32_t) SoundEmojiSynthesizer::minAppregrioDescendInterpolation},
    {12, (const uint32_t) SoundEmojiSynthesizer::dimAppregrioAscendInterpolation},
    {13, (const uint32_t) SoundEmojiSynthesizer::dimAppregrioDescendInterpolation},
    {14, (const uint32_t) SoundEmojiSynthesizer::chromaticAppregrioAscendInterpolation},
    {15, (const uint32_t) SoundEmojiSynthesizer::chromaticAppregrioDescendInterpolation},
    {16, (const uint32_t) SoundEmojiSynthesizer::toneAppregrioAscendInterpolation},
    {17, (const uint32_t) SoundEmojiSynthesizer::toneAppregrioDescendInterpolation},
    {20, (const uint32_t) SoundEmojiSynthesizer::logarithmicInterpolation}

};
CREATE_KEY_VALUE_TABLE(soundEmojiInterpolators, soundEmojiInterpolatorsData);

static const KeyValueTableEntry soundEmojiStepsData[] = {
    {500, 9},
    {1150, 15},
    {1800, 20},
    {2070, 50},
    {2100, 90}
};
CREATE_KEY_VALUE_TABLE(soundEmojiSteps, soundEmojiStepsData);

/**
 * Interpolation functions
 */
float SoundEmojiSynthesizer::noInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount){
    return startFrequency;
}

// Linear interpolate function
float SoundEmojiSynthesizer::linearInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount){
    float interval = (endFrequency - startFrequency) / stepCount;
    return startFrequency+interval*stepNumber;
}
float SoundEmojiSynthesizer::logarithmicInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount){
    return startFrequency+(log10(stepNumber)*(endFrequency-startFrequency)/1.95);
}

// Curve interpolate function
float SoundEmojiSynthesizer::curveInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount){
    return (sin(stepNumber*3.12159f/180.0f)*(endFrequency-startFrequency)+startFrequency);
}

// Cosine interpolate function
float SoundEmojiSynthesizer::slowVibratoInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount){
    return sin(stepNumber/10)*endFrequency+startFrequency;
}

//warble function
float SoundEmojiSynthesizer::warbleInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount){
    return (sin(stepNumber)*(endFrequency-startFrequency)+startFrequency);
}

// Vibrato function
float SoundEmojiSynthesizer::vibratoInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount){
    return startFrequency + sin(stepNumber)*endFrequency;
}

// Exponential rising function
float SoundEmojiSynthesizer::exponentialRisingInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount){
    return startFrequency + sin(0.01745329f*stepNumber)*endFrequency;
}

// Exponential falling function
float SoundEmojiSynthesizer::exponentialFallingInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount){
    return startFrequency + cos(0.01745329f*stepNumber)*endFrequency;
}

float SoundEmojiSynthesizer::majAppregrioAscendInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount){
    //TODO: implement
    return startFrequency;
}
float SoundEmojiSynthesizer::majAppregrioDescendInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount){
    //TODO: implement
    return startFrequency;
}
float SoundEmojiSynthesizer::minAppregrioAscendInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount){
    //TODO: implement
    return startFrequency;
}
float SoundEmojiSynthesizer::minAppregrioDescendInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount){
    //TODO: implement
    return startFrequency;
}
float SoundEmojiSynthesizer::dimAppregrioAscendInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount){
    //TODO: implement
    return startFrequency;
}
float SoundEmojiSynthesizer::dimAppregrioDescendInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount){
    //TODO: implement
    return startFrequency;
}
float SoundEmojiSynthesizer::chromaticAppregrioAscendInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount){
    //TODO: implement
    return startFrequency;
}
float SoundEmojiSynthesizer::chromaticAppregrioDescendInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount){
    //TODO: implement
    return startFrequency;
}
float SoundEmojiSynthesizer::toneAppregrioAscendInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount){
    //TODO: implement
    return startFrequency;
}
float SoundEmojiSynthesizer::toneAppregrioDescendInterpolation(float startFrequency, int endFrequency, int stepNumber, int stepCount){
    //TODO: implement
    return startFrequency;
}




/**
  * Class definition for a Synthesizer.
  * A Synthesizer generates a tone waveform based on a number of overlapping waveforms.
  */
SoundEmojiSynthesizer::SoundEmojiSynthesizer(int sampleRate) : buffer(EMOJI_SYNTHESIZER_BUFFER_SIZE)
{
    this->downStream = NULL;
    this->bufferSize = EMOJI_SYNTHESIZER_BUFFER_SIZE;
    this->bytesWritten = 0;
    this->position = 0.0f;
    this->effect = NULL;

    this->samplesToWrite = 0;
    this->samplesWritten = 0;

    setSampleRate(sampleRate);
    setSampleRange(1023);
}

/**
 * Destructor.
 * Removes all resources held by the instance.
 */
SoundEmojiSynthesizer::~SoundEmojiSynthesizer()
{
}

/**
 * Define a downstream component for data stream.
 *
 * @sink The component that data will be delivered to, when it is availiable
 */
void SoundEmojiSynthesizer::connect(DataSink &sink)
{
	this->downStream = &sink;
}

/**
 *  Determine the data format of the buffers streamed out of this component.
 */
int SoundEmojiSynthesizer::getFormat()
{
    return DATASTREAM_FORMAT_16BIT_UNSIGNED;
}

/**
* Define the size of the audio buffer to hold. The larger the buffer, the lower the CPU overhead, but the longer the delay.
* @param size The new bufer size to use, in bytes.
* @return DEVICE_OK on success, or DEVICE_INVALID_PARAMETER
*/
int SoundEmojiSynthesizer::setBufferSize(int size)
{
    if (bufferSize <= 0)
        return DEVICE_INVALID_PARAMETER;

    this->bufferSize = size;
    return DEVICE_OK;
}

/**
* Schedules playout of the given sound effect.
* @param sound A buffer containing an array of one or more SoundEffects.
* @return DEVICE_OK on success, or DEVICE_INVALID_PARAMETER
*/
int SoundEmojiSynthesizer::play(ManagedBuffer sound)
{
    // Validate inputs
    if (sound.length() < (int) sizeof(SoundEffect))
        return DEVICE_INVALID_PARAMETER;

    // If a playout is already in progress, block until it has been scheduled.
    lock.wait();

    // Store the requested sequence of sound effects.
    effectBuffer = sound;

    // Scheduled this sound effect for playout. 
    // Generation will start the next time a pull() operation is called from downstream.
    nextSoundEffect();

    // Perform on demand activiation if this is the first time this compoennt has been used.
    // Simply issue a pull request to start the process.
    if (!(status & EMOJI_SYNTHESIZER_STATUS_ACTIVE))
    {
        status |= EMOJI_SYNTHESIZER_STATUS_ACTIVE;
        downStream->pullRequest();
    }

    return DEVICE_OK;
}

/**
 * Schedules the next sound effect as defined in the effectBuffer, if available.
 */
void SoundEmojiSynthesizer::nextSoundEffect()
{
    // If a sequence of SoundEffects are being played, attempt to move on to the next.
    // If not, select the first in the buffer.
    if (effect)
        effect++;
    else
        effect = (SoundEffect *) &effectBuffer[0];
    
    // Validate that we have a valid sound effect. If not, record that we have nothing to play.
    if ((uint8_t *)effect >= &effectBuffer[0] + effectBuffer.length())
    {
        effect = NULL;
        samplesToWrite = 0;
        samplesPerStep = 0;
        return;
    }

    // We have a valid buffer. Set up our synthesizer to the requested parameters.
    tonePrint = (TonePrintFunction)soundEmojiTonePrint.get(effect->tone);
    tonePrintArg = NULL;

    interpolator = (ToneInterpolator)soundEmojiInterpolators.get(effect->effect);

    samplesToWrite = determineSampleCount(effect->duration);
    samplesWritten = 0;
    position = 0.0f;

    // Define the number of steps in the effect. This seems fairly arbitrary, but maintaining here for compatibility...
    // TODO: Simply replace all this with a configurable parameter???
    step = 0;
    steps = soundEmojiSteps.find(effect->duration)->value;

    // major/minor effects have four steps.
    if (effect->effect >=8 && effect->effect < 12)
        steps = 4;

    // diminished effects have five steps.
    if (effect->effect >=12 && effect->effect < 14)
        steps = 5;

    // chromatic effects have 13 steps.
    if (effect->effect >=14 && effect->effect < 15)
        steps = 13;

    // wholetone effects have 7 steps.
    if (effect->effect >=16 && effect->effect < 18)
        steps = 7;

    samplesPerStep = (float) samplesToWrite / (float) steps;

    frequency = effect->startFrequency;
    volume = effect->startvolume;
    volumeDeltaPerStep = (effect->endVolume - effect->startvolume) / steps;
}

/**
 * Provide the next available ManagedBuffer to our downstream caller, if available.
 */
ManagedBuffer SoundEmojiSynthesizer::pull()
{
    // Generate a buffer on demand. This is likely to be in interrupt context, so
    // the receiver driven nature reduces glitching on audio output.
    bool done = false;
    uint16_t sample;

    buffer = ManagedBuffer(bufferSize);

    while (!done)
    {
        if (samplesWritten == samplesToWrite)
        {
            bool renderComplete = samplesWritten > 0;
            
            nextSoundEffect();

            // If we have just completed active playout of an effect, and there are no more effects scheduled, 
            // unblock any waiting fibers.
            if (samplesToWrite == 0)
            {
                if (renderComplete)
                    lock.notify();

                break;
            }
        }
        
        // Generate some samples with the current effect parameters.
        while(step < steps)
        {
            float skip = ((EMOJI_SYNTHESIZER_TONE_WIDTH_F * frequency) / sampleRate);
            int stepEndPosition = (int) (samplesPerStep * step);

            while (samplesWritten < stepEndPosition)
            {
                // Stop processing when we've filled the requested buffer
                if (bytesWritten == bufferSize)
                {
                    bytesWritten = 0;
                    downStream->pullRequest();
                    return buffer;
                }

                // Synthesize a sample
                float s = tonePrint((void *)effect->effectParameter1, (int) position);

                // Apply volume envelope. 
                // n.b. would like to avoid this integer/float recast here!
                sample = (uint16_t) (s * volume);

                // Apply Or mask, if specified.
                sample |= orMask;

                // Move on our pointers.
                samplesWritten++;
                bytesWritten += 2;
                position += skip;

                // Keep our toneprint pointer in range
                while(position > EMOJI_SYNTHESIZER_TONE_WIDTH_F)
                    position -= EMOJI_SYNTHESIZER_TONE_WIDTH_F;
            }

            step++;
            if (step < steps)
            {
                frequency = interpolator(effect->startvolume, effect->endFrequency, step, steps);
                volume = (sampleRange * (effect->startvolume + step * volumeDeltaPerStep)) / 1024.0;
            }
        }
    }

    // Issue a Pull Request so that we are always receiver driven, and we're done.
    downStream->pullRequest();
    return buffer;
}

/**
 * Determine the sample rate currently in use by this Synthesizer.
 * @return the current sample rate, in Hz.
 */
int SoundEmojiSynthesizer::getSampleRate()
{
    return sampleRate;
}

/**
 * Change the sample rate used by this Synthesizer,
 * @param frequency The new sample rate, in Hz.
 * @return DEVICE_OK on success.
 */
int SoundEmojiSynthesizer::setSampleRate(int sampleRate)
{
    this->sampleRate = sampleRate;
    return DEVICE_OK;
}

/**
 * Determine the number of samples required for the given playout time.
 *
 * @param playoutTimeUs The playout time (in microseconds)
 * @return The number of samples required to play for the given amount of time
 * (at the currently defined sample rate)
 */
int SoundEmojiSynthesizer::determineSampleCount(int playoutTimeUs)
{
    float seconds = (float)playoutTimeUs / 1000000.0f;
    return (int) ((float)sampleRate * seconds);
}

/**
 * Determines the sample range used by this Synthesizer,
 * @return the maximum sample value that will be output by the Synthesizer.
 */
uint16_t SoundEmojiSynthesizer::getSampleRange()
{
    return sampleRange;
}

/**
 * Change the sample range used by this Synthesizer,
 * @param sampleRange The new sample range, that defines by the maximum sample value that will be output by the Synthesizer.
 * @return DEVICE_OK on success.
 */
int SoundEmojiSynthesizer::setSampleRange(uint16_t sampleRange)
{
    this->sampleRange = sampleRange;
    return DEVICE_OK;
}

/**
 * Defines an optional bit mask to logical OR with each sample.
 * Useful if the downstream component encodes control data within its samples.
 *
 * @param mask The bitmask to to apply to each sample.
 * @return DEVICE_OK on success.
 */
int SoundEmojiSynthesizer::setOrMask(uint16_t mask)
{
    orMask = mask;
    return DEVICE_OK;
}

