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
* File:     RFM95.h
* Author:   Gerben den Hartog
* Compagny: Ideetron B.V.
* Website:  http://www.ideetron.nl/LoRa
* E-mail:   info@ideetron.nl
****************************************************************************************/
/****************************************************************************************
* Created on:         06-01-2017
* Supported Hardware: ID150119-02 Nexus board with RFM95
****************************************************************************************/

#ifndef RFM95_H
#define RFM95_H

/*
********************************************************************************************
* INCLUDES
********************************************************************************************
*/

#include "Struct.h"

/*
*****************************************************************************************
* TYPE DEFENITIONS
*****************************************************************************************
*/

typedef enum {NO_MESSAGE,NEW_MESSAGE,CRC_OK,MIC_OK,ADDRESS_OK,MESSAGE_DONE,TIMEOUT,WRONG_MESSAGE} message_t;

/*
*****************************************************************************************
* FUNCTION PROTOTYPES
*****************************************************************************************
*/

void RFM_Init();
void RFM_Send_Package(sBuffer *RFM_Tx_Package, sSettings *LoRa_Settings);
message_t RFM_Single_Receive(sSettings *LoRa_Settings);
void RFM_Continuous_Receive(sSettings *LoRa_Settings);
message_t RFM_Get_Package(sBuffer *RFM_Rx_Package);
unsigned char RFM_Read(unsigned char RFM_Address);
void RFM_Write(unsigned char RFM_Address, unsigned char RFM_Data);
void RFM_Change_Datarate(unsigned char Datarate);
void RFM_Change_Channel(unsigned char Channel);
void RFM_Switch_Mode(unsigned char Mode);

#endif

