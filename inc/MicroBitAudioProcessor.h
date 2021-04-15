/*
The MIT License (MIT)
Copyright (c) 2020 Arm Limited.
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

#include "MicroBit.h"
#include "DataStream.h"
#define ARM_MATH_CM4
#include "arm_math.h"

#ifndef MICROBIT_AUDIO_PROCESSOR_H
#define MICROBIT_AUDIO_PROCESSOR_H

#define MIC_SAMPLE_RATE         (1000000 / MIC_SAMPLE_DELTA)
#define AUDIO_SAMPLES_NUMBER    512
#define HISTORY_LENGTH          50

#define RECOGNITION_START_FREQ  1400
#define RECOGNITION_END_FREQ    4500

#define ANALYSIS_STD_MULT_THRESHOLD     3
#define ANALYSIS_STD_THRESHOLD          75 
#define ANALYSIS_MEAN_THRESHOLD         0 

// #define SQUARE_BEFORE_ANALYSIS
#define HARMONIC_PRODUCT_SPECTRUM
#define HANN_WINDOW


class MicroBitAudioProcessor : public DataSink, public DataSource
{
public:

    struct AudioFrameAnalysis {
        uint8_t size;
        uint16_t buf[3];
    };

private:

    DataSource      &audiostream;   
    DataSink        *recogniser;       
    int             zeroOffset;             // unsigned value that is the best effort guess of the zero point of the data source
    int             divisor;                // Used for LINEAR modes
    arm_rfft_fast_instance_f32 fft_instance;
    float *buf; 
    float *output; 
    float *mag; 
    uint16_t position;
    bool recording;

#ifdef HANN_WINDOW
    float32_t           hann_window[AUDIO_SAMPLES_NUMBER];
#endif

    AudioFrameAnalysis  out_buffer[HISTORY_LENGTH * 2];
    uint16_t            out_buffer_len;
    bool                consumed_buffer;

    // What is this used for? Couldn't find any references to it 
    float rec[AUDIO_SAMPLES_NUMBER * 2];
    int lastFreq;

    uint16_t    frequencyToIndex(int freq);
    float32_t   indexToFrequency(int index);
    void        sendAnalysis(uint16_t* freq, uint8_t size);

    public:
    MicroBitAudioProcessor(DataSource& source); 
    ~MicroBitAudioProcessor(); 
    virtual int pullRequest();
    void connect(DataSink *downstream);
    virtual ManagedBuffer pull();
    int getFrequency();
    int setDivisor(int d);
    void startRecording();
    void stopRecording(MicroBit& uBit);
};

#endif