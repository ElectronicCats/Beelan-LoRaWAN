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

void UART_Send_Newline()
{
  Serial.write(0x0D); //Carriage return
  Serial.write(0x0A); //Line feed
}

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

void UART_Send_Datarate(unsigned char *Datarate)
{

  switch(*Datarate)
  {
      case 0x00:
          Serial.write("SF 12 BW 125");
          break;
      case 0x01:
          Serial.write("SF 11 BW 125");
          break;
      case 0x02:
          Serial.write("SF 10 BW 125");
          break;
      case 0x03:
          Serial.write("SF 9 BW 125");
          break;
      case 0x04:
          Serial.write("SF 8 BW 125");
          break;
      case 0x05:
          Serial.write("SF 7 BW 125");
          break;
      case 0x06:
          Serial.write("SF 7 BW 250");
          break;
  }

  UART_Send_Newline();
}

void UART_Send_Channel(unsigned char *Channel)
{
  switch(*Channel)
  {
    #ifdef AS_923
    case 0x00:
      Serial.write("923.200");
      break;
    case 0x01:
      Serial.write("923.400");
      break;
    case 0x02:
      Serial.write("923.600");
      break;
    case 0x03:
      Serial.write("923.800");
      break;
    case 0x04:
      Serial.write("924.000");
      break;
    case 0x05:
      Serial.write("924.200");
      break;
    case 0x06:
      Serial.write("924.400");
      break;
    case 0x07:
      Serial.write("924.600");
      break;
    case 0x08:
      Serial.write("924.500");
      break;
    case 0x10:
      Serial.write("923.200");
      break;
    #else
    case 0x00:
      Serial.write("868.100");
      break;
    case 0x01:
      Serial.write("868.300");
      break;
    case 0x02:
      Serial.write("868.500");
      break;
    case 0x03:
      Serial.write("867.100");
      break;
    case 0x04:
      Serial.write("867.300");
      break;
    case 0x05:
      Serial.write("867.500");
      break;
    case 0x06:
      Serial.write("867.700");
      break;
    case 0x07:
      Serial.write("867.900");
      break;
    case 0x10:
      Serial.write("869.525");
      break;
    #endif
  }

    UART_Send_Newline();

}

void Mac_DevAddr(unsigned char *buffer, unsigned char *DevAddr)
{

  //Check if it is a set command and there is enough data sent
  DevAddr[0] = ASCII2Hex(buffer[0],buffer[1]);
  DevAddr[1] = ASCII2Hex(buffer[2],buffer[3]);
  DevAddr[2] = ASCII2Hex(buffer[4],buffer[5]);
  DevAddr[3] = ASCII2Hex(buffer[6],buffer[7]);

  //Send set DevAddr
  Serial.write("DevAddr: ");
  UART_Send_Data(DevAddr, 0x04);
  UART_Send_Newline();
}

void Mac_NwkSKey(unsigned char *buffer, unsigned char *NwkSKey)
{
  unsigned char i;

  for(i = 0; i < 16; i++)
  {
    NwkSKey[i] = ASCII2Hex(buffer[i*2],buffer[(i*2)+1]);
  }

  //Send NwkSkey
  Serial.write("NwkSKey: ");
  UART_Send_Data(NwkSKey,0x10);
  UART_Send_Newline();
}

void Mac_AppSKey(unsigned char *buffer, unsigned char *AppSKey)
{
  unsigned char i;

  for(i = 0; i < 16; i++)
  {
    AppSKey[i] = ASCII2Hex(buffer[i*2],buffer[(i*2)+1]);
  }

  //Send AppSkey
  Serial.write("AppSKey: ");
  UART_Send_Data(AppSKey,0x10);
  UART_Send_Newline();
}

void Mac_AppKey(unsigned char *buffer, unsigned char *AppKey)
{
  unsigned char i;

  for(i = 0; i < 16; i++)
  {
    AppKey[i] = ASCII2Hex(buffer[i*2],buffer[(i*2)+1]);
  }

  //Send AppKey
  Serial.write("AppKey: ");
  UART_Send_Data(AppKey,0x10);
  UART_Send_Newline();

}

void Mac_AppEUI(unsigned char *buffer, unsigned char *AppEUI)
{
  unsigned char i;

  for(i = 0; i < 8; i++)
  {
    AppEUI[i] = ASCII2Hex(buffer[i*2],buffer[(i*2)+1]);
  }

  //Send AppEUI
  Serial.write("AppEUI: ");
  UART_Send_Data(AppEUI,0x08);
  UART_Send_Newline();
}

