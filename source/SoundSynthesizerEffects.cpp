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

void SoundSynthesizerEffects::noInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
}

// Linear interpolate function.
// parameter[0]: end frequency
void SoundSynthesizerEffects::linearInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
    float interval = (context->parameter[0] - synth->effect->frequency) / context->steps;
    synth->frequency = synth->effect->frequency+interval*context->step;
}

// Linear interpolate function.
// parameter[0]: end frequency
void SoundSynthesizerEffects::logarithmicInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
    synth->frequency = synth->effect->frequency+(log10(context->step)*(context->parameter[0]-synth->effect->frequency)/1.95);
}

// Curve interpolate function
// parameter[0]: end frequency
void SoundSynthesizerEffects::curveInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
    synth->frequency = (sin(context->step*3.12159f/180.0f)*(context->parameter[0]-synth->effect->frequency)+synth->effect->frequency);
}

// Cosine interpolate function
// parameter[0]: end frequency
void SoundSynthesizerEffects::slowVibratoInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context){
    synth->frequency = sin(context->step/10)*context->parameter[0]+synth->effect->frequency;
}

//warble function
// parameter[0]: end frequency
void SoundSynthesizerEffects::warbleInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
    synth->frequency = (sin(context->step)*(context->parameter[0]-synth->effect->frequency)+synth->effect->frequency);
}

// Vibrato function
// parameter[0]: end frequency
void SoundSynthesizerEffects::vibratoInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context){
    synth->frequency = synth->effect->frequency + sin(context->step)*context->parameter[0];
}

// Exponential rising function
// parameter[0]: end frequency
void SoundSynthesizerEffects::exponentialRisingInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
    synth->frequency = synth->effect->frequency + sin(0.01745329f*context->step)*context->parameter[0];
}

// Exponential falling function
void SoundSynthesizerEffects::exponentialFallingInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
    synth->frequency = synth->effect->frequency + cos(0.01745329f*context->step)*context->parameter[0];
}

void SoundSynthesizerEffects::majAppregrioAscendInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
    //TODO: implement
}
void SoundSynthesizerEffects::majAppregrioDescendInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
    //TODO: implement
}
void SoundSynthesizerEffects::minAppregrioAscendInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
    //TODO: implement
}
void SoundSynthesizerEffects::minAppregrioDescendInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
    //TODO: implement
}
void SoundSynthesizerEffects::dimAppregrioAscendInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
    //TODO: implement
}
void SoundSynthesizerEffects::dimAppregrioDescendInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
    //TODO: implement
}
void SoundSynthesizerEffects::chromaticAppregrioAscendInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
    //TODO: implement
}
void SoundSynthesizerEffects::chromaticAppregrioDescendInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
    //TODO: implement
}
void SoundSynthesizerEffects::toneAppregrioAscendInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
    //TODO: implement
}
void SoundSynthesizerEffects::toneAppregrioDescendInterpolation(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
    //TODO: implement
}

/**
 * Frequency Delta effects
 */

// Vibrato function
// parameter[0]: vibrato frequency range
void SoundSynthesizerEffects::vibratoEffect(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
    synth->frequency += sin(context->step)*context->parameter[0];
}

// Slow Vibrato function
// parameter[0]: vibrato frequency range
void SoundSynthesizerEffects::slowVibratoEffect(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
    synth->frequency += sin(context->step/10)*context->parameter[0];
}

/**
 * Volume Delta effects
 */

/** Simple ADSR enveleope effect.
 * parameter[0]: Centre volume
 * parameter[1]: End volume
 * effect->volume: start volume
 */
void SoundSynthesizerEffects::adsrVolumeEffect(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
    float halfSteps = context->steps*0.5f;

    if(context->step <= halfSteps)
    {
        float delta = (context->parameter[0] - synth->effect->volume ) / halfSteps;
        synth->volume = synth->effect->volume + context->step * delta;
    }else{
        float delta = (context->parameter[1] - context->parameter[0]) / halfSteps;
        synth->volume = context->parameter[0] + (context->step - halfSteps) * delta;         
    }
}

/**
 * Simple volume ramp effect
 * parameter[0]: End volume
 * effect->volume: start volume
 */
void SoundSynthesizerEffects::volumeRampEffect(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
    float delta = (context->parameter[0] - synth->effect->volume) / context->steps;
    synth->volume = synth->effect->volume + context->step * delta;
}
