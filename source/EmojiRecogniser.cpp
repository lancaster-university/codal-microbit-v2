
#include "EmojiRecogniser.h"

EmojiRecogniser::EmojiRecogniser( MicroBitAudioProcessor& processor ) 
                    : MicroBitSoundRecogniser(processor){
    sounds = new Sound* [5];
    sound_ids = new uint16_t[5];
    addHappySound();
    addHelloSound();
    addSadSound();
    addSoaringSound();
    addTwinkleSound();
}

/*
 * The function to call when a sound is recognised.
 */
void EmojiRecogniser::recognisedSound(uint16_t id){
    Event evnt(DEVICE_ID_EMOJI_RECOGNISER, id);
}

/*
 * Converts from id to sound name.
 */
ManagedString EmojiRecogniser::getSoundName(Event& evnt){
    if(evnt.source != DEVICE_ID_EMOJI_RECOGNISER)
        return ManagedString("");
    switch (evnt.value) {
    case DEVICE_EMOJI_RECOGNISER_EVT_HAPPY:
        return ManagedString("happy");
    case DEVICE_EMOJI_RECOGNISER_EVT_HELLO:
        return ManagedString("hello");
    case DEVICE_EMOJI_RECOGNISER_EVT_SAD:
        return ManagedString("sad");
    case DEVICE_EMOJI_RECOGNISER_EVT_SOARING:
        return ManagedString("soaring");
    case DEVICE_EMOJI_RECOGNISER_EVT_TWINKLE:
        return ManagedString("twinkle");
    }
    return ManagedString("");
}

// HAPPY Sound ---- 

#if MIC_SAMPLE_DELTA == 91 

const uint8_t happy_sequences = 2;
const uint8_t happy_max_deviations = 2;

uint16_t happy_samples[happy_sequences][2][8] = {
    {
        { 4, 2121, 2394, 2646, 2646}, 	
        { 5, 2121, 2373, 2373, 2646, 2646}
    },
    {
        { 7, 2646, 2835, 2646, 2646, 2394, 2394, 2394}, 	
        { 7, 2646, 2835, 2835, 2646, 2394, 2373, 2394}
    }
};

uint16_t happy_thresholds[happy_sequences] = {
    40,
    50
};

uint8_t happy_deviations[happy_sequences] = {
    1,
    2
};

uint8_t happy_nr_samples[happy_sequences] = {
    2,
    2
};

#elif MIC_SAMPLE_DELTA == 45

const uint8_t happy_sequences = 3;
const uint8_t happy_max_deviations = 7;

uint16_t happy_samples[happy_sequences][2][10] = {
    {
        { 6, 2107, 2107, 2365, 2365, 2365, 2623}, 	
        { 7, 2107, 2107, 2365, 2365, 2365, 2365, 2623} 
    },
    {
        { 9, 2623, 2623, 2623, 2623, 2623, 2795, 2795, 2795, 2623}, 	
        { 9, 2623, 2623, 2623, 2623, 2666, 2795, 2795, 2666, 2623}
    },
    {
        { 8, 2623, 2623, 2623, 2365, 2365, 2365, 2365, 2365}, 	
        { 8, 2623, 2623, 2365, 2365, 2365, 2365, 2365, 2365}
    }
};

uint16_t happy_thresholds[happy_sequences] = {
    100,
    100,
    70
};

uint8_t happy_deviations[happy_sequences] = {
    3,
    4,
    4
};

uint8_t happy_nr_samples[happy_sequences] = {
    2,
    2,
    2
};

#endif


void EmojiRecogniser::addHappySound() {

    uint8_t it = sounds_size;
    sounds_size ++;
    sound_ids[it] = DEVICE_EMOJI_RECOGNISER_EVT_HAPPY;

    uint8_t history = 0;

    for(uint8_t i = 0; i < happy_sequences; i++)
        for(uint8_t j = 0; j < happy_nr_samples[i]; j ++)
            history = max(history, happy_samples[i][j][0] + 4);

    sounds[it] = new Sound(happy_sequences, happy_max_deviations, history, true);

    for(uint8_t i = 0; i < happy_sequences; i++){
        sounds[it] -> sequences[i] = new SoundSequence(happy_nr_samples[i], happy_thresholds[i], happy_deviations[i]);
        for(uint8_t j = 0; j < happy_nr_samples[i]; j ++)
            sounds[it] -> sequences[i] -> samples[j] = new SoundSample(happy_samples[i][j] + 1, happy_samples[i][j][0]);
    }

}

