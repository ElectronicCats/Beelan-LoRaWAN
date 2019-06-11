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
* File:     Encrypt.cpp
* Author:   Gerben den Hartog
* Compagny: Ideetron B.V.
* Website:  http://www.ideetron.nl/LoRa
* E-mail:   info@ideetron.nl
****************************************************************************************/
/****************************************************************************************
* Created on:         09-02-2017
* Supported Hardware: ID150119-02 Nexus board with RFM95
****************************************************************************************/

/*
*****************************************************************************************
* INCLUDE FILES
*****************************************************************************************
*/

#include "Encrypt.h"
#include "AES-128.h"
#include "Struct.h"

/*
*****************************************************************************************
* INCLUDE GLOBAL VARIABLES
*****************************************************************************************
*/

static void XOR(unsigned char *New_Data,unsigned char *Old_Data)
{
	unsigned char i;

	for(i = 0; i < 16; i++)
	{
		New_Data[i] = New_Data[i] ^ Old_Data[i];
	}
}

static void Shift_Left(unsigned char *Data)
{
	unsigned char i;
	unsigned char Overflow = 0;
	//unsigned char High_Byte, Low_Byte;

	for(i = 0; i < 16; i++)
	{
		//Check for overflow on next byte except for the last byte
		if(i < 15)
		{
			//Check if upper bit is one
			if((Data[i+1] & 0x80) == 0x80)
			{
				Overflow = 1;
			}
			else
			{
				Overflow = 0;
			}
		}
		else
		{
			Overflow = 0;
		}

		//Shift one left
		Data[i] = (Data[i] << 1) + Overflow;
	}
}
/*
*****************************************************************************************
* Description : Function used to encrypt and decrypt the data in a LoRaWAN data message
*
* Arguments   : *Buffer pointer to the buffer containing the data to de/encrypt
*				*Session_Data pointer to sLoRa_Session sturct
*				*Message pointer to sLoRa_Message struct containing the message specific variables
*****************************************************************************************
*/
void Encrypt_Payload(sBuffer *Buffer, unsigned char *Key, sLoRa_Message *Message)
{
	unsigned char i = 0x00;
	unsigned char j;
	unsigned char Number_of_Blocks = 0x00;
	unsigned char Incomplete_Block_Size = 0x00;

	unsigned char Block_A[16];

	//Calculate number of blocks
	Number_of_Blocks = Buffer->Counter / 16;
	Incomplete_Block_Size = Buffer->Counter % 16;
	if(Incomplete_Block_Size != 0)
	{
		Number_of_Blocks++;
	}

	for(i = 0x00; i < Number_of_Blocks; i++)
	{
		Block_A[0] = 0x01;
		Block_A[1] = 0x00;
		Block_A[2] = 0x00;
		Block_A[3] = 0x00;
		Block_A[4] = 0x00;

		Block_A[5] = Message->Direction;

		Block_A[6] = Message->DevAddr[3];
		Block_A[7] = Message->DevAddr[2];
		Block_A[8] = Message->DevAddr[1];
		Block_A[9] = Message->DevAddr[0];

		Block_A[10] = (Message->Frame_Counter & 0x00FF);
		Block_A[11] = ((Message->Frame_Counter >> 8) & 0x00FF);

		Block_A[12] = 0x00; //Frame counter upper Bytes
		Block_A[13] = 0x00;

		Block_A[14] = 0x00;

		Block_A[15] = i + 1;

		//Calculate S
		AES_Encrypt(Block_A,Key);

		//Check for last block
		if(i != (Number_of_Blocks - 1))
		{
			for(j = 0; j < 16; j++)
			{
				Buffer->Data[(i*16)+j] ^= Block_A[j];
			}
		}
		else
		{
			if(Incomplete_Block_Size == 0)
			{
				Incomplete_Block_Size = 16;
			}
			for(j = 0; j < Incomplete_Block_Size; j++)
			{
				Buffer->Data[(i*16)+j] ^= Block_A[j];
			}
		}
	}
}

/*
*****************************************************************************************
* Description : Function used to build a the data that is used for calculating the MIC of a data message
*
* Arguments   : *Buffer pointer to the buffer containing the data
*				*Session_Data pointer to sLoRa_Session sturct
*				*Message pointer to sLoRa_Message struct containing the message specific variables
*****************************************************************************************
*/
void Construct_Data_MIC(sBuffer *Buffer, sLoRa_Session *Session_Data, sLoRa_Message *Message)
{
    unsigned char i;
    unsigned char MIC_Data[80];
    sBuffer MIC_Buffer = { &MIC_Data[0], 0x00 };

    unsigned char Block_B[16];

    //Construct Block B
	Block_B[0] = 0x49;
	Block_B[1] = 0x00;
	Block_B[2] = 0x00;
	Block_B[3] = 0x00;
	Block_B[4] = 0x00;

	Block_B[5] = Message->Direction;

	Block_B[6] = Message->DevAddr[3];
	Block_B[7] = Message->DevAddr[2];
	Block_B[8] = Message->DevAddr[1];
	Block_B[9] = Message->DevAddr[0];

	Block_B[10] = (Message->Frame_Counter & 0x00FF);
	Block_B[11] = ((Message->Frame_Counter >> 8) & 0x00FF);

	Block_B[12] = 0x00; //Frame counter upper bytes
	Block_B[13] = 0x00;

	Block_B[14] = 0x00;
	Block_B[15] = Buffer->Counter;

    //Copy Block B into MIC data
    for(i = 0x00; i < 16; i++)
    {
        MIC_Data[i] = Block_B[i];
    }

    //Add data to it
    for(i = 0x00; i < Buffer->Counter; i++)
    {
        MIC_Data[i + 16] = Buffer->Data[i];
    }

    //Calculate the correct buffer length
    MIC_Buffer.Counter = 16 + Buffer->Counter;

    //Calculate the MIC
    Calculate_MIC(&MIC_Buffer, Session_Data->NwkSKey, Message);
}

