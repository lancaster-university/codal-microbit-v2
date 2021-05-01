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

/*
 * Provides the fundamental frequencies in the microphone data.
 * 
 * It takes in the microphone data (sampled at MIC_SAMPLE_RATE Hz 
 * which is ~11000 Hz now) and produces AudioFrameAnalysis data.
 * 
*/

// Default configuration values
#define MIC_SAMPLE_RATE         (1000000 / MIC_SAMPLE_DELTA)
#define DEFAULT_AUDIO_SAMPLES_NUMBER    512
#define EMOJI_AUDIO_SAMPLES_NUMBER      512
#define MORSE_AUDIO_SAMPLES_NUMBER      128

#define RECOGNITION_START_FREQ  1700
#define RECOGNITION_END_FREQ    5400

#define ANALYSIS_STD_MULT_THRESHOLD     3
#define ANALYSIS_STD_THRESHOLD          50
#define ANALYSIS_MEAN_THRESHOLD         0 

#define MAXIMUM_NUMBER_OF_FREQUENCIES   3
#define SIMILAR_FREQ_THRESHOLD          100

class MicroBitAudioProcessor : public DataSink, public DataSource
{
public:

    /*
     * An AudioFrameAnalysis has the fundamental frequencies of a 
     * frame - maximum MAXIMUM_NUMBER_OF_FREQUENCIES and ordered
     * from the most likely to the least. 
     */ 
    struct AudioFrameAnalysis {
        uint8_t size;
        uint16_t buf[MAXIMUM_NUMBER_OF_FREQUENCIES];
    };

private:
    
    DataSource      &audiostream;             // the stream of data to analyse 
    DataSink        *recogniser;              // the recogniser the frequencies should be send to 
    uint16_t        audio_samples_number;     // the number of samples to collect before analysing a frame
    arm_rfft_fast_instance_f32 fft_instance;  // the instance of CMSIS fft that is used to run fft
    float           *buf;                     // the buffer to store the incoming data
    float           *fft_output;              // an array to store the result of the fft
    float           *mag;                     // an array to store the magnitudes of the frequencies 

    uint16_t    buf_len;                      // the length of the incoming buffer
    bool        recording;                    // whether it should analyse the data or be idle

    AudioFrameAnalysis  output;               // the result of the analysis  

    /*
     * Converts from frequency to the index in the array.
     * 
     * @param freq a frequency in the range 0 - 5000 Hz. 
     * 
     * @return the index to the frequency bucket freq is in 
     *         as it comes out of the fft
     */
    uint16_t    frequencyToIndex(int freq);

    /*
     * Converts from the index in the array to frequency.
     * 
     * @param index a index in the range 0 - audio_samples_number / 2. 
     * 
     * @return the avg frequency in the bucket
     */
    float32_t   indexToFrequency(int index);

    public:

    /*
     * Constructor.
     * 
     * Initialize the MicroBitAduioProcessor.
     */
    MicroBitAudioProcessor(DataSource& source, uint16_t audio_samples_number = DEFAULT_AUDIO_SAMPLES_NUMBER); 
    
    /*
     * Destructor.
     * 
     * Deallocates all the memory allocated dynamically.
     */
    ~MicroBitAudioProcessor(); 

    /*
     * A callback for when the data is ready.
     *
     * Analyses the data when enough of it comes in, using 
     * the following algorithm:
     * 
     * The audio processor accumulates microphone data as it comes
     * in and after getting audio_samples_number of them it process 
     * the frame.
     * 
     * It transforms the date from time domain to frequency domain
     * using the CMSIS fft. 
     * 
     * If the mean of the magnitudes of frequnecies is lower than
     * ANALYSIS_MEAN_THRESHOLD or the standard deviation (std) is
     * lower than ANALYSIS_STD_THRESHOLD then the frame is considered 
     * silence - no fundamental frequency. 
     * 
     * It then filters out the frequencies that have a magnitude lower 
     * than the mean + ANALYSIS_STD_MULT_THRESHOLD * std. This ensures 
     * that only outlier frequencies are being considered.
     * 
     * It then filters out the neighbour frequencies around the peaks.
     * 
     * Some of these operations are implemented together to optimize the 
     * algorithm.
     */
    virtual int pullRequest();

    /*
     * Allow out downstream component to register itself with us
     */
    void connect(DataSink *downstream);

    /*
     * Provides the next available data to the downstream caller.
     */
    virtual ManagedBuffer pull();
    
    /*
     * Starts recording and analysing.
     */
    void startRecording();

    /*
     * Stops from recording and analysing.
     */
    void stopRecording();
};

#endif