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



MicroBitAudioProcessor::MicroBitAudioProcessor(DataSource& source) 
                            : audiostream(source), recogniser(NULL) {
    
    divisor = 1;
    lastFreq = 0;
    arm_rfft_fast_init_f32(&fft_instance, AUDIO_SAMPLES_NUMBER);

    /* Double Buffering: We allocate twice the number of samples*/
    // the C++ way for doing this is (as far as I know)
    // new float32_t[AUDIO_SAMPLES_NUMBER * 2];
    buf = (float *)malloc(sizeof(float) * AUDIO_SAMPLES_NUMBER * 2);
    output = (float *)malloc(sizeof(float) * AUDIO_SAMPLES_NUMBER);
    mag = (float *)malloc(sizeof(float) * AUDIO_SAMPLES_NUMBER / 2);
    
    memset(buf, 0, sizeof(buf));

#ifdef HANN_WINDOW 
    for(int i=0; i < AUDIO_SAMPLES_NUMBER; i++)
        hann_window[i] = 0.5 * (1 - arm_cos_f32(2 * 3.14159265 * i / AUDIO_SAMPLES_NUMBER));
#endif

    position = 0;
    recording = false;

    if (buf == NULL || output == NULL || mag == NULL) {
        DMESG("DEVICE_NO_RESOURCES");
        target_panic(DEVICE_OOM);
    }
    DMESG("%s %p", "Audio Processor connecting to upstream, Pointer to me : ", this);
    audiostream.connect(*this);
}

MicroBitAudioProcessor::~MicroBitAudioProcessor()
{
    free(buf);
    free(output);
    free(mag);
}

uint16_t MicroBitAudioProcessor::frequencyToIndex(int freq) {
    return (freq / ((uint32_t)MIC_SAMPLE_RATE / AUDIO_SAMPLES_NUMBER));
} 

float32_t MicroBitAudioProcessor::indexToFrequency(int index) {
    return ((uint32_t)MIC_SAMPLE_RATE / AUDIO_SAMPLES_NUMBER) * index;
} 

void MicroBitAudioProcessor::sendAnalysis(uint16_t* freq, uint8_t size) {

    if(consumed_buffer) 
        out_buffer_len = 0;
    
    if(out_buffer_len == 2 * HISTORY_LENGTH){
        memcpy(out_buffer, &out_buffer[HISTORY_LENGTH], (sizeof(AudioFrameAnalysis) * HISTORY_LENGTH));
        out_buffer_len = HISTORY_LENGTH;
    }

    out_buffer[out_buffer_len].size = min(size, 3);
    if(size > 2)
        out_buffer[out_buffer_len].buf[2] = freq[2];
    if(size > 1)
        out_buffer[out_buffer_len].buf[1] = freq[1];
    if(size > 0)
        out_buffer[out_buffer_len].buf[0] = freq[0];
    out_buffer_len ++;

    if(recogniser)      
        recogniser -> pullRequest();
}

void MicroBitAudioProcessor::connect(DataSink *downstream){
    recogniser = downstream;
}

ManagedBuffer MicroBitAudioProcessor::pull()
{
    consumed_buffer = true;
    return ManagedBuffer((uint8_t*) out_buffer, sizeof(AudioFrameAnalysis) * out_buffer_len);
}


int MicroBitAudioProcessor::pullRequest()
{

    int s;
    int result;

    auto mic_samples = audiostream.pull();

    if (!recording)
        return DEVICE_OK;

    // Should be int8_t as this is the type the microphone
    // records it   
    int8_t *data = (int8_t *) &mic_samples[0];

    int samples = mic_samples.length();

    for (int i=0; i < samples; i++)
    {


        s = (int) *data;
        result = s;

        data++;
        buf[position++] = (float)result;


        if (!(position % AUDIO_SAMPLES_NUMBER))
        {
            position = 0;

            uint16_t from   = frequencyToIndex(RECOGNITION_START_FREQ);
            uint16_t to     = min(frequencyToIndex(RECOGNITION_END_FREQ), AUDIO_SAMPLES_NUMBER / 2);
            uint16_t size   = to - from;

            uint16_t    result_freq[size];
            uint8_t     result_size = 0;

        #ifdef HANN_WINDOW
            // arm_mult_f32 not found in arm_math.h? -- using an old version of arm_math.h
            // arm_mult_f32(buf, hann_window, buf, AUDIO_SAMPLES_NUMBER);
            for (uint16_t i=0;i<size;i++)
                mag[i] *= hann_window[i];
        #endif
        
        arm_rfft_fast_f32(&fft_instance, buf , output, 0);

        #ifdef HARMONIC_PRODUCT_SPECTRUM
            for (uint16_t i=from;i<to;i++)
                mag[i] *= mag[i * 2];
        #endif

            arm_cmplx_mag_f32(output+ from, mag, size);

        #ifdef SQUARE_BEFORE_ANALYSIS
            // arm_mult_f32 not found in arm_math.h? -- using an old version of arm_math.h
            // arm_mult_f32(mag, mag, mag, size);
            for (uint16_t i=0;i<size;i++)
                mag[i] *= mag[i];
        #endif

            float32_t maxFreq = 0;
            float32_t mean = 0;
            float32_t std = 0;
            
            arm_mean_f32 (mag, size, &mean);
            arm_std_f32  (mag, size, &std);

            float32_t threshold = mean + std * ANALYSIS_STD_MULT_THRESHOLD;
            
            if(std > ANALYSIS_STD_THRESHOLD && mean > ANALYSIS_MEAN_THRESHOLD) {
                result_freq[0] = 0;
                for(uint16_t i=0; i < size; i++)
                    // for now just picking the maximum if satisfies the conditions 
                    if(mag[i] > threshold && mag[i] > result_freq[0])
                        result_freq[0] = indexToFrequency(i + from), result_size = 1;
                
            }
   
            sendAnalysis(result_freq, result_size);
        }
    }

    return DEVICE_OK;
}

int MicroBitAudioProcessor::getFrequency(){
    return lastFreq;
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