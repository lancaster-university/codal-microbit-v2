
#ifndef EMOJI_RECOGNISER_H
#define EMOJI_RECOGNISER_H

/*
 *  
 * The emoji recogniser is a subclass of sound recogniser that defines
 * the actual samples for the emoji sounds. They are just parts of the 
 * emoji sounds that can be recognised: remain quite consistent across
 * multiple plays of the sound.
 *   
 *   
 * Example
 *   
 * Taking the happy sound as an example, there are a few constants defined:
 *      
 *     happy_sequences         the number of sequences in the happy sound 
 *       
 *     happy_max_deviations    the maximum number of deviations in the 
 *                             sound - i.e. a deviation is considered
 *                             a data point that is more than the allowed 
 *                             threshold off the sampled frequency
 *       
 *     happy_samples           a 3-dimensional array with the sampled sound:
 *                                 - the first dimension is the different 
 *                                   sequences
 *                                 - the second is the samples in each sequence
 *                                 - the third is the data points in each sample 
 *                                   of each sequence
 *       
 *     happy_thresholds        an array with the thresholds for each of the 
 *                             sequences 
 *       
 *     happy_deviations        an array with the maximum deviations for each 
 *                             sequence
 *       
 *     happy_nr_samples        an array with the number of samples in each 
 *                             sequence
 *       
 * All these are packaged in a Sound struct.
 */

#include "MicroBitSoundRecogniser.h"

#define DEVICE_EMOJI_RECOGNISER_EVT_HAPPY       1            
#define DEVICE_EMOJI_RECOGNISER_EVT_HELLO       2
#define DEVICE_EMOJI_RECOGNISER_EVT_SAD         3
#define DEVICE_EMOJI_RECOGNISER_EVT_SOARING     4
#define DEVICE_EMOJI_RECOGNISER_EVT_TWINKLE     5

// 37 is the first unused id in CodalComponent.
// It might be better for this to be in CodalComponent.
#define DEVICE_ID_EMOJI_RECOGNISER    37

class EmojiRecogniser : public MicroBitSoundRecogniser
{
    void addHappySound();
    void addHelloSound();
    void addSadSound();
    void addSoaringSound();
    void addTwinkleSound();

    protected:
    /*
     * The function to call when a sound is recognised.
     */
    void recognisedSound(uint16_t id); 

    public:
    EmojiRecogniser(MicroBitAudioProcessor& processor);

    /*
     * Converts from id to sound name.
     */
    static ManagedString getSoundName(Event& evnt);
};

#endif