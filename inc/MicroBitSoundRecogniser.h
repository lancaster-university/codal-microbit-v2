
#ifndef MICROBIT_SOUND_RECOGNISER_H
#define MICROBIT_SOUND_RECOGNISER_H

#include "MicroBit.h"
#include "DataStream.h"
#include "MicroBitAudioProcessor.h"
#include "arm_math.h"

/*
 *
 * The sound recogniser takes in data from the audio processor - in
 * form of AudioFrameAnalysis. It then tries to match the history 
 * against samples of sounds. 
 *  
 *  
 * Sound fingerprint
 *  
 * A sound has multiple sequences, each sequence having multiple samples
 * to account for the randomness. 
 *  
 * Each sequence also has:
 *     a threshold -   the maximum absolute difference from the sampled 
 *                     frequency and the heard frequency.  
 *     maximum number of deviations - the maximum number of datapoints 
 *                     that can be more than the threshold away from the
 *                     sampled frequency
 *  
 * A sound also has a maximum number of deviations - the total maximum 
 * deviations across all sequences.
 */


/*
 * The maximum length of the buffer.
 * 
 * It should be >= than the maximum length of datapoints in 
 * any sample.
 */
#define HISTORY_LENGTH 30

class MicroBitSoundRecogniser : public DataSink
{   
    private:

        MicroBitAudioProcessor& audio_proceesor;    // the audio processor it takes data from
        
        bool analysing;                             // whether it should analyse the data or be idle

        MicroBitAudioProcessor::AudioFrameAnalysis buffer[2 * HISTORY_LENGTH];  // the buffer to collect the incoming data in
        uint8_t buffer_len;                                                     // the length of the buffer

    protected:

        /*
         * A struct to package a sample.
         */
        struct SoundSample {
            SoundSample(const uint16_t* _frames, uint8_t size);
            ~SoundSample();

            uint8_t         size;   // the number of data points in the sample
            const uint16_t* frames; // the data points 
        };


        /*
         * A struct to package a sequence.
         */
        struct SoundSequence {
            SoundSequence(uint8_t size, uint32_t threshold, uint8_t deviation);
            ~SoundSequence();

            uint8_t         size;       // the number of samples
            uint32_t        threshold;  // the threshold for the sequence
            uint8_t         deviation;  // the maximum number of deviations allowed for the sequence
            SoundSample**   samples;    // ponter to the array of samples
        };


        /*
         * A struct to package a sound.
         */
        struct Sound {
            Sound(uint8_t size, uint8_t max_deviation, uint8_t max_history_len, bool consider_all_frequencies);
            ~Sound();
            
            bool            consider_all_frequencies;   // whether or not to consider all frequencies detected or just the dominant one
            uint8_t         max_deviation;              // the maximum total number of deviations allowed
            uint8_t         size;                       // the number of sequences in the sound
            SoundSequence** sequences;                  // pointer to the array of sequences
            
            /*
             * Update called when new data comes in.
             *
             * @param buffer the buffer with the last data points that came in
             * 
             * @note This keeps the history updated which is needed for the 
             *       dynamic programming approach chosen for matching
             */
            void update( MicroBitAudioProcessor::AudioFrameAnalysis* buffer, 
                         uint8_t buffer_len );

            /*
             * Whether or not the sound matched in the last frame.
             *
             * @return whether or not the sound matched in the last frame.
             * 
             * @note Should be called after update when new data comes in.
             */
            bool matched();

            /*
             * Resets the history buffer.
             *
             * @note Used when the data stops coming in - e.g. when the analyser 
             *       is paused
             */
            void resetHistory();

            private:
            
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
            uint8_t matchSequence(  uint8_t seq_id, 
                                    MicroBitAudioProcessor::AudioFrameAnalysis* buffer, 
                                    uint8_t buffer_len) const;

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
            uint8_t getDeviation(uint8_t frames_ago, uint8_t seq_id) const;

            /*
             * Adds to the history buffer a deviation for a certain sequence.
             *
             * @param seq_id the id of the sequence to add the value to.
             * 
             * @param value the value to be added to the history buffer
             */
            void    addToHistory(uint8_t seq_id, uint8_t value);

            /*
             * Ends a history frame, increasing the length of the buffer. 
             */
            void    endHistoryFrame();

            uint8_t*    history;            // the array of to keep the history buffer in
            uint8_t     history_len;        // the size of the history buffer
            uint8_t     max_history_len;    // the maximum length of the history buffer. Used for double buffering
        };

        /*
         * Constructor.
         * 
         * Initialize the MicroBitSoundRecogniser.
         * 
         * @note is protected to make the class abstract.
         */
        MicroBitSoundRecogniser(MicroBitAudioProcessor& processor);

        /*
         * Virtual function to call when a sound is recognised.
         */
        virtual void recognisedSound(uint16_t id) = 0; 
        
        Sound**         sounds;         // pointer to the array of sounds to recognise
        uint16_t*       sound_ids;      // the array of sound ids
        uint8_t         sounds_size;    // the number of sounds to try to recognise

    public:

        /*
         * Destructor.
         * 
         * Deallocates all the memory allocated dynamically.
         */
        ~MicroBitSoundRecogniser();


        /*
         * A callback for when the data is ready.
         */
        virtual int pullRequest();
        
        /*
         * Starts analysing the data that comes in.
         */
        void startAnalisying();

        
        /*
         * Stops analysing the data and also stops the audio processor
         * from receiving.
         */
        void stopAnalisying();
};

#endif
