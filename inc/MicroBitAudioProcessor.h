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

#define MIC_SAMPLE_RATE         (11 * 1024)
#define AUDIO_SAMPLES_NUMBER    1024

class MicroBitAudioProcessor : public DataSink
{
    DataSource      &audiostream;          
    int             zeroOffset;             // unsigned value that is the best effort guess of the zero point of the data source
    int             divisor;                // Used for LINEAR modes
    arm_rfft_fast_instance_f32 fft_instance;
    float *buf; 
    float *output; 
    float *mag; 
    uint16_t position;
    bool recording;
    float rec[AUDIO_SAMPLES_NUMBER * 2];
    int lastFreq;

    public:
    MicroBitAudioProcessor(DataSource& source); 
    ~MicroBitAudioProcessor(); 
    virtual int pullRequest();
    int getFrequency();
    int setDivisor(int d);
    void startRecording();
    void stopRecording(MicroBit& uBit);
};

#endif