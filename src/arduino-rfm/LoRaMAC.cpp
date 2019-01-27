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
* File:     LoRaMAC.cpp
* Author:   Gerben den Hartog
* Compagny: Ideetron B.V.
* Website:  http://www.ideetron.nl/LoRa
* E-mail:   info@ideetron.nl
****************************************************************************************/
/****************************************************************************************
* Created on:         06-01-2017
* Supported Hardware: ID150119-02 Nexus board with RFM95
****************************************************************************************/
/*
*****************************************************************************************
* INCLUDE FILES
*****************************************************************************************
*/

#include <SPI.h>
#include "AES-128.h"
#include "RFM95.h"
#include "Encrypt.h"
#include "LoRaMAC.h"
#include "Struct.h"
#include "Commands.h"
#include "loraid-arduino-rfm.h"

/*
*****************************************************************************************
* FUNCTIONS
*****************************************************************************************
*/


/*
*****************************************************************************************
* Description : Function that handles a send and receive cycle with timing for receive slots.
*				This function is only used for Class A motes. The wait times are tested with
*				the iot.semtech.com site.
*
* Arguments   : *Data_Tx pointer to tranmit buffer
*				*Data_Rx pointer to receive buffer
*				*RFM_Command pointer to current RFM state
*				*Session_Data pointer to sLoRa_Session sturct
*				*OTAA_Data pointer to sLoRa_OTAA struct
*				*Message_Rx pointer to sLoRa_Message struct used for the received message information
*				*LoRa_Settings pointer to sSetting struct
*****************************************************************************************
*/
void LORA_Cycle(sBuffer *Data_Tx, sBuffer *Data_Rx, RFM_command_t *RFM_Command, sLoRa_Session *Session_Data, sLoRa_OTAA *OTAA_Data, sLoRa_Message *Message_Rx, sSettings *LoRa_Settings)
{
	unsigned char i;
	unsigned int Receive_Delay_1 = 950;
	unsigned int Receive_Delay_2 = 1950;
	unsigned int Receive_Delay_JoinAck = 5950;
	unsigned long prevTime = 0;

	unsigned char Channel_Rx_1 = LoRa_Settings->Channel_Tx;
	unsigned char Datarate_Rx_1 = LoRa_Settings->Datarate_Tx;
	unsigned char Channel_Rx_2 = LoRa_Settings->Channel_Rx;
	unsigned char Datarate_Rx_2 = LoRa_Settings->Datarate_Rx;

	if(*RFM_Command == JOIN)
  	{
  	  //Send join Reqest message
  	  LoRa_Send_JoinReq(OTAA_Data, LoRa_Settings);

      delay(Receive_Delay_JoinAck);
  	}

	//Send normal data message
  	if(*RFM_Command == NEW_RFM_COMMAND)
  	{
  	  LORA_Send_Data(Data_Tx, Session_Data, LoRa_Settings);
	  
	  // Start timer
	  prevTime = millis();

	  // Class C open RX2 immediately after sending data
      if(LoRa_Settings->Mote_Class == 0x01)
	  {
	    // RX2 window
        LoRa_Settings->Channel_Rx = Channel_Rx_2;    // set Rx2 channel
        LoRa_Settings->Datarate_Rx = Datarate_Rx_2;   //set data rate Rx2
	  	LORA_Receive_Data(Data_Rx, Session_Data, OTAA_Data, Message_Rx, LoRa_Settings);
		if(Data_Rx->Counter > 0) {
			Serial.print((char *)Data_Rx->Data);
		} else {
			Serial.println("No Data RX2 Class C");
		}
	  }

	  // Wait for RX1 delay
	  while(millis() - prevTime < Receive_Delay_1);

	  // Add RX1 window
      LoRa_Settings->Channel_Rx = Channel_Rx_1;    // set Rx1 channel, same with Tx
      LoRa_Settings->Datarate_Rx = Datarate_Rx_1;   //set data rate Rx1, same with Tx

	  LORA_Receive_Data(Data_Rx, Session_Data, OTAA_Data, Message_Rx, LoRa_Settings);
	  if(Data_Rx->Counter > 0) {
		Serial.print((char *)Data_Rx->Data);
	  } else {
		Serial.println("No Data RX1");
	  }
      // Wait for RX2 delay
      while(millis() - prevTime < Receive_Delay_2);

	  // RX2 window
      LoRa_Settings->Channel_Rx = Channel_Rx_2;    // set Rx2 channel
      LoRa_Settings->Datarate_Rx = Datarate_Rx_2;   //set data rate Rx2  	  
    }

    LORA_Receive_Data(Data_Rx, Session_Data, OTAA_Data, Message_Rx, LoRa_Settings);
	if(Data_Rx->Counter > 0) {
		Serial.print((char *)Data_Rx->Data);
	} else {
		Serial.println("No Data RX2");
	}
}

