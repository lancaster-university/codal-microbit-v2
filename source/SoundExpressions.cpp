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

#include "SoundExpressions.h"

#include "NRF52PWM.h"
#include "Synthesizer.h"
#include "SoundEmojiSynthesizer.h"
#include "SoundSynthesizerEffects.h"
#include "ManagedString.h"
#include "NotifyEvents.h"

#define CLAMP(lo, v, hi) ((v) = ((v) < (lo) ? (lo) : (v) > (hi) ? (hi) : (v)))

/**
  * Default Constructor.
  */
SoundExpressions::SoundExpressions(SoundEmojiSynthesizer &synth): synth(synth)
{}

/**
  * Destructor.
  *
  * Removes resources held by the instance.
  */
SoundExpressions::~SoundExpressions()
{
}

void SoundExpressions::play(ManagedString sound) {
    fiber_wake_on_event(synth.id, DEVICE_SOUND_EMOJI_SYNTHESIZER_EVT_DONE);
    playAsync(sound);
    schedule();
}

void SoundExpressions::playAsync(ManagedString sound) {
    // Sound is either encoded data or a name of a built-in sound for which we have the data.
    sound = lookupBuiltIn(sound);
    const unsigned soundLen = sound.length();
    const char *soundChars = sound.toCharArray();

    // 72 characters of sound data comma separated
    const unsigned charsPerEffect = 72;
    const unsigned effectCount = (soundLen + 1) / (charsPerEffect + 1);
    const unsigned expectedLength = effectCount * (charsPerEffect + 1) - 1;
    if (soundLen != expectedLength) {
        return;
    }
    
    ManagedBuffer b(sizeof(SoundEffect) * effectCount);
    SoundEffect *fx = (SoundEffect *) &b[0];
    for (unsigned i = 0; i < effectCount; ++i)  {
        const int start = i * charsPerEffect + i;
        if (start > 0 && soundChars[start - 1] != ',') {
            return;
        }
        if (!parseSoundExpression(&soundChars[start], fx++)) {
            return;
        }
    }
    synth.play(b);
}

int SoundExpressions::parseDigits(const char *input, const int digits) {
    int result = 0;
    for (int i = 0; i < digits; ++i) {
        const int n = input[i] - '0';
        if (n < 0 || n > 9) {
            return -1;
        }
        const int powers = digits - i;
        switch (powers) {
            case 4: result += 1000 * n; break;
            case 3: result += 100 * n; break;
            case 2: result += 10 * n; break;
            case 1: result += n; break;
        }
    }
    return result;
}

int SoundExpressions::applyRandom(int value, int rand) {
    if (value < 0 || rand < 0) {
        return -1;
    }
    const int delta = random(rand * 2 + 1) - rand;
    return abs(value + delta);
}

