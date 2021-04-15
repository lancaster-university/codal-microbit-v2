
#include "MicroBitSoundRecogniser.h"

MicroBitSoundRecogniser::MicroBitSoundRecogniser(MicroBitAudioProcessor& audio_processor, MicroBit& uBit) 
                                : audio_proceesor(audio_processor), uBit(uBit) {
    analysing = false;
    audio_proceesor.connect(this);
    buffer_len = 0;
    sounds_size = 0;
}

MicroBitSoundRecogniser::~MicroBitSoundRecogniser(){
    if(sounds_size != 0){
        for(uint8_t i = 0; i < sounds_size; i++) {
            delete sounds[i];
            delete sounds_names[i];
        }
        delete [] sounds;
        delete [] sounds_names;
    }
}

int MicroBitSoundRecogniser::pullRequest(){

    auto frames = audio_proceesor.pull();

    if(!analysing) return DEVICE_OK;

    // I only send one at a time when calling pullRequest from MicroBitAudioProcessor.
    // Is there a way to make them concurrent -- might save some resources?
    MicroBitAudioProcessor::AudioFrameAnalysis* buf = (MicroBitAudioProcessor::AudioFrameAnalysis* ) &frames[0];
    buffer[buffer_len].size = buf[0].size;
    for(uint8_t i = 0; i<buf[0].size; i++)  
        buffer[buffer_len].buf[i] = buf[0].buf[i];
    buffer_len ++;

    // gives an amortised O(1) for keeping a buffer in order. 
    // could be changed to a circular buffer to reduce the memory
    // to 1/2 if needed -- more complex functions for accessing it 
    // would be necessary which might have a greater constant than this 
    if(buffer_len == 2 * HISTORY_LENGTH){ 
        memcpy(&buffer[0], &buffer[HISTORY_LENGTH], 
                        sizeof(MicroBitAudioProcessor::AudioFrameAnalysis) * HISTORY_LENGTH);
        buffer_len = HISTORY_LENGTH;
    }

    for(uint8_t sound_it; sound_it < sounds_size; sound_it ++){
        sounds[sound_it] -> update(buffer, buffer_len);
        if(sounds[sound_it] -> matched()){
            if(callback != NULL)
                callback(*sounds_names[sound_it]);
            return DEVICE_OK;
        }
    }

    return DEVICE_OK;
}

MicroBitAudioProcessor* MicroBitSoundRecogniser::getAudioProcessor(){
    return &audio_proceesor;
}

void MicroBitSoundRecogniser::setCallback(void (*_callback)(ManagedString)){
    callback = _callback;
}

void MicroBitSoundRecogniser::startAnalisying(void (*_callback)(ManagedString)){
    setCallback(_callback);
    analysing = true;
    audio_proceesor.startRecording();
}

void MicroBitSoundRecogniser::stopAnalisying(){
    analysing = false;
    buffer_len = 0;
    audio_proceesor.stopRecording(uBit);
    for(uint8_t sound_it; sound_it < sounds_size; sound_it ++)
        sounds[sound_it] -> resetHistory();
}

MicroBitSoundRecogniser::SoundSample::SoundSample(const uint16_t* _frames, uint8_t size) 
                                    : size(size) {
    frames = new uint16_t[size];
    memcpy(frames, _frames, sizeof(uint16_t) * size);
}

MicroBitSoundRecogniser::SoundSample::~SoundSample() { 
    delete[] frames; 
}

MicroBitSoundRecogniser::SoundSequence::SoundSequence(  uint8_t size, 
                                                        uint32_t threshold, 
                                                        uint8_t deviation)
                                    : size(size), threshold(threshold), 
                                      deviation(deviation) {
    samples = new SoundSample* [size];
}

MicroBitSoundRecogniser::SoundSequence::~SoundSequence() { 
    for(uint8_t i = 0 ; i < size; i++)
        delete samples[i];
    delete [] samples; 
}