/*
*****************************************************************************************
* Description : Function that is used to build a LoRaWAN data message and then tranmit it.
*
* Arguments   : *Data_Tx pointer to tranmit buffer
*				*Session_Data pointer to sLoRa_Session sturct
*				*LoRa_Settings pointer to sSetting struct
*****************************************************************************************
*/
void LORA_Send_Data(sBuffer *Data_Tx, sLoRa_Session *Session_Data, sSettings *LoRa_Settings)
{
  //Define variables
  unsigned char i;

  //Initialise RFM buffer
  unsigned char RFM_Data[64];
  sBuffer RFM_Package = {&RFM_Data[0], 0x00};

  //Initialise Message struct for a transmit message
  sLoRa_Message Message;

  Message.MAC_Header = 0x00;
  Message.Frame_Port = 0x01; //Frame port always 1 for now
  Message.Frame_Control = 0x00;

  //Load device addres from session data into the message
  Message.DevAddr[0] = Session_Data->DevAddr[0];
  Message.DevAddr[1] = Session_Data->DevAddr[1];
  Message.DevAddr[2] = Session_Data->DevAddr[2];
  Message.DevAddr[3] = Session_Data->DevAddr[3];

  //Set up direction
  Message.Direction = 0x00;

  //Load the frame counter from the session data into the message
  Message.Frame_Counter = *Session_Data->Frame_Counter;

  //Set confirmation
  //Unconfirmed
  if(LoRa_Settings->Confirm == 0x00)
  {
      Message.MAC_Header = Message.MAC_Header | 0x40;
  }
  //Confirmed
  else
  {
      Message.MAC_Header = Message.MAC_Header | 0x80;
  }

  //Build the Radio Package
  //Load mac header
  RFM_Data[0] = Message.MAC_Header;

  //Load device address
  RFM_Data[1] = Message.DevAddr[3];
  RFM_Data[2] = Message.DevAddr[2];
  RFM_Data[3] = Message.DevAddr[1];
  RFM_Data[4] = Message.DevAddr[0];

  //Load frame control
  RFM_Data[5] = Message.Frame_Control;

  //Load frame counter
  RFM_Data[6] = (*Session_Data->Frame_Counter & 0x00FF);
  RFM_Data[7] = ((*Session_Data->Frame_Counter >> 8) & 0x00FF);

  //Set data counter to 8
  RFM_Package.Counter = 8;

  //If there is data load the Frame_Port field
  //Encrypt the data and load the data
  if(Data_Tx->Counter > 0x00)
  {
	//Load Frame port field
    RFM_Data[8] = Message.Frame_Port;

    //Raise package counter
    RFM_Package.Counter++;

    //Encrypt the data
    Encrypt_Payload(Data_Tx, Session_Data->AppSKey, &Message);

    //Load Data
    for(i = 0; i < Data_Tx->Counter; i++)
    {
      RFM_Data[RFM_Package.Counter + i] = Data_Tx->Data[i];
    }

    //Add data Lenth to package counter
    RFM_Package.Counter = RFM_Package.Counter + Data_Tx->Counter;
  }

  //Calculate MIC
  Construct_Data_MIC(&RFM_Package, Session_Data, &Message);

  //Load MIC in package
  for(i = 0; i < 4; i++)
  {
    RFM_Data[i + RFM_Package.Counter] = Message.MIC[i];
  }

  //Add MIC length to RFM package length
  RFM_Package.Counter = RFM_Package.Counter + 4;

  //Send Package
  RFM_Send_Package(&RFM_Package, LoRa_Settings);

  //Raise Frame counter
  if(*Session_Data->Frame_Counter != 0xFFFF)
  {
    //Raise frame counter
    *Session_Data->Frame_Counter = *Session_Data->Frame_Counter + 1;
  }
  else
  {
    *Session_Data->Frame_Counter = 0x0000;
  }

  //Change channel for next message if hopping is activated
  if(LoRa_Settings->Channel_Hopping == 0x01)
  {
    if(LoRa_Settings->Channel_Tx < 0x07)
    {
      LoRa_Settings->Channel_Tx++;
    }
    else
    {
      LoRa_Settings->Channel_Tx = 0x00;
    }
  }
}