// HELLO Sound ---- 


#if MIC_SAMPLE_DELTA == 91

const uint8_t hello_sequences = 2;
const uint8_t hello_max_deviations = 3;

uint16_t hello_samples[hello_sequences][3][10] = {
    // First sequence
    {
        { 3, 2688, 2919, 3528}, 	
        { 3, 2751, 2919, 3528}, 	
        { 3, 2688, 3402, 3528}
    },
    {
        { 9, 3507, 3150, 3087, 3003, 2961, 2940, 2940, 2961, 2940}, 	
        { 9, 3339, 3108, 3024, 2982, 2940, 2940, 2961, 2961, 2940}, 	
        { 9, 3381, 3150, 3087, 3003, 2961, 2940, 2940, 2961, 2961}
    }
};

uint16_t hello_thresholds[hello_sequences] = {
    50,
    80
};

uint8_t hello_deviations[hello_sequences] = {
    2,
    3
};

uint8_t hello_nr_samples[hello_sequences] = {
    3,
    3
};

#elif MIC_SAMPLE_DELTA == 45

const uint8_t hello_sequences = 3;
const uint8_t hello_max_deviations = 3;

uint16_t hello_samples[hello_sequences][3][10] = {
    {
        { 6, 2881, 2623, 2838, 3139, 3397, 3526}, 	
        { 6, 2838, 2623, 2881, 3268, 3440, 3526}, 	
        { 7, 2795, 2494, 2666, 3053, 3268, 3483, 3526}
    },
    {
        { 4, 3526, 3440, 3311, 3139}, 
        { 4, 3483, 3311, 3182, 3139}
    },
    {
        { 9, 3139, 3096, 3053, 3010, 2967, 2967, 2924, 2924, 2924}, 	
        { 9, 3096, 3053, 3010, 2967, 2967, 2924, 2924, 2924, 2924}
    }
};

uint16_t hello_thresholds[hello_sequences] = {
    100,
    80,
    80
};

uint8_t hello_deviations[hello_sequences] = {
    3,
    2,
    3
};

uint8_t hello_nr_samples[hello_sequences] = {
    3,
    2,
    2
};

#endif

void EmojiRecogniser::addHelloSound() {
    uint8_t it = sounds_size;
    sounds_size ++;
    sound_ids[it] = DEVICE_EMOJI_RECOGNISER_EVT_HELLO;

    uint8_t history = 0;

    for(uint8_t i = 0; i < hello_sequences; i++)
        for(uint8_t j = 0; j < hello_nr_samples[i]; j ++)
            history = max(history, hello_samples[i][j][0] + 4);

    sounds[it] = new Sound(hello_sequences, hello_max_deviations, history, true);

    for(uint8_t i = 0; i < hello_sequences; i++){
        sounds[it] -> sequences[i] = new SoundSequence(hello_nr_samples[i], hello_thresholds[i], hello_deviations[i]);
        for(uint8_t j = 0; j < hello_nr_samples[i]; j ++)
            sounds[it] -> sequences[i] -> samples[j] = new SoundSample(hello_samples[i][j] + 1, hello_samples[i][j][0]);
    }
}


// SAD Sound ----

#if MIC_SAMPLE_DELTA == 91

const uint8_t sad_sequences = 2;
const uint8_t sad_max_deviations = 5;