void Mac_DevEUI(unsigned char *buffer, unsigned char *DevEUI)
{
  unsigned char i;

  for(i = 0; i < 8; i++)
  {
    DevEUI[i] = ASCII2Hex(buffer[i*2],buffer[(i*2)+1]);
  }

  //Send DevEUI
  Serial.write("DevEUI: ");
  UART_Send_Data(DevEUI,0x08);
  UART_Send_Newline();
}

void Mac_DrTx(unsigned char drate_tx, unsigned char *Datarate)
{
  unsigned char Datarate_Temp;

  //Convert to temp
  Datarate_Temp = drate_tx;

  //Check if the value is oke
  if(Datarate_Temp <= 0x06)
  {
    *Datarate = Datarate_Temp;
  }

  Serial.write("Datarate Tx: ");

  UART_Send_Datarate(Datarate);
}

void Mac_DrRx(unsigned char drate_rx, unsigned char *Datarate)
{
  unsigned char Datarate_Temp;

  //Convert to temp
  Datarate_Temp = drate_rx;

  //Check if the value is oke
  if(Datarate_Temp <= 0x06)
  {
    *Datarate = Datarate_Temp;
  }

  Serial.write("Datarate Rx: ");

  UART_Send_Datarate(Datarate);
}

void Mac_ChTx(unsigned char ch_tx_idx, unsigned char *Channel)
{
 unsigned char Channel_Temp;

  //Convert to temp
  Channel_Temp = ch_tx_idx;

  //Check if the value is oke
  #ifdef AS_923
  if(Channel_Temp <= 0x08 || Channel_Temp == 0x10)
  {
    *Channel = Channel_Temp;
  }
  #else
  if(Channel_Temp <= 0x07 || Channel_Temp == 0x10)
  {
    *Channel = Channel_Temp;
  }
  #endif

  Serial.write("Channel Tx: ");

  UART_Send_Channel(Channel);
}

void Mac_ChRx(unsigned char ch_rx_idx, unsigned char *Channel)
{
  unsigned char Channel_Temp;

  //Convert to temp
  Channel_Temp = ch_rx_idx;

  //Check if the value is oke
  #ifdef AS_923
  if(Channel_Temp <= 0x08 || Channel_Temp == 0x10)
  {
    *Channel = Channel_Temp;
  }
  #else
  if(Channel_Temp <= 0x07 || Channel_Temp == 0x10)
  {
    *Channel = Channel_Temp;
  }
  #endif

  Serial.write("Channel Rx: ");

  UART_Send_Channel(Channel);
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

  //Send answer
  Serial.write("Power: ");
  UART_Send_Data(Power,0x01);
  UART_Send_Newline();
}

void Mac_Confirm(unsigned char confirm_msg, unsigned char *Confirm)
{
  
  *Confirm = confirm_msg;
  if(*Confirm >= 0x01)
  {
    *Confirm = 0x01;
  }

  //Send answer
  Serial.write("Confirm: ");
  UART_Send_Data(Confirm,0x01);
  UART_Send_Newline();
}

void Mac_Channel_Hopping(unsigned char hop_enable, unsigned char *Channel_Hopping)
{
  *Channel_Hopping = hop_enable;

  if(*Channel_Hopping >= 0x01)
  {
    *Channel_Hopping = 0x01;
  }

  //Send answer
  Serial.write("Channel Hopping: ");
  UART_Send_Data(Channel_Hopping,0x01);
  UART_Send_Newline();
}

void Mac_Class(devclass_t dev_class, sSettings *LoRa_Settings)
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
  Serial.write("Mote Class: ");
  if(LoRa_Settings->Mote_Class == 0x00)
  {

    //Switch RFM to standby
    RFM_Switch_Mode(0x01);
    
    Serial.write("A");
  }
  else
  {
    //Switch RFM to continuou receive
    RFM_Continuous_Receive(LoRa_Settings);
    
    Serial.write("C");
  }
  UART_Send_Newline();
}

void Mac_Data(unsigned char *buffer, unsigned int len, sBuffer *RFM_Buffer)
{
  RFM_Buffer->Counter = len;
  memcpy(RFM_Buffer->Data, buffer, len);
  // unsigned int len_temp = len;
  // unsigned char i;

  // //Check for an even number of received data bytes other wise fill whit 0x00
  // if(len % 2)
  // {
  //   buffer[len] = 0x00;
  //   len_temp++;
  // }

  // RFM_Buffer->Counter = len_temp / 2;

  // for(i = 0x00; i < RFM_Buffer->Counter; i++)
  // {
  //   RFM_Buffer->Data[i] = ASCII2Hex(buffer[i*2],buffer[(i*2)+1]);
  // }

  Serial.write("Data ");
  UART_Send_Data(RFM_Buffer->Data,RFM_Buffer->Counter);
  UART_Send_Newline();
}