/*
*****************************************************************************************
* Description : Function that is used to receive a LoRaWAN message and retrieve the data from the RFM
*               Also checks on CRC, MIC and Device Address
*               This function is used for Class A and C motes.
*
* Arguments   : *Data_Rx pointer to receive buffer
*				*Session_Data pointer to sLoRa_Session sturct
*				*OTAA_Data pointer to sLoRa_OTAA struct
*				*Message_Rx pointer to sLoRa_Message struct used for the received message information
*				*LoRa_Settings pointer to sSetting struct
*****************************************************************************************
*/
void LORA_Receive_Data(sBuffer *Data_Rx, sLoRa_Session *Session_Data, sLoRa_OTAA *OTAA_Data, sLoRa_Message *Message, sSettings *LoRa_Settings)
{
	unsigned char i;

    //Initialise RFM buffer
	unsigned char RFM_Data[64];
	sBuffer RFM_Package = {&RFM_Data[0], 0x00};

	unsigned char MIC_Check;
  unsigned char Address_Check;

	unsigned char Frame_Options_Length;

	unsigned char Data_Location;

	message_t Message_Status = NO_MESSAGE;

	//If it is a type A device switch RFM to single receive
	if(LoRa_Settings->Mote_Class == 0x00)
	{
		Message_Status = RFM_Single_Receive(LoRa_Settings);  
	}
	else
	{
		//Switch RFM to standby
		RFM_Switch_Mode(0x01);

		Message_Status = NEW_MESSAGE;
	}

	//If there is a message received get the data from the RFM
	if(Message_Status == NEW_MESSAGE)
	{
		Message_Status = RFM_Get_Package(&RFM_Package);

		//If mote class C switch RFM back to continuous receive
		if(LoRa_Settings->Mote_Class == 0x01)
		{
			//Switch RFM to Continuous Receive
			RFM_Continuous_Receive(LoRa_Settings);
		}
	}

	//if CRC ok breakdown package
	if(Message_Status == CRC_OK)
	{
		//Get MAC_Header
    	Message->MAC_Header = RFM_Data[0];

		//Join Accept message
		if(Message->MAC_Header == 0x20)
		{
			//Copy the data into the data array
			for(i = 0x00; i < RFM_Package.Counter; i++)
			{
				Data_Rx->Data[i] = RFM_Package.Data[i];
			}

			//Set data counter
			Data_Rx->Counter = RFM_Package.Counter;

			//Decrypt the data
			for(i = 0x00; i < ((Data_Rx->Counter - 1) / 16); i++)
			{
				AES_Encrypt(&(Data_Rx->Data[(i*16)+1]),OTAA_Data->AppKey);
			}

			//Calculate MIC
			//Remove MIC from number of bytes
			Data_Rx->Counter -= 4;

			//Get MIC
			Calculate_MIC(Data_Rx, OTAA_Data->AppKey, Message);

			//Clear MIC check counter
			MIC_Check = 0x00;

			//Compare MIC
			for(i = 0x00; i < 4; i++)
			{
				if(Data_Rx->Data[Data_Rx->Counter + i] == Message->MIC[i])
				{
					MIC_Check++;
				}
			}

			//Check if MIC compares
			if(MIC_Check == 0x04)
			{
				Message_Status = MIC_OK;

				Serial.write("MIC OK");
				UART_Send_Newline();
			}
			else
			{
				Message_Status = WRONG_MESSAGE;

				//Send NOK
				Serial.write("MIC NOK");
				UART_Send_Newline();
			}

			//Get Key's and data from package when MIC is OK
			if(Message_Status == MIC_OK)
			{
				//Get AppNonce
				OTAA_Data->AppNonce[0] = Data_Rx->Data[1];
				OTAA_Data->AppNonce[1] = Data_Rx->Data[2];
				OTAA_Data->AppNonce[2] = Data_Rx->Data[3];

				//Get Net ID
				OTAA_Data->NetID[0] = Data_Rx->Data[4];
				OTAA_Data->NetID[1] = Data_Rx->Data[5];
				OTAA_Data->NetID[2] = Data_Rx->Data[6];

				//Get session Device address
				Session_Data->DevAddr[3] = Data_Rx->Data[7];
				Session_Data->DevAddr[2] = Data_Rx->Data[8];
				Session_Data->DevAddr[1] = Data_Rx->Data[9];
				Session_Data->DevAddr[0] = Data_Rx->Data[10];

				//Calculate Network Session Key
				Session_Data->NwkSKey[0] = 0x01;

				//Load AppNonce
				Session_Data->NwkSKey[1] = OTAA_Data->AppNonce[0];
				Session_Data->NwkSKey[2] = OTAA_Data->AppNonce[1];
				Session_Data->NwkSKey[3] = OTAA_Data->AppNonce[2];

				//Load NetID
				Session_Data->NwkSKey[4] = OTAA_Data->NetID[0];
				Session_Data->NwkSKey[5] = OTAA_Data->NetID[1];
				Session_Data->NwkSKey[6] = OTAA_Data->NetID[2];

				//Load Dev Nonce
				Session_Data->NwkSKey[7] = OTAA_Data->DevNonce[0];
				Session_Data->NwkSKey[8] = OTAA_Data->DevNonce[1];

				//Pad with zeros
				Session_Data->NwkSKey[9] = 0x00;
				Session_Data->NwkSKey[10] = 0x00;
				Session_Data->NwkSKey[11] = 0x00;
				Session_Data->NwkSKey[12] = 0x00;
				Session_Data->NwkSKey[13] = 0x00;
				Session_Data->NwkSKey[14] = 0x00;
				Session_Data->NwkSKey[15] = 0x00;

				//Copy to AppSkey
				for(i = 0x00; i < 16; i++)
				{
					Session_Data->AppSKey[i] = Session_Data->NwkSKey[i];
				}

				//Change first byte of AppSKey
				Session_Data->AppSKey[0] = 0x02;

				//Calculate the keys
				AES_Encrypt(Session_Data->NwkSKey,OTAA_Data->AppKey);
				AES_Encrypt(Session_Data->AppSKey,OTAA_Data->AppKey);

				//Reset Frame counter
				*Session_Data->Frame_Counter = 0x0000;

				//Send dat over USB
				Serial.write("DevAddr: ");
				UART_Send_Data(Session_Data->DevAddr,0x04);
				UART_Send_Newline();

				Serial.write("NwkSKey: ");
				UART_Send_Data(Session_Data->NwkSKey,0x10);
				UART_Send_Newline();

				Serial.write("AppSKey: ");
				UART_Send_Data(Session_Data->AppSKey,0x10);
				UART_Send_Newline();
				UART_Send_Newline();

				//Clear Data counter
				Data_Rx->Counter = 0x00;
			}
		}

		//Data message
		if(Message->MAC_Header == 0x40 || Message->MAC_Header == 0x60 || Message->MAC_Header == 0x80 || Message->MAC_Header == 0xA0)
		{
			//Get device address from received data
			Message->DevAddr[0] = RFM_Data[4];
			Message->DevAddr[1] = RFM_Data[3];
			Message->DevAddr[2] = RFM_Data[2];
			Message->DevAddr[3] = RFM_Data[1];

			//Get frame control field
			Message->Frame_Control = RFM_Data[5];

			 //Get frame counter
			Message->Frame_Counter = RFM_Data[7];
			Message->Frame_Counter = (Message->Frame_Counter << 8) + RFM_Data[6];

			//Lower Package length with 4 to remove MIC length
			RFM_Package.Counter -= 4;

			//Calculate MIC
			Construct_Data_MIC(&RFM_Package, Session_Data, Message);

			MIC_Check = 0x00;

            //Compare MIC
			for(i = 0x00; i < 4; i++)
			{
				if(RFM_Data[RFM_Package.Counter + i] == Message->MIC[i])
				{
					MIC_Check++;
				}
			}

			//Check MIC
      		if(MIC_Check == 0x04)
      		{
      		  Message_Status = MIC_OK;

      		  Serial.write("MIC OK");
      		}
      		else
      		{
      		  Message_Status = WRONG_MESSAGE;

      		  //Send NOK
      		  Serial.write("MIC NOK");

      		}

      		UART_Send_Newline();

      		Address_Check = 0;

      		//Check address
      		if(MIC_Check == 0x04)
      		{
			      for(i = 0x00; i < 4; i++)
			      {
			        if(Session_Data->DevAddr[i] == Message->DevAddr[i])
			        {
				        Address_Check++;
			        }
			      }
      		}

		  	if(Address_Check == 0x04)
		  	{
				Message_Status = ADDRESS_OK;

				Serial.write("ADDRESS OK");
		  	}
		  	else
		  	{
				Message_Status = WRONG_MESSAGE;

				//Send NOK
				Serial.write("ADDRESS NOK");

		  	}

		  	UART_Send_Newline();

			//if the address is OK then decrypt the data
			//Send the data to USB
			if(Message_Status == ADDRESS_OK)
			{
				//Send Mac Header
				Serial.write("Mac Header: ");
				UART_Send_Data(&Message->MAC_Header, 0x01);
				UART_Send_Newline();

				//Send Dev addr
				Serial.write("Dev addr: ");
				UART_Send_Data(Message->DevAddr,0x04);
				UART_Send_Newline();

				//Send Frame control field
				Serial.write("Frame Control: ");
				UART_Send_Data(&Message->Frame_Control,0x01);
				UART_Send_Newline();

				//Send frame counter
				Serial.write("Frame Counter: ");
				UART_Send_Data(&RFM_Data[7],0x01);
				UART_Send_Data(&RFM_Data[6],0x01);
				UART_Send_Newline();

				Data_Location = 8;

				//Get length of frame options field
				Frame_Options_Length = (Message->Frame_Control & 0x0F);

				//Add length of frame options field to data location
				Data_Location = Data_Location + Frame_Options_Length;

				//Check if ther is data in the package
				if(RFM_Package.Counter == Data_Location)
				{
					Data_Rx->Counter = 0x00;
				}
				else
				{
					//Get port field when ther is data
					Message->Frame_Port = RFM_Data[8];

					//Send Port field
					Serial.write("Frame Port: ");
					UART_Send_Data(&(Message->Frame_Port),0x01);
					UART_Send_Newline();

					//Calculate the amount of data in the package
					Data_Rx->Counter = (RFM_Package.Counter - Data_Location -1);

					//Correct the data location by 1 for the Fport field
					Data_Location = (Data_Location + 1);
				}

				//Copy and decrypt the data
				if(Data_Rx->Counter != 0x00)
				{
					for(i = 0; i < Data_Rx->Counter; i++)
					{
						Data_Rx->Data[i] = RFM_Data[Data_Location + i];
					}

         //Check frame port fiels. When zero it is a mac command message encrypted with NwkSKey
         if(Message->Frame_Port == 0x00)
         {
          Encrypt_Payload(Data_Rx, Session_Data->NwkSKey, Message);
         }
         else
         {
          Encrypt_Payload(Data_Rx, Session_Data->AppSKey, Message);
         }

					Message_Status = MESSAGE_DONE;
				}
			}
		}

		if(Message_Status == WRONG_MESSAGE)
		{
			Data_Rx->Counter = 0x00;
 		}
	}
}

