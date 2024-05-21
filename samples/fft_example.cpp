#include "MicroBit.h"
#include "CodalDmesg.h"
#include "MicroBitAudioProcessor.h"
#include "StreamNormalizer.h"

static NRF52ADCChannel *mic = NULL;
static StreamNormalizer *processor = NULL;
static MicroBitAudioProcessor *fft = NULL;

MicroBit uBit;

/**
  * fft_test function - creates an example MicroBitAudioProcessor and then queries it for results.
  * Currently configured to use 1024 samples with 8bit signed data.
  */
void fft_test() {
    uBit.display.print("L");
/*
     if (mic == NULL){
        mic = uBit.adc.getChannel(uBit.io.microphone);
        mic->setGain(7,0);
    }

    if (processor == NULL)
        processor = new StreamNormalizer(mic->output, 1.0f, true, DATASTREAM_FORMAT_8BIT_SIGNED, 10);

    if (fft == NULL)
        fft = new MicroBitAudioProcessor(processor->output);

    uBit.io.runmic.setDigitalValue(1);
    uBit.io.runmic.setHighDrive(true);
*/

    // Code above commented out was from the original example, which can
    // probably be replaced with this single line, but we need to double check
    // the configuration because the fft calculation results are twice the frequency
    fft = new MicroBitAudioProcessor(*uBit.audio.splitter->createChannel());

    //Start fft running
    fft->startRecording();

    while (1){
        //TODO - de-noise : if last X samples are same - display ect.
        //The output values depend on the input type (DATASTREAM_FORMAT_8BIT_SIGNED) and the size
        //of the FFT - which is changed using the 'AUDIO_SAMPLES_NUMBER' in MicroBitAudioProcessor.h
        //default is 1024
        uBit.sleep(100);
        int freq = fft->getFrequency();
        DMESG("%s %d", "frequency: ", freq);
        if(freq > 0)
            uBit.display.print("?");
        if(freq > 530)
            uBit.display.print("C");
        if(freq > 600)
            uBit.display.print("D");
        if(freq > 680)
            uBit.display.print("E");
        if(freq > 710)
            uBit.display.print("F");
        if(freq > 800)
            uBit.display.print("G");
        if(freq > 900)
            uBit.display.print("A");
        if(freq > 1010)
            uBit.display.print("B");
        if(freq > 1050)
            uBit.display.print("?");
    }
}

int main() {
    uBit.init();
    fft_test();
}
