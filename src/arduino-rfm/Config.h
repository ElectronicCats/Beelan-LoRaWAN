#ifndef _CONFIG_H_
#define _CONFIG_H_

// Board definition
#define BOARD_DRAGINO_SHIELD
// #define BOARD_CYTRON_SHIELD
// #define CUSTOM_BOARD

// Your custom pins
#ifdef CUSTOM_BOARD
#if defined(ESP8266)    // for ESP based board
    #define DIO0    2
    #define DIO1    6
    #define DIO5    8
    #define DIO2    7
    #define CS      10
    #define RFM_RST 9        
#else                   // for Arduino board
    #define DIO0    2
    #define DIO1    6
    #define DIO5    8
    #define DIO2    7
    #define CS      10
    #define RFM_RST 9        
#endif  
#endif // _CUSTOM_BOARD_


// LoRaWAN freq. band
#define AS_923
// #define EU_868


// ------------------------------------------------
// ------------- Pre-defined PINS -----------------
// ------------------------------------------------

#if defined(ESP8266)
#ifdef BOARD_DRAGINO_SHIELD
    #define DIO0    16
    #define DIO1    2
    #define DIO5    12
    #define DIO2    14
    #define CS      15
    #define RFM_RST 13        
#elif defined BOARD_CYTRON_SHIELD
    #define DIO0    16
    #define DIO1    0
    #define DIO2    2
    #define CS      15
    #define RFM_RST 14       
#elif defined BOARD_CUSTOM
#else
#error "Unsupported shield"
#endif
#else
#ifdef BOARD_DRAGINO_SHIELD
    #define DIO0    2
    #define DIO1    6
    #define DIO5    8
    #define DIO2    7
    #define CS      10
    #define RFM_RST 9        
#elif defined BOARD_CYTRON_SHIELD
    #define DIO0    2
    #define DIO1    5
    #define DIO2    6
    #define CS      10
    #define RFM_RST 7        
#elif defined BOARD_CUSTOM
#else
#error "Unsupported shield"
#endif
#endif

#endif