/*
*****************************************************************************************
* Description : Function that is used to send a join request to a network.
*
* Arguments   : *OTAA_Data pointer to sLoRa_OTAA struct
*				*LoRa_Settings pointer to sSetting struct
*****************************************************************************************
*/
void LoRa_Send_JoinReq(sLoRa_OTAA *OTAA_Data, sSettings *LoRa_Settings)
{
    unsigned char i;

	//Initialise RFM data buffer
    unsigned char RFM_Data[23];
    sBuffer RFM_Package = { &RFM_Data[0], 0x00};

 	//Initialise message sturct
    sLoRa_Message Message;

    Message.MAC_Header = 0x00; //Join request
    Message.Direction = 0x00; //Set up Direction

    //Construct OTAA Request message
    //Load Header in package
    RFM_Data[0] = Message.MAC_Header;

    //Load AppEUI in package
    for(i = 0x00; i < 8; i++)
    {
        RFM_Data[i+1] = OTAA_Data->AppEUI[7-i];
    }

    //Load DevEUI in package
    for(i= 0x00; i < 8; i++)
    {
        RFM_Data[i+9] = OTAA_Data->DevEUI[7-i];
    }

    //Generate DevNonce
    Generate_DevNonce(OTAA_Data->DevNonce);

    //Load DevNonce in package
    RFM_Data[17] = OTAA_Data->DevNonce[0];
    RFM_Data[18] = OTAA_Data->DevNonce[1];

    //Set length of package
    RFM_Package.Counter = 19;

    //Get MIC
    Calculate_MIC(&RFM_Package, OTAA_Data->AppKey, &Message);

    //Load MIC in package
    RFM_Data[19] = Message.MIC[0];
    RFM_Data[20] = Message.MIC[1];
    RFM_Data[21] = Message.MIC[2];
    RFM_Data[22] = Message.MIC[3];

    //Set lenght of package to the right length
    RFM_Package.Counter = 23;

    //Send Package
    RFM_Send_Package(&RFM_Package, LoRa_Settings);
}

/*
*****************************************************************************************
* Description : Function that is used to generate device nonce used in the join reqeust function
*				This is based on a psuedo random function in the arduino library
*
* Arguments   : *Devnonce pointer to the devnonce arry of withc is unsigned char[2]
*****************************************************************************************
*/
void Generate_DevNonce(unsigned char *DevNonce)
{
  unsigned int RandNumber;

  RandNumber = random(0xFFFF);

  DevNonce[0] = RandNumber & 0x00FF;
  DevNonce[1] = (RandNumber >> 8) & 0x00FF;
}


