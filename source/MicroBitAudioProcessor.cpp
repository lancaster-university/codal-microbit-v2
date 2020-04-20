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
    //arm_rfft_fast_init_f32(&fft_instance, AUDIO_SAMPLES_NUMBER);

    /* Double Buffering: We allocate twice the number of samples*/
    buf = (float *)malloc(sizeof(float) * AUDIO_SAMPLES_NUMBER * 2);
    output = (float *)malloc(sizeof(float) * AUDIO_SAMPLES_NUMBER);

    position = 0;

    if (buf == NULL || output == NULL) {
        DMESG("DEVICE_NO_RESOURCES");
        target_panic(DEVICE_OOM);
    }
}

MicroBitAudioProcessor::~MicroBitAudioProcessor()
{
    free(buf);
    free(output);
}

int MicroBitAudioProcessor::pullRequest()
{
    int z = 0;
    int minimum = 0;
    int maximum = 0;
    int s;
    int8_t result;

    auto mic_samples = audiostream.pull().getBytes();

    int16_t *data = (int16_t *) &mic_samples[0];
    int samples = AUDIO_SAMPLES_NUMBER / 2;

    for (int i=0; i < samples; i++)
    {
        z += *data;

        s = (int) *data;
        s = s - zeroOffset;
        s = s / divisor;
        result = (int8_t)s;

        if (s < minimum)
            minimum = s;

        if (s > maximum)
            maximum = s;

        data++;
        buf[position++] = (float)result;

        if (!(position % AUDIO_SAMPLES_NUMBER))
        {
        /* We have AUDIO_SAMPLES_NUMBER samples, we can run the FFT on them */
            uint8_t offset = position <= AUDIO_SAMPLES_NUMBER ? 0 : AUDIO_SAMPLES_NUMBER;
            if (offset != 0)
                position = 0;

            //arm_rfft_fast_f32(&fft_instance, buf + offset, output, 0);
            DMESG("o[0]: %d", output[0]);
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
