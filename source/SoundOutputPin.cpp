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

/**
  * Class definition for Pin.
  *
  * Commonly represents an I/O pin on the edge connector.
  */
#include "SoundOutputPin.h"
#include "Synthesizer.h"
#include "CodalDmesg.h"
#include "MicroBitAudio.h"

using namespace codal;

/**
 * Constructor.
 * Create a DevicePin instance, generally used to represent a pin on the edge connector.
 *
 * @param id the unique EventModel id of this component.
 * @param mixer the mixer to use
 */
SoundOutputPin::SoundOutputPin(Mixer2 &mix, int id) : codal::Pin(id, 0, PIN_CAPABILITY_ANALOG), mixer(mix), synth(DEVICE_ID_SOUND_EMOJI_SYNTHESIZER_1)
{
    this->value = 512;
    this->periodUs = 0;
    this->fx = NULL;
    this->channel = NULL;
    synth.allowEmptyBuffers(true);
}


/**
  * Configures this IO pin as an analog/pwm output, and change the output value to the given level.
  *
  * @param value the level to set on the output pin. The value is normalized to match the behaviour of 
  * a typical hardware PWM pin. Hence, the value set will clip at a maximum value of 0..128 / 896..1024. 
  * Values within those ranges will be mapped to 0..100% volume of the associated audio channel.
  *
  * @return DEVICE_OK on success, DEVICE_INVALID_PARAMETER if value is out of range, or DEVICE_NOT_SUPPORTED
  *         if the given pin does not have analog capability.
  */
int SoundOutputPin::setAnalogValue(int value)
{
    if (value < 0 || value > 1024)
        return DEVICE_INVALID_PARAMETER;

    // Normalize the duty cycle (we don't care if duty cycle is hi or lo)
    if (value > 512)
        value = 1024 - value;

    // Normalize volume to approximately the behaviour of a micro:bit v1 with headphones on edge connector pin 0.
    this->value = min(128, value);
    update();

    return DEVICE_OK;
}

/**
  * Configures this IO pin as an analogue input (if necessary), and samples the Pin for its analog value.
  *
  * @return the current analogue level on the pin, in the range 0 - 1024, or
  *         DEVICE_NOT_SUPPORTED if the given pin does not have analog capability.
  */
int SoundOutputPin::getAnalogValue()
{
    return value;
}


/**
  * Configures the PWM period of the analog output to the given value.
  *
  * @param period The new period for the analog output in microseconds.
  *
  * @return DEVICE_OK on success.
  */
int SoundOutputPin::setAnalogPeriodUs(uint32_t period)
{
    this->periodUs = period;
    update();

    return DEVICE_OK;
}

/**
  * Configures the PWM period of the analog output to the given value.
  *
  * @param period The new period for the analog output in milliseconds.
  *
  * @return DEVICE_OK on success, or DEVICE_NOT_SUPPORTED if the
  *         given pin is not configured as an analog output.
  */
int SoundOutputPin::setAnalogPeriod(int period)
{
    return setAnalogPeriodUs(period*1000);
}

/**
  * Obtains the PWM period of the analog output in microseconds.
  *
  * @return the period on success, or DEVICE_NOT_SUPPORTED if the
  *         given pin is not configured as an analog output.
  */
uint32_t SoundOutputPin::getAnalogPeriodUs()
{
    return periodUs;
}

/**
  * Obtains the PWM period of the analog output in milliseconds.
  *
  * @return the period on success, or DEVICE_NOT_SUPPORTED if the
  *         given pin is not configured as an analog output.
  */
int SoundOutputPin::getAnalogPeriod()
{
    return getAnalogPeriodUs() / 1000;
}

/**
 * Update the sound being played to match that equested.
 */
void SoundOutputPin::update()
{
    float v = (float) value;
    float volume = (4.0f * v) / 512.0f;

    // If this is the first time we've been asked to produce a sound, prime a SoundEffect buffer.
    if (fx == NULL)
    {
        // Enable the audio output pipeline, if needed.
        MicroBitAudio::requestActivation();

        // Create a sound effect buffer, and configure a synthesizer for a raw squarewave output into the mixer.
        ManagedBuffer b(sizeof(SoundEffect));
        fx = (SoundEffect *)&b[0];
        fx->duration = -CONFIG_SOUND_OUTPUT_PIN_PERIOD;
        fx->tone.tonePrint = Synthesizer::SquareWaveTone;
        fx->frequency = periodUs == 0 ? 6068 : 1000000.0f / (float) periodUs;
        fx->volume = periodUs == 0 ? 0 : volume;
        channel = mixer.addChannel(synth);
        synth.play(b);
    }
    else
    {
        fx->frequency = periodUs == 0 ? 6068 : 1000000.0f / (float) periodUs;
        fx->volume = periodUs == 0 ? 0 : volume;
    }
}
