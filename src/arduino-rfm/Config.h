//Uncomment for debug 
//#define DEBUG

// LoRaWAN freq band
// #define AS_923
//#define EU_868
#define US_915 1
//#define AU_915 1

#ifdef US_915
    //Select the subband youre working on
    // make sure your gateway is working in one of these bands
    //#define SUBND_0     // 902.3 - 903.7 Mhz
    #define SUBND_1     // 903.9 - 905.3 Mhz TTN
    //#define SUBND_2     // 905.5 - 906.9 Mhz
    //#define SUBND_3     // 907.1 - 908.5 Mhz
    //#define SUBND_4     // 908.7 - 910.1 Mhz
    //#define SUBND_5     // 910.3 - 911.7 Mhz
    //#define SUBND_6     // 911.9 - 913.3 Mhz
    //#define SUBND_7     // 913.5 - 914.9 Mhz
#endif

#ifdef AU_915
    //Select the subband youre working on
    // make sure your gateway is working in one of these bands
    #define SUBND_0     // 915.2 - 916.6 Mhz
    //#define SUBND_1     // 916.8 - 918.2 Mhz
    //#define SUBND_2     // 918.4 - 919.8 Mhz
    //#define SUBND_3     // 920.0 - 921.4 Mhz
    //#define SUBND_4     // 921.6 - 923.0 Mhz
    //#define SUBND_5     // 923.2 - 924.6 Mhz
    //#define SUBND_6     // 924.8 - 926.2 Mhz
    //#define SUBND_7     // 926.4 - 927.8 Mhz
#endif