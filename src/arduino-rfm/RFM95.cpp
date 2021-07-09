/******************************************************************************************
* Copyright 2017 Ideetron B.V.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************************/
/****************************************************************************************
* File:     RFM95.cpp
* Author:   Gerben den Hartog
* Compagny: Ideetron B.V.
* Website:  http://www.ideetron.nl/LoRa
* E-mail:   info@ideetron.nl
****************************************************************************************/
/****************************************************************************************
* Created on:         06-01-2017
* Supported Hardware: ID150119-02 Nexus board with RFM95
****************************************************************************************/

#include <Arduino.h>
#include <SPI.h>
#include "RFM95.h"
#include "Config.h"

/**
 *  Lora Frequencies
 *    Tested on all subbands in US915 
 */
#if defined(US_915)
  #if defined(SUBND_0)//[902.3 - 903.7] MHz
    static const PROGMEM unsigned char LoRa_TX_Freq[8][3] = {
      { 0xE1, 0x93, 0x59 }, //Channel [0], 902.3 MHz / 61.035 Hz = 14783321 = 0xE19359
      { 0xE1, 0xA0, 0x26 }, //Channel [1], 902.5 MHz / 61.035 Hz = 14786598 = 0xE1A026
      { 0xE1, 0xAC, 0xF3 }, //Channel [2], 902.7 MHz / 61.035 Hz = 14789875 = 0xE1ACF3
      { 0xE1, 0xB9, 0xBF }, //Channel [3], 902.9 MHz / 61.035 Hz = 14793151 = 0xE1B9BF
      { 0xE1, 0xC6, 0x8C }, //Channel [4], 903.1 MHz / 61.035 Hz = 14796428 = 0xE1C68C
      { 0xE1, 0xD3, 0x59 }, //Channel [5], 903.3 MHz / 61.035 Hz = 14799705 = 0xE1D359
      { 0xE1, 0xE0, 0x26 }, //Channel [6], 903.5 MHz / 61.035 Hz = 14802982 = 0xE1E026
      { 0xE1, 0xEC, 0xF3 }, //Channel [7], 903.7 MHz / 61.035 Hz = 14806259 = 0xE1ECF3
    };
  #elif  defined(SUBND_1)//[903.9 - 905.3] MHz
    static const PROGMEM unsigned char LoRa_TX_Freq[8][3] = {
      { 0xE1, 0xF9, 0xC0 }, //Channel [0], 903.9 MHz / 61.035 Hz = 14809536 = 0xE1F9C0
      { 0xE2, 0x06, 0x8C }, //Channel [1], 904.1 MHz / 61.035 Hz = 14812812 = 0xE2068C
      { 0xE2, 0x13, 0x59 }, //Channel [2], 904.3 MHz / 61.035 Hz = 14816089 = 0xE21359
      { 0xE2, 0x20, 0x26 }, //Channel [3], 904.5 MHz / 61.035 Hz = 14819366 = 0xE22026
      { 0xE2, 0x2C, 0xF3 }, //Channel [4], 904.7 MHz / 61.035 Hz = 14822643 = 0xE22CF3
      { 0xE2, 0x39, 0xC0 }, //Channel [5], 904.9 MHz / 61.035 Hz = 14825920 = 0xE239C0
      { 0xE2, 0x46, 0x8C }, //Channel [6], 905.1 MHz / 61.035 Hz = 14829196 = 0xE2468C
      { 0xE2, 0x53, 0x59 }, //Channel [7], 905.3 MHz / 61.035 Hz = 14832473 = 0xE25359
    };
  #elif  defined(SUBND_2)//[905.5 - 906.9] MHz
    static const PROGMEM unsigned char LoRa_TX_Freq[8][3] = {
      { 0xE2, 0x60, 0x26 }, //Channel [0], 905.5 MHz / 61.035 Hz = 14835750 = 0xE26026
      { 0xE2, 0x6C, 0xF3 }, //Channel [1], 905.7 MHz / 61.035 Hz = 14839027 = 0xE26CF3
      { 0xE2, 0x79, 0xC0 }, //Channel [2], 905.9 MHz / 61.035 Hz = 14842304 = 0xE279C0
      { 0xE2, 0x86, 0x8C }, //Channel [3], 906.1 MHz / 61.035 Hz = 14845580 = 0xE2868C
      { 0xE2, 0x93, 0x59 }, //Channel [4], 906.3 MHz / 61.035 Hz = 14848857 = 0xE29359
      { 0xE2, 0xA0, 0x26 }, //Channel [5], 906.5 MHz / 61.035 Hz = 14852134 = 0xE2A026
      { 0xE2, 0xAC, 0xF3 }, //Channel [6], 906.7 MHz / 61.035 Hz = 14855411 = 0xE2ACF3
      { 0xE2, 0xB9, 0xC0 }, //Channel [7], 906.9 MHz / 61.035 Hz = 14858688 = 0xE2B9C0
    };
  #elif  defined(SUBND_3)//[907.1 - 908.5] MHz
    static const PROGMEM unsigned char LoRa_TX_Freq[8][3] = {
      { 0xE2, 0xC6, 0x8C }, //Channel [0], 907.1 MHz / 61.035 Hz = 14861964 = 0xE2C68C
      { 0xE2, 0xD3, 0x59 }, //Channel [1], 907.3 MHz / 61.035 Hz = 14865241 = 0xE2D359
      { 0xE2, 0xE0, 0x26 }, //Channel [2], 907.5 MHz / 61.035 Hz = 14868518 = 0xE2E026
      { 0xE2, 0xEC, 0xF3 }, //Channel [3], 907.7 MHz / 61.035 Hz = 14871795 = 0xE2ECF3
      { 0xE2, 0xF9, 0xC0 }, //Channel [4], 907.9 MHz / 61.035 Hz = 14875072 = 0xE2F9C0
      { 0xE3, 0x06, 0x8C }, //Channel [5], 908.1 MHz / 61.035 Hz = 14878348 = 0xE3068C
      { 0xE3, 0x13, 0x59 }, //Channel [6], 908.3 MHz / 61.035 Hz = 14881625 = 0xE31359
      { 0xE3, 0x20, 0x26 }, //Channel [7], 908.5 MHz / 61.035 Hz = 14884902 = 0xE32026
    };
  #elif  defined(SUBND_4)//[908.7 - 910.1] MHz
    static const PROGMEM unsigned char LoRa_TX_Freq[8][3] = {
      { 0xE3, 0x2C, 0xF3 }, //Channel [0], 908.7 MHz / 61.035 Hz = 14888179 = 0xE32CF3
      { 0xE3, 0x39, 0xC0 }, //Channel [1], 908.9 MHz / 61.035 Hz = 14891456 = 0xE339C0
      { 0xE3, 0x46, 0x8D }, //Channel [2], 909.1 MHz / 61.035 Hz = 14894733 = 0xE3468D
      { 0xE3, 0x53, 0x59 }, //Channel [3], 909.3 MHz / 61.035 Hz = 14898009 = 0xE35359
      { 0xE3, 0x60, 0x26 }, //Channel [4], 909.5 MHz / 61.035 Hz = 14901286 = 0xE36026
      { 0xE3, 0x6C, 0xF3 }, //Channel [5], 909.7 MHz / 61.035 Hz = 14904563 = 0xE36CF3
      { 0xE3, 0x79, 0xC0 }, //Channel [6], 909.9 MHz / 61.035 Hz = 14907840 = 0xE379C0
      { 0xE3, 0x86, 0x8D }, //Channel [7], 910.1 MHz / 61.035 Hz = 14911117 = 0xE3868D
    };
  #elif  defined(SUBND_5)//[910.3 - 911.7] MHz
    static const PROGMEM unsigned char LoRa_TX_Freq[8][3] = {
      { 0xE3, 0x93, 0x59 }, //Channel [0], 910.3 MHz / 61.035 Hz = 14914393 = 0xE39359
      { 0xE3, 0xA0, 0x26 }, //Channel [1], 910.5 MHz / 61.035 Hz = 14917670 = 0xE3A026
      { 0xE3, 0xAC, 0xF3 }, //Channel [2], 910.7 MHz / 61.035 Hz = 14920947 = 0xE3ACF3
      { 0xE3, 0xB9, 0xC0 }, //Channel [3], 910.9 MHz / 61.035 Hz = 14924224 = 0xE3B9C0
      { 0xE3, 0xC6, 0x8D }, //Channel [4], 911.1 MHz / 61.035 Hz = 14927501 = 0xE3C68D
      { 0xE3, 0xD3, 0x59 }, //Channel [5], 911.3 MHz / 61.035 Hz = 14930777 = 0xE3D359
      { 0xE3, 0xE0, 0x26 }, //Channel [6], 911.5 MHz / 61.035 Hz = 14934054 = 0xE3E026
      { 0xE3, 0xEC, 0xF3 }, //Channel [7], 911.7 MHz / 61.035 Hz = 14937331 = 0xE3ECF3
    };
  #elif  defined(SUBND_6)//[911.9 - 913.3] MHz
    static const PROGMEM unsigned char LoRa_TX_Freq[8][3] = {
      { 0xE3, 0xF9, 0xC0 }, //Channel [0], 911.9 MHz / 61.035 Hz = 14940608 = 0xE3F9C0
      { 0xE4, 0x06, 0x8D }, //Channel [1], 912.1 MHz / 61.035 Hz = 14943885 = 0xE4068D
      { 0xE4, 0x13, 0x59 }, //Channel [2], 912.3 MHz / 61.035 Hz = 14947161 = 0xE41359
      { 0xE4, 0x20, 0x26 }, //Channel [3], 912.5 MHz / 61.035 Hz = 14950438 = 0xE42026
      { 0xE4, 0x2C, 0xF3 }, //Channel [4], 912.7 MHz / 61.035 Hz = 14953715 = 0xE42CF3
      { 0xE4, 0x39, 0xC0 }, //Channel [5], 912.9 MHz / 61.035 Hz = 14956992 = 0xE439C0
      { 0xE4, 0x46, 0x8D }, //Channel [6], 913.1 MHz / 61.035 Hz = 14960269 = 0xE4468D
      { 0xE4, 0x53, 0x5A }, //Channel [7], 913.3 MHz / 61.035 Hz = 14963546 = 0xE4535A
    };
  #elif  defined(SUBND_7)//[913.5 - 914.9] MHz
    static const PROGMEM unsigned char LoRa_TX_Freq[8][3] = {
      { 0xE4, 0x60, 0x26 }, //Channel [0], 913.5 MHz / 61.035 Hz = 14966822 = 0xE46026
      { 0xE4, 0x6C, 0xF3 }, //Channel [1], 913.7 MHz / 61.035 Hz = 14970099 = 0xE46CF3
      { 0xE4, 0x79, 0xC0 }, //Channel [2], 913.9 MHz / 61.035 Hz = 14973376 = 0xE479C0
      { 0xE4, 0x86, 0x8D }, //Channel [3], 914.1 MHz / 61.035 Hz = 14976653 = 0xE4868D
      { 0xE4, 0x93, 0x5A }, //Channel [4], 914.3 MHz / 61.035 Hz = 14979930 = 0xE4935A
      { 0xE4, 0xA0, 0x26 }, //Channel [5], 914.5 MHz / 61.035 Hz = 14983206 = 0xE4A026
      { 0xE4, 0xAC, 0xF3 }, //Channel [6], 914.7 MHz / 61.035 Hz = 14986483 = 0xE4ACF3
      { 0xE4, 0xB9, 0xC0 }, //Channel [7], 914.9 MHz / 61.035 Hz = 14989760 = 0xE4B9C0
    };
  #endif
  static const PROGMEM unsigned char LoRa_RX_Freq[8][3] = {
    { 0xE6, 0xD3, 0x5A}, //Rcv Channel [0] 923.3 Mhz / 61.035 Hz = 15127386 = 0xE6D35A
    { 0xE6, 0xF9, 0xC0}, //Rcv Channel [1] 923.9 Mhz / 61.035 Hz = 15137216 = 0xE6F9C0
    { 0xE7, 0x20, 0x27}, //Rcv Channel [2] 924.5 Mhz / 61.035 Hz = 15147047 = 0xE72027
    { 0xE7, 0x46, 0x8D}, //Rcv Channel [3] 925.1 Mhz / 61.035 Hz = 15156877 = 0xE7468D
    { 0xE7, 0x6C, 0xF4}, //Rcv Channel [4] 925.7 Mhz / 61.035 Hz = 15166708 = 0xE76CF4
    { 0xE7, 0x93, 0x5A}, //Rcv Channel [5] 926.3 Mhz / 61.035 Hz = 15176538 = 0xE7935A
    { 0xE7, 0xB9, 0xC0}, //Rcv Channel [6] 926.9 Mhz / 61.035 Hz = 15186368 = 0xE7B9C0
    { 0xE7, 0xE0, 0x27}, //Rcv Channel [7] 927.5 Mhz / 61.035 Hz = 15196199 = 0xE7E027
  };
