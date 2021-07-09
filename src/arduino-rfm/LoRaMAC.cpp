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
#include "Config.h"
#include "Arduino.h"

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
void LORA_Cycle(sBuffer *Data_Tx, sBuffer *Data_Rx, RFM_command_t *RFM_Command, sLoRa_Session *Session_Data,
 									sLoRa_OTAA *OTAA_Data, sLoRa_Message *Message_Rx, sSettings *LoRa_Settings)
{
	static const unsigned int Receive_Delay_1 = 1000;
	static const unsigned int Receive_Delay_2 = 1000;
	unsigned long prevTime = 0;
	unsigned char rx1_ch = LoRa_Settings->Channel_Rx;
	#ifdef US_915   
    unsigned char rx1_dr = LoRa_Settings->Datarate_Tx+10;
	#elif defined(EU_868)   
    unsigned char rx1_dr = LoRa_Settings->Datarate_Tx;
	#endif

  	//Transmit
	if(*RFM_Command == NEW_RFM_COMMAND){
		#if (SAMR34)
		pinMode(RFM_SWITCH,OUTPUT);
		digitalWrite(RFM_SWITCH,0); //Rf switch inside RAK module change to Tx
		#endif	
		//Lora send data
    	LORA_Send_Data(Data_Tx, Session_Data, LoRa_Settings);
		prevTime = millis();
		
		#if (SAMR34)
		digitalWrite(RFM_SWITCH,1); //Rf switch inside RAK module change to Rx 
		#endif

		// Class C open RX2 immediately after sending data
		if(LoRa_Settings->Mote_Class == CLASS_C){
			#ifdef US_915
			LoRa_Settings->Channel_Rx = 0x08;    // set Rx2 channel 923.3 MHZ
			LoRa_Settings->Datarate_Rx = SF12BW500;   //set RX2 datarate 12
			#elif defined(EU_868)
			LoRa_Settings->Channel_Rx = CHRX2;    // set Rx2 channel 923.3 MHZ 
			LoRa_Settings->Datarate_Rx = SF12BW125;   //set RX2 datarate 12
			#endif
			LORA_Receive_Data(Data_Rx, Session_Data, OTAA_Data, Message_Rx, LoRa_Settings);  //BUG DETECT SENDED PACKET ALWAYS (IT DOES UPDATE)
		}
		//Wait rx1 window delay 
		//Receive on RX2 if countinous mode is available
		//check if anything if coming on class C RX2 window in class A no DIO0 flag will be activated
		do{
			if(digitalRead(RFM_pins.DIO0))		//Poll Rx done for getting message
				LORA_Receive_Data(Data_Rx, Session_Data, OTAA_Data, Message_Rx, LoRa_Settings);
		}while(millis() - prevTime < Receive_Delay_1);
		//Return if message on RX2 
		if (Data_Rx->Counter>0)return;
		
		//Update time for counting 1 sec more
		prevTime = millis(); 
		//Return to datarate and channel for RX1
		LoRa_Settings->Channel_Rx = rx1_ch;    // set RX1 channel 
		LoRa_Settings->Datarate_Rx = rx1_dr;   //set RX1 datarate
		//Receive Data RX1
		LORA_Receive_Data(Data_Rx, Session_Data, OTAA_Data, Message_Rx, LoRa_Settings);
		//Wait rx2 window delay 
		do{
			//Poll Rx done for getting message
			//DIO0 flag will only be active while class C
			if(digitalRead(RFM_pins.DIO0))
				LORA_Receive_Data(Data_Rx, Session_Data, OTAA_Data, Message_Rx, LoRa_Settings); 
		}while(millis() - prevTime < Receive_Delay_2);
		//Return if message on RX1
		if (Data_Rx->Counter>0)return;

		//Configure datarate and channel for RX2
		LoRa_Settings->Channel_Rx = 0x08;    // set RX2 channel 
		LoRa_Settings->Datarate_Rx = 0x08;   //set RX2 datarate
		//Receive Data RX2 
		//If class A timeout will apply
		//If class C continous Rx will happen
		LORA_Receive_Data(Data_Rx, Session_Data, OTAA_Data, Message_Rx, LoRa_Settings);
		*RFM_Command = NO_RFM_COMMAND;
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
	unsigned char RFM_Data[MAX_UPLINK_PAYLOAD_SIZE+65];
	sBuffer RFM_Package = {&RFM_Data[0], 0x00};

	//Initialise Message struct for a transmit message
	sLoRa_Message Message;

	Message.MAC_Header = 0x00;
	Message.Frame_Port = 0x01; //Frame port always 1 for now
	Message.Frame_Control = 0x00;

	//Load device address from session data into the message
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
	RFM_Package.Data[0] = Message.MAC_Header;

	//Load device address
	RFM_Package.Data[1] = Message.DevAddr[3];
	RFM_Package.Data[2] = Message.DevAddr[2];
	RFM_Package.Data[3] = Message.DevAddr[1];
	RFM_Package.Data[4] = Message.DevAddr[0];

	//Load frame control
	RFM_Package.Data[5] = Message.Frame_Control;

	//Load frame counter
	RFM_Package.Data[6] = (*Session_Data->Frame_Counter & 0x00FF);
	RFM_Package.Data[7] = ((*Session_Data->Frame_Counter >> 8) & 0x00FF);

	//Set data counter to 8
	RFM_Package.Counter = 8;

	//If there is data load the Frame_Port field
	//Encrypt the data and load the data
	if(Data_Tx->Counter > 0x00)
	{
	//Load Frame port field
	//RFM_Data[8] = Message.Frame_Port;
	RFM_Package.Data[8] = LoRa_Settings->Mport;

	//Raise package counter
	RFM_Package.Counter++;

	//Encrypt the data
	Encrypt_Payload(Data_Tx, Session_Data->AppSKey, &Message);

	//Load Data
	for(i = 0; i < Data_Tx->Counter; i++)
	{
		RFM_Package.Data[RFM_Package.Counter++] = Data_Tx->Data[i];
	}


	}

	//Calculate MIC
	Construct_Data_MIC(&RFM_Package, Session_Data, &Message);

	//Load MIC in package
	for(i = 0; i < 4; i++)
	{
	RFM_Package.Data[RFM_Package.Counter++] = Message.MIC[i];
	}

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
	unsigned char RFM_Data[MAX_DOWNLINK_PAYLOAD_SIZE+65];
	sBuffer RFM_Package = {&RFM_Data[0], 0x00};

	unsigned char MIC_Check;
  	unsigned char Address_Check;

	unsigned char Frame_Options_Length;

	unsigned char Data_Location;

	message_t Message_Status = NO_MESSAGE;

	//If it is a type A device switch RFM to single receive
	if(LoRa_Settings->Mote_Class == CLASS_A)
	{
		Message_Status = RFM_Single_Receive(LoRa_Settings);  
	}
	else
	{
		//Switch RFM to standby
		RFM_Switch_Mode(RFM_MODE_STANDBY);
		Message_Status = NEW_MESSAGE;
	}

	//If there is a message received get the data from the RFM
	if(Message_Status == NEW_MESSAGE)
	{
		Message_Status = RFM_Get_Package(&RFM_Package);

		//If mote class C switch RFM back to continuous receive
		if(LoRa_Settings->Mote_Class == CLASS_C)
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
      		}
      		else
      		{
      			Message_Status = WRONG_MESSAGE;
      		}

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
		  	}
		  	else
		  	{
				Message_Status = WRONG_MESSAGE;
		  	}

			//if the address is OK then decrypt the data
			//Send the data to USB
			if(Message_Status == ADDRESS_OK)
			{

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
* Description : Function that is used to generate device nonce used in the join request function
*				This is based on a pseudo random function in the arduino library
*
* Arguments   : *Devnonce pointer to the devnonce arry of withc is unsigned char[2]
*****************************************************************************************
*/
static void Generate_DevNonce(unsigned char *DevNonce)
{
	unsigned int RandNumber;

	RandNumber = random(0xFFFF);

	DevNonce[0] = RandNumber & 0x00FF;
	DevNonce[1] = (RandNumber >> 8) & 0x00FF;
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

	//Set length of package to the right length
	RFM_Package.Counter = 23;

	//Send Package
	RFM_Send_Package(&RFM_Package, LoRa_Settings);
}
bool LORA_join_Accept(sBuffer *Data_Rx,sLoRa_Session *Session_Data, sLoRa_OTAA *OTAA_Data, sLoRa_Message *Message, sSettings *LoRa_Settings)
{
	bool joinStatus = false;
	unsigned char i;
    //Initialise RFM buffer
	unsigned char RFM_Data[64];
	sBuffer RFM_Package = {&RFM_Data[0], 0x00};
	unsigned char MIC_Check;

	message_t Message_Status = NO_MESSAGE;

	//RFM to single receive
	Message_Status = RFM_Single_Receive(LoRa_Settings);  
	//If there is a message received get the data from the RFM
	if(Message_Status == NEW_MESSAGE)
		Message_Status = RFM_Get_Package(&RFM_Package);
	
	#if defined(ESP8266) || defined(ESP32) 
	yield();
	#endif

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
				Data_Rx->Data[i] = RFM_Package.Data[i];

			//Set data counter
			Data_Rx->Counter = RFM_Package.Counter;

			//Decrypt the data
			for(i = 0x00; i < ((Data_Rx->Counter - 1) / 16); i++)
				AES_Encrypt(&(Data_Rx->Data[(i*16)+1]),OTAA_Data->AppKey);

			//Calculate MIC
			//Remove MIC from number of bytes
			Data_Rx->Counter -= 4;

			//Get MIC
			Calculate_MIC(Data_Rx, OTAA_Data->AppKey, Message);

			//Clear MIC check counter
			MIC_Check = 0x00;

			//Compare MIC
			for(i = 0x00; i < 4; i++)
				if(Data_Rx->Data[Data_Rx->Counter + i] == Message->MIC[i])
					MIC_Check++;

			//Check if MIC compares
			if(MIC_Check == 0x04)
				Message_Status = MIC_OK;
			else
				Message_Status = WRONG_MESSAGE;

			//Get Key's and data from package when MIC is OK
			if(Message_Status == MIC_OK)
			{
				//Get AppNonce
				for(i = 0; i< 3; i++)
					OTAA_Data->AppNonce[i] = Data_Rx->Data[i+1];

				//Get Net ID
				for(i = 0; i< 3; i++)
					OTAA_Data->NetID[i] = Data_Rx->Data[i + 4];

				//Get session Device address
				for(i = 0; i< 4; i++)
					Session_Data->DevAddr[3-i] = Data_Rx->Data[i + 7];

				//Calculate Network Session Key
				Session_Data->NwkSKey[0] = 0x01;

				//Load AppNonce
				for(i = 0; i < 3; i++)
					Session_Data->NwkSKey[i+1] = OTAA_Data->AppNonce[i];

				//Load NetID
				for(i = 0; i < 3; i++)
					Session_Data->NwkSKey[i+4] = OTAA_Data->NetID[i];

				//Load Dev Nonce
				Session_Data->NwkSKey[7] = OTAA_Data->DevNonce[0];
				Session_Data->NwkSKey[8] = OTAA_Data->DevNonce[1];

				//Pad with zeros
				for(i = 9; i <= 15; i++)
					Session_Data->NwkSKey[i] = 0x00;

				//Copy to AppSkey
				for(i = 0x00; i < 16; i++)
					Session_Data->AppSKey[i] = Session_Data->NwkSKey[i];

				//Change first byte of AppSKey
				Session_Data->AppSKey[0] = 0x02;

				//Calculate the keys
				AES_Encrypt(Session_Data->NwkSKey,OTAA_Data->AppKey);
				AES_Encrypt(Session_Data->AppSKey,OTAA_Data->AppKey);

				//Reset Frame counter
				*Session_Data->Frame_Counter = 0x0000;

				//Clear Data counter
				Data_Rx->Counter = 0x00;

#ifdef DEBUG
				Serial.print(F("NwkSKey: "));
				for(byte i = 0; i < 16 ;++i)
					Serial.print(Session_Data->NwkSKey[i],HEX);
				Serial.print(F("\nAppSKey: "));
				for(byte i = 0; i < 16 ;++i)
					Serial.print(Session_Data->AppSKey[i],HEX);
				Serial.println();
#endif	
				joinStatus = true;
			}
		}
	}
	return joinStatus;
}



