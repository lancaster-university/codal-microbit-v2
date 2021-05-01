
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

class EmojiRecogniser : public MicroBitSoundRecogniser
{
    void addHappySound();
    void addHelloSound();
    void addSadSound();
    void addSoaringSound();
    void addTwinkleSound();

    public:
    EmojiRecogniser(MicroBitAudioProcessor& processor);
};

#endif