#elif defined(AU_915)
  #if defined(SUBND_0)//[915.2 - 916.6] MHz
    static const PROGMEM unsigned char LoRa_TX_Freq[8][3] = {
      { 0xE4, 0xCC, 0xF3 }, //Channel [0], 915,2 MHz / 61.035 Hz = 14994675 = 0xE4CCF3
      { 0xE4, 0xD9, 0xC0 }, //Channel [1], 915,4 MHz / 61.035 Hz = 14997952 = 0xE4D9C0
      { 0xE4, 0xE6, 0x8D }, //Channel [2], 915,6 MHz / 61.035 Hz = 15001229 = 0xE4E68D
      { 0xE4, 0xF3, 0x5A }, //Channel [3], 915,8 MHz / 61.035 Hz = 15004506 = 0xE4F35A
      { 0xE5, 0x00, 0x26 }, //Channel [4], 916,0 MHz / 61.035 Hz = 15007782 = 0xE50026
      { 0xE5, 0x0C, 0xF3 }, //Channel [5], 916,2 MHz / 61.035 Hz = 15011059 = 0xE50CF3
      { 0xE5, 0x19, 0xC0 }, //Channel [6], 916,4 MHz / 61.035 Hz = 15014336 = 0xE519C0
      { 0xE5, 0x26, 0x8D }, //Channel [7], 916,6 MHz / 61.035 Hz = 15017613 = 0xE5268D
    };
  #elif  defined(SUBND_1)//[916.8 - 918.2] MHz
    static const PROGMEM unsigned char LoRa_TX_Freq[8][3] = {
      { 0xE5, 0x33, 0x5A }, //Channel [0], 916,8 MHz / 61.035 Hz = 15020890 = 0xE5335A
      { 0xE5, 0x40, 0x26 }, //Channel [1], 917,0 MHz / 61.035 Hz = 15024166 = 0xE54026
      { 0xE5, 0x4C, 0xF3 }, //Channel [2], 917,2 MHz / 61.035 Hz = 15027443 = 0xE54CF3
      { 0xE5, 0x59, 0xC0 }, //Channel [3], 917,4 MHz / 61.035 Hz = 15030720 = 0xE559C0
      { 0xE5, 0x66, 0x8D }, //Channel [4], 917,6 MHz / 61.035 Hz = 15033997 = 0xE5668D
      { 0xE5, 0x73, 0x5A }, //Channel [5], 917,8 MHz / 61.035 Hz = 15037274 = 0xE5735A
      { 0xE5, 0x80, 0x27 }, //Channel [6], 918,0 MHz / 61.035 Hz = 15040551 = 0xE58027
      { 0xE5, 0x8C, 0xF3 }, //Channel [7], 918,2 MHz / 61.035 Hz = 15043827 = 0xE58CF3
    };
  #elif  defined(SUBND_2)//[918.4 - 919.8] MHz
    static const PROGMEM unsigned char LoRa_TX_Freq[8][3] = {
      { 0xE5, 0x99, 0xC0 }, //Channel [0], 918,4 MHz / 61.035 Hz = 15047104 = 0xE599C0
      { 0xE5, 0xA6, 0x8D }, //Channel [1], 918,6 MHz / 61.035 Hz = 15050381 = 0xE5A68D
      { 0xE5, 0xB3, 0x5A }, //Channel [2], 918,8 MHz / 61.035 Hz = 15053658 = 0xE5B35A
      { 0xE5, 0xC0, 0x27 }, //Channel [3], 919,0 MHz / 61.035 Hz = 15056935 = 0xE5C027
      { 0xE5, 0xCC, 0xF3 }, //Channel [4], 919,2 MHz / 61.035 Hz = 15060211 = 0xE5CCF3
      { 0xE5, 0xD9, 0xC0 }, //Channel [5], 919,4 MHz / 61.035 Hz = 15063488 = 0xE5D9C0
      { 0xE5, 0xE6, 0x8D }, //Channel [6], 919,6 MHz / 61.035 Hz = 15066765 = 0xE5E68D
      { 0xE5, 0xF3, 0x5A }, //Channel [7], 919,8 MHz / 61.035 Hz = 15070042 = 0xE5F35A
    };
  #elif  defined(SUBND_3)//[920.0 - 921.4] MHz
    static const PROGMEM unsigned char LoRa_TX_Freq[8][3] = {
      { 0xE6, 0x00, 0x27 }, //Channel [0], 920,0 MHz / 61.035 Hz = 15073319 = 0xE60027
      { 0xE6, 0x0C, 0xF3 }, //Channel [1], 920,2 MHz / 61.035 Hz = 15076595 = 0xE60CF3
      { 0xE6, 0x19, 0xC0 }, //Channel [2], 920,4 MHz / 61.035 Hz = 15079872 = 0xE619C0
      { 0xE6, 0x26, 0x8D }, //Channel [3], 920,6 MHz / 61.035 Hz = 15083149 = 0xE6268D
      { 0xE6, 0x33, 0x5A }, //Channel [4], 920,8 MHz / 61.035 Hz = 15086426 = 0xE6335A
      { 0xE6, 0x40, 0x27 }, //Channel [5], 921,0 MHz / 61.035 Hz = 15089703 = 0xE64027
      { 0xE6, 0x4C, 0xF3 }, //Channel [6], 921,2 MHz / 61.035 Hz = 15092979 = 0xE64CF3
      { 0xE6, 0x59, 0xC0 }, //Channel [7], 921,4 MHz / 61.035 Hz = 15096256 = 0xE659C0
    };
  #elif  defined(SUBND_4)//[921.6 - 923.0] MHz
    static const PROGMEM unsigned char LoRa_TX_Freq[8][3] = {
      { 0xE6, 0x66, 0x8D }, //Channel [0], 921,6 MHz / 61.035 Hz = 15099533 = 0xE6668D
      { 0xE6, 0x73, 0x5A }, //Channel [1], 921,8 MHz / 61.035 Hz = 15102810 = 0xE6735A
      { 0xE6, 0x80, 0x27 }, //Channel [2], 922,0 MHz / 61.035 Hz = 15106087 = 0xE68027
      { 0xE6, 0x8C, 0xF3 }, //Channel [3], 922,2 MHz / 61.035 Hz = 15109363 = 0xE68CF3
      { 0xE6, 0x99, 0xC0 }, //Channel [4], 922,4 MHz / 61.035 Hz = 15112640 = 0xE699C0
      { 0xE6, 0xA6, 0x8D }, //Channel [5], 922,6 MHz / 61.035 Hz = 15115917 = 0xE6A68D
      { 0xE6, 0xB3, 0x5A }, //Channel [6], 922,8 MHz / 61.035 Hz = 15119194 = 0xE6B35A
      { 0xE6, 0xC0, 0x27 }, //Channel [7], 923,0 MHz / 61.035 Hz = 15122471 = 0xE6C027
    };
  #elif  defined(SUBND_5)//[923.2 - 924.6] MHz
    static const PROGMEM unsigned char LoRa_TX_Freq[8][3] = {
      { 0xE6, 0xCC, 0xF4 }, //Channel [0], 923,2 MHz / 61.035 Hz = 15125748 = 0xE6CCF4
      { 0xE6, 0xD9, 0xC0 }, //Channel [1], 923,4 MHz / 61.035 Hz = 15129024 = 0xE6D9C0
      { 0xE6, 0xE6, 0x8D }, //Channel [2], 923,6 MHz / 61.035 Hz = 15132301 = 0xE6E68D
      { 0xE6, 0xF3, 0x5A }, //Channel [3], 923,8 MHz / 61.035 Hz = 15135578 = 0xE6F35A
      { 0xE7, 0x00, 0x27 }, //Channel [4], 924,0 MHz / 61.035 Hz = 15138855 = 0xE70027
      { 0xE7, 0x0C, 0xF4 }, //Channel [5], 924,2 MHz / 61.035 Hz = 15142132 = 0xE70CF4
      { 0xE7, 0x19, 0xC0 }, //Channel [6], 924,4 MHz / 61.035 Hz = 15145408 = 0xE719C0
      { 0xE7, 0x26, 0x8D }, //Channel [7], 924,6 MHz / 61.035 Hz = 15148685 = 0xE7268D
    };
  #elif  defined(SUBND_6)//[924.8 - 926.2] MHz
    static const PROGMEM unsigned char LoRa_TX_Freq[8][3] = {
      { 0xE7, 0x33, 0x5A }, //Channel [0], 924,8 MHz / 61.035 Hz = 15151962 = 0xE7335A
      { 0xE7, 0x40, 0x27 }, //Channel [1], 925,0 MHz / 61.035 Hz = 15155239 = 0xE74027
      { 0xE7, 0x4C, 0xF4 }, //Channel [2], 925,2 MHz / 61.035 Hz = 15158516 = 0xE74CF4
      { 0xE7, 0x59, 0xC0 }, //Channel [3], 925,4 MHz / 61.035 Hz = 15161792 = 0xE759C0
      { 0xE7, 0x66, 0x8D }, //Channel [4], 925,6 MHz / 61.035 Hz = 15165069 = 0xE7668D
      { 0xE7, 0x73, 0x5A }, //Channel [5], 925,8 MHz / 61.035 Hz = 15168346 = 0xE7735A
      { 0xE7, 0x80, 0x27 }, //Channel [6], 926,0 MHz / 61.035 Hz = 15171623 = 0xE78027
      { 0xE7, 0x8C, 0xF4 }, //Channel [7], 926,2 MHz / 61.035 Hz = 15174900 = 0xE78CF4
    };
  #elif  defined(SUBND_7)//[926.4 - 927.8] MHz
    static const PROGMEM unsigned char LoRa_TX_Freq[8][3] = {
      { 0xE7, 0x99, 0xC0 }, //Channel [0], 926,4 MHz / 61.035 Hz = 15178176 = 0xE799C0
      { 0xE7, 0xA6, 0x8D }, //Channel [1], 926,6 MHz / 61.035 Hz = 15181453 = 0xE7A68D
      { 0xE7, 0xB3, 0x5A }, //Channel [2], 926,8 MHz / 61.035 Hz = 15184730 = 0xE7B35A
      { 0xE7, 0xC0, 0x27 }, //Channel [3], 927,0 MHz / 61.035 Hz = 15188007 = 0xE7C027
      { 0xE7, 0xCC, 0xF4 }, //Channel [4], 927,2 MHz / 61.035 Hz = 15191284 = 0xE7CCF4
      { 0xE7, 0xD9, 0xC0 }, //Channel [5], 927,4 MHz / 61.035 Hz = 15194560 = 0xE7D9C0
      { 0xE7, 0xE6, 0x8D }, //Channel [6], 927,6 MHz / 61.035 Hz = 15197837 = 0xE7E68D
      { 0xE7, 0xF3, 0x5A }, //Channel [7], 927,8 MHz / 61.035 Hz = 15201114 = 0xE7F35A
    };
  #endif
  static const PROGMEM unsigned char LoRa_RX_Freq[8][3] = {
    { 0xE6, 0xD3, 0x5A}, //Rcv Channel [0] 923.3 Mhz / 61.035 Hz = 15127386 = 0xE6D35A
    { 0xE6, 0xF9, 0xC0}, //Rcv Channel [1] 923.9 Mhz / 61.035 Hz = 15137216 = 0xE6F9C0
    { 0xE7, 0x20, 0x27}, //Rcv Channel [2] 924.5 Mhz / 61.035 Hz = 15147047 = 0xE72027
    { 0xE7, 0x46, 0x8D}, //Rcv Channel [3] 925.1 Mhz / 61.035 Hz = 15156877 = 0xE7468D
    { 0xE7, 0x6C, 0xF4}, //Rcv Channel [4] 925.7 Mhz / 61.035 Hz = 15166708 = 0xE76CF4
    { 0xE7, 0x93, 0x5A}, //Rcv Channel [5] 926.3 Mhz / 61.035 Hz = 15176538 = 0xE7935A
    { 0xE7, 0xB9, 0xC0}, //Rcv Channel [6] 926.9 Mhz / 61.035 Hz = 15186368 = 0xE7B9C0
    { 0xE7, 0xE0, 0x27}, //Rcv Channel [7] 927.5 Mhz / 61.035 Hz = 15196199 = 0xE7E027
  };
