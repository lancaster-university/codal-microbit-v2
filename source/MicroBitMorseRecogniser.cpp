#include "MicroBitMorseRecogniser.h"


MicroBitMorseRecogniser::MicroBitMorseRecogniser(MicroBitAudioProcessor& audio_processor, uint16_t freq, uint16_t _timeUnit) 
                                : audio_proceesor(audio_processor), frequency(freq) {
    audio_proceesor.connect(this);

    analysing = false;
    buffer_len = 0;
    output_len = 0;

    timeUnit = 1.0 * MIC_SAMPLE_RATE * _timeUnit / 1000 / MORSE_AUDIO_SAMPLES_NUMBER + 0.5;

    memset(buffer, 0, sizeof(buffer));
    memset(normalised_buffer, 0, sizeof(normalised_buffer));
    memset(output, 0, sizeof(output));

    syncronised = false;
    zeros = 0;
    ones = 0;

    // Compensate for distorsion
    frequency -= 50;

}

MicroBitMorseRecogniser::~MicroBitMorseRecogniser(){
}

MicroBitAudioProcessor* MicroBitMorseRecogniser::getAudioProcessor(){
    return &audio_proceesor;
}

void MicroBitMorseRecogniser::startAnalisying(){
    analysing = true;
    audio_proceesor.startRecording();
}

void MicroBitMorseRecogniser::stopAnalisying(){
    analysing = false;
    buffer_len = 0;
    audio_proceesor.stopRecording();
}

int MicroBitMorseRecogniser::pullRequest() {
    auto frames = audio_proceesor.pull();

    if(!analysing) return DEVICE_OK;

    MicroBitAudioProcessor::AudioFrameAnalysis* buf = (MicroBitAudioProcessor::AudioFrameAnalysis* ) &frames[0];
    uint16_t size = frames.length() / sizeof(MicroBitAudioProcessor::AudioFrameAnalysis);
    
    for(uint16_t i = 0; i < size; i++)
        processFrame(&buf[i]);
    
    return DEVICE_OK;
}


void MicroBitMorseRecogniser::connect(DataSink *sink){
    interpreter = sink;
}

ManagedBuffer MicroBitMorseRecogniser::pull()
{
    uint16_t len = output_len;
    output_len = 0;
    return ManagedBuffer(((uint8_t *) (&output)), len*sizeof(uint8_t));
}


bool MicroBitMorseRecogniser::recogniseLastMorseFrame(uint16_t to, uint16_t threshold = 255) {
    if(to < timeUnit) return false;
    if(threshold == 255) threshold = timeUnit * MORSE_FRAME_TRUE_RATE_THRESHOLD;

    uint16_t nr_true = 0;
    for(uint16_t it = to - timeUnit; it < to; it ++)
        if(buffer[it]) nr_true ++;

    return nr_true >= threshold;
}

void MicroBitMorseRecogniser::pushOut(char c){
    if (output_len == 0 && (c == ' ' || c == ';' || c == '#')) return;
    output[output_len] = c;
    output_len++;
    if (c == '#')
        interpreter->pullRequest();
}

void MicroBitMorseRecogniser::processFrame(MicroBitAudioProcessor::AudioFrameAnalysis* frame) {
  
    bool detected_freq = false;
    for (uint16_t i = 0; i < frame -> size && !detected_freq; i++ )
        if(abs(frame -> buf[i] - frequency) < DETECTION_THRESHOLD) 
            detected_freq = true;

    buffer[buffer_len] = detected_freq;
    buffer_len ++;

    if(buffer_len < timeUnit) return;

    uint16_t threshold = timeUnit * MORSE_FRAME_TRUE_RATE_THRESHOLD;

    if(!syncronised && recogniseLastMorseFrame(buffer_len, threshold) && 
                        buffer[buffer_len - timeUnit]) {
        
        zeros += (int)((0.5 + buffer_len) / timeUnit) - 1; 

        if(zeros > 1 && zeros <= 4) pushOut(' ');
        else if (zeros > 4 && zeros <= 8) pushOut(';'); 

        buffer_len = 0;
        zeros = 0;
        ones = 1;
        syncronised = true;

        return;
    } 

    if(syncronised && !recogniseLastMorseFrame(buffer_len, timeUnit - threshold) && 
                        !buffer[buffer_len - timeUnit]) {
        ones += (int)((0.5 + buffer_len) / timeUnit) - 1;   

        if(ones > 2) 
            pushOut('-');
        else 
            pushOut('.');
                         
        buffer_len = 0;
        syncronised = false;
        zeros = 1;
        ones = 0;
        return;
    }

    if(buffer_len == 2 * timeUnit) {

        if(zeros == 9) 
            pushOut('#');
        
        if (syncronised)
            ones ++;
        else 
            zeros ++;

        memcpy(buffer, &buffer[timeUnit], sizeof(bool) * timeUnit);
        buffer_len = timeUnit;
    }
}

