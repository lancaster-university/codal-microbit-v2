
#include "MicroBitSoundRecogniser.h"

/*
 * Constructor.
 * 
 * Initialize the MicroBitSoundRecogniser.
 */
MicroBitSoundRecogniser::MicroBitSoundRecogniser(MicroBitAudioProcessor& audio_processor) 
                                : audio_proceesor(audio_processor){
    analysing = false;
    audio_proceesor.connect(this);
    buffer_len = 0;
    sounds_size = 0;
}

/*
 * Destructor.
 * 
 * Deallocates all the memory allocated dynamically.
 */
MicroBitSoundRecogniser::~MicroBitSoundRecogniser(){
    if(sounds_size != 0){
        for(uint8_t i = 0; i < sounds_size; i++) {
            delete sounds[i];
        }
        delete [] sounds;
        delete [] sound_ids;
    }
}

/*
 * A callback for when the data is ready.
 */
int MicroBitSoundRecogniser::pullRequest(){

    auto frames = audio_proceesor.pull();

    if(!analysing) return DEVICE_OK;

    MicroBitAudioProcessor::AudioFrameAnalysis* buf = (MicroBitAudioProcessor::AudioFrameAnalysis* ) &frames[0];
    buffer[buffer_len].size = buf[0].size;
    for(uint8_t i = 0; i<buf[0].size; i++)  
        buffer[buffer_len].buf[i] = buf[0].buf[i];
    buffer_len ++;

    /* Double buffering
     *
     * gives an amortised O(1) for keeping a buffer in order. 
     * could be changed to a circular buffer to reduce the memory
     * to 1/2 if needed -- more complex functions for accessing it 
     * would be necessary which might have a greater impact than this
     */
    if(buffer_len == 2 * HISTORY_LENGTH){ 
        memcpy(&buffer[0], &buffer[HISTORY_LENGTH], 
                        sizeof(MicroBitAudioProcessor::AudioFrameAnalysis) * HISTORY_LENGTH);
        buffer_len = HISTORY_LENGTH;
    }

    for(uint8_t sound_it; sound_it < sounds_size; sound_it ++){
        sounds[sound_it] -> update(buffer, buffer_len);
        if(sounds[sound_it] -> matched()){
            recognisedSound(sound_ids[sound_it]);
            return DEVICE_OK;
        }
    }

    return DEVICE_OK;
}

/*
 * Starts analysing the data that comes in. Also sets the callback. 
 * 
 * @TODO change it to send a message on the message bus 
 *       rather than having a callback
 */
void MicroBitSoundRecogniser::startAnalisying(){
    analysing = true;
    audio_proceesor.startRecording();
}

/*
 * Stops analysing the data and also stops the audio processor
 * from receiving.
 */
void MicroBitSoundRecogniser::stopAnalisying(){
    analysing = false;
    buffer_len = 0;
    audio_proceesor.stopRecording();
    for(uint8_t sound_it; sound_it < sounds_size; sound_it ++)
        sounds[sound_it] -> resetHistory();
}


MicroBitSoundRecogniser::SoundSample::SoundSample(const uint16_t* _frames, uint8_t size) 
                                    : size(size), frames(_frames) { }

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

MicroBitSoundRecogniser::Sound::Sound(uint8_t size, uint8_t max_deviation, uint8_t max_history_len, bool consider_all_frequencies) 
                            : size(size), max_deviation(max_deviation), history_len(0), 
                              max_history_len(max_history_len), consider_all_frequencies(consider_all_frequencies) {
    sequences   = new SoundSequence* [size];
    history     = new uint8_t[2 * max_history_len * size];
}

MicroBitSoundRecogniser::Sound::~Sound() {
    for(uint8_t i = 0 ; i < size; i++)
        delete sequences[i];
    delete [] sequences;
    delete [] history;
}

/*
 * Update called when new data comes in.
 *
 * @param buffer the buffer with the last data points that came in
 * 
 * @note This keeps the history updated which is needed for the 
 *       dynamic programming approach chosen for matching
 */
void MicroBitSoundRecogniser::Sound::update(MicroBitAudioProcessor::AudioFrameAnalysis* buffer, 
                                    uint8_t buffer_len){
    for(uint8_t seq_it = 0; seq_it < size; seq_it ++) {
        uint8_t x = matchSequence(seq_it, buffer, buffer_len);
        addToHistory(seq_it, x);
    }
    endHistoryFrame();
}

