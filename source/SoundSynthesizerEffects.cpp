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
#include "CodalDmesg.h"

using namespace codal;

/**
 * Definitions of standard progressions.
 */
const float MusicalIntervals::chromaticInterval[] = {1.000000f, 1.059463f, 1.122462f, 1.189207f, 1.259921f, 1.334840f, 1.414214f, 1.498307f, 1.587401f, 1.681793f, 1.781797f, 1.887749f};
const float MusicalIntervals::majorScaleInterval[] = {chromaticInterval[0], chromaticInterval[2], chromaticInterval[4], chromaticInterval[5], chromaticInterval[7], chromaticInterval[9], chromaticInterval[11]};
const float MusicalIntervals::minorScaleInterval[] = {chromaticInterval[0], chromaticInterval[2], chromaticInterval[3], chromaticInterval[5], chromaticInterval[7], chromaticInterval[8], chromaticInterval[10]};
const float MusicalIntervals::pentatonicScaleInterval[] = {chromaticInterval[0], chromaticInterval[2], chromaticInterval[4], chromaticInterval[7], chromaticInterval[9]};
const float MusicalIntervals::majorTriadInterval[] = {chromaticInterval[0], chromaticInterval[4], chromaticInterval[7]};
const float MusicalIntervals::minorTriadInterval[] = {chromaticInterval[0], chromaticInterval[3], chromaticInterval[7]};
const float MusicalIntervals::diminishedInterval[] = {chromaticInterval[0], chromaticInterval[3], chromaticInterval[6], chromaticInterval[9]};
const float MusicalIntervals::wholeToneInterval[] = {chromaticInterval[0], chromaticInterval[2], chromaticInterval[4], chromaticInterval[6], chromaticInterval[8], chromaticInterval[10]};

static const Progression _chromatic = {MusicalIntervals::chromaticInterval, 12};
static const Progression _majorScale = {MusicalIntervals::majorScaleInterval, 7};
static const Progression _minorScale = {MusicalIntervals::minorScaleInterval, 7};
static const Progression _pentatonicScale = {MusicalIntervals::pentatonicScaleInterval, 5};
static const Progression _majorTriad = {MusicalIntervals::majorTriadInterval, 3};
static const Progression _minorTriad = {MusicalIntervals::minorTriadInterval, 3};
static const Progression _diminished = {MusicalIntervals::diminishedInterval, 4};
static const Progression _wholeTone = {MusicalIntervals::wholeToneInterval, 6};


// Export pointers to standard progressions for ease of use.
const Progression* MusicalProgressions::chromatic = &_chromatic;
const Progression* MusicalProgressions::majorScale = &_majorScale;
const Progression* MusicalProgressions::minorScale = &_minorScale;
const Progression* MusicalProgressions::pentatonic = &_pentatonicScale;
const Progression* MusicalProgressions::majorTriad = &_majorTriad;
const Progression* MusicalProgressions::minorTriad = &_minorTriad;
const Progression* MusicalProgressions::diminished = &_diminished;
const Progression* MusicalProgressions::wholeTone = &_wholeTone;


/**
 * Determine the frequency of a given note in a given progressions
 * 
 * @param root The root frequency of the progression
 * @param progression The Progression to use
 * @param offset The offset (interval) of the note to generate
 * @return The frequency of the note requested in Hz.
 */
static float calculateFrequencyFromProgression(float root, const Progression *progression, int offset)
{
    int octave = (offset / progression->length);
    int index = offset % progression->length;

    return root * powf(2, octave) * progression->interval[index];
}

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

// Argeppio functions
void SoundSynthesizerEffects::appregrioAscending(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
    synth->frequency = calculateFrequencyFromProgression(synth->effect->frequency, (Progression *)context->parameter_p[0], context->step);
}

void SoundSynthesizerEffects::appregrioDescending(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
    synth->frequency = calculateFrequencyFromProgression(synth->effect->frequency, (Progression *)context->parameter_p[0], context->steps - context->step - 1);
}


/**
 * Frequency Delta effects
 */

// Frequency vibrato function
// parameter[0]: vibrato frequency multiplier
void SoundSynthesizerEffects::frequencyVibratoEffect(SoundEmojiSynthesizer *synth, ToneEffect *context)
{  
    if (context->step == 0)
        return;

    if(context->step % 2 == 0)
        synth->frequency /= context->parameter[0];
    else
        synth->frequency *= context->parameter[0];
}

// Volume vibrato function
// parameter[0]: vibrato volume multiplier
void SoundSynthesizerEffects::volumeVibratoEffect(SoundEmojiSynthesizer *synth, ToneEffect *context)
{
    if (context->step == 0)
        return;

    if(context->step % 2 == 0)
        synth->volume /= context->parameter[0];
    else
        synth->volume *= context->parameter[0];

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
