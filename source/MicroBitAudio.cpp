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
#include "NRF52PWM.h"
#include "Synthesizer.h"
#include "SoundExpressions.h"
#include "SoundEmojiSynthesizer.h"
#include "StreamSplitter.h"

using namespace codal;

MicroBitAudio* MicroBitAudio::instance = NULL;

MicroBitAudio::MicroBitAudio(NRF52Pin &pin, NRF52Pin &speaker, NRF52ADC &adc, NRF52Pin &microphone, NRF52Pin &runmic):
    micEnabled(false),
    micSleepState(false),
    speakerEnabled(true),
    pinEnabled(true),
    pin(&pin),
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

    // Request a periodic callback
    status |= DEVICE_COMPONENT_STATUS_SYSTEM_TICK;

    synth.allowEmptyBuffers(true);

    mic = adc.getChannel(microphone, false);
    adc.setSamplePeriod( 1e6 / 11000 );
    mic->setGain(7, 0);

    activateMic();

    // Implementers note: The order that the pipeline comes up here is quite sensitive. If we connect up to splitters after starting to
    // listen for events from them, we'll see channel startup events (which are valid!) that we don't want. So roughly always follow
    // the following schema:
    //
    // 1. Create the splitter
    // 2. Attach any stages to the splitter
    // 3. Start listening for splitter events
    //
    // This prevents any cases where the pipeline stages cause a connect() message to be emitted, which then auto-activates the mic.

    //Initilise input splitter
    rawSplitter = new StreamSplitter(mic->output);

    //Initilise stream normalizer
    processor = new StreamNormalizer(*rawSplitter->createChannel(), 0.08f, true, DATASTREAM_FORMAT_8BIT_SIGNED, 10);

    //Initilise level detector SPL and attach to splitter
    //levelSPL = new LevelDetectorSPL(*rawSplitter->createChannel(), 85.0, 65.0, 16.0, 0, DEVICE_ID_MICROPHONE, false);
    levelSPL = new LevelDetectorSPL(*rawSplitter->createChannel(), 85.0, 65.0, 16.0, 52.0, DEVICE_ID_SYSTEM_LEVEL_DETECTOR);

    //Initilise stream splitter
    splitter = new StreamSplitter(processor->output, DEVICE_ID_SPLITTER);
}

void MicroBitAudio::periodicCallback()
{
    if (mic->isEnabled() && !micEnabled)
    {
        DMESG("MicroBitAudio::periodicCallback: activateMic()...");
        activateMic();
    }

    if (!mic->isEnabled() && micEnabled)
    {
        DMESG("MicroBitAudio::periodicCallback: deactivateMic()...");
        deactivateMic();
    }
}

void MicroBitAudio::activateMic(){
    runmic.setDigitalValue(1);
    runmic.setHighDrive(true);
    adc.activateChannel(mic);
    this->micEnabled = true;
}

void MicroBitAudio::deactivateMic(){
    this->micEnabled = false;
    runmic.setDigitalValue(0);
    runmic.setHighDrive(false);
}

void MicroBitAudio::setMicrophoneGain(int gain){
    processor->setGain(gain/100);
}

int MicroBitAudio::enable()
{ 
    if (pwm == NULL)
    {
        pwm = new NRF52PWM( NRF_PWM1, mixer, 44100 );
        pwm->setDecoderMode( PWM_DECODER_LOAD_Common );

        mixer.setSampleRange( pwm->getSampleRange() );
        mixer.setOrMask( 0x8000 );

        setSpeakerEnabled( speakerEnabled );
        setPinEnabled( pinEnabled );

        if ( soundExpressionChannel == NULL )
            soundExpressionChannel = mixer.addChannel(synth);
    }
    return DEVICE_OK;
}

int MicroBitAudio::disable()
{
    setSpeakerEnabled( false );
    setPinEnabled( false );

    pwm->disable();

    return DEVICE_OK;
}

void MicroBitAudio::requestActivation()
{
    if (MicroBitAudio::instance)
        MicroBitAudio::instance->enable();
}

int MicroBitAudio::setVolume(int volume)
{
    if (volume < 0 || volume > 255)
        return DEVICE_INVALID_PARAMETER;

    mixer.setVolume(volume*4);

    return DEVICE_OK;
}

int MicroBitAudio::getVolume() {
    return mixer.getVolume() / 4;
}

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

bool MicroBitAudio::isSpeakerEnabled() {
    return speakerEnabled;
}

bool MicroBitAudio::isMicrophoneEnabled() {
    return micEnabled;
}

void MicroBitAudio::setPin(NRF52Pin &pin)
{
    bool wasEnabled = pinEnabled;

    setPinEnabled(false);
    this->pin = &pin;
    setPinEnabled(wasEnabled);
}

void MicroBitAudio::setPinEnabled(bool on)
{
    pinEnabled = on;

    if (pwm)
    {
        if (on)
            pwm->connectPin(*pin, 0);
        else
            pwm->disconnectPin(*pin);
    }
}

bool MicroBitAudio::isPinEnabled()
{
    return this->pinEnabled;
}

MicroBitAudio::~MicroBitAudio()
{
    if (pwm)
    {
        pwm->disconnectPin(speaker);
        pwm->disconnectPin(*pin);
    }
}

int MicroBitAudio::setSleep(bool doSleep)
{
    if (doSleep)
    {
      if (pwm)
      {
          status |= MICROBIT_AUDIO_STATUS_DEEPSLEEP;
          NVIC_DisableIRQ(PWM1_IRQn);
          pwm->disable();
          pwm->disconnectPin(speaker);
          pwm->disconnectPin(*pin);
          delete pwm;
          pwm = NULL;
      }
      this->micSleepState = this->micEnabled;
      deactivateMic();
    }
    else
    {
      if ( status & MICROBIT_AUDIO_STATUS_DEEPSLEEP)
      { 
          status &= ~MICROBIT_AUDIO_STATUS_DEEPSLEEP;
          enable();
      }
      if( this->micSleepState )
        activateMic();
    }
   
    return DEVICE_OK;
}

bool MicroBitAudio::isPlaying()
{
    uint32_t t = system_timer_current_time_us();
    uint32_t start = mixer.getSilenceStartTime();
    uint32_t end = mixer.getSilenceEndTime();

    return !(
        (start == 0 && end == 0) || (
            (start && t >= (start + CONFIG_AUDIO_MIXER_OUTPUT_LATENCY_US)) &&
            (end == 0 || t < (end + CONFIG_AUDIO_MIXER_OUTPUT_LATENCY_US - 100))
        )
    );
}