/*
 * Whether or not the sound matched in the last frame.
 *
 * @return whether or not the sound matched in the last frame.
 * 
 * @note Should be called after update when new data comes in.
 */
bool MicroBitSoundRecogniser::Sound::matched() {
    if(getDeviation(1, size - 1) <= max_deviation){
        history_len = 0;
        return true;
    }
    return false;
}
            
/* 
 * Matches a sequence to the last couple of data points in the buffer.
 * 
 * @param seq_id the id of the sequence to try to match
 * 
 * @param buffer the buffer of data points that came in
 * 
 * @param buffer_len the length of the buffer
 * 
 * @return the number of deviations in the last data points to the sound up to 
 *         the seq_id sequence or 255 if it's above the maximums allowed.
 */
uint8_t MicroBitSoundRecogniser::Sound::matchSequence(uint8_t seq_id,
                                            MicroBitAudioProcessor::AudioFrameAnalysis* buffer, 
                                            uint8_t buffer_len) const {
    SoundSequence* seq = sequences[seq_id];
    uint8_t min_dev = 255;
    for(uint8_t sample_it = 0; sample_it < seq -> size; sample_it ++) {
        uint8_t sample_len = seq -> samples[sample_it] -> size;
        if(buffer_len < sample_len) continue;
        
        uint8_t deviation = 255;
        if(seq_id == 0) deviation = 0;
        else if (seq_id && deviation > getDeviation(sample_len, seq_id - 1))
            deviation = getDeviation(sample_len, seq_id - 1);
        else if (seq_id && deviation > getDeviation(sample_len + 1, seq_id - 1))
            deviation = getDeviation(sample_len, seq_id - 1);

        if(deviation > max_deviation || deviation >= min_dev) continue;

        uint32_t    diff = 0;
        uint8_t     nr_of_diffs = 0;
        uint8_t     deviations_left = seq->deviation;
        
        for(uint8_t i = 0; i < sample_len; i++) {
            if(seq -> samples[sample_it] -> frames[i] == 0) continue;
            if(buffer[buffer_len - sample_len + i].size == 0) {
                deviation ++;
                continue;
            }
            
            uint16_t freq = seq -> samples[sample_it] -> frames[i];

            diff = abs(freq - buffer[buffer_len - sample_len + i].buf[0]);

            if(consider_all_frequencies)
                for(uint8_t j = 1; j < buffer[buffer_len - sample_len + i].size; j++)
                    diff = min(diff, abs(freq - buffer[buffer_len - sample_len + i].buf[j]) );
                
            if(deviations_left && diff > seq -> threshold && deviation < max_deviation){
                deviations_left --;
                deviation ++;
            } 
            else if(diff > seq -> threshold ){
                deviation = 255;
                break;
            }
        }

        if(deviation < min_dev && deviation <= max_deviation) 
            min_dev = deviation;
        
    }

    return min_dev;           
}

/* 
 * Getter for the internal history buffer of the sound.
 *
 * @param frames_ago the number of frames ago for which the
 *        query was made
 * 
 * @param seq_id the id of the sequence to get the deviation for
 * 
 * @return the deviation (or 255 if it didn't match) up to the 
 *         sequence seq_id that was frames_ago frames ago.
 * 
 * @note used to check if the sound matched up to the first seq_id
 *       sequences so that the matching doesn't need to recheck 
 *       those sequences.
 */
uint8_t MicroBitSoundRecogniser::Sound::getDeviation(uint8_t frames_ago, uint8_t seq_id) const {
    if(history_len < frames_ago) return 255;
    return history[(history_len - frames_ago) * size + seq_id];
}

/*
 * Adds to the history buffer a deviation for a certain sequence.
 *
 * @param seq_id the id of the sequence to add the value to.
 * 
 * @param value the value to be added to the history buffer
 */
void MicroBitSoundRecogniser::Sound::addToHistory(uint8_t seq_id, uint8_t value){
    history[history_len * size + seq_id] = value;
}

/*
 * Ends a history frame, increasing the length of the buffer. 
 */
void MicroBitSoundRecogniser::Sound::endHistoryFrame(){
    history_len ++;
    // double buffering
    if(history_len == 2 * max_history_len) {
        memcpy(&history[0], &history[max_history_len * size], sizeof(uint8_t) * max_history_len * size);
        history_len = max_history_len;
    }
}

/*
 * Resets the history buffer.
 *
 * @note Used when the data stops coming in - e.g. when the analyser 
 *       is paused
 */
void MicroBitSoundRecogniser::Sound::resetHistory(){
    history_len = 0;
}



