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
* File:     Commands.cpp
* Author:   Gerben den Hartog
* Compagny: Ideetron B.V.
* Website:  http://www.ideetron.nl/LoRa
* E-mail:   info@ideetron.nl
****************************************************************************************/
/****************************************************************************************
* Created on:         12-01-2017
* Supported Hardware: ID150119-02 Nexus board with RFM95
****************************************************************************************/

/*
*****************************************************************************************
* INCLUDE FILES
*****************************************************************************************
*/

#include <SPI.h>
#include "Commands.h"
#include "Conversions.h"
#include "RFM95.h"
#include "Struct.h"
#include "Config.h"

void UART_Send_Data(unsigned char *Data, unsigned char Length)
{
  unsigned char i;
  unsigned char Upper_Nibble;
  unsigned char Lower_Nibble;

  for(i = 0; i < Length; i++)
  {
    Hex2ASCII(Data[i], &Upper_Nibble, &Lower_Nibble);

    //Send the data
    Serial.write(Upper_Nibble);
    Serial.write(Lower_Nibble);
  }
}

void Mac_DevAddr(unsigned char *buffer, unsigned char *DevAddr)
{

  //Check if it is a set command and there is enough data sent
  DevAddr[0] = ASCII2Hex(buffer[0],buffer[1]);
  DevAddr[1] = ASCII2Hex(buffer[2],buffer[3]);
  DevAddr[2] = ASCII2Hex(buffer[4],buffer[5]);
  DevAddr[3] = ASCII2Hex(buffer[6],buffer[7]);
}

void Mac_NwkSKey(unsigned char *buffer, unsigned char *NwkSKey)
{
  unsigned char i;

  for(i = 0; i < 16; i++)
  {
    NwkSKey[i] = ASCII2Hex(buffer[i*2],buffer[(i*2)+1]);
  }
}

void Mac_AppSKey(unsigned char *buffer, unsigned char *AppSKey)
{
  unsigned char i;

  for(i = 0; i < 16; i++)
  {
    AppSKey[i] = ASCII2Hex(buffer[i*2],buffer[(i*2)+1]);
  }
}

void Mac_AppKey(unsigned char *buffer, unsigned char *AppKey)
{
  unsigned char i;

  for(i = 0; i < 16; i++)
  {
    AppKey[i] = ASCII2Hex(buffer[i*2],buffer[(i*2)+1]);
  }
}

void Mac_AppEUI(unsigned char *buffer, unsigned char *AppEUI)
{
  unsigned char i;

  for(i = 0; i < 8; i++)
  {
    AppEUI[i] = ASCII2Hex(buffer[i*2],buffer[(i*2)+1]);
  }
}

void Mac_DevEUI(unsigned char *buffer, unsigned char *DevEUI)
{
  unsigned char i;

  for(i = 0; i < 8; i++)
  {
    DevEUI[i] = ASCII2Hex(buffer[i*2],buffer[(i*2)+1]);
  }
}

void Mac_DrTx(unsigned char drate_tx, unsigned char *Datarate)
{
#ifndef US_915
  //Check if the value is oke
  if(drate_tx <= 0x06)
  {
    *Datarate = drate_tx;
  }
#else
  if(drate_tx <= 0x04)
    *Datarate = drate_tx;
#endif
}

void Mac_DrRx(unsigned char drate_rx, unsigned char *Datarate)
{
#ifndef US_915
  //Check if the value is oke
  if(drate_rx <= 0x06)
  {
    *Datarate = drate_rx;
  }
#else
  if (drate_rx >= 0x08 && drate_rx <= 0x0D) 
    *Datarate = drate_rx;
#endif
}

void Mac_ChTx(unsigned char ch_tx_idx, unsigned char *Channel)
{
  //Check if the value is oke
#ifdef AS_923
  if(ch_tx_idx <= 0x08 || ch_tx_idx == 0x10)
    *Channel = ch_tx_idx;
#else
  if(ch_tx_idx <= 0x07 )
    *Channel = ch_tx_idx;
#endif
}

void Mac_ChRx(unsigned char ch_rx_idx, unsigned char *Channel)
{
  //Check if the value is oke
#ifdef AS_923
  if(ch_rx_idx <= 0x08 || ch_rx_idx == 0x10)
    *Channel = ch_rx_idx;
#else
  if(ch_rx_idx >= 0x08 || ch_rx_idx <= 0x0F)
    *Channel = ch_rx_idx;
#endif
}

void Mac_Power(unsigned char pwr_idx, unsigned char *Power)
{

  unsigned char RFM_Data;

  *Power = pwr_idx;

  //Check if power is not over 0x0F
  if(*Power > 0x0F)
  {
    *Power = 0x0F;
  }

  //Set all ther correct bits for the RFM register
  RFM_Data = *Power + 0xF0;
  //Write power to RFM module
  RFM_Write(0x09,RFM_Data);
}

void Mac_Confirm(unsigned char confirm_msg, unsigned char *Confirm)
{
  *Confirm = confirm_msg;
  if(*Confirm >= 0x01)
  {
    *Confirm = 0x01;
  }
}

void Mac_Channel_Hopping(unsigned char hop_enable, unsigned char *Channel_Hopping)
{
  *Channel_Hopping = hop_enable;

  if(*Channel_Hopping >= 0x01)
  {
    *Channel_Hopping = 0x01;
  }
}

int Mac_Class(devclass_t dev_class, sSettings *LoRa_Settings)
{
  if(dev_class == CLASS_A) 
  {
    LoRa_Settings->Mote_Class = 0x00;
  }
  else 
  {
    LoRa_Settings->Mote_Class = 0x01;
  }

  //Send answer and switch rfm to standby or receive
  if(LoRa_Settings->Mote_Class == 0x00)
  {

    //Switch RFM to standby
    RFM_Switch_Mode(0x01);
    
    return 0x01; //Class A;
  }
  else
  {
    //Switch RFM to continuou receive
    RFM_Continuous_Receive(LoRa_Settings);
    
    return 0x03; //Class C;
  }
}

void Mac_Data(unsigned char *buffer, unsigned int len, sBuffer *RFM_Buffer)
{
  RFM_Buffer->Counter = len;
  memcpy(RFM_Buffer->Data, buffer, len);
}