#include "MicroBit.h"

MicroBit uBit;

const char * const happy_emoji ="\
    000,255,000,255,000\n\
    000,000,000,000,000\n\
    255,000,000,000,255\n\
    000,255,255,255,000\n\
    000,000,000,000,000\n";

const char * const wink_emoji ="\
    000,255,000,000,000\n\
    000,000,000,000,000\n\
    255,000,000,000,255\n\
    000,255,255,255,000\n\
    000,000,000,000,000\n";

int
main()
{
    MicroBitImage smile(happy_emoji);
    MicroBitImage wink(wink_emoji);

    uBit.init();

    while(1)
    {
        uBit.display.print(smile);
        uBit.sleep(1000);
        uBit.display.print(wink);
        uBit.sleep(500);
        uBit.display.print(smile);
        uBit.sleep(1000);
        uBit.display.clear();

        uBit.display.scroll("Hello World");
    }
}