/*
*****************************************************************************************
* Description : Function used to calculate the MIC of data
*
* Arguments   : *Buffer pointer to the buffer containing the data the MIC should be calculated from
*				*Key pointer to key used for the MIC calculation
*				*Message pointer to sLoRa_Message struct containing the message specific variables
*****************************************************************************************
*/
void Calculate_MIC(sBuffer *Buffer, unsigned char *Key, sLoRa_Message *Message)
{
	unsigned char i, j;
	unsigned char Key_K1[16] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	unsigned char Key_K2[16] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	unsigned char Old_Data[16] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	unsigned char New_Data[16] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	unsigned char Number_of_Blocks = 0x00;
	unsigned char Incomplete_Block_Size = 0x00;

	//Calculate number of Blocks and blocksize of last block
	Number_of_Blocks = Buffer->Counter / 16;
	Incomplete_Block_Size = Buffer->Counter % 16;

    //if there is an incomplete block at the end add 1 to the number of blocks
	if(Incomplete_Block_Size != 0)
	{
		Number_of_Blocks++;
	}

	Generate_Keys(Key, Key_K1, Key_K2);

	//Perform full calculating until n-1 message blocks
    for(j = 0x0; j < (Number_of_Blocks - 1); j++)
	{
		//Copy data into array
		for(i = 0; i < 16; i++)
		{
			New_Data[i] = Buffer->Data[(j*16)+i];
		}

		//Perform XOR with old data
		XOR(New_Data,Old_Data);

		//Perform AES encryption
		AES_Encrypt(New_Data,Key);

		//Copy New_Data to Old_Data
		for(i = 0; i < 16; i++)
		{
			Old_Data[i] = New_Data[i];
		}
	}

	//Perform calculation on last block
	//Check if Datalength is a multiple of 16
	if(Incomplete_Block_Size == 0)
	{
		//Copy last data into array
		for(i = 0; i < 16; i++)
		{
			New_Data[i] = Buffer->Data[((Number_of_Blocks -1)*16)+i];
		}

		//Perform XOR with Key 1
		XOR(New_Data,Key_K1);

		//Perform XOR with old data
		XOR(New_Data,Old_Data);

		//Perform last AES routine
		AES_Encrypt(New_Data,Key);
	}
	else
	{
		//Copy the remaining data and fill the rest
		for(i =  0; i < 16; i++)
		{
			if(i < Incomplete_Block_Size)
			{
				New_Data[i] = Buffer->Data[((Number_of_Blocks -1)*16)+i];
			}
			if(i == Incomplete_Block_Size)
			{
				New_Data[i] = 0x80;
			}
			if(i > Incomplete_Block_Size)
			{
				New_Data[i] = 0x00;
			}
		}

		//Perform XOR with Key 2
		XOR(New_Data,Key_K2);

		//Perform XOR with Old data
		XOR(New_Data,Old_Data);

		//Perform last AES routine
		AES_Encrypt(New_Data,Key);
	}

	Message->MIC[0] = New_Data[0];
	Message->MIC[1] = New_Data[1];
	Message->MIC[2] = New_Data[2];
	Message->MIC[3] = New_Data[3];
}

/*
*****************************************************************************************
* Description : Function used to generate keys for the MIC calculation
*
* Arguments   : *Key pointer to key used for the MIC calculation
*				*K1 pointer to Key1
*				*K2 pointer ot Key2
*****************************************************************************************
*/
void Generate_Keys(unsigned char *Key, unsigned char *K1, unsigned char *K2)
{
	unsigned char i;
	unsigned char MSB_Key;

	//Encrypt the zeros in K1 with the NwkSkey
	AES_Encrypt(K1,Key);

	//Create K1
	//Check if MSB is 1
	if((K1[0] & 0x80) == 0x80)
	{
		MSB_Key = 1;
	}
	else
	{
		MSB_Key = 0;
	}

	//Shift K1 one bit left
	Shift_Left(K1);

	//if MSB was 1
	if(MSB_Key == 1)
	{
		K1[15] = K1[15] ^ 0x87;
	}

	//Copy K1 to K2
	for( i = 0; i < 16; i++)
	{
		K2[i] = K1[i];
	}

	//Check if MSB is 1
	if((K2[0] & 0x80) == 0x80)
	{
		MSB_Key = 1;
	}
	else
	{
		MSB_Key = 0;
	}

	//Shift K2 one bit left
	Shift_Left(K2);

	//Check if MSB was 1
	if(MSB_Key == 1)
	{
		K2[15] = K2[15] ^ 0x87;
	}
}





