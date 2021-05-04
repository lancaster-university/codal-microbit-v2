#include "MicroBitMorsePlayer.h"
#include "MorseEncoder.h"

MicroBitMorsePlayer::MicroBitMorsePlayer(MicroBit& bit, int freq, int dur)
                            :uBit(bit), frequency(freq), duration(dur), randomness(DEFAULT_RANDOMNESS){
    createFrames();
}

void MicroBitMorsePlayer::play(char* in){
    int i = 0;
    while (in[i]!=0) i++;
    char* out = new char[i*7];

    MorseEncoder me = MorseEncoder();
    me.encode(in, out);
    
    i = 0;
    while (out[i]!=0){
        switch(out[i]){
        case '#':
            uBit.sleep(duration * 10);
            break;
        case ' ':
            uBit.sleep(duration * 3);
            break;
        case ';':
            uBit.sleep(duration * 7);
            break;
        case '.':
            uBit.audio.soundExpressions.play(dotFrame);
            if (out[i+1] == '.' || out[i+1] == '-') uBit.sleep(duration);
            break;
        case '-':
            uBit.audio.soundExpressions.play(dashFrame);
            if (out[i+1] == '.' || out[i+1] == '-') uBit.sleep(duration);
            break;
        }
        i++;
    }
}

ManagedString fourString(int n){
    if (n < 10)
        return ManagedString("000") + ManagedString(n);
    if (n < 100)
        return ManagedString("00") + ManagedString(n);
    if (n < 1000)
        return ManagedString("0") + ManagedString(n);
    if (n > 9999)
        return ManagedString("9999");
    return ManagedString(n);
}

void MicroBitMorsePlayer::createFrames(){
    ManagedString freqString = fourString(frequency);
    ManagedString dotDurationString = fourString(duration);
    ManagedString dashDurationString = fourString(duration*3);
    ManagedString randomString = fourString(randomness);


    dotFrame = ManagedString("01023")+
                freqString+
                dotDurationString+
                ManagedString("02440")+
                freqString+
                ManagedString("0888102301280002000024")+
                randomString+
                ManagedString("000000000000000000000000");

    dashFrame = ManagedString("01023")+
                freqString+
                dashDurationString+
                ManagedString("02440")+
                freqString+
                ManagedString("0888102301280002000024")+
                randomString+
                ManagedString("000000000000000000000000");
}