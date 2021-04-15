
#include "EmojiRecogniser.h"


EmojiRecogniser::EmojiRecogniser(   MicroBitAudioProcessor& processor, 
                                    MicroBit& uBit) 
                    : MicroBitSoundRecogniser(processor, uBit){
    sounds = new Sound* [7];
    sounds_names = new ManagedString* [7];
    addHappySound();
}

void EmojiRecogniser::addHappySound() {

    uint16_t happy[3][6][8] = {
        // First sequence
        {
            { 4, 4368, 0, 0, 2751}, 	
            { 5, 4431, 0, 0, 0, 2751}, 	
            { 6, 4473, 0, 0, 0, 0, 2751}, 	
            { 5, 4263, 0, 0, 0, 2751}, 	
            { 6, 4263, 0, 0, 0, 0, 2751}, 	
            { 4, 4263, 0, 0, 2751}
        },
        // Second sequence
        {
            { 5, 3024, 3024, 3276, 3276, 3255}, 	
            { 6, 3024, 3024, 3717, 3276, 3276, 3255}, 	
            { 5, 2751, 3003, 3276, 3276, 3276}
        }, 
        // Third sequence
        {
            { 6, 3423, 3276, 3276, 3024, 3024, 3024}, 	
            { 6, 3423, 3276, 3276, 3255, 3024, 3024}, 	
            { 6, 3423, 3276, 3276, 3024, 3024, 2961}, 	
            { 6, 3276, 3234, 3234, 3276, 3003, 3003}
        }
    };
    
    uint16_t happy_thresholds[3] = {
        50 * 50,
        50 * 100,
        50 * 100
    };

    uint8_t happy_deviations[3] = {
        0,
        1,
        1
    };

    uint8_t happy_samples[3] = {
        6,
        3,
        4
    };

    uint8_t it = sounds_size;
    sounds_size ++;
    sounds_names[it] = new ManagedString("happy");

    sounds[it] = new Sound(2, 3, 10);

    for(uint8_t i = 1; i < 3 ; i++){
        sounds[it] -> sequences[i -1] = new SoundSequence(happy_samples[i], happy_thresholds[i], happy_deviations[i]);
        for(uint8_t j = 0; j < happy_samples[i]; j ++)
            sounds[it] -> sequences[i-1] -> samples[j] = new SoundSample(happy[i][j] + 1, happy[i][j][0]);
    }

}


