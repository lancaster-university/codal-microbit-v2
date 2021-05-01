#include "MicroBitMorseCodeRecogniser.h"


MicroBitMorseCodeRecogniser::MicroBitMorseCodeRecogniser(MicroBitAudioProcessor& audio_processor, MicroBit& uBit, uint16_t freq, uint16_t _timeUnit) 
                                : audio_proceesor(audio_processor), uBit(uBit), frequency(freq) {
    analysing = false;
    audio_proceesor.connect(this);
    buffer_len = 0;

    timeUnit = 1.0 * MIC_SAMPLE_RATE * _timeUnit / 1000 / MORSE_AUDIO_SAMPLES_NUMBER + 0.5;

    memset(buffer, 0, sizeof(buffer));
    memset(normalised_buffer, 0, sizeof(normalised_buffer));

    syncronised = false;
    pauses = 0;

    uBit.serial.send("time unit: ") ;
    uBit.serial.send(ManagedString((int) timeUnit)) ;
    uBit.serial.send("\n") ;
}

MicroBitMorseCodeRecogniser::~MicroBitMorseCodeRecogniser(){
}

MicroBitAudioProcessor* MicroBitMorseCodeRecogniser::getAudioProcessor(){
    return &audio_proceesor;
}

void MicroBitMorseCodeRecogniser::setCallback(void (*_callback)(ManagedString)){
    callback = _callback;
}

void MicroBitMorseCodeRecogniser::startAnalisying(void (*_callback)(ManagedString)){
    setCallback(_callback);
    analysing = true;
    audio_proceesor.startRecording();
}

void MicroBitMorseCodeRecogniser::stopAnalisying(){
    analysing = false;
    buffer_len = 0;
    audio_proceesor.stopRecording();
}


int MicroBitMorseCodeRecogniser::pullRequest() {

    auto frames = audio_proceesor.pull();

    if(!analysing) return DEVICE_OK;

    MicroBitAudioProcessor::AudioFrameAnalysis* buf = (MicroBitAudioProcessor::AudioFrameAnalysis* ) &frames[0];
    uint16_t size = frames.length() / sizeof(MicroBitAudioProcessor::AudioFrameAnalysis);
    
    for(uint16_t i = 0; i < size; i++)
        processFrame(&buf[i]);
    
    return DEVICE_OK;
}


bool MicroBitMorseCodeRecogniser::recogniseLastMorseFrame(uint16_t to, uint16_t threshold = 255) {
    if(to < timeUnit) return false;
    if(threshold == 255) threshold = timeUnit * MORSE_FRAME_TRUE_RATE_THRESHOLD;

    uint16_t nr_true = 0;
    for(uint16_t it = to - timeUnit; it < to; it ++)
        if(buffer[it]) nr_true ++;

    return nr_true >= threshold;
}

void MicroBitMorseCodeRecogniser::processFrame(MicroBitAudioProcessor::AudioFrameAnalysis* frame) {

    // if (frame -> size >= 1) 
    //     uBit.serial.send(ManagedString((int)frame -> buf[0]));
    // uBit.serial.send("\n");
    
    // uBit.serial.send("\n");

    bool detected_freq = false;
    for (uint16_t i = 0; i < frame -> size && !detected_freq; i++ )
        if(abs(frame -> buf[i] - frequency) < DETECTION_THRESHOLD) 
            detected_freq = true;

    bool added = false;

    buffer[buffer_len] = detected_freq;
    buffer_len ++;


    // uBit.serial.send(ManagedString((int)detected_freq) );

    if(!syncronised && buffer_len > timeUnit && recogniseLastMorseFrame(buffer_len, timeUnit - 1) && 
                                                buffer[buffer_len - timeUnit]) {
        normalised_buffer_len = 0;
        buffer_len = 0;
        syncronised = true;

        normalised_buffer[normalised_buffer_len] = true;
        normalised_buffer_len ++;

        added = true;

        if(pauses <= 4) callback(ManagedString(" "));
        else   callback(ManagedString(";"));
        
        pauses = 0;

        // if(normalised_buffer[normalised_buffer_len-1]) uBit.serial.send("   1\n");
        // else  uBit.serial.send("   .\n");
    } 

    if(syncronised && buffer_len == timeUnit){
        normalised_buffer[normalised_buffer_len] = recogniseLastMorseFrame(buffer_len);
        normalised_buffer_len ++;
        buffer_len = 0;

        added = true;

        // if(normalised_buffer[normalised_buffer_len-1]) uBit.serial.send("   1\n");
        // else  uBit.serial.send("   .\n");
    }

    if(buffer_len == 2 * timeUnit) {
        pauses ++;
        if (syncronised)
            normalised_buffer[normalised_buffer_len ++] = recogniseLastMorseFrame(buffer_len - timeUnit);
        memcpy(buffer, &buffer[timeUnit], sizeof(bool) * timeUnit);
        buffer_len = timeUnit;

        // uBit.serial.send("|");

        added = true;
    }


    if(!syncronised || !added) return;

    // end of word 
    if(normalised_buffer_len == 6) {
        callback(ManagedString(";"));
        syncronised = false;
        normalised_buffer_len = 0;
        return;
    }

    if (normalised_buffer_len == 1 && !normalised_buffer[0]) {
        pauses ++;
        normalised_buffer_len = 0;
        syncronised = false;
    }

    // Not enough data 
    if (normalised_buffer_len < 2) return;

    bool firstFrame = normalised_buffer[normalised_buffer_len - 2];
    bool secondFrame = normalised_buffer[normalised_buffer_len - 1];
    
    // end of symbol
    if(firstFrame && !secondFrame) {
        // too short for dash => dot
        if(normalised_buffer_len <= 2)     
            callback(ManagedString("."));
        else 
            callback(ManagedString("-"));

        pauses = 1;
        normalised_buffer_len = 0;
        return;
    }
}

