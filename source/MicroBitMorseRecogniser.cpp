#include "MicroBitMorseRecogniser.h"

/*
 * The sound recogniser takes in data from the audio processor - in 
 * form of AudioFrameAnalysis. It then tries to identify whether 
 * the listening frequency was played during that frame. Based on 
 * the last frames it sends dots ('.'), dashes ('-'), letter spaces 
 * (' '), word spaces (';'), and end-of-transmission ('#') to the 
 * data sink connected.
 * 
 * Algorithm
 * 
 * The recogniser first converts the AudioFrameAnalysis data in 
 * booleans which represent whether or not the listening frequency was 
 * being played in that frame. A timeUnit is calculated based on the 
 * sampling frequency and the number of samples taken before analysis 
 * by the audio processor - it represents the number of frames a dot 
 * should take.
 * 
 * It has 2 values: zeros and ones which counts for how many timeUnits 
 * the frequency wasn't being played/ was being played. The value 
 * synchronised keeps track of the last transition - true if the last 
 * transition was from the frequency not being played to it being played, 
 * false otherwise. At each transition, the values of zeros or ones are 
 * being interpreted into the right character and then reset.
 * 
 * All characters are being accumulated into a buffer and send together 
 * when the end-of-transmission character ('#') is found.
 */

/*
 * Constructor. 
 * 
 * Initializes the MicroBitMorseRecogniser.
 * 
 * @param the audio processor it should take data from
 * 
 * @param freq the frequency it should listen for
 * 
 * @param timeUnit the amount of time in ms it takes a dot
 *                 to be transmitted
 */
MicroBitMorseRecogniser::MicroBitMorseRecogniser(MicroBitAudioProcessor& audio_processor, uint16_t freq, uint16_t _timeUnit) 
                                : audio_proceesor(audio_processor), frequency(freq) {
    audio_proceesor.connect(this);

    analysing = false;
    buffer_len = 0;
    output_len = 0;

    timeUnit = 1.0 * MIC_SAMPLE_RATE * _timeUnit / 1000 / MORSE_AUDIO_SAMPLES_NUMBER + 0.5;

    memset(buffer, 0, sizeof(buffer));
    memset(output, 0, sizeof(output));

    synchronised = false;
    zeros = 0;
    ones = 0;

    // Compensate for distorsion
    frequency -= 50;

}

/*
 * Destructor.
 *
 * Deallocates all the memory allocated dynamically.
 */
MicroBitMorseRecogniser::~MicroBitMorseRecogniser(){
}

/*
 * Starts analysing the data that comes in.
 */
void MicroBitMorseRecogniser::startAnalysing(){
    analysing = true;
    audio_proceesor.startRecording();
}

/*
 * Stops analysing the data and also stops the audio processor
 * from receiving.
 */
void MicroBitMorseRecogniser::stopAnalysing(){
    analysing = false;
    buffer_len = 0;
    audio_proceesor.stopRecording();
}

/*
 * A callback for when the data is ready.
 */
int MicroBitMorseRecogniser::pullRequest() {
    auto frames = audio_proceesor.pull();

    if(!analysing) return DEVICE_OK;

    MicroBitAudioProcessor::AudioFrameAnalysis* buf = (MicroBitAudioProcessor::AudioFrameAnalysis* ) &frames[0];
    uint16_t size = frames.length() / sizeof(MicroBitAudioProcessor::AudioFrameAnalysis);
    
    for(uint16_t i = 0; i < size; i++)
        processFrame(&buf[i]);
    
    return DEVICE_OK;
}

/*
 * Allow out downstream component to register itself with us
 */
void MicroBitMorseRecogniser::connect(DataSink *sink){
    interpreter = sink;
}

/*
 * Provides the next available data to the downstream caller.
 */
ManagedBuffer MicroBitMorseRecogniser::pull()
{
    uint16_t len = output_len;
    output_len = 0;
    return ManagedBuffer(((uint8_t *) (&output)), len*sizeof(uint8_t));
}

/*
 * Checks if the number of ones in the last timeUnit frames up to 
 * a given position in buffer fit in a given threshold.
 * 
 * @param to specifies the interval [to - timeUnit, to) which is 
 *           analysed 
 * 
 * @param threshold the minimum number of ones in the interval
 * 
 * @return number of trues in buffer[to - timeUnit, to) >= threshold
 * 
 * @note if threshold is 255 thet it will use 
 *       timeUnit * MORSE_FRAME_TRUE_RATE_THRESHOLD as the threshold
 */
bool MicroBitMorseRecogniser::recogniseLastMorseFrame(uint16_t to, uint16_t threshold = 255) {
    if(to < timeUnit) return false;
    if(threshold == 255) threshold = timeUnit * MORSE_FRAME_TRUE_RATE_THRESHOLD;

    uint16_t nr_true = 0;
    for(uint16_t it = to - timeUnit; it < to; it ++)
        if(buffer[it]) nr_true ++;

    return nr_true >= threshold;
}

/*
 * Adds a character to the output buffer. If it adds the 
 * end-of-transmission ('#') character, then it sends the buffer 
 * to the next component in the pipeline - 'interpreter'.
 * 
 * @param c the character to be added to the out buffer
 */
void MicroBitMorseRecogniser::pushOut(char c){
    if (output_len == 0 && (c == ' ' || c == ';' || c == '#')) return;
    output[output_len] = c;
    output_len++;
    if (c == '#')
        interpreter->pullRequest();
}

/*
 * Processes a incoming frame.
 *
 * @param frame the frame it should process
 */
void MicroBitMorseRecogniser::processFrame(MicroBitAudioProcessor::AudioFrameAnalysis* frame) {
  
    bool detected_freq = false;
    for (uint16_t i = 0; i < frame -> size && !detected_freq; i++ )
        if(abs(frame -> buf[i] - frequency) < DETECTION_THRESHOLD) 
            detected_freq = true;

    buffer[buffer_len] = detected_freq;
    buffer_len ++;

    if(buffer_len < timeUnit) return;

    uint16_t true_threshold  = timeUnit * MORSE_FRAME_TRUE_RATE_THRESHOLD;
    uint16_t false_threshold = timeUnit * MORSE_FRAME_FALSE_RATE_THRESHOLD;

    if(!synchronised && recogniseLastMorseFrame(buffer_len, true_threshold) && 
                        buffer[buffer_len - timeUnit]) {
        
        zeros += (int)((0.5 + buffer_len) / timeUnit) - 1; 

        if(zeros > 1 && zeros <= 4) pushOut(' ');
        else if (zeros > 4 && zeros <= 8) pushOut(';'); 

        buffer_len = 0;
        zeros = 0;
        ones = 1;
        synchronised = true;

        return;
    } 

    if(synchronised && !recogniseLastMorseFrame(buffer_len, timeUnit - false_threshold) && 
                        !buffer[buffer_len - timeUnit]) {
        ones += (int)((0.5 + buffer_len) / timeUnit) - 1;   

        if(ones > 2) 
            pushOut('-');
        else 
            pushOut('.');
                         
        buffer_len = 0;
        synchronised = false;
        zeros = 1;
        ones = 0;
        return;
    }

    if(buffer_len == 2 * timeUnit) {

        if(zeros == 9) 
            pushOut('#');
        
        if (synchronised)
            ones ++;
        else 
            zeros ++;

        memcpy(buffer, &buffer[timeUnit], sizeof(bool) * timeUnit);
        buffer_len = timeUnit;
    }
}