uint16_t sad_samples[sad_sequences][3][16] = {
    // First sequence
    {
        { 8, 3423, 3339, 3255, 3087, 2961, 2856, 2709, 2604}, 	
        { 8, 3381, 3318, 3192, 3045, 2898, 2793, 2625, 2520}, 	
        { 8, 3318, 3255, 3087, 2919, 2793, 2688, 2562, 2436} 
    },
    {
        { 15, 3591, 3423, 3318, 3171, 3024, 2940, 2877, 2835, 2814, 2814, 2835, 2898, 2940, 3045, 2898}, 	
        { 15, 3507, 3423, 3213, 3087, 3003, 2919, 2856, 2814, 2814, 2814, 2856, 2919, 3003, 3150, 2898}, 	
        { 14, 3402, 3234, 3108, 3045, 2940, 2856, 2814, 2814, 2814, 2856, 2898, 2982, 3066, 2898} 
    }
};

uint16_t sad_thresholds[sad_sequences] = {
    50,
    100
};

uint8_t sad_deviations[sad_sequences] = {
    4,
    6
};

uint8_t sad_nr_samples[sad_sequences] = {
    3,
    3
};

#elif MIC_SAMPLE_DELTA == 45

const uint8_t sad_sequences = 4;
const uint8_t sad_max_deviations = 9;

uint16_t sad_samples[sad_sequences][3][13] = {
    {
        { 11, 5289, 5246, 5203, 5117, 5074, 5031, 4945, 4902, 4816, 4730, 4601}, 	
        { 10, 5332, 5289, 5246, 5160, 5117, 5074, 5031, 4945, 4902, 4816}
    },
    {
        { 11, 3096, 3010, 2924, 2881, 2795, 2709, 2666, 2580, 2537, 2451, 2408}, 	
        { 9, 3010, 2924, 2838, 2752, 2709, 2623, 2580, 2494, 2451}, 	
        { 11, 3139, 3053, 3010, 2924, 2838, 2795, 2709, 2623, 2580, 2494, 2451} 
    },
    {
        { 11, 3440, 3397, 3311, 3225, 3139, 3096, 3053, 3010, 2967, 2924, 2881}, 	
        { 11, 3483, 3397, 3354, 3268, 3182, 3139, 3096, 3010, 2967, 2924, 2881}, 	
        { 12, 3569, 3483, 3397, 3311, 3268, 3182, 3139, 3053, 3010, 2967, 2924, 2881}
    },
    {
        { 12, 2838, 2838, 2838, 2795, 2795, 2795, 2795, 2838, 2838, 2881, 2924, 2967}, 	
        { 11, 2881, 2838, 2838, 2795, 2795, 2795, 2795, 2838, 2838, 2881, 2881}
    }
};

uint16_t sad_thresholds[sad_sequences] = {
    100,
    120,
    100,
    80,
};

uint8_t sad_deviations[sad_sequences] = {
    4,
    4,
    4,
    3
};

uint8_t sad_nr_samples[sad_sequences] = {
    2,
    3,
    3,
    2
};
#endif

void EmojiRecogniser::addSadSound() {
    uint8_t it = sounds_size;
    sounds_size ++;
    sound_ids[it] = DEVICE_EMOJI_RECOGNISER_EVT_SAD;

    uint8_t history = 0;

    for(uint8_t i = 0; i < sad_sequences; i++)
        for(uint8_t j = 0; j < sad_nr_samples[i]; j ++)
            history = max(history, sad_samples[i][j][0] + 4);

    sounds[it] = new Sound(sad_sequences, sad_max_deviations, history, true);

    for(uint8_t i = 0; i < sad_sequences; i++){
        sounds[it] -> sequences[i] = new SoundSequence(sad_nr_samples[i], sad_thresholds[i], sad_deviations[i]);
        for(uint8_t j = 0; j < sad_nr_samples[i]; j ++)
            sounds[it] -> sequences[i] -> samples[j] = new SoundSample(sad_samples[i][j] + 1, sad_samples[i][j][0]);
    }
}


// SOARING Sound ----

#if MIC_SAMPLE_DELTA == 91

const uint8_t soaring_sequences = 7;
const uint8_t soaring_max_deviations = 7;

