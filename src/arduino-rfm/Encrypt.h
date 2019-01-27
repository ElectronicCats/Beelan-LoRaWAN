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
* File:     Encrypt.h
* Author:   Gerben den Hartog
* Compagny: Ideetron B.V.
* Website:  http://www.ideetron.nl/LoRa
* E-mail:   info@ideetron.nl
****************************************************************************************/
/****************************************************************************************
* Created on:         09-02-2017
* Supported Hardware: ID150119-02 Nexus board with RFM95
****************************************************************************************/

#ifndef ENCRYPT_H
#define ENCRYPT_H

/*
********************************************************************************************
* INCLUDES
********************************************************************************************
*/

#include "Struct.h"

/*
*****************************************************************************************
* FUNCTION PROTOTYPES
*****************************************************************************************
*/

void Construct_Data_MIC(sBuffer *Buffer, sLoRa_Session *Session_Data, sLoRa_Message *Message);
void Calculate_MIC(sBuffer *Buffer, unsigned char *Key, sLoRa_Message *Message);
void Encrypt_Payload(sBuffer *Buffer, unsigned char *Key, sLoRa_Message *Message);
void Generate_Keys(unsigned char *Key, unsigned char *K1, unsigned char *K2);
void Shift_Left(unsigned char *Data);
void XOR(unsigned char *New_Data,unsigned char *Old_Data);

#endif

