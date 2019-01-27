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
* File:     Commands.h
* Author:   Gerben den Hartog
* Compagny: Ideetron B.V.
* Website:  http://www.ideetron.nl/LoRa
* E-mail:   info@ideetron.nl
****************************************************************************************/
/****************************************************************************************
* Created on:         13-01-2017
* Supported Hardware: ID150119-02 Nexus board with RFM95
****************************************************************************************/

#ifndef COMMANDS_H
#define COMMANDS_H

#include "Struct.h"

/*
*****************************************************************************************
* FUNCTION PROTOTYPES
*****************************************************************************************
*/

void UART_Send_Newline();
void UART_Send_Data(unsigned char *Data, unsigned char Length);
void UART_Send_Datarate(unsigned char *Datarate);
void UART_Send_Channel(unsigned char *Channel);
void Mac_DevAddr(unsigned char *buffer, unsigned char *DevAddr);
void Mac_NwkSKey(unsigned char *buffer, unsigned char *NwkSKey);
void Mac_AppSKey(unsigned char *buffer, unsigned char *AppSKey);
void Mac_AppKey(unsigned char *buffer, unsigned char *AppKey);
void Mac_AppEUI(unsigned char *buffer, unsigned char *AppEUI);
void Mac_DevEUI(unsigned char *buffer, unsigned char *DevEUI);
void Mac_DrTx(unsigned char drate_tx, unsigned char *Datarate);
void Mac_DrRx(unsigned char drate_rx, unsigned char *Datarate);
void Mac_ChTx(unsigned char ch_tx_idx, unsigned char *Channel);
void Mac_ChRx(unsigned char ch_rx_idx, unsigned char *Channel);
void Mac_Power(unsigned char pwr_idx, unsigned char *Power);
void Mac_Confirm(unsigned char confirm_msg, unsigned char *Confirm);
void Mac_Channel_Hopping(unsigned char hop_enable, unsigned char *Channel_Hopping);
void Mac_Class(devclass_t dev_class, sSettings *LoRa_Settings);
void Mac_Data(unsigned char *buffer, unsigned int len, sBuffer *RFM_Buffer);

#endif