#elif defined(AS_923)
  static const PROGMEM unsigned char  [9][3] = {//[923.2 - 924.8] MHz
    { 0xE6, 0xCC, 0xF4 }, //Channel [0], 923.2 MHz / 61.035 Hz = 15125748 = 0xE6CCF4
    { 0xE6, 0xD9, 0xC0 }, //Channel [1], 923.4 MHz / 61.035 Hz = 15129024 = 0xE6D9C0
    { 0xE6, 0xE6, 0x8D }, //Channel [2], 923.6 MHz / 61.035 Hz = 15132301 = 0xE6E68D
    { 0xE6, 0xF3, 0x5A }, //Channel [3], 923.8 MHz / 61.035 Hz = 15135578 = 0xE6F35A
    { 0xE7, 0x00, 0x27 }, //Channel [4], 924.0 MHz / 61.035 Hz = 15138855 = 0xE70027
    { 0xE7, 0x0C, 0xF4 }, //Channel [5], 924.2 MHz / 61.035 Hz = 15142132 = 0xE70CF4
    { 0xE7, 0x19, 0xC0 }, //Channel [6], 924.4 MHz / 61.035 Hz = 15145408 = 0xE719C0
    { 0xE7, 0x26, 0x8D }, //Channel [7], 924.6 MHz / 61.035 Hz = 15148685 = 0xE7268D
    { 0xE7, 0x33, 0x5A }, //Channel [8], 924.8 MHz / 61.035 Hz = 15151962 = 0xE7335A
  };
