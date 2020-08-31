/*
The MIT License (MIT)

Copyright (c) 2020 Lancaster University.

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

#include "SoundSynthesizerEffects.h"

using namespace codal;

/**
 * Root Frequency Interpolation Effect Functions
 */

void SoundSynthesizerEffects::noInterpolation(SoundEmojiSynthesizer *synth, float *parameter)
{
}

// Linear interpolate function.
// parameter[0]: end frequency
void SoundSynthesizerEffects::linearInterpolation(SoundEmojiSynthesizer *synth, float *parameter)
{
    float interval = (parameter[0] - synth->effect->frequency) / synth->effect->steps;
    synth->frequency = synth->effect->frequency+interval*synth->step;
}

// Linear interpolate function.
// parameter[0]: end frequency
void SoundSynthesizerEffects::logarithmicInterpolation(SoundEmojiSynthesizer *synth, float *parameter)
{
    synth->frequency = synth->effect->frequency+(log10(synth->step)*(parameter[0]-synth->effect->frequency)/1.95);
}

// Curve interpolate function
// parameter[0]: end frequency
void SoundSynthesizerEffects::curveInterpolation(SoundEmojiSynthesizer *synth, float *parameter)
{
    synth->frequency = (sin(synth->step*3.12159f/180.0f)*(parameter[0]-synth->effect->frequency)+synth->effect->frequency);
}

// Cosine interpolate function
// parameter[0]: end frequency
void SoundSynthesizerEffects::slowVibratoInterpolation(SoundEmojiSynthesizer *synth, float *parameter){
    synth->frequency = sin(synth->step/10)*parameter[0]+synth->effect->frequency;
}

//warble function
// parameter[0]: end frequency
void SoundSynthesizerEffects::warbleInterpolation(SoundEmojiSynthesizer *synth, float *parameter)
{
    synth->frequency = (sin(synth->step)*(parameter[0]-synth->effect->frequency)+synth->effect->frequency);
}

// Vibrato function
// parameter[0]: end frequency
void SoundSynthesizerEffects::vibratoInterpolation(SoundEmojiSynthesizer *synth, float *parameter){
    synth->frequency = synth->effect->frequency + sin(synth->step)*parameter[0];
}

// Exponential rising function
// parameter[0]: end frequency
void SoundSynthesizerEffects::exponentialRisingInterpolation(SoundEmojiSynthesizer *synth, float *parameter)
{
    synth->frequency = synth->effect->frequency + sin(0.01745329f*synth->step)*parameter[0];
}

// Exponential falling function
void SoundSynthesizerEffects::exponentialFallingInterpolation(SoundEmojiSynthesizer *synth, float *parameter)
{
    synth->frequency = synth->effect->frequency + cos(0.01745329f*synth->step)*parameter[0];
}

void SoundSynthesizerEffects::majAppregrioAscendInterpolation(SoundEmojiSynthesizer *synth, float *parameter)
{
    //TODO: implement
}
void SoundSynthesizerEffects::majAppregrioDescendInterpolation(SoundEmojiSynthesizer *synth, float *parameter)
{
    //TODO: implement
}
void SoundSynthesizerEffects::minAppregrioAscendInterpolation(SoundEmojiSynthesizer *synth, float *parameter)
{
    //TODO: implement
}
void SoundSynthesizerEffects::minAppregrioDescendInterpolation(SoundEmojiSynthesizer *synth, float *parameter)
{
    //TODO: implement
}
void SoundSynthesizerEffects::dimAppregrioAscendInterpolation(SoundEmojiSynthesizer *synth, float *parameter)
{
    //TODO: implement
}
void SoundSynthesizerEffects::dimAppregrioDescendInterpolation(SoundEmojiSynthesizer *synth, float *parameter)
{
    //TODO: implement
}
void SoundSynthesizerEffects::chromaticAppregrioAscendInterpolation(SoundEmojiSynthesizer *synth, float *parameter)
{
    //TODO: implement
}
void SoundSynthesizerEffects::chromaticAppregrioDescendInterpolation(SoundEmojiSynthesizer *synth, float *parameter)
{
    //TODO: implement
}
void SoundSynthesizerEffects::toneAppregrioAscendInterpolation(SoundEmojiSynthesizer *synth, float *parameter)
{
    //TODO: implement
}
void SoundSynthesizerEffects::toneAppregrioDescendInterpolation(SoundEmojiSynthesizer *synth, float *parameter)
{
    //TODO: implement
}

/**
 * Frequency Delta effects
 */

// Vibrato function
// parameter[0]: vibrato frequency range
void SoundSynthesizerEffects::vibratoEffect(SoundEmojiSynthesizer *synth, float *parameter)
{
    synth->frequency += sin(synth->step)*parameter[0];
}

// Slow Vibrato function
// parameter[0]: vibrato frequency range
void SoundSynthesizerEffects::slowVibratoEffect(SoundEmojiSynthesizer *synth, float *parameter)
{
    synth->frequency += sin(synth->step/10)*parameter[0];
}

/**
 * Volume Delta effects
 */

/** Simple ADSR enveleope effect.
 * parameter[0]: Centre volume
 * parameter[1]: End volume
 * effect->volume: start volume
 */
void SoundSynthesizerEffects::adsrVolumeEffect(SoundEmojiSynthesizer *synth, float *parameter)
{
    float halfSteps = synth->effect->steps*0.5f;

    if(synth->step <= halfSteps)
    {
        float delta = (parameter[0] - synth->effect->volume ) / halfSteps;
        synth->volume = synth->effect->volume + synth->step * delta;
    }else{
        float delta = (parameter[1] - parameter[0]) / halfSteps;
        synth->volume = parameter[0] + (synth->step - halfSteps) * delta;         
    }
}

/**
 * Simple volume ramp effect
 * parameter[0]: End volume
 * effect->volume: start volume
 */
void SoundSynthesizerEffects::volumeRampEffect(SoundEmojiSynthesizer *synth, float *parameter)
{
    float delta = (parameter[0] - synth->effect->volume) / synth->effect->steps;
    synth->volume = synth->effect->volume + synth->step * delta;
}
