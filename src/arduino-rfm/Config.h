#ifndef _CONFIG_H_
#define _CONFIG_H_

// debug 
//#define DEBUG

// LoRaWAN freq. band
// #define AS_923
// #define EU_868
#define US_915
    //Select the subband youre working on
    // make sure your gateway is working in one of these bands
    //#define SUBND_0     // 902.3 - 903.7 Mhz
    #define SUBND_1     // 903.9 - 905.3 Mhz
    //#define SUBND_2     // 905.5 - 906.9 Mhz
    //#define SUBND_3     // 907.1 - 908.5 Mhz
    //#define SUBND_4     // 908.7 - 910.1 Mhz
    //#define SUBND_5     // 910.3 - 911.7 Mhz
    //#define SUBND_6     // 911.9 - 913.3 Mhz
    //#define SUBND_7     // 913.5 - 914.9 Mhz
#endif