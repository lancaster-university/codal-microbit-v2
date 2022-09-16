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
#include "MicroBitAudio.h"

using namespace codal;

/**
  * Class definition for a Synthesizer.
  * A Synthesizer generates a tone waveform based on a number of overlapping waveforms.
  */
SoundEmojiSynthesizer::SoundEmojiSynthesizer(uint16_t id, int sampleRate) : CodalComponent(id, 0), buffer(EMOJI_SYNTHESIZER_BUFFER_SIZE), emptyBuffer(0)
{
    this->downStream = NULL;
    this->bufferSize = EMOJI_SYNTHESIZER_BUFFER_SIZE;
    this->position = 0.0f;
    this->effect = NULL;

    this->samplesToWrite = 0;
    this->samplesWritten = 0;

    setSampleRate(sampleRate);
    setSampleRange(1023);
    setOrMask(0);
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
    // Enable audio pipeline if needed.
    MicroBitAudio::requestActivation();

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

void SoundEmojiSynthesizer::stop() {
    if (effect)
        status |= EMOJI_SYNTHESIZER_STATUS_STOPPING;
}

/**
 * Schedules the next sound effect as defined in the effectBuffer, if available.
 * @return true if we've just completed a buffer of effects, false otherwise.
 */
bool SoundEmojiSynthesizer::nextSoundEffect()
{
    const bool hadEffect = effect != NULL;
    if (status & EMOJI_SYNTHESIZER_STATUS_STOPPING)
    {
        effect = NULL;
        effectBuffer = emptyBuffer;
    }

    // If a sequence of SoundEffects are being played, attempt to move on to the next.
    // If not, select the first in the buffer.
    if (effect)
        effect++;
    else
        effect = (SoundEffect *) &effectBuffer[0];
    
    // Validate that we have a valid sound effect. If not, record that we have nothing to play.
    if ((uint8_t *)effect >= &effectBuffer[0] + effectBuffer.length())
    {
        // if we have an effect with a negative duration, reset the buffer (unless there is an update pending)
        effect = (SoundEffect *) &effectBuffer[0];

        if (effect->duration >= 0 || lock.getWaitCount() > 0)
        {
            effect = NULL;
            effectBuffer = emptyBuffer;
            samplesWritten = 0;
            samplesToWrite = 0;
            position = 0.0f;
            return hadEffect;
        }
    }

    // We have a valid buffer. Set up our synthesizer to the requested parameters.
    samplesToWrite = determineSampleCount(effect->duration);
    frequency = effect->frequency;
    volume = effect->volume;
    samplesWritten = 0;

    // validate and initialise per effect rendering state.
    for (int i=0; i<EMOJI_SYNTHESIZER_TONE_EFFECTS; i++)
    {
        effect->effects[i].step = 0;
        effect->effects[i].steps = max(effect->effects[i].steps, 1);
        samplesPerStep[i] = (float) samplesToWrite / (float) effect->effects[i].steps;
    }
    return false;
}

/**
 * Provide the next available ManagedBuffer to our downstream caller, if available.
 */
ManagedBuffer SoundEmojiSynthesizer::pull()
{
    // Generate a buffer on demand. This is likely to be in interrupt context, so
    // the receiver driven nature reduces glitching on audio output.
    bool done = false;
    uint16_t *sample = NULL;
    uint16_t *bufferEnd;

    while (!done)
    {
        if (samplesWritten == samplesToWrite || status & EMOJI_SYNTHESIZER_STATUS_STOPPING)
        {
            bool renderComplete = nextSoundEffect();

            // If we have just completed active playout of an effect, and there are no more effects scheduled, 
            // unblock any fibers that may be waiting to play a sound effect.
            if (samplesToWrite == 0 || status & EMOJI_SYNTHESIZER_STATUS_STOPPING)
            {
                done = true;
                if (renderComplete || status & EMOJI_SYNTHESIZER_STATUS_STOPPING)
                {
                    // Wait for a hanful of milliseconds while we finish playing the last buffer we sent out ...
                    //fiber_sleep( buffer.length() / sampleRate ); // Removed due to causing issues for Python

                    // ... THEN flip out status and fire the event
                    status &= ~EMOJI_SYNTHESIZER_STATUS_STOPPING;
                    Event(id, DEVICE_SOUND_EMOJI_SYNTHESIZER_EVT_DONE);
                    lock.notify();
                }
            }
        }
        
        // If we have something to do, ensure our buffers are created.
        // We defer creation to avoid unecessary heap allocation when genertaing silence.
        if (((samplesWritten < samplesToWrite) || !(status & EMOJI_SYNTHESIZER_STATUS_OUTPUT_SILENCE_AS_EMPTY)) && sample == NULL)
        {
            buffer = ManagedBuffer(bufferSize);
            sample = (uint16_t *) &buffer[0];
            bufferEnd = (uint16_t *) (&buffer[0] + buffer.length());
        }

        // Generate some samples with the current effect parameters.
        while(samplesWritten < samplesToWrite)
        {
            float skip = ((EMOJI_SYNTHESIZER_TONE_WIDTH_F * frequency) / sampleRate);
            float gain = (sampleRange * volume) / 1024.0f;
            float offset = 512.0f - (512.0f * gain);

            int effectStepEnd[EMOJI_SYNTHESIZER_TONE_EFFECTS];

            for (int i = 0; i < EMOJI_SYNTHESIZER_TONE_EFFECTS; i++)
            {
                effectStepEnd[i] = (int) (samplesPerStep[i] * (effect->effects[i].step));
                if (effect->effects[i].step == effect->effects[i].steps - 1)
                    effectStepEnd[i] = samplesToWrite;
            }
                
            int stepEndPosition = effectStepEnd[0];
            for (int i = 1; i < EMOJI_SYNTHESIZER_TONE_EFFECTS; i++)
                stepEndPosition = min(stepEndPosition, effectStepEnd[i]);

            // Write samples until the end of the next effect-step
            while (samplesWritten < stepEndPosition)
            {
                // Stop processing when we've filled the requested buffer
                if (sample == bufferEnd)
                {
                    downStream->pullRequest();
                    return buffer;
                }

                // Synthesize a sample
                float s = effect->tone.tonePrint(effect->tone.parameter, (int) position);

                // Apply volume scaling and OR mask (if specified).
                *sample = ((uint16_t) ((s * gain) + offset)) | orMask;

                // Move on our pointers.
                sample++;
                samplesWritten++;
                position += skip;

                // Keep our toneprint pointer in range
                while(position > EMOJI_SYNTHESIZER_TONE_WIDTH_F)
                    position -= EMOJI_SYNTHESIZER_TONE_WIDTH_F;
            }

            // Invoke the effect function for any effects that are due.
            for (int i = 0; i < EMOJI_SYNTHESIZER_TONE_EFFECTS; i++)
            {
                if (samplesWritten == effectStepEnd[i])
                {
                    if (effect->effects[i].step < effect->effects[i].steps)
                    {
                        if (effect->effects[i].effect)
                            effect->effects[i].effect(this, &effect->effects[i]);

                        effect->effects[i].step++;
                    }
                }
            }
        }
    }

    // if we have no data to send, return an empty buffer (if requested)
    if (sample == NULL)
    {
        buffer = ManagedBuffer();
    }
    else
    {
        // Pad the output buffer with silence if necessary.
        uint16_t silence = ((uint16_t) (sampleRange *0.5f)) | orMask;
        while(sample < bufferEnd)
        {
            *sample = silence;
            sample++;
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
 * Change the sample rate used by this Synthesizer.
 * 
 * This will prohibit updates if the requested sampleRate is less than 1, to safeguard processing later.
 * 
 * @param frequency The new sample rate, in Hz.
 * @return DEVICE_OK on success or ErrorCode::DEVICE_INVALID_STATUS if the sample rate is out of range.
 */
int SoundEmojiSynthesizer::setSampleRate(int sampleRate)
{
    if( sampleRate < 1 )
        return ErrorCode::DEVICE_INVALID_PARAMETER;
    this->sampleRate = sampleRate;
    return DEVICE_OK;
}

/**
 * Determine the number of samples required for the given playout time.
 *
 * @param playoutTime The playout time (in milliseconds)
 * @return The number of samples required to play for the given amount of time
 * (at the currently defined sample rate)
 */
int SoundEmojiSynthesizer::determineSampleCount(float playoutTime)
{
    if (playoutTime < 0)
        playoutTime = -playoutTime;

    float seconds = playoutTime / 1000.0f;
    return (int) ((float)sampleRate * seconds);
}

/**
 * Determines the sample range used by this Synthesizer,
 * @return the maximum sample value that will be output by the Synthesizer.
 */
uint16_t SoundEmojiSynthesizer::getSampleRange()
{
    return (uint16_t)sampleRange;
}

/**
 * Change the sample range used by this Synthesizer,
 * @param sampleRange The new sample range, that defines by the maximum sample value that will be output by the Synthesizer.
 * @return DEVICE_OK on success.
 */
int SoundEmojiSynthesizer::setSampleRange(uint16_t sampleRange)
{
    this->sampleRange = (float)sampleRange;
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


/**
 * Define how silence is treated on this components output.
 * 
 * @param mode if set to true, this component will return empty buffers when there is no data to send.
 * Otherwise, a full buffer containing silence will be genersated.
 */
void SoundEmojiSynthesizer::allowEmptyBuffers(bool mode)
{
    if (mode)
        this->status |= EMOJI_SYNTHESIZER_STATUS_OUTPUT_SILENCE_AS_EMPTY;
    else
        this->status &= ~EMOJI_SYNTHESIZER_STATUS_OUTPUT_SILENCE_AS_EMPTY;

    
}