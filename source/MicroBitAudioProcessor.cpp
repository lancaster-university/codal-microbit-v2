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
#include "MicroBitAudioProcessor.h"

#include <vector>
#include <algorithm>


MicroBitAudioProcessor::MicroBitAudioProcessor( DataSource& source, 
                                                MicroBit& uBit, 
                                                uint16_t audio_samples_number) 
                            : audiostream(source), 
                              recogniser(NULL), 
                              uBit(uBit), 
                              audio_samples_number(audio_samples_number) {
    
    divisor = 1;
    
    arm_rfft_fast_init_f32(&fft_instance, audio_samples_number);

    /* Double Buffering: We allocate twice the number of samples*/
    buf         = new float[audio_samples_number * 2];
    fft_output  = new float[audio_samples_number];
    mag         = new float[audio_samples_number / 2];

    memset(buf, 0, sizeof(buf));

    position = 0;
    recording = false;

    if (buf == NULL || fft_output == NULL || mag == NULL || played == NULL) {
        DMESG("DEVICE_NO_RESOURCES");
        target_panic(DEVICE_OOM);
    }
    DMESG("%s %p", "Audio Processor connecting to upstream, Pointer to me : ", this);
    audiostream.connect(*this);
}

MicroBitAudioProcessor::~MicroBitAudioProcessor()
{
    delete buf;
    delete fft_output;
    delete mag;
}

uint16_t MicroBitAudioProcessor::frequencyToIndex(int freq) {
    return (freq / ((uint32_t)MIC_SAMPLE_RATE / audio_samples_number));
} 

float32_t MicroBitAudioProcessor::indexToFrequency(int index) {
    return ((uint32_t)MIC_SAMPLE_RATE / audio_samples_number) * index;
} 

void MicroBitAudioProcessor::connect(DataSink *downstream){
    recogniser = downstream;
}

ManagedBuffer MicroBitAudioProcessor::pull()
{
    return ManagedBuffer(((uint8_t *) (&output)), (int) sizeof(AudioFrameAnalysis));
}

int MicroBitAudioProcessor::pullRequest()
{

    int s;
    int result;

    auto mic_samples = audiostream.pull();

    if (!recording)
        return DEVICE_OK;

    int8_t *data = (int8_t *) &mic_samples[0];

    int samples = mic_samples.length();

    for (int i=0; i < samples; i++)
    {


        s = (int) *data;
        result = s;

        data++;
        buf[position++] = (float)result;


        if (!(position % audio_samples_number))
        {
            position = 0;

            uint16_t from   = frequencyToIndex(RECOGNITION_START_FREQ);
            uint16_t to     = min(frequencyToIndex(RECOGNITION_END_FREQ), audio_samples_number / 2);
        
            arm_rfft_fast_f32(&fft_instance, buf , fft_output, 0);
            arm_cmplx_mag_f32(&fft_output[0], mag,  to);

            float32_t maxFreq = 0;
            uint32_t index;
            arm_max_f32(&mag[5], (uint32_t)to - 5, &maxFreq, &index);

            float32_t mean = 0;
            float32_t std = 0;

            arm_mean_f32 (&mag[from], to - from, &mean);
            arm_std_f32  (&mag[from], to - from, &std);

            float32_t threshold = mean + std * ANALYSIS_STD_MULT_THRESHOLD;
            
            memset(played, 0, sizeof(played));
            output.size = 0;


            if(std > ANALYSIS_STD_THRESHOLD && mean > ANALYSIS_MEAN_THRESHOLD) {
                std::vector<std::pair<uint16_t, float32_t>> freq_played; 

                for(uint16_t i=from; i < to; i++)
                    if(mag[i] > threshold) 
                        freq_played.push_back(std::make_pair(indexToFrequency(i), mag[i]));

                sort(freq_played.begin(), freq_played.end(), 
                        [&](std::pair<uint16_t, float32_t> a, 
                            std::pair<uint16_t, float32_t> b) {
                                return a.second > b.second;
                            });
                
                for(uint16_t i = 0; i < freq_played.size(); i++) {
                    if(output.size == 0) {
                        output.buf[output.size ++] = freq_played[i].first;
                        continue;
                    }

                    bool similar_found = false;
                    for (uint16_t j = 0; j < output.size; j ++)
                        if(abs(output.buf[j] - freq_played[i].first) <= SIMILAR_FREQ_THRESHOLD)
                            similar_found = true;

                    if(!similar_found) {
                        output.buf[output.size ++] = freq_played[i].first;
                        if(output.size >= MAXIMUM_NUMBER_OF_FREQUENCIES)
                            break;
                    }
                }
            }
    
            if(recogniser) 
                recogniser -> pullRequest();
        }
    }

    return DEVICE_OK;
}


int MicroBitAudioProcessor::setDivisor(int d)
{
    divisor = d;
    return DEVICE_OK;
}


void MicroBitAudioProcessor::startRecording()
{
    this->recording = true;
    DMESG("START RECORDING");
}

void MicroBitAudioProcessor::stopRecording(MicroBit& uBit)
{
    this->recording = false;
    DMESG("STOP RECORDING");
}