MicroBitSoundRecogniser::Sound::Sound(uint8_t size, uint8_t max_zeros, uint8_t max_history_len) 
                            : size(size), max_zeros(max_zeros), history_len(0), 
                              max_history_len(max_history_len){
    sequences   = new SoundSequence* [size];
    history     = new uint8_t[2 * max_history_len * size];
}

MicroBitSoundRecogniser::Sound::~Sound() {
    for(uint8_t i = 0 ; i < size; i++)
        delete sequences[i];
    delete [] sequences;
    delete [] history;
}

void MicroBitSoundRecogniser::Sound::update(MicroBitAudioProcessor::AudioFrameAnalysis* buffer, 
                                    uint8_t buffer_len){
    for(uint8_t seq_it = 0; seq_it < size; seq_it ++)
        addToHistory(seq_it, matchSequence(seq_it, buffer, buffer_len));
    endHistoryFrame();
}

bool MicroBitSoundRecogniser::Sound::matched() {
    if(getZeros(1, size - 1) <= max_zeros){
        history_len = 0;
        return true;
    }
    return false;
}

uint8_t MicroBitSoundRecogniser::Sound::matchSequence(uint8_t seq_id,
                                            MicroBitAudioProcessor::AudioFrameAnalysis* buffer, 
                                            uint8_t buffer_len) const {
    SoundSequence* seq = sequences[seq_id];
    uint8_t min_zeros = 255;
    for(uint8_t sample_it = 0; sample_it < seq -> size; sample_it ++) {
        uint8_t sample_len = seq -> samples[sample_it] -> size;
        if(buffer_len < sample_len) continue;
        
        uint8_t zeros = 255;
        if(seq_id == 0) zeros = 0;
        else if (seq_id && zeros > getZeros(sample_len, seq_id - 1))
            zeros = getZeros(sample_len, seq_id - 1);
        else if (seq_id && zeros > getZeros(sample_len, seq_id - 1))
            zeros = getZeros(sample_len, seq_id - 1);

        if(zeros > max_zeros) continue;

        uint32_t    dist = 0;
        uint32_t    diff = 0;
        uint8_t     nr_of_diffs = 0;
        uint8_t     deviations_left = seq->deviation;
        
        for(uint8_t i = 0; i < sample_len; i++ ){
            if(seq -> samples[sample_it] -> frames[i] == 0) continue;
            if(buffer[buffer_len - sample_len + i ].size == 0){
                zeros ++;
                continue;
            }
            
            uint16_t freq = seq -> samples[sample_it] -> frames[i];

            nr_of_diffs ++;
            diff = 10000;
            for(uint8_t j = 0; j < buffer[buffer_len - sample_len + i].size; j++){
                diff = min(diff, max(0, abs(freq - buffer[buffer_len - sample_len + i].buf[j]) - 100 ));
            }

            if(deviations_left && diff*diff > nr_of_diffs * seq -> threshold){
                deviations_left --;
                nr_of_diffs --;
            }
            else
                dist += diff * diff;
        }

        if(dist <= nr_of_diffs * seq -> threshold && min_zeros > zeros) {
            min_zeros = zeros;
        }
    }

    return min_zeros;           
}


uint8_t MicroBitSoundRecogniser::Sound::getZeros(uint8_t frames_ago, uint8_t seq_id) const {
    if(history_len < frames_ago) return 255;
    return history[(history_len - frames_ago) * size + seq_id];
}

void MicroBitSoundRecogniser::Sound::addToHistory(uint8_t seq_id, uint8_t value){
    history[history_len * size + seq_id] = value;
}

void MicroBitSoundRecogniser::Sound::endHistoryFrame(){
    history_len ++;
    // same type of buffer as the buffer from MicroBitSoundRecogniser
    if(history_len == 2 * max_history_len) {
        memcpy(&history[0], &history[max_history_len * size], sizeof(uint8_t) * max_history_len * size);
        history_len = max_history_len;
    }
}

void MicroBitSoundRecogniser::Sound::resetHistory(){
    history_len = 0;
}



