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
#define DEFAULT_AUDIO_SAMPLES_NUMBER    512
#define EMOJI_AUDIO_SAMPLES_NUMBER      512
#define MORSE_AUDIO_SAMPLES_NUMBER      128

#define RECOGNITION_START_FREQ  1700
#define RECOGNITION_END_FREQ    5000

#define ANALYSIS_STD_MULT_THRESHOLD     3
#define ANALYSIS_STD_THRESHOLD          60 
#define ANALYSIS_MEAN_THRESHOLD         0 

#define MAXIMUM_NUMBER_OF_FREQUENCIES   3
#define SIMILAR_FREQ_THRESHOLD          100

class MicroBitAudioProcessor : public DataSink, public DataSource
{
public:

    struct AudioFrameAnalysis {
        uint8_t size;
        uint16_t buf[MAXIMUM_NUMBER_OF_FREQUENCIES];
    };

private:
    
    MicroBit&               uBit;

    DataSource      &audiostream;   
    DataSink        *recogniser;       
    int             zeroOffset;             // unsigned value that is the best effort guess of the zero point of the data source
    int             divisor;                // Used for LINEAR modes
    uint16_t        audio_samples_number;
    arm_rfft_fast_instance_f32 fft_instance;
    float *buf; 
    float *fft_output; 
    float *mag; 

    uint16_t position;
    bool recording;

    AudioFrameAnalysis  output;

    uint16_t    frequencyToIndex(int freq);
    float32_t   indexToFrequency(int index);

    public:
    MicroBitAudioProcessor(DataSource& source, MicroBit& uBit, uint16_t audio_samples_number = DEFAULT_AUDIO_SAMPLES_NUMBER); 
    ~MicroBitAudioProcessor(); 
    virtual int pullRequest();
    void connect(DataSink *downstream);
    virtual ManagedBuffer pull();
    
    int setDivisor(int d);
    void startRecording();
    void stopRecording(MicroBit& uBit);
};

#endif