#elif defined(EU_868)
  static const PROGMEM unsigned char LoRa_Frequency[9][3] = {//[868.1 - 867.9] MHz
    { 0xD9, 0x06, 0x8B }, //Channel [0], 868.1 MHz / 61.035 Hz = 14222987 = 0xD9068B
    { 0xD9, 0x13, 0x58 }, //Channel [1], 868.3 MHz / 61.035 Hz = 14226264 = 0xD91358
    { 0xD9, 0x20, 0x24 }, //Channel [2], 868.5 MHz / 61.035 Hz = 14229540 = 0xD92024
    { 0xD8, 0xC6, 0x8B }, //Channel [3], 867.1 MHz / 61.035 Hz = 14206603 = 0xD8C68B
    { 0xD8, 0xD3, 0x58 }, //Channel [4], 867.3 MHz / 61.035 Hz = 14209880 = 0xD8D358
    { 0xD8, 0xE0, 0x24 }, //Channel [5], 867.5 MHz / 61.035 Hz = 14213156 = 0xD8E024
    { 0xD8, 0xEC, 0xF1 }, //Channel [6], 867.7 MHz / 61.035 Hz = 14216433 = 0xD8ECF1
    { 0xD8, 0xF9, 0xBE }, //Channel [7], 867.9 MHz / 61.035 Hz = 14219710 = 0xD8F9BE
    { 0xD9, 0x61, 0xBE }, // RX2 Receive channel 869.525 MHz / 61.035 Hz = 14246334 = 0xD961BE    
  };
