
#ifndef MICROBIT_SOUND_RECOGNISER_H
#define MICROBIT_SOUND_RECOGNISER_H

#include "MicroBit.h"
#include "DataStream.h"
#include "MicroBitAudioProcessor.h"
#include "arm_math.h"

// Should be the minimum number of elements needed in the buffer 
// at any time
#define HISTORY_LENGTH 50

class MicroBitSoundRecogniser : public DataSink
{   
    private:
        MicroBitAudioProcessor& audio_proceesor;
        MicroBit&               uBit;
        
        bool analysing;

        void (*callback)(ManagedString) = NULL;

        MicroBitAudioProcessor::AudioFrameAnalysis buffer[2 * HISTORY_LENGTH];
        uint8_t buffer_len;

    protected:
        struct SoundSample {
            SoundSample(const uint16_t* _frames, uint8_t size);
            ~SoundSample();

            uint8_t     size;
            uint16_t*   frames;
        };

        struct SoundSequence {
            SoundSequence(uint8_t size, uint32_t threshold, uint8_t deviation);
            ~SoundSequence();

            uint8_t         size;
            uint32_t        threshold;
            uint8_t         deviation;
            SoundSample**   samples;
        };

        struct Sound {
            Sound(uint8_t size, uint8_t max_deviation, uint8_t max_history_len, bool consider_all_frequencies,  MicroBit& ubit);
            ~Sound();
            
            // debuging only
            MicroBit& ubit;
            
            bool            consider_all_frequencies;
            uint8_t         max_deviation;
            uint8_t         size;
            SoundSequence** sequences;
            
            void    update( MicroBitAudioProcessor::AudioFrameAnalysis* buffer, 
                            uint8_t buffer_len );
            bool    matched();
            void    resetHistory();

            private:
            
            uint8_t matchSequence(  uint8_t seq_id, 
                                    MicroBitAudioProcessor::AudioFrameAnalysis* buffer, 
                                    uint8_t buffer_len) const;

            uint8_t getDeviation(uint8_t frames_ago, uint8_t seq_id) const;
            void    addToHistory(uint8_t seq_id, uint8_t value);
            void    endHistoryFrame();

            uint8_t*    history;
            uint8_t     history_len;
            uint8_t     max_history_len;
        };

        MicroBitSoundRecogniser(MicroBitAudioProcessor& processor, MicroBit& uBit);
        
        Sound**         sounds;
        ManagedString** sounds_names;
        uint8_t         sounds_size;

    public:
        ~MicroBitSoundRecogniser();

        virtual int pullRequest();
        
        MicroBitAudioProcessor* getAudioProcessor();
        void setCallback(void (*_callback)(ManagedString));
        void startAnalisying(void (*_callback)(ManagedString));
        void stopAnalisying();
};

#endif
