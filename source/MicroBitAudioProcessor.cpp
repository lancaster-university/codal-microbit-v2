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

MicroBitAudioProcessor::MicroBitAudioProcessor(DataSource& source) : audiostream(source)
{
    
    divisor = 1;
    lastFreq = 0;
    arm_rfft_fast_init_f32(&fft_instance, AUDIO_SAMPLES_NUMBER);

    /* Double Buffering: We allocate twice the number of samples*/
    buf = (float *)malloc(sizeof(float) * AUDIO_SAMPLES_NUMBER * 2);
    output = (float *)malloc(sizeof(float) * AUDIO_SAMPLES_NUMBER);
    mag = (float *)malloc(sizeof(float) * AUDIO_SAMPLES_NUMBER / 2);

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

int MicroBitAudioProcessor::pullRequest()
{

    int s;
    int result;

    auto mic_samples = audiostream.pull();

    if (!recording)
        return DEVICE_OK;

    //using 8 bits produces more accurate to input results (not 2x like using 16) but issue with 
    //F and G both producing 363hz?

    // A 440 matches perfectly, but the rest of the notes dont?
    //int8_t *data = (int8_t *) &mic_samples[0];

    //Legacy Version
    int16_t *data = (int16_t *) &mic_samples[0];

    int samples = mic_samples.length() / 2;

    for (int i=0; i < samples; i++)
    {


        s = (int) *data;
        result = s;

        data++;
        buf[position++] = (float)result;


        if (!(position % AUDIO_SAMPLES_NUMBER))
        {
            float maxValue = 0;
            uint32_t index = 0;

            /* We have AUDIO_SAMPLES_NUMBER samples, we can run the FFT on them */
            uint16_t offset = position <= AUDIO_SAMPLES_NUMBER ? 0 : AUDIO_SAMPLES_NUMBER;
            if (offset != 0)
                position = 0;

            //DMESG("Run FFT, %d", offset);
            //auto a = system_timer_current_time();
            arm_rfft_fast_f32(&fft_instance, buf + offset, output, 0);
            arm_cmplx_mag_f32(output, mag, AUDIO_SAMPLES_NUMBER / 2);
            arm_max_f32(mag + 1, AUDIO_SAMPLES_NUMBER / 2 - 1, &maxValue, &index);
            //auto b = system_timer_current_time();

            //DMESG("Before FFT: %d", (int)a);
            //DMESG("After FFT: %d (%d)", (int)b, (int)(b - a));

            uint32_t freq = ((uint32_t)MIC_SAMPLE_RATE / AUDIO_SAMPLES_NUMBER) * (index + 1);
            lastFreq = (int) freq;
            // DMESG("Freq: %d (max: %d.%d, Index: %d)",
            //       freq,
            //       (int)maxValue,
            //       ((int)(maxValue * 100) % 100),
            //       index);
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