bool SoundExpressions::parseSoundExpression(const char *soundChars, SoundEffect *fx) {
    // Encoded as a sequence of zero padded decimal strings.
    // This encoding is worth reconsidering if we can!
    // The ADSR effect (and perhaps others in future) has two parameters which cannot be expressed.

    // 72 chars total
    //  [0] 0-4 wave
    int wave = parseDigits(&soundChars[0], 1);
    //  [1] 0000-1023 volume
    int effectVolume = parseDigits(&soundChars[1], 4);
    //  [5] 0000-9999 frequency
    int frequency = parseDigits(&soundChars[5], 4);
    //  [9] 0000-9999 duration
    int duration = parseDigits(&soundChars[9], 4);
    // [13] 00 shape (specific known values)
    int shape = parseDigits(&soundChars[13], 2);
    // [15] XXX unused/bug. This was startFrequency but we use frequency above.
    // [18] 0000-9999 end frequency
    int endFrequency = parseDigits(&soundChars[18], 4);
    // [22] XXXX unused. This was start volume but we use volume above.
    // [26] 0000-1023 end volume
    int endVolume = parseDigits(&soundChars[26], 4);
    // [30] 0000-9999 steps
    int steps = parseDigits(&soundChars[30], 4);
    // [34] 00-03 fx choice
    int fxChoice = parseDigits(&soundChars[34], 2);
    // [36] 0000-9999 fxParam
    int fxParam = parseDigits(&soundChars[36], 4);
    // [40] 0000-9999 fxnSteps
    int fxnSteps = parseDigits(&soundChars[40], 4);

    // Details that encoded randomness to be applied when frame is used:
    // Can the randomness cause any parameters to go out of range?
    // [44] 0000-9999 frequency random
    frequency = applyRandom(frequency, parseDigits(&soundChars[44], 4));
    // [48] 0000-9999 end frequency random
    endFrequency = applyRandom(endFrequency, parseDigits(&soundChars[48], 4));
    // [52] 0000-9999 volume random
    effectVolume = applyRandom(effectVolume, parseDigits(&soundChars[52], 4));
    // [56] 0000-9999 end volume random
    endVolume = applyRandom(endVolume, parseDigits(&soundChars[56], 4));
    // [60] 0000-9999 duration random
    duration = applyRandom(duration, parseDigits(&soundChars[60], 4));
    // [64] 0000-9999 fxParamRandom
    fxParam = applyRandom(fxParam, parseDigits(&soundChars[64], 4));
    // [68] 0000-9999 fxnStepsRandom
    fxnSteps = applyRandom(fxnSteps, parseDigits(&soundChars[68], 4));

    if (frequency == -1 || endFrequency == -1 || effectVolume == -1 || endVolume == -1 || duration == -1 || fxParam == -1 || fxnSteps == -1) {
        return false;
    }

    float volumeScaleFactor = 1.0f;

    switch(wave) {
        case 0:
            fx->tone.tonePrint = Synthesizer::SineTone;
            break;
        case 1:
            fx->tone.tonePrint = Synthesizer::SawtoothTone;
            break;
        case 2:
            fx->tone.tonePrint = Synthesizer::TriangleTone;
            break;
        case 3:
            fx->tone.tonePrint = Synthesizer::SquareWaveTone;
            break;
        case 4:
            fx->tone.tonePrint = Synthesizer::NoiseTone;
            break;
    }

    fx->frequency = (float) frequency;
    fx->duration = (float) duration;

    fx->effects[0].steps = steps;
    switch(shape) {
        case 0:
            fx->effects[0].effect = SoundSynthesizerEffects::noInterpolation;
            break;
        case 1:
            fx->effects[0].effect = SoundSynthesizerEffects::linearInterpolation;
            fx->effects[0].parameter[0] = (float) endFrequency;
            break;
        case 2:
            fx->effects[0].effect = SoundSynthesizerEffects::curveInterpolation;
            fx->effects[0].parameter[0] = (float) endFrequency;
            break;
        case 5:
            fx->effects[0].effect = SoundSynthesizerEffects::exponentialRisingInterpolation;
            fx->effects[0].parameter[0] = (float) endFrequency;
            break;
        case 6:
            fx->effects[0].effect = SoundSynthesizerEffects::exponentialFallingInterpolation;
            fx->effects[0].parameter[0] = (float) endFrequency;
            break;
        case 8: // various ascending scales - see next switch
        case 10:
        case 12:
        case 14:
        case 16:
            fx->effects[0].effect = SoundSynthesizerEffects::appregrioAscending;
            break;
        case 9: // various descending scales - see next switch
        case 11:
        case 13:
        case 15:
        case 17:
            fx->effects[0].effect = SoundSynthesizerEffects::appregrioDescending;
            break;
        case 18:
            fx->effects[0].effect = SoundSynthesizerEffects::logarithmicInterpolation;
            break;
    }

    // Scale    
    switch(shape) {
        case 8:
        case 9:
            fx->effects[0].parameter_p[0] = MusicalProgressions::majorScale;
            break;
        case 10:
        case 11:
            fx->effects[0].parameter_p[0] = MusicalProgressions::minorScale;
            break;
        case 12:
        case 13:
            fx->effects[0].parameter_p[0] = MusicalProgressions::diminished;
            break;
        case 14:
        case 15:
            fx->effects[0].parameter_p[0] = MusicalProgressions::chromatic;
            break;
        case 16:
        case 17:
            fx->effects[0].parameter_p[0] = MusicalProgressions::wholeTone;
            break;
    }

    // Volume envelope
    float effectVolumeFloat = (float) CLAMP(0, effectVolume, 1023) / 1023.0;
    float endVolumeFloat = (float) CLAMP(0, endVolume, 1023) / 1023.0;
    fx->volume = volumeScaleFactor * effectVolumeFloat;
    fx->effects[1].effect = SoundSynthesizerEffects::volumeRampEffect;
    fx->effects[1].steps = 36;
    fx->effects[1].parameter[0] = volumeScaleFactor * endVolumeFloat;

    // Vibrato effect
    // Steps need to be spread across duration evenly.
    float normalizedFxnSteps = (fx->duration / 10000) * fxnSteps;
    switch(fxChoice) {
        case 1:
            fx->effects[2].steps = normalizedFxnSteps;
            fx->effects[2].effect = SoundSynthesizerEffects::frequencyVibratoEffect;
            fx->effects[2].parameter[0] = (float) fxParam;
            break;
        case 2:
            fx->effects[2].steps = normalizedFxnSteps;
            fx->effects[2].effect = SoundSynthesizerEffects::volumeVibratoEffect;
            fx->effects[2].parameter[0] = (float) fxParam;
            break;
        case 3:
            fx->effects[2].steps = normalizedFxnSteps;
            fx->effects[2].effect = SoundSynthesizerEffects::warbleInterpolation;
            fx->effects[2].parameter[0] = (float) fxParam;
            break;
    }
    return true;
}