#endif

/*
*****************************************************************************************
* Description : Function that reads a register from the RFM and returns the value
*
* Arguments   : RFM_Address Address of register to be read
*
* Returns   : Value of the register
*****************************************************************************************
*/

static unsigned char RFM_Read(unsigned char RFM_Address)
{
  unsigned char RFM_Data;

  //Add transactions in Read and Write methods
  SPI.beginTransaction(SPISettings(4000000,MSBFIRST,SPI_MODE0));
  
  //Set NSS pin low to start SPI communication
  digitalWrite(RFM_pins.CS,LOW);

  //Send Address
  SPI.transfer(RFM_Address);
  //Send 0x00 to be able to receive the answer from the RFM
  RFM_Data = SPI.transfer(0x00);

  //Set NSS high to end communication
  digitalWrite(RFM_pins.CS,HIGH);

  //End the transaction so that other hardware can use the bus
  SPI.endTransaction();

  #ifdef DEBUG
  Serial.print("SPI Read ADDR: ");
  Serial.print(RFM_Address, HEX);
  Serial.print(" DATA: ");
  Serial.println(RFM_Data, HEX);
  #endif

  //Return received data
  return RFM_Data;
}
/********************************************************************************************
* Description : Change Spread Factor and Band Width
* 
* Arguments:    _SF = {6,7,8,9,10,11,12}
*               _BW = {0x00 -> 7.8khz   , 0x01 -> 10.4khz, 0x02 -> 15.6khz, 0x03 -> 20.8khz,
*                      0x04 -> 31.25khz , 0x05 -> 41.7khz, 0x06 -> 62.5khz, 0x07 -> 125khz, 
*                      0x08 -> 250khz   , 0x09 -> 500khz}
********************************************************************************************/
static void RFM_change_SF_BW(unsigned char _SF, unsigned char _BW)
{
	RFM_Write(RFM_REG_MODEM_CONFIG2, (_SF << 4) | 0b0100); //SFx CRC On
	RFM_Write(RFM_REG_MODEM_CONFIG1,(_BW << 4) | 0x02); //x kHz 4/5 coding rate explicit header mode

  #ifdef EU_868
  if(_SF>10)
    RFM_Write(RFM_REG_MODEM_CONFIG3, 0b1100); //Low datarate optimization on AGC auto on 
  else
    RFM_Write(RFM_REG_MODEM_CONFIG3, 0b0100); //Mobile node, low datarate optimization on AGC acorging to register LnaGain  
  #else
  RFM_Write(RFM_REG_MODEM_CONFIG3, 0b0100); //Mobile node, low datarate optimization on AGC acorging to register LnaGain
  #endif
    
}
/*
*****************************************************************************************
* Description : Function to change the datarate of the RFM module. Setting the following
*				register: Spreading factor, Bandwidth and low datarate optimisation.
*
* Arguments   : Datarate the datarate to set
*****************************************************************************************
*/
static void RFM_Change_Datarate(unsigned char Datarate)
{
#if defined(US_915)
  switch (Datarate) {
  case 0x00:  // SF10BW125
    RFM_change_SF_BW(10,0x07);
    break;
  case 0x01:  // SF9BW125
    RFM_change_SF_BW(9,0x07);
    break;
  case 0x02:  // SF8BW125
    RFM_change_SF_BW(8,0x07);
    break;
  case 0x03:  // SF7BW125
    RFM_change_SF_BW(7,0x07);
    break;
  case 0x04:  // SF8BW500
    RFM_change_SF_BW(8,0x09);
    break;
  case 0x08:  // SF12BW500
    RFM_change_SF_BW(12,0x09);
    break;
  case 0x09:  // SF11BW500
    RFM_change_SF_BW(11,0x09);
    break;
  case 0x0A:  // SF10BW500
    RFM_change_SF_BW(10,0x09);
    break;
  case 0x0B:  // SF9BW500
    RFM_change_SF_BW(9,0x09);
    break;
  case 0x0C:  // SF8BW500
    RFM_change_SF_BW(8,0x09);
    break;
  case 0x0D:  // SF7BW500
    RFM_change_SF_BW(7,0x09);
    break;
  }
#elif defined(AU_915)
  switch (Datarate) {
  case 0x00:  // SF10BW125
    RFM_change_SF_BW(10,0x07);
    break;
  case 0x01:  // SF9BW125
    RFM_change_SF_BW(9,0x07);
    break;
  case 0x02:  // SF8BW125
    RFM_change_SF_BW(8,0x07);
    break;
  case 0x03:  // SF7BW125
    RFM_change_SF_BW(7,0x07);
    break;
  case 0x04:  // SF8BW500
    RFM_change_SF_BW(8,0x09);
    break;
  case 0x08:  // SF12BW500
    RFM_change_SF_BW(12,0x09);
    break;
  case 0x09:  // SF11BW500
    RFM_change_SF_BW(11,0x09);
    break;
  case 0x0A:  // SF10BW500
    RFM_change_SF_BW(10,0x09);
    break;
  case 0x0B:  // SF9BW500
    RFM_change_SF_BW(9,0x09);
    break;
  case 0x0C:  // SF8BW500
    RFM_change_SF_BW(8,0x09);
    break;
  case 0x0D:  // SF7BW500
    RFM_change_SF_BW(7,0x09);
    break;
  }
#else //EU_868 or AS_923
  switch (Datarate) {
  case 0x00:  // SF12BW125
    RFM_change_SF_BW(12,0x07);
    break;
  case 0x01:  // SF11BW125
    RFM_change_SF_BW(11,0x07);
    break;
  case 0x02:  // SF10BW125
    RFM_change_SF_BW(10,0x07);
    break;
  case 0x03:  // SF9BW125
    RFM_change_SF_BW(9,0x07);
    break;
  case 0x04:  // SF8BW125
    RFM_change_SF_BW(8,0x07);
    break;
  case 0x05:  // SF7BW125
    RFM_change_SF_BW(7,0x07);
    break;
  case 0x06:  // SF7BW250
    RFM_change_SF_BW(7,0x08);
    break;
  }
#endif
}
/*
*****************************************************************************************
* Description : Function to change the channel of the RFM module. Setting the following
*				register: Channel
*
* Arguments   : Channel the channel to set
*****************************************************************************************
*/
static void RFM_Change_Channel(unsigned char Channel)
{
#if defined(AS_923)
  if (Channel <= 0x08)
    for(unsigned char i = 0 ; i < 3 ; ++i)
      RFM_Write(RFM_REG_FR_MSB + i, pgm_read_byte(&(LoRa_Frequency[Channel][i])));
  else if (Channel == 0x10)
    for(unsigned char i = 0 ; i < 3 ; ++i)
      RFM_Write(RFM_REG_FR_MSB + i, pgm_read_byte(&(LoRa_Frequency[0][i])));
#elif defined(EU_868)
  // in EU_868 v1.02 uses same freq for uplink and downlink
  if (Channel <= 0x08) 
    for(unsigned char i = 0 ; i < 3 ; ++i)
      RFM_Write(RFM_REG_FR_MSB + i, pgm_read_byte(&(LoRa_Frequency[Channel][i])));  
#else   //US915 or AU_915
  if (Channel <= 0x07)
    for(unsigned char i = 0 ; i < 3 ; ++i)
      RFM_Write(RFM_REG_FR_MSB + i, pgm_read_byte(&(LoRa_TX_Freq[Channel][i])));
  else if (Channel >= 0x08 && Channel <= 0x0F)
    for(unsigned char i = 0 ; i < 3 ; ++i) { 
      RFM_Write(RFM_REG_FR_MSB + i, pgm_read_byte(&(LoRa_RX_Freq[Channel - 0x08][i])));
    }
#endif
}

