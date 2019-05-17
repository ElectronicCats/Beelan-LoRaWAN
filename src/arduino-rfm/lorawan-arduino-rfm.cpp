/*
    loraid-arduino-rfm.cpp
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
        *The following settings can be done
        *Channel Receive and Transmit
        *Datarate Receive and Transmit
        *Transmit power
        *Confirmed or unconfirmed messages
        *Device Address
        *Application Session Key
        *Network Session Key
        *Device EUI
        *Application EUI
        *Application key
        *Mote Class

    Use of this source code is governed by the MIT license that can be found in the LICENSE file.
*/


#include "lorawan-arduino-rfm.h"


LoRaWANClass::LoRaWANClass()
{

}

LoRaWANClass::~LoRaWANClass()
{

}

bool LoRaWANClass::init(void)
{
    // Lora Setting Class
    dev_class = CLASS_A;
    // Random seed
    randomSeed(analogRead(0));

    // Status
    RFM_Command_Status = NO_RFM_COMMAND;
    Rx_Status = NO_RX;

    // current channel
    currentChannel = MULTI;

    // Initialise session data struct (Semtech default key)
    memset(Address_Tx, 0x00, 4);
    memset(NwkSKey, 0x00, 16);
    memset(AppSKey, 0x00, 16);
    // Mac_NwkSKey(semtech_key, NwkSKey);
    // Mac_AppSKey(semtech_key, AppSKey);
    Frame_Counter_Tx = 0x0000;
    Session_Data.NwkSKey = NwkSKey;
    Session_Data.AppSKey = AppSKey;
    Session_Data.DevAddr = Address_Tx;
    Session_Data.Frame_Counter = &Frame_Counter_Tx;

    //Initialize OTAA data struct
    memset(DevEUI, 0x00, 8);
    memset(AppEUI, 0x00, 8);
    // Mac_AppKey(semtech_key, AppKey);
    memset(AppKey, 0x00, 16);
    memset(DevNonce, 0x00, 2);
    memset(AppNonce, 0x00, 3);
    memset(NetID, 0x00, 3);
    OTAA_Data.DevEUI = DevEUI;
    OTAA_Data.AppEUI = AppEUI;
    OTAA_Data.AppKey = AppKey;
    OTAA_Data.DevNonce = DevNonce;
    OTAA_Data.AppNonce = AppNonce;
    OTAA_Data.NetID = NetID;
    
    // Device Class
    LoRa_Settings.Mote_Class = 0x00; //0x00 is type A, 0x01 is type C
    
    // Rx
#if defined(AS_923)
    LoRa_Settings.Datarate_Rx = 0x02;   //set to SF10 BW 125 kHz
#elif defined(EU_868)
    LoRa_Settings.Datarate_Rx = 0x03;   //set to SF9 BW 125 kHz
#else //US_915
    LoRa_Settings.Datarate_Rx = 0x0C;   //set to SF8 BW 500 kHz
#endif
    LoRa_Settings.Channel_Rx = 0x0A;    // set to recv channel

    // Tx
#if defined(US_915)
    LoRa_Settings.Datarate_Tx = drate_common = 0x02;   //set to SF7 BW 125 kHz
#else
    LoRa_Settings.Datarate_Tx = drate_common = 0x00;   //set to SF12 BW 125 kHz
#endif
    LoRa_Settings.Channel_Tx = 0x00;    // set to channel 0

    LoRa_Settings.Confirm = 0x00; //0x00 unconfirmed, 0x01 confirmed
    LoRa_Settings.Channel_Hopping = 0x00; //0x00 no channel hopping, 0x01 channel hopping

    // Initialise buffer for data to transmit
    memset(Data_Tx, 0x00, sizeof(Data_Tx));
    Buffer_Tx.Data = Data_Tx;
    Buffer_Tx.Counter = 0x00;
    // Initialise buffer for data to receive
    memset(Data_Rx, 0x00, sizeof(Data_Rx));
    Buffer_Rx.Data = Data_Rx;
    Buffer_Rx.Counter = 0x00;
    Message_Rx.Direction = 0x01; //Set down direction for Rx message

    //Initialize I/O pins
    pinMode(RFM_pins.DIO0,INPUT);
    pinMode(RFM_pins.DIO1,INPUT);
    #ifdef BOARD_DRAGINO_SHIELD
    pinMode(RFM_pins.DIO5,INPUT);
    #endif
    pinMode(RFM_pins.DIO2,INPUT);
    pinMode(RFM_pins.CS,OUTPUT);
    pinMode(RFM_pins.RST,OUTPUT);
    
    digitalWrite(RFM_pins.CS,HIGH);
    
    // Reset
    digitalWrite(RFM_pins.RST,HIGH);
    delay(10);
    digitalWrite(RFM_pins.RST,LOW);
    delay(10);
    digitalWrite(RFM_pins.RST,HIGH);

    //Initialise the SPI port
    SPI.begin();
    SPI.beginTransaction(SPISettings(4000000,MSBFIRST,SPI_MODE0));

    //Wait until RFM module is started
    delay(50);
    
    //Initialize RFM module
    if(!RFM_Init()){
    return 0;
    }
    return 1;
}

