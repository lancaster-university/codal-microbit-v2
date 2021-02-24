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
#include "MicroBit.h"
#include "CodalDmesg.h"
#include "NRF52PWM.h"
#include "Synthesizer.h"
#include "SoundExpressions.h"
#include "SoundEmojiSynthesizer.h"
#include "StreamSplitter.h"

using namespace codal;

MicroBitAudio* MicroBitAudio::instance = NULL;

/**
  * Default Constructor.
  */
MicroBitAudio::MicroBitAudio(NRF52Pin &pin, NRF52Pin &speaker, NRF52ADC &adc, NRF52Pin &microphone, NRF52Pin &runmic):
    speakerEnabled(true),
    pinEnabled(true),
    pin(pin), 
    speaker(speaker),
    synth(DEVICE_ID_SOUND_EMOJI_SYNTHESIZER_0),
    soundExpressionChannel(NULL),
    pwm(NULL),
    adc(adc),
    microphone(microphone),
    runmic(runmic),
    soundExpressions(synth),
    virtualOutputPin(mixer)
{
    // If we are the first instance created, schedule it for on demand activation
    if (MicroBitAudio::instance == NULL)
        MicroBitAudio::instance = this;

    synth.allowEmptyBuffers(true);

    if (mic == NULL){
        mic = adc.getChannel(microphone, false);
        mic->setGain(7,0);
    }

    //Initilise stream normalizer
    if (processor == NULL)
        processor = new StreamNormalizer(mic->output, 1.0f, true, DATASTREAM_FORMAT_UNKNOWN, 10);

    //Initilise stream splitter
    if (splitter == NULL)
        splitter = new StreamSplitter(processor->output);

    //Initilise SPL level detector and attach to splitter
    if (level == NULL)  
        level = new LevelDetector(*splitter, 150, 75);

    //Initilise FFT and attach to splitter
    // if (level == NULL)  
    //     fft = new AudioProcessor(*splitter);

    // Register listeners with splitter events to activate and deactivate mic channel when needed
    if(EventModel::defaultEventBus){
        EventModel::defaultEventBus->listen(DEVICE_ID_SPLITTER, SPLITTER_ACTIVATE_CHANNEL, this, &MicroBitAudio::activateMicChannel,MESSAGE_BUS_LISTENER_IMMEDIATE);
        EventModel::defaultEventBus->listen(DEVICE_ID_SPLITTER, SPLITTER_DEACTIVATE_CHANNEL, this, &MicroBitAudio::deactivateMicChannel,MESSAGE_BUS_LISTENER_IMMEDIATE);
    }
}

/**
 * Activate Mic Input Channel
 */
void MicroBitAudio::activateMicChannel(MicroBitEvent){
    runmic.setDigitalValue(1);
    runmic.setHighDrive(true);
    adc.activateChannel(mic);
}

/**
 * Dectivate Mic Input Channel
 */
void MicroBitAudio::deactivateMicChannel(MicroBitEvent){
    runmic.setDigitalValue(0);
    runmic.setHighDrive(false);
    adc.releaseChannel(microphone);
}

/**
 * post-constructor initialisation method
 */
int MicroBitAudio::enable()
{ 
    if (pwm == NULL)
    {
        pwm = new NRF52PWM(NRF_PWM1, mixer, 44100);
        pwm->setDecoderMode(PWM_DECODER_LOAD_Common);

        mixer.setSampleRange(pwm->getSampleRange());
        mixer.setOrMask(0x8000);

        setSpeakerEnabled(speakerEnabled);
        setPinEnabled(pinEnabled);

        soundExpressionChannel = mixer.addChannel(synth);
    }
    return DEVICE_OK;
}

/**
 * Demand request from a component to enable the default instance of this audio pipeline
 */
void MicroBitAudio::requestActivation()
{
    if (MicroBitAudio::instance)
        MicroBitAudio::instance->enable();
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
 * Define which pin on the edge connector is used for audio.
 * @param pin The pin to use for auxiliary audio.
 */
void MicroBitAudio::setPin(NRF52Pin &pin)
{
    bool wasEnabled = pinEnabled;

    setPinEnabled(false);
    this->pin = pin;
    setPinEnabled(wasEnabled);
}

/**
 * Define if audio is enabled on the edge connector pin.
 * @param on New value.
 */
void MicroBitAudio::setPinEnabled(bool on)
{
    pinEnabled = on;

    if (pwm)
    {
        if (on)
            pwm->connectPin(pin, 0);
        else
            pwm->disconnectPin(pin);
    }
}

/**
 * Query whether the audio is enabled on the edge connector.
 * @return true if enabled, false otherwise.
 */
bool MicroBitAudio::isPinEnabled()
{
    return this->pinEnabled;
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