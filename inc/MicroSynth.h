/*
MIT License

Copyright (c) 2022 Thom Johansen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "MicroBit.h"

#if CONFIG_ENABLED(CODAL_POLYSYNTH)

#ifndef MICROSYNTH_H
#define MICROSYNTH_H

#include <cmath>
#include <cstdint>

#define _PI 3.14159265359f

namespace codal {


static constexpr int SynthBlockSize = 256;
static constexpr int SynthSampleRate = 44100;
static constexpr float SynthSampleRate_f = static_cast<float>(SynthSampleRate);

/** 
 * Class containing certain precalculated synth data.
 */
class SynthTables
{
    static float notetab_[129]; // including guard point
    static bool inited_;
public:
    /**
     * Calculates tables. Must be called once before using rest of class.
     */
    static void init();
    /**
     * Convert (fractional) MIDI note number to linear scale factor, corresponding to
     * f(x) = 2^((x - 69)/12)
     * @param ind fractional MIDI note number, 0 to 127 
     * @return scale factor. Note number 69 returns 1.0
     */
    static float noteToScaler(float ind);
};

enum class OscType : uint8_t
{
    Saw = 0,
    Pulse,
    Triangle
};

enum class FilterType : uint8_t
{
    LPF = 0,
    HPF,
    BPF
};

/**
  * Class containing all synthesizer settings.
  */
struct SynthPreset
{
    // oscillator shapes
    OscType osc1Shape, osc2Shape;
    // additive transpose factor in notes, typical -24 to 24
    float osc2Transpose;
    // linear amplitude factor, 0 to 1
    float osc1Vol, osc2Vol;
    // -1 to 1, 0 is square
    float osc1Pw, osc2Pw;
    // LFO to osc PWM, 0 to 1
    float osc1Pwm, osc2Pwm;
    // Osc1 -> Osc2 PM amount
    float fmAmount;
    // filter type
    FilterType filterType;
    // 0 to 1, covers almost all spectrum
    float filterCutoff;
    // 1 is self resonance, 0 is no resonance
    float filterReso;
    // portion of envelope to add to vcfCutoff, 0 to 1
    float filterEnv;
    // portion of lfo to add to vcfCutoff, 0 to 1
    float filterLfo;
    // portion of note freq to add to cutoff, 0 to 1
    float filterKeyFollow;
    // seconds, sustain is amplitude factor 0 to 1
    float envA, envD, envS, envR;
    // shape of lfo
    OscType lfoShape;
    // frequency of lfo in hz
    float lfoFreq;
    // vibrato frequency in hz
    float vibFreq;
    // vibrato amount in semitones
    float vibAmount;
    // voi
    float gain;
    // relative tuning for everything, in semitones
    float tune;
    // level of noise, linear amplitude
    float noise;
    // true to use smoothed gate as amplitude envelope, adsr if false
    bool ampGate;
};

/**
  * Class containing Vadim Zavilishin's TPT state variable filter
  * from the free book "The Art of VA Filter Design".
  */
class StateVariableFilter
{
    float g_, g1_, d_;
    float s1_, s2_;
    float tan(float x);
public:
    /** 
     * Constructor. 
     */
    StateVariableFilter();
    /**
     * Set filter cutoff frequency and resonance.
     * @param cutoff cutoff frequency, range 0 to 0.5 corresponding to 0 hz and nyquist
     * @param res resonance level, range 0 (no resonance) to 1 (self resonating)
     */
    void set(float cutoff, float res);
    /**
     * Filter an input sample.
     * @param x input sample
     * @param f filter type to use
     * @reeturn filtered sample
     */
    float process(float x, FilterType f = FilterType::LPF);
    /**
     * Resets internal filter history.
     */
    void reset();
};

/**
 * Class containing implementation of simple ADSR envelope with linear segments.
 */
class ADSREnv
{
    enum class State : uint8_t
    {
        A = 0, D, S, R, Done
    };
    float phase_, phase_inc_ = 0.f;
    float inc_[4] = { 0.f, 0.f, 0.f, 0.f };
    float levels_[5] = { 0.f, 1.f, 0.5f, 0.f, 0.f };
    float start_val_ = 0.f;
    float cur_ = 0.f;
    State state_;
public:
    /** 
     * Constructor.
     * Creates new envelope in Done state.
     */
    ADSREnv();
    /** 
     * Generate next envelope value.
     * @return envelope value
     */
    float process();
    /** 
     * Set envelope gate state. 
     * @param g gate status. True for active gate. 
     */
    void gate(bool g = true);
    /** 
     * Get envelope state
     * @return envelope value
     */
    bool done() const;
    /** 
     * Set envelope times and levels.
     * @param a attack time in seconds
     * @param d decay time in seconds
     * @param s sustain level, usually 0 to 1
     * @param r release time in seconds
     */
    void set(float a, float d, float s, float r);
    /** 
     * Reset envelope state to Done.
     */
    void reset();
    /** 
     * Get current envelope value.
     * @return envelope value
     */
    float value() const;
};