uint16_t soaring_samples[soaring_sequences][1][7] = {
    {
        { 5, 4179, 4179, 4179, 4179, 4179}
    },	
    {
        { 5, 4284, 4284, 4284, 4284, 4284}
    }, 	
    {
        { 5, 4389, 4389, 4389, 4389, 4389}
    },	
    {
        { 5, 4494, 4494, 4494, 4494, 4494}
    },	
    {
        { 5, 4599, 4599, 4599, 4599, 4599}
    },	
    {
        { 5, 4704, 4704, 4704, 4704, 4704}
    },	
    {
     	{ 5, 4809, 4809, 4809, 4809, 4809}
    } 
};

const uint16_t soaring_thresholds[soaring_sequences] = {
    100,
    100,
    100,
    100,
    100,
    100,
    100
};

const uint8_t soaring_deviations[soaring_sequences] = {
    3,
    3,
    3,
    3,
    3,
    3,
    3
};

const uint8_t soaring_nr_samples[soaring_sequences] = {
    1,
    1,
    1,
    1,
    1,
    1,
    1
};

#elif MIC_SAMPLE_DELTA == 45

const uint8_t soaring_sequences = 7;
const uint8_t soaring_max_deviations = 10;

uint16_t soaring_samples[soaring_sequences][1][11] = {
    {
        { 10, 4179, 4179, 4179, 4179, 4179, 4179, 4179, 4179, 4179, 4179}
    },	
    {
        { 10, 4284, 4284, 4284, 4284, 4284, 4284, 4284, 4284, 4284, 4284}
    }, 	
    {
        { 10, 4389, 4389, 4389, 4389, 4389, 4389, 4389, 4389, 4389, 4389}
    },	
    {
        { 10, 4494, 4494, 4494, 4494, 4494, 4494, 4494, 4494, 4494, 4494}
    },	
    {
        { 10, 4599, 4599, 4599, 4599, 4599, 4599, 4599, 4599, 4599, 4599}
    },	
    {
        { 10, 4704, 4704, 4704, 4704, 4704, 4704, 4704, 4704, 4704, 4704}
    },	
    {
     	{ 10, 4809, 4809, 4809, 4809, 4809, 4809, 4809, 4809, 4809, 4809}
    } 
};

const uint16_t soaring_thresholds[soaring_sequences] = {
    100,
    100,
    100,
    100,
    100,
    100,
    100
};

const uint8_t soaring_deviations[soaring_sequences] = {
    3,
    3,
    3,
    3,
    3,
    3,
    3
};

const uint8_t soaring_nr_samples[soaring_sequences] = {
    1,
    1,
    1,
    1,
    1,
    1,
    1
};

#endif


void EmojiRecogniser::addSoaringSound() {
    uint8_t it = sounds_size;
    sounds_size ++;
    sound_ids[it] = DEVICE_EMOJI_RECOGNISER_EVT_SOARING;

    uint8_t history = 0;

    for(uint8_t i = 0; i < soaring_sequences; i++)
        for(uint8_t j = 0; j < soaring_nr_samples[i]; j ++)
            history = max(history, soaring_samples[i][j][0] + 4);

    sounds[it] = new Sound(soaring_sequences, soaring_max_deviations, history, true);

    for(uint8_t i = 0; i < soaring_sequences; i++){
        sounds[it] -> sequences[i] = new SoundSequence(soaring_nr_samples[i], soaring_thresholds[i], soaring_deviations[i]);
        for(uint8_t j = 0; j < soaring_nr_samples[i]; j ++)
            sounds[it] -> sequences[i] -> samples[j] = new SoundSample(soaring_samples[i][j] + 1, soaring_samples[i][j][0]);
    }
}


// TWINKLE Sound ----

#if MIC_SAMPLE_DELTA == 91

const uint8_t twinkle_sequences = 4;
const uint8_t twinkle_max_deviations = 5;

