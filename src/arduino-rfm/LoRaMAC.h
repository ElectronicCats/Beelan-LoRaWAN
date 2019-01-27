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
* File:     LoRaMAC.h
* Author:   Gerben den Hartog
* Compagny: Ideetron B.V.
* Website:  http://www.ideetron.nl/LoRa
* E-mail:   info@ideetron.nl
****************************************************************************************/
/****************************************************************************************
* Created on:         06-01-2017
* Supported Hardware: ID150119-02 Nexus board with RFM95
****************************************************************************************/

#ifndef LORAMAC_H
#define LORAMAC_H

/*
********************************************************************************************
* INCLUDES
********************************************************************************************
*/

#include "Struct.h"

/*
********************************************************************************************
* TYPE DEFINITION
********************************************************************************************
*/

typedef enum {NO_RFM_COMMAND, NEW_RFM_COMMAND, RFM_COMMAND_DONE, JOIN} RFM_command_t;

/*
*****************************************************************************************
* FUNCTION PROTOTYPES
*****************************************************************************************
*/

void LORA_Cycle(sBuffer *Data_Tx, sBuffer *Data_Rx, RFM_command_t *RFM_Command, sLoRa_Session *Session_Data, sLoRa_OTAA *OTAA_Data, sLoRa_Message *Message_Rx, sSettings *LoRa_Settings);
void LORA_Send_Data(sBuffer *Data_Tx, sLoRa_Session *Session_Data, sSettings *LoRa_Settings);
void LORA_Receive_Data(sBuffer *Data_Rx, sLoRa_Session *Session_Data, sLoRa_OTAA *OTAA_Data, sLoRa_Message *Message, sSettings *LoRa_Settings);
void LoRa_Send_JoinReq(sLoRa_OTAA *OTAA_Data, sSettings *LoRa_Settings);
void Generate_DevNonce(unsigned char *DevNonce);
#endif