bool LoRaWANClass::join(void)
{
    //Check if there is no command pending
    if(RFM_Command_Status == NO_RFM_COMMAND)
    {
        //Set join command
        RFM_Command_Status = JOIN;
        return true;
    }
    else
    {
        return false;
    }
    
}

void LoRaWANClass::setNwkSKey(unsigned char *NwkKey_in)
{
    Mac_NwkSKey(NwkKey_in, NwkSKey);
    //Reset frame counter
    Frame_Counter_Tx = 0x0000;

    //Reset RFM command status
    RFM_Command_Status = NO_RFM_COMMAND;
}

void LoRaWANClass::setNwkSKey(char *NwkKey_in)
{
    setNwkSKey((unsigned char *)NwkKey_in);
}

void LoRaWANClass::setAppSKey(unsigned char *ApskKey_in)
{
    Mac_AppSKey(ApskKey_in, AppSKey);
    //Reset frame counter
    Frame_Counter_Tx = 0x0000;

    //Reset RFM command status
    RFM_Command_Status = NO_RFM_COMMAND;
}

void LoRaWANClass::setAppSKey(char *ApskKey_in)
{
    setAppSKey((unsigned char *)ApskKey_in);
}


void LoRaWANClass::setDevAddr(unsigned char *devAddr_in)
{
    memset(Session_Data.DevAddr, 0x30, sizeof(Session_Data.DevAddr));

    Mac_DevAddr(devAddr_in, Address_Tx);

    //Reset frame counter
    Frame_Counter_Tx = 0x0000;

    //Reset RFM command status
    RFM_Command_Status = NO_RFM_COMMAND;
}

void LoRaWANClass::setDevAddr(char *devAddr_in)
{
    setDevAddr((unsigned char *)devAddr_in);
}

void LoRaWANClass::setDeviceClass(devclass_t dev_class)
{
    Mac_Class(dev_class, &LoRa_Settings);
    
    //Reset RFM command
    RFM_Command_Status = NO_RFM_COMMAND;
}