/**
 * Class containing naive oscillators with plenty of aliasing.
 */
class Oscillator
{
    float acc_ = 0.f, delta_ = 0.f, pw_ = 0.f;
    OscType wave_ = OscType::Saw;
public:
    /**
     * Generate a oscillator sample.
     * @return oscillator sample
     */
    float process();
    /**
     * Generate an oscillator sample with phase modulation.
     * @param pm phase modulation value in range -1 to 1
     * @return oscillator sample 
     */
    float processPM(float pm);
    /**
     * Set oscillator frequency.
     * @param f frequency in hz
     */
    void setFreq(float f);
    /**
     * Set oscillator type.
     * @param t oscillator type
     */
    void setType(OscType t);
    /**
     * Set pulse width for Pulse waveform.
     * @param pw pulse width, range from -1 to 1, where 0 is a square wave
     */
    void setPW(float pw);
};

/**
 * A single synthesizer voice.
 * All parameter modulations except the amplitude envelope are computed once per block
 * to save on processing time.
 */
class Voice
{
    Oscillator osc_[2];
    Oscillator lfo_;
    Oscillator vibLfo_;
    StateVariableFilter filter_;
    ADSREnv env_;
    float gain_;            // gain and velocity combined
    float smoothedGate_;    // lowpass filtered gate for use instead of envelope
    int gateLength_ = -1;   // -1 means no preset time duration
    int8_t note_ = -1;      // -1 means inactive voice
    bool stopping_ = false; // set to true after we've received a note off
    const SynthPreset* preset_ = nullptr;
    int32_t noise_;         // linear congruential noise state
    void apply_preset();
    void set_note(float note);
    // per sample process
    float process();
public:
    Voice();
    /** 
     * Run voice synthesis loop.
     * @param buf buffer to mix voice output into
     * @param num number of samples to generate
     */
    void process(float* buf, int num);
    /** 
     * Trigger a new voice, potentially stealing an active voice to do so.
     * @param note MIDI note number
     * @param velocity note velocity, from 0 to 1 (max velocity). Currently controls voice gain
     * @param preset preset to use for this voice
     * @param length length of note in samples. Use -1 to let the gate decide
     */
    void trig(int8_t note, float velocity, const SynthPreset* preset, int length = -1);
    /** 
     * Release a voice, starting the envelope release phase.
     */
    void detrig();
    /**
    * Get note number this voice was created with.
    * @return Note number
    */
    int8_t getNote() const;
    /**
    * Get note status.
    * @return true if not is in release phase, false if not
    */
    bool isStopping() const;
};

/** 
 * Polyphonic synthesizer. 
 */
class PolySynth
{
    Voice* voice_;
    float mixbuf_[SynthBlockSize];
    int numVoices_;

    int findVoice(int8_t note);
    Voice& alloc(int note);
    void process_noclip(float* buf, int num);
public:
    PolySynth(int num_voices);
    ~PolySynth();
    /**
    * Allocates a voice and starts playing a note with given parameters.
    * @param note MIDI Note number
    * @param velocity Note velocity, from 0 to 1
    * @param duration Note duration, in seconds
    * @param preset Pointer to preset data to use for voice
    */
    void noteOn(int8_t note, float velocity, float duration, const SynthPreset* preset);
    /**
    * Starts release phase of voice playing given note.
    * @param note MIDI Note number
    */
    void noteOff(int8_t note);
    /**
    * Synthesize a buffer of sound, float buffer version.
    * @param buf buffer of floats to render sound to
    * @param num number of samples to generate
    */
    void process(float* buf, int num);
    /**
    * Synthesize a buffer of sound, integer buffer version.
    * @param buf buffer of integers to render sound to
    * @param num number of samples to generate
    */
    void process(uint16_t* buf, int num);
};

/**
 * Class wrapping PolySynth in a CODAL DataSource for real-time use.
 */
class PolySynthSource : public DataSource
{
    DataSink* downStream_;
    bool init_ = false;
    PolySynth& synth_;
public:
    /**
     * Constructor.
     * @param s synthesizer object. Caller has responsibility for accessing this safely with regard to simultaneous
     * use in the audio interrupt.
     */
    PolySynthSource(PolySynth& s);
    /**
     * Starts audio processing.
     */
    void start();
    /**
     * Define a downstream component for data stream.
     * @param sink The component that data will be delivered to, when it is availiable
     */
    virtual void connect(DataSink& sink) override;
    /**
     * Determine the data format of the buffers streamed out of this component.
     * @return data format
     */
    virtual int getFormat() override;
    /**
     * Provide the next available ManagedBuffer to our downstream caller, if available.
     * @return next ManagedBuffer of audio
     */
    virtual ManagedBuffer pull() override;
};

} // namespace codal

#endif

#endif // CONFIG
