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
    static const unsigned char LoRa_TX_Freq[8][3] = {
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
    static const unsigned char LoRa_TX_Freq[8][3] = {
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
    static const unsigned char LoRa_TX_Freq[8][3] = {
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
    static const unsigned char LoRa_TX_Freq[8][3] = {
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
    static const unsigned char LoRa_TX_Freq[8][3] = {
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
    static const unsigned char LoRa_TX_Freq[8][3] = {
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
    static const unsigned char LoRa_TX_Freq[8][3] = {
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
    static const unsigned char LoRa_TX_Freq[8][3] = {
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
  static const unsigned char LoRa_RX_Freq[8][3] = {
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
  const unsigned char  [9][3] = {//[923.2 - 924.8] MHz
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
  static const unsigned char LoRa_Frequency[9][3] = {//[868.1 - 867.9] MHz
    { 0xD9, 0x06, 0x8B }, //Channel [0], 868.1 MHz / 61.035 Hz = 14222987 = 0xD9068B
    { 0xD9, 0x13, 0x58 }, //Channel [1], 868.3 MHz / 61.035 Hz = 14226264 = 0xD91358
    { 0xD9, 0x20, 0x24 }, //Channel [2], 868.5 MHz / 61.035 Hz = 14229540 = 0xD92024
    { 0xD8, 0xC6, 0x8B }, //Channel [3], 867.1 MHz / 61.035 Hz = 14206603 = 0xD8C68B
    { 0xD8, 0xD3, 0x58 }, //Channel [4], 867.3 MHz / 61.035 Hz = 14209880 = 0xD8D358
    { 0xD8, 0xE0, 0x24 }, //Channel [5], 867.5 MHz / 61.035 Hz = 14213156 = 0xD8E024
    { 0xD8, 0xEC, 0xF1 }, //Channel [6], 867.7 MHz / 61.035 Hz = 14216433 = 0xD8ECF1
    { 0xD8, 0xF9, 0xBE }, //Channel [7], 867.9 MHz / 61.035 Hz = 14219710 = 0xD8F9BE
    { 0xD9, 0x61, 0xBE }, //Receive channel 869.525 MHz / 61.035 Hz = 14246334 = 0xD961BE    
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

  //Set NSS pin low to start SPI communication
  digitalWrite(RFM_pins.CS,LOW);

  //Send Address
  SPI.transfer(RFM_Address);
  //Send 0x00 to be able to receive the answer from the RFM
  RFM_Data = SPI.transfer(0x00);

  //Set NSS high to end communication
  digitalWrite(RFM_pins.CS,HIGH);

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
* Arguments:    _SF = {6,7,,8,9,10,11,12}
*               _BW = {0x00 -> 7.8khz   , 0x01 -> 10.4khz, 0x02 -> 15.6khz, 0x03 -> 20.8khz,
*                      0x04 -> 31.25khz , 0x05 -> 41.7khz, 0x06 -> 62.5khz, 0x07 -> 125khz, 
*                      0x08 -> 250khz   , 0x09 -> 500khz}
********************************************************************************************/
static void RFM_change_SF_BW(unsigned char _SF, unsigned char _BW)
{
	RFM_Write(0x1E,(_SF << 4) | 0x04); //SFx CRC On
	RFM_Write(0x1D,(_BW << 4) | 0x02); //x kHz 4/5 coding rate explicit header mode
	RFM_Write(0x26,0x04); //Mobile node, low datarate optimization on AGC acorging to register LnaGain
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
#else
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
      RFM_Write(0x06 + i, pgm_read_byte(&(LoRa_Frequency[Channel][i])));
  else if (Channel == 0x10)
    for(unsigned char i = 0 ; i < 3 ; ++i)
      RFM_Write(0x06 + i, pgm_read_byte(&(LoRa_Frequency[0][i])));
#elif defined(EU_868)
  if (Channel <= 0x08)
    for(unsigned char i = 0 ; i < 3 ; ++i)
      RFM_Write(0x06 + i, pgm_read_byte(&(LoRa_Frequency[Channel][i])));
  else if (Channel == 0x10)
    for(unsigned char i = 0 ; i < 3 ; ++i)
      RFM_Write(0x06 + i, pgm_read_byte(&(LoRa_Frequency[8][i])));
#else   //US915
  if (Channel <= 0x07)
    for(unsigned char i = 0 ; i < 3 ; ++i)
      RFM_Write(0x06 + i, pgm_read_byte(&(LoRa_TX_Freq[Channel][i])));
  else if (Channel >= 0x08 && Channel <= 0x0F)
    for(unsigned char i = 0 ; i < 3 ; ++i)
      RFM_Write(0x06 + i, pgm_read_byte(&(LoRa_RX_Freq[Channel - 0x08][i])));
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
  RFM_Write(0x01,0x00);

  //Wait until RFM is in sleep mode
  delay(50);
 
  //Set RFM in LoRa mode
  //DON'T USE Switch mode function
  RFM_Write(0x01,0x80);
  //Switch RFM to standby
  RFM_Switch_Mode(0x01);
  //Set channel to channel 0
  RFM_Change_Channel(0x00);
  //PA pin (minimal power)
  //RFM_Write(0x09,0xF0);
  //set to 17dbm
  RFM_Write(0x09,0xF0);


  //Switch LNA boost on
  RFM_Write(0x0C,0x23);

  //Set RFM To datarate 0 SF12 BW 125 kHz
  RFM_Change_Datarate(0x00);

  //Rx Timeout set to 37 symbols
  RFM_Write(0x1F,0x25);

  //Preamble length set to 8 symbols
  //0x0008 + 4 = 12
  RFM_Write(0x20,0x00);
  RFM_Write(0x21,0x08);

  //Set LoRa sync word
  RFM_Write(0x39,0x34);

  //Set FIFO pointers
  //TX base address
  RFM_Write(0x0E,0x80);
  //Rx base address
  RFM_Write(0x0F,0x00);
  return 1;
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
  RFM_Switch_Mode(0x01);

  //Switch Datarate
  RFM_Change_Datarate(LoRa_Settings->Datarate_Tx);

  //Switch Channel
  RFM_Change_Channel(LoRa_Settings->Channel_Tx);

  //Switch DIO0 to TxDone
  RFM_Write(0x40,0x40);

  //Set IQ to normal values
  RFM_Write(0x33,0x27);
  RFM_Write(0x3B,0x1D);

  //Set payload length to the right length
  RFM_Write(0x22,RFM_Tx_Package->Counter);

  //Get location of Tx part of FiFo
  RFM_Tx_Location = RFM_Read(0x0E);

  //Set SPI pointer to start of Tx part in FiFo
  RFM_Write(0x0D,RFM_Tx_Location);

  //Write Payload to FiFo
  for (i = 0;i < RFM_Tx_Package->Counter; i++)
  {
    RFM_Write(0x00, RFM_Tx_Package->Data[i]);
  }

  //Switch RFM to Tx
  RFM_Write(0x01,0x83);

  //Wait for TxDone
  while(digitalRead(RFM_pins.DIO0) == LOW);

  //Clear interrupt
  RFM_Write(0x12,0x08);

  //Switch RFM back to receive if it is a type C mote
  if(LoRa_Settings->Mote_Class == 0x01)
  {
    //Switch Back to Continuous receive
    RFM_Continuous_Receive(LoRa_Settings);
  }
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
  RFM_Write(0x40,0x00);

  //Invert IQ Back
  RFM_Write(0x33,0x67);
  RFM_Write(0x3B,0x19);

  //Change Datarate
  RFM_Change_Datarate(LoRa_Settings->Datarate_Rx);

  //Change Channel
  RFM_Change_Channel(LoRa_Settings->Channel_Rx);

  //Switch RFM to Single reception
  RFM_Switch_Mode(0x06);

  //Wait until RxDone or Timeout
  //Wait until timeout or RxDone interrupt
  while((digitalRead(RFM_pins.DIO0) == LOW) && (digitalRead(RFM_pins.DIO1) == LOW));

  //Check for Timeout
  if(digitalRead(RFM_pins.DIO1) == HIGH)
  {
    //Clear interrupt register
    RFM_Write(0x12,0xE0);
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
  //Change DIO 0 back to RxDone
  RFM_Write(0x40,0x00);

  //Invert IQ Back
  RFM_Write(0x33,0x67);
  RFM_Write(0x3B,0x19);
  
	//Change Datarate
	RFM_Change_Datarate(LoRa_Settings->Datarate_Rx);

	//Change Channel
	RFM_Change_Channel(LoRa_Settings->Channel_Rx);

	//Switch to continuous receive
	RFM_Switch_Mode(0x05);
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

  //Check CRC
  if((RFM_Interrupts & 0x20) != 0x20)
  {
	  Message_Status = CRC_OK;
  }
  else
  {
	  Message_Status = WRONG_MESSAGE;
  }

  RFM_Package_Location = RFM_Read(0x10); /*Read start position of received package*/
  RFM_Rx_Package->Counter = RFM_Read(0x13); /*Read length of received package*/

  RFM_Write(0x0D,RFM_Package_Location); /*Set SPI pointer to start of package*/

  for (i = 0x00; i < RFM_Rx_Package->Counter; i++)
  {
    RFM_Rx_Package->Data[i] = RFM_Read(0x00);
  }

  //Clear interrupt register
  RFM_Write(0x12,0xE0);

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

  //Set NSS pin Low to start communication
  digitalWrite(RFM_pins.CS,LOW);

  //Send Address with MSB 1 to make it a writ command
  SPI.transfer(RFM_Address | 0x80);
  //Send Data
  SPI.transfer(RFM_Data);

  //Set NSS pin High to end communication
  digitalWrite(RFM_pins.CS,HIGH);
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
    RFM_Write(0x01,Mode);
}