// Names and data for each built-in sound expression.
static ManagedString giggle("giggle");
static ManagedString giggleData("010230988019008440044008881023001601003300240000000000000000000000000000,110232570087411440044008880352005901003300010000000000000000010000000000,310232729021105440288908880091006300000000240700020000000000003000000000,310232729010205440288908880091006300000000240700020000000000003000000000,310232729011405440288908880091006300000000240700020000000000003000000000");
static ManagedString happy("happy");
static ManagedString happyData("010231992066911440044008880262002800001800020500000000000000010000000000,002322129029508440240408880000000400022400110000000000000000007500000000,000002129029509440240408880145000400022400110000000000000000007500000000");
static ManagedString hello("hello");
static ManagedString helloData("310230673019702440118708881023012800000000240000000000000000000000000000,300001064001602440098108880000012800000100040000000000000000000000000000,310231064029302440098108881023012800000100040000000000000000000000000000");
static ManagedString mysterious("mysterious");
static ManagedString mysteriousData("400002390033100440240408880477000400022400110400000000000000008000000000,405512845385000440044008880000012803010500160000000000000000085000500015");
static ManagedString sad("sad");
static ManagedString sadData("310232226070801440162408881023012800000100240000000000000000000000000000,310231623093602440093908880000012800000100240000000000000000000000000000");
static ManagedString slide("slide");
static ManagedString slideData("105202325022302440240408881023012801020000110400000000000000010000000000,010232520091002440044008881023012801022400110400000000000000010000000000");
static ManagedString soaring("soaring");
static ManagedString soaringData("210234009530905440599908881023002202000400020250000000000000020000000000,402233727273014440044008880000003101024400030000000000000000000000000000");
static ManagedString spring("spring");
static ManagedString springData("306590037116312440058708880807003400000000240000000000000000050000000000,010230037116313440058708881023003100000000240000000000000000050000000000");
static ManagedString twinkle("twinkle");
static ManagedString twinkleData("010180007672209440075608880855012800000000240000000000000000000000000000");
static ManagedString yawn("yawn");
static ManagedString yawnData("200002281133202440150008881023012801024100240400030000000000010000000000,005312520091002440044008880636012801022400110300000000000000010000000000,008220784019008440044008880681001600005500240000000000000000005000000000,004790784019008440044008880298001600000000240000000000000000005000000000,003210784019008440044008880108001600003300080000000000000000005000000000");

ManagedString SoundExpressions::lookupBuiltIn(ManagedString sound) {
    if (sound == giggle)
        return giggleData;
    if (sound == happy)
        return happyData;
    if (sound == hello)
        return helloData;
    if (sound == mysterious)
        return mysteriousData;
    if (sound == sad)
        return sadData;
    if (sound == slide)
        return slideData;
    if (sound == soaring)
        return soaringData;
    if (sound == spring)
        return springData;
    if (sound == twinkle)
        return twinkleData;
    if (sound == yawn)
        return yawnData;
    return sound;
}