uint16_t twinkle_samples[twinkle_sequences][5][8] = {
    // First sequence
    {	
        { 5, 1827, 2709, 3612, 4053, 4809}, 	
        { 6, 1827, 2709, 2709, 3612, 4053, 4809}, 	
        { 6, 1827, 2730, 3612, 4053, 4053, 4809}, 	
        { 6, 1827, 2709, 3591, 3612, 4053, 4809}	
    }, 
    {		
        { 7, 4788, 4767, 4473, 4473, 4011, 3570, 3339}, 	
        { 6, 4788, 4767, 4473, 4032, 3570, 3360}, 	
        { 7, 4809, 4767, 4473, 4032, 3570, 3570, 3339}
    }, 
    {	
        { 7, 1827, 2625, 2373, 2226, 2016, 2016, 1785}, 	
        { 7, 1827, 1827, 2604, 2373, 2226, 2016, 1785}, 	
        { 7, 1827, 1827, 2373, 2373, 2226, 2016, 1785}, 	
        { 7, 4116, 1827, 2394, 2373, 2226, 2016, 1785}, 	
        { 6, 4137, 2688, 2373, 2226, 2016, 1785} 	
    }, 
    {	
        { 6, 2982, 2982, 2688, 2373, 2226, 2016}, 	
        { 6, 3360, 2982, 2688, 2667, 2373, 2226}, 	
        { 6, 3339, 2982, 2982, 2688, 2373, 2226} 	
    }
};

const uint16_t twinkle_thresholds[twinkle_sequences] = {
    80,
    80,
    80,
    80
};

const uint8_t twinkle_deviations[twinkle_sequences] = {
    3,
    3,
    3,
    3
};

const uint8_t twinkle_nr_samples[twinkle_sequences] = {
    4,
    3,
    5,
    3
};

#elif MIC_SAMPLE_DELTA == 45

const uint8_t twinkle_sequences = 4;
const uint8_t twinkle_max_deviations = 9;

uint16_t twinkle_samples[twinkle_sequences][3][10] = {
    {	
        { 9, 2236, 2236, 2021, 2021, 1763, 1763, 1763, 1677, 1677}, 	
        { 9, 2236, 2236, 2021, 2021, 2021, 1763, 1763, 1677, 1677}, 	
        { 9, 2236, 2236, 2236, 2021, 2021, 1763, 1763, 1677, 1677}	 	
    }, 
    {		
        { 8, 2967, 2967, 2666, 2666, 2666, 2365, 2365, 2236}, 	
        { 8, 2967, 2967, 2967, 2666, 2666, 2365, 2365, 2236}	 	
    }, 
    {	
        { 7, 2236, 3010, 3010, 2666, 2666, 2666, 2494}, 	
        { 7, 2236, 2236, 3010, 3010, 2666, 2666, 2494}	 			
    }, 
    {	
        { 5, 2494, 2967, 2967, 2666, 2666}, 	
        { 5, 2494, 2494, 2967, 2967, 2666}	 
    }
};

const uint16_t twinkle_thresholds[twinkle_sequences] = {
    100,
    100,
    100,
    100
};

const uint8_t twinkle_deviations[twinkle_sequences] = {
    5,
    4,
    3,
    3
};

const uint8_t twinkle_nr_samples[twinkle_sequences] = {
    3,
    2,
    2,
    2
};

#endif

void EmojiRecogniser::addTwinkleSound() {
    uint8_t it = sounds_size;
    sounds_size ++;
    sound_ids[it] = DEVICE_EMOJI_RECOGNISER_EVT_TWINKLE;

    uint8_t history = 0;

    for(uint8_t i = 0; i < twinkle_sequences; i++)
        for(uint8_t j = 0; j < twinkle_nr_samples[i]; j ++)
            history = max(history, twinkle_samples[i][j][0] + 4);

    sounds[it] = new Sound(twinkle_sequences, twinkle_max_deviations, history, true);

    for(uint8_t i = 0; i < twinkle_sequences; i++){
        sounds[it] -> sequences[i] = new SoundSequence(twinkle_nr_samples[i], twinkle_thresholds[i], twinkle_deviations[i]);
        for(uint8_t j = 0; j < twinkle_nr_samples[i]; j ++)
            sounds[it] -> sequences[i] -> samples[j] = new SoundSample(twinkle_samples[i][j] + 1, twinkle_samples[i][j][0]);
    }
}