/*
*****************************************************************************************
* Description: Function used to initialize the RFM module on startup
*****************************************************************************************
*/
bool RFM_Init()
{
  
  uint8_t ver = RFM_Read(0x42);
  if(ver!=18){
    return 0;
  }
  //Switch RFM to sleep
  //DON'T USE Switch mode function
  RFM_Write(RFM_REG_OP_MODE, RFM_MODE_SLEEP);

  //Wait until RFM is in sleep mode
  delay(50);
 
  //Set RFM in LoRa mode
  //DON'T USE Switch mode function
  RFM_Write(RFM_REG_OP_MODE ,RFM_MODE_LORA);
  //Switch RFM to standby
  RFM_Switch_Mode(RFM_MODE_STANDBY);
  //Set channel to channel 0
  RFM_Change_Channel(CH0);
  //Set default power to maximun on US915 TODO AS/AU/EU config
  //Set the default output pin as PA_BOOST
  RFM_Set_Tx_Power(20, PA_BOOST_PIN);

  //Switch LNA boost on
  RFM_Write(RFM_REG_LNA,0x23);

  //Set RFM To datarate 0 SF12 BW 125 kHz
  RFM_Change_Datarate(0x00);

  //Rx Timeout set to 37 symbols
  RFM_Write(RFM_REG_SYM_TIMEOUT_LSB, 0x25);
  //RFM_Write(RFM_REG_SYM_TIMEOUT_LSB, 0x05);
  
  //Preamble length set to 8 symbols
  //0x0008 + 4 = 12
  RFM_Write(RFM_REG_PREAMBLE_MSB,0x00);
  RFM_Write(RFM_REG_PREAMBLE_LSB,0x08);

  //Set LoRa sync word
  RFM_Write(RFM_REG_SYNC_WORD, 0x34);

  //Set FIFO pointers
  //TX base address
  RFM_Write(0x0E,0x80);
  //Rx base address
  RFM_Write(0x0F,0x00);
  return 1;
}


void RFM_Set_Tx_Power(int level, int outputPin)
{
  if (RFO_PIN == outputPin) {
    // RFO
    if (level < 0) {
      level = 0;
    } else if (level > 14) {
      level = 14;
    }

    RFM_Write(RFM_REG_PA_CONFIG, 0x70 | level);
  } else {
    // PA BOOST
    if (level > 17) {
      if (level > 20) {
        level = 20;
      }

      // subtract 3 from level, so 18 - 20 maps to 15 - 17
      level -= 3;

      // High Power +20 dBm Operation (Semtech SX1276/77/78/79 5.4.3.)
      RFM_Write(RFM_REG_PA_DAC, 0x87);
      RFM_Set_OCP(140);
    } else {
      if (level < 2) {
        level = 2;
      }
      //Default value PA_HF/LF or +17dBm
      RFM_Write(RFM_REG_PA_DAC, 0x84);
      RFM_Set_OCP(100);
    }

    RFM_Write(RFM_REG_PA_CONFIG, 0x80 | (level - 2));  //PA Boost mask
  }
}


void RFM_Set_OCP(uint8_t mA)
{
  uint8_t ocpTrim = 27;

  if (mA <= 120) {
    ocpTrim = (mA - 45) / 5;
  } else if (mA <=240) {
    ocpTrim = (mA + 30) / 10;
  }

  RFM_Write(RFM_REG_OCP, 0x20 | (0x1F & ocpTrim));
}


/*
*****************************************************************************************
* Description : Function for sending a package with the RFM
*
* Arguments   : *RFM_Tx_Package pointer to buffer with data and counter of data
*               *LoRa_Settings pointer to sSettings struct
*****************************************************************************************
*/

void RFM_Send_Package(sBuffer *RFM_Tx_Package, sSettings *LoRa_Settings)
{
  unsigned char i;
  unsigned char RFM_Tx_Location = 0x00;

  //Set RFM in Standby mode
  RFM_Switch_Mode(RFM_MODE_STANDBY);

  //Switch Datarate
  RFM_Change_Datarate(LoRa_Settings->Datarate_Tx);

  //Switch Channel
  RFM_Change_Channel(LoRa_Settings->Channel_Tx);

  //Switch DIO0 to TxDone
  RFM_Write(RFM_REG_DIO_MAPPING1, 0x40);

  //Set IQ to normal values
  RFM_Write(RFM_REG_INVERT_IQ,0x27);
  RFM_Write(RFM_REG_INVERT_IQ2,0x1D);

  //Set payload length to the right length
  RFM_Write(RFM_REG_PAYLOAD_LENGTH,RFM_Tx_Package->Counter);

  //Get location of Tx part of FiFo
  RFM_Tx_Location = RFM_Read(0x0E);

  //Set SPI pointer to start of Tx part in FiFo
  RFM_Write(RFM_REG_FIFO_ADDR_PTR, RFM_Tx_Location);

  //Write Payload to FiFo
  for (i = 0;i < RFM_Tx_Package->Counter; i++)
  {
    RFM_Write(RFM_REG_FIFO, RFM_Tx_Package->Data[i]);
  }

  //Switch RFM to Tx
  RFM_Write(RFM_REG_OP_MODE,0x83);

  //Wait for TxDone
  while(digitalRead(RFM_pins.DIO0) == LOW);

  //Clear interrupt
  RFM_Write(RFM_REG_IRQ_FLAGS,0x08);
}

/*
*****************************************************************************************
* Description : Function to switch RFM to single receive mode, used for Class A motes
*
* Arguments   : *LoRa_Settings pointer to sSettings struct
*
* Return	  : message_t Status of the received message
*****************************************************************************************
*/