void LoRaWANClass::sendUplink(unsigned char *data, unsigned int len, unsigned char confirm)
{
    unsigned char freq_idx;

    // Random freq
#ifdef AS_923
    if (currentChannel == MULTI)
      freq_idx = random(0, 9);
    else
      freq_idx = currentChannel;
    // freq_idx = 4;    // test
    
    // limit drate, ch 8 -> sf7bw250
    if(freq_idx == 0x08)
    {
        Mac_DrTx(0x06, &LoRa_Settings.Datarate_Tx);
    }
    else
    {
        Mac_DrTx(drate_common, &LoRa_Settings.Datarate_Tx);
    }
#else
    if (currentChannel == MULTI)
      freq_idx = random(0, 8);
    else
      freq_idx = currentChannel;
    Mac_ChRx(freq_idx + 0x08, &LoRa_Settings.Channel_Rx);
#endif
    Mac_ChTx(freq_idx, &LoRa_Settings.Channel_Tx);

    Mac_Confirm(confirm, &LoRa_Settings.Confirm);

    //Set new command for RFM
    RFM_Command_Status = NEW_RFM_COMMAND;
    
    Mac_Data(data, len, &Buffer_Tx);
}

void LoRaWANClass::sendUplink(char *data, unsigned int len, unsigned char confirm)
{
    sendUplink((unsigned char *)data, len, confirm);
}


void LoRaWANClass::setDataRate(unsigned char data_rate)
{
    drate_common = data_rate;
    Mac_DrTx(data_rate, &LoRa_Settings.Datarate_Tx);
#ifdef US_915
    Mac_DrRx(data_rate + 0x0A, &LoRa_Settings.Datarate_Rx);
#endif
    //Reset RFM command
    RFM_Command_Status = NO_RFM_COMMAND;
}

void LoRaWANClass::setChannel(channel_t channel)
{
    if (channel <= 7 || channel == MULTI)
      currentChannel = channel;
}

void LoRaWANClass::setChannel(unsigned char channel) 
{
    setChannel((channel_t)channel);
}

void LoRaWANClass::setTxPower(unsigned char power_idx)
{
    Mac_Power(power_idx, &LoRa_Settings.Transmit_Power);
}

int LoRaWANClass::readData(char *outBuff)
{
    int res = 0;

    //If there is new data
    if(Rx_Status == NEW_RX)
    {
        res = Buffer_Rx.Counter;
        memset(outBuff, 0x00, res + 1);
        memcpy(outBuff, Buffer_Rx.Data, res);
        
        // Clear Buffer counter
        Buffer_Rx.Counter = 0x00;
        
        Rx_Status = NO_RX;
    }

    return res;
}

void LoRaWANClass::update(void)
{
    //Type A mote transmit receive cycle
    if((RFM_Command_Status == NEW_RFM_COMMAND || RFM_Command_Status == JOIN) && LoRa_Settings.Mote_Class == 0x00)
    {
      //LoRa cycle
      LORA_Cycle(&Buffer_Tx, &Buffer_Rx, &RFM_Command_Status, &Session_Data, &OTAA_Data, &Message_Rx, &LoRa_Settings);

      if(Buffer_Rx.Counter != 0x00)
      {
        Rx_Status = NEW_RX;
      }
      
      RFM_Command_Status = NO_RFM_COMMAND;
    }

    //Type C mote transmit and receive handling
    if(LoRa_Settings.Mote_Class == 0x01)
    {
      if(RFM_Command_Status == JOIN)
      {
        //Start join precedure
        LoRa_Send_JoinReq(&OTAA_Data, &LoRa_Settings);

        //Clear RFM_Command
        RFM_Command_Status = NO_RFM_COMMAND;
      }

      //Transmit
      if(RFM_Command_Status == NEW_RFM_COMMAND)
      {
        //Lora send data
        LORA_Send_Data(&Buffer_Tx, &Session_Data, &LoRa_Settings);

        RFM_Command_Status = NO_RFM_COMMAND;
      }

      //Receive
      if(digitalRead(RFM_pins.DIO0) == HIGH)
      {
        //Get data
        LORA_Receive_Data(&Buffer_Rx, &Session_Data, &OTAA_Data, &Message_Rx, &LoRa_Settings);

        if(Buffer_Rx.Counter != 0x00)
        {
          Rx_Status = NEW_RX;
        }        
      }
      RFM_Command_Status = NO_RFM_COMMAND;
    }

}

// define lora objet 
LoRaWANClass lora;
