/*
    loraid-arduino-rfm.h
    Copyright © 2018 lora.id. All right reserved.

    Author: Andri Rahmadhani
    Date: 2018-04-25

    Encapsulate Ideetron LoRaWAN simple node demonstrator
    *This fimrware supports
        *Over The Air Activation
        *Activation By Personalization
        *Class switching between Class A and Class C motes
        *Channel hopping
        *
        *The following settings kan be done
        *Channel Receive and Transmit
        *Datarate Receive and Transmit
        *Tranmit power
        *Confirmed or unconfirmed messages
        *Device Addres
        *Application Session Key
        *Network Session Key
        *Device EUI
        *Application EUI
        *Application key
        *Mote Class

    Use of this source code is governed by the MIT license that can be found in the LICENSE file.
*/

#ifndef _LORAID_ARDUINO_RFM_H_
#define _LORAID_ARDUINO_RFM_H_

#include <SPI.h>
#include <Arduino.h>
#include "AES-128.h"
#include "Encrypt.h"
#include "RFM95.h"
#include "LoRaMAC.h"
#include "Commands.h"
#include "Struct.h"
#include "Config.h"

/*
********************************************************************************************
* TYPE DEFINITION
********************************************************************************************
*/

typedef enum {NO_RX, NEW_RX} rx_t;

#define LORAID_VERSION "1.0.0"
/*
*****************************************************************************************
* CLASS
*****************************************************************************************
*/

class LoRaIdClass
{
    public:
    
        LoRaIdClass();
        ~LoRaIdClass();
        
        void init(void);
        void join(void);
        void setAccessKey(unsigned char *accessKey_in);
        void setAccessKey(char *accessKey_in);
        void setDeviceId(unsigned char *devAddr_in);
        void setDeviceId(char *devAddr_in);
        void setDeviceClass(devclass_t dev_class);
        void sendToAntares(unsigned char *data, unsigned int len, unsigned char confirm);
        String makeData(int data1, int data2, int data3, String ddata1, String ddata2, String ddata3);
        void sendToAntares(char *data, unsigned int len, unsigned char confirm);
        void setDataRate(unsigned char data_rate);
        void setTxPower(unsigned char power_idx);
        int readData(char *outBuff);
        void update(void);

    private:        
        // Messages
        unsigned char Data_Tx[64];
        sBuffer Buffer_Tx;
        unsigned char Data_Rx[64];
        sBuffer Buffer_Rx;
        sLoRa_Message Message_Rx;

        // Declare ABP session
        unsigned char Address_Tx[4];
        unsigned char NwkSKey[16];
        unsigned char AppSKey[16];
        unsigned int Frame_Counter_Tx;
        sLoRa_Session Session_Data;

        // Declare OTAA data struct
        unsigned char DevEUI[8];
        unsigned char AppEUI[8];
        unsigned char AppKey[16];
        unsigned char DevNonce[2];
        unsigned char AppNonce[3];
        unsigned char NetID[3];
        sLoRa_OTAA OTAA_Data;

        // Declare LoRA settings struct
        sSettings LoRa_Settings;
        unsigned char drate_common;

        // Lora Setting Class
        devclass_t dev_class;

        // UART
        RFM_command_t RFM_Command_Status;
        rx_t Rx_Status;
};

#endif