message_t RFM_Single_Receive(sSettings *LoRa_Settings)
{
  message_t Message_Status = NO_MESSAGE;
  
  //Change DIO 0 back to RxDone
  RFM_Write(RFM_REG_DIO_MAPPING1, 0x00);

  //Invert IQ Back
  RFM_Write(RFM_REG_INVERT_IQ, 0x67);
  RFM_Write(RFM_REG_INVERT_IQ2, 0x19);

  //Change Datarate
  RFM_Change_Datarate(LoRa_Settings->Datarate_Rx);

  //Change Channel
  RFM_Change_Channel(LoRa_Settings->Channel_Rx);

  //Switch RFM to Single reception
  RFM_Switch_Mode(RFM_MODE_RXSINGLE);

  //Wait until RxDone or Timeout
  //Wait until timeout or RxDone interrupt
  while((digitalRead(RFM_pins.DIO0) == LOW) && (digitalRead(RFM_pins.DIO1) == LOW));

  //Check for Timeout
  if(digitalRead(RFM_pins.DIO1) == HIGH)
  {
    //Clear interrupt register
    RFM_Write(RFM_REG_IRQ_FLAGS,0xE0);
    Message_Status = TIMEOUT;
  }

  //Check for RxDone
  if(digitalRead(RFM_pins.DIO0) == HIGH)
  {
	  Message_Status = NEW_MESSAGE;
  }

  return Message_Status;
}


/*
*****************************************************************************************
* Description : Function to switch RFM to continuous receive mode, used for Class C motes
*
* Arguments   : *LoRa_Settings pointer to sSettings struct
*****************************************************************************************
*/
void RFM_Continuous_Receive(sSettings *LoRa_Settings)
{
  //Change DIO 0 back to RxDone and DIO 1 to rx timeout
  RFM_Write(RFM_REG_DIO_MAPPING1,0x00);

  //Invert IQ Back
  RFM_Write(RFM_REG_INVERT_IQ, 0x67);
  RFM_Write(RFM_REG_INVERT_IQ2, 0x19);
  
	//Change Datarate and channel.
  // This depends on regional parameters
#ifdef EU_868
  RFM_Change_Datarate(SF12BW125);
  RFM_Change_Channel(CHRX2);
#else
  //Datarate for downlink should be 8 but testing on 10
  //LoRa_Settings->Datarate_Rx=10;
  //LoRa_Settings->Channel_Rx=0;
  //Serial.println("DataRate Rx "+String(LoRa_Settings->Datarate_Rx));
  //Serial.println("Cannel Rx "+String(LoRa_Settings->Channel_Rx));
  
	RFM_Change_Datarate(LoRa_Settings->Datarate_Rx);
	RFM_Change_Channel(LoRa_Settings->Channel_Rx);
#endif

	//Switch to continuous receive
	RFM_Switch_Mode(RFM_MODE_RXCONT);
}

/*
*****************************************************************************************
* Description : Function to retrieve a message received by the RFM
*
* Arguments   : *RFM_Rx_Package pointer to sBuffer struct containing the data received
*				and number of bytes received
*
* Return	  : message_t Status of the received message
*****************************************************************************************
*/

message_t RFM_Get_Package(sBuffer *RFM_Rx_Package)
{
  unsigned char i;
  unsigned char RFM_Interrupts = 0x00;
  unsigned char RFM_Package_Location = 0x00;
  message_t Message_Status;

  //Get interrupt register
  RFM_Interrupts = RFM_Read(0x12);

 
  if((RFM_Interrupts & 0x40)){ //IRQ_RX_DONE_MASK
    if((RFM_Interrupts & 0x20) != 0x20)  //Check CRC
    {
      Message_Status = CRC_OK;
    }
    else
    {
      Message_Status = WRONG_MESSAGE;
    }
  }
  RFM_Package_Location = RFM_Read(0x10); /*Read start position of received package*/
  RFM_Rx_Package->Counter = RFM_Read(0x13); /*Read length of received package*/

  RFM_Write(RFM_REG_FIFO_ADDR_PTR,RFM_Package_Location); /*Set SPI pointer to start of package*/

  for (i = 0x00; i < RFM_Rx_Package->Counter; i++)
  {
    RFM_Rx_Package->Data[i] = RFM_Read(RFM_REG_FIFO);
  }

  //Clear interrupt register
  RFM_Write(RFM_REG_IRQ_FLAGS,RFM_Interrupts);

  return Message_Status;
}

/*
*****************************************************************************************
* Description : Function that writes a register from the RFM
*
* Arguments   : RFM_Address Address of register to be written
*         RFM_Data    Data to be written
*****************************************************************************************
*/

void RFM_Write(unsigned char RFM_Address, unsigned char RFM_Data)
{
  // br: SPI Transfer Debug
  #ifdef DEBUG
    Serial.print("SPI Write ADDR: ");
    Serial.print(RFM_Address, HEX);
    Serial.print(" DATA: ");
    Serial.println(RFM_Data, HEX);
  #endif

  //Add transactions in Read and Write methods
  SPI.beginTransaction(SPISettings(4000000,MSBFIRST,SPI_MODE0));

  //Set NSS pin Low to start communication
  digitalWrite(RFM_pins.CS,LOW);

  //Send Address with MSB 1 to make it a writ command
  SPI.transfer(RFM_Address | 0x80);
  //Send Data
  SPI.transfer(RFM_Data);

  //Set NSS pin High to end communication
  digitalWrite(RFM_pins.CS,HIGH);

  //End the transaction so that other hardware can use the bus
  SPI.endTransaction();
}

/*
*****************************************************************************************
* Description : Function to change the operation mode of the RFM. Switching mode and wait
*				for mode ready flag
*               DO NOT USE FOR SLEEP
*
* Arguments   : Mode the mode to set
*****************************************************************************************
*/
void RFM_Switch_Mode(unsigned char Mode)
{
    Mode = Mode | 0x80; //Set high bit for LoRa mode

    //Switch mode on RFM module
    RFM_Write(RFM_REG_OP_MODE,Mode);
}

/*
*****************************************************************************************
* Description : Function to retrieve value of the last received packet rssi register
*****************************************************************************************
*/
unsigned char RFM_Get_Rssi()
{
  return RFM_Read(RFM_REG_LAST_RSSI);
}
