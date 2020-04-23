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
    audiostream.connect(*this);
    zeroOffset = 0;
    divisor = 1;
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
}

MicroBitAudioProcessor::~MicroBitAudioProcessor()
{
    free(buf);
    free(output);
    free(mag);
}

int MicroBitAudioProcessor::pullRequest()
{
    int z = 0;
    int minimum = 0;
    int maximum = 0;
    int s;
    int result;

    auto mic_samples = audiostream.pull();

    if (!recording)
        return DEVICE_OK;

    int16_t *data = (int16_t *) &mic_samples[0];
    int samples = mic_samples.length() / 2;

    for (int i=0; i < samples; i++)
    {
        z += *data;

        s = (int) *data;
        //s = s - zeroOffset;
        //s = s / divisor;
        result = s;

        if (s < minimum)
            minimum = s;

        if (s > maximum)
            maximum = s;

        //if (recording)
        //    rec[position] = (float)result;

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

            DMESG("Run FFT, %d", offset);
            arm_rfft_fast_f32(&fft_instance, buf + offset, output, 0);
            arm_cmplx_mag_f32(output, mag, AUDIO_SAMPLES_NUMBER / 2);
            arm_max_f32(mag + 1, AUDIO_SAMPLES_NUMBER / 2 - 1, &maxValue, &index);

            uint32_t freq = ((uint32_t)MIC_SAMPLE_RATE / AUDIO_SAMPLES_NUMBER) * (index + 1);
            DMESG("Freq: %d (max: %d.%d, Index: %d)",
                  freq,
                  (int)maxValue,
                  ((int)(maxValue * 100) % 100),
                  index);
        }
    }

    z = z / samples;
    zeroOffset = z;
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
