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

#include "MicroBitAudio.h"

#include "CodalDmesg.h"
#include "NRF52PWM.h"
#include "Synthesizer.h"
#include "SoundExpressions.h"
#include "SoundEmojiSynthesizer.h"

using namespace codal;

/**
  * Default Constructor.
  */
MicroBitAudio::MicroBitAudio(NRF52Pin &pin, NRF52Pin &speaker):
    speakerEnabled(false),
    pin(pin), 
    speaker(speaker),
    synth(44100),
    soundExpressionChannel(NULL),
    pwm(NULL),
    soundExpressions(synth),
    virtualOutputPin(mixer)
{
}

/**
 * post-constructor initialisation method
 */
int MicroBitAudio::init()
{
    pwm = new NRF52PWM(NRF_PWM1, mixer, 44100);
    pwm->setDecoderMode(PWM_DECODER_LOAD_Common);
    pwm->connectPin(pin, 0);

    mixer.setSampleRange(pwm->getSampleRange());
    mixer.setOrMask(0x8000);
    
    setSpeakerEnabled(true);

    soundExpressionChannel = mixer.addChannel(synth);

    return DEVICE_OK;
}

/**
 * Define the volume.
 * @param volume The new output volume, in the range 0..255
 * @return DEVICE_OK on success, or DEVICE_INVALID_PARAMETER
 */
int MicroBitAudio::setVolume(int volume)
{
    if (volume < 0 || volume > 255)
        return DEVICE_INVALID_PARAMETER;

    mixer.setVolume(volume*4);

    return DEVICE_OK;
}

/**
 * Get the current volume.
 * @return The output volume, in the range 0..255.
 */
int MicroBitAudio::getVolume() {
    return mixer.getVolume() / 4;
}

/**
 * Enable or disable use of the on-board speaker.
 * @param on New value.
 */
void MicroBitAudio::setSpeakerEnabled(bool on) {
    speakerEnabled = on;
    
    if (pwm)
    {
        if (on)
            pwm->connectPin(speaker, 1);
        else
            pwm->disconnectPin(speaker);
    }
    
}

/**
 * Query whether the speaker is enabled.
 * @return true if enabled, false otherwise.
 */
bool MicroBitAudio::isSpeakerEnabled() {
    return speakerEnabled;
}

/**
  * Destructor.
  *
  * Removes resources held by the instance.
  */
MicroBitAudio::~MicroBitAudio()
{
    if (pwm)
    {
        pwm->disconnectPin(speaker);
        pwm->disconnectPin(pin);
    }
}