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
* TYPE DEFINITIONS
*****************************************************************************************
*/

typedef enum {NO_MESSAGE,NEW_MESSAGE,CRC_OK,MIC_OK,ADDRESS_OK,MESSAGE_DONE,TIMEOUT,WRONG_MESSAGE} message_t;

#define PA_OUTPUT_RFO_PIN          0
#define PA_OUTPUT_PA_BOOST_PIN     1

/*
*****************************************************************************************
* REGISTER DEFINITIONS
*****************************************************************************************
*/

typedef enum {
    RFM_REG_FIFO            = 0x00,
    RFM_REG_OP_MODE         = 0x01,
    RFM_REG_FR_MSB          = 0x06,
    RFM_REG_FR_MID          = 0x07,
    RFM_REG_FR_LSB          = 0x08,
    RFM_REG_OCP             = 0x0b,
    RFM_REG_PA_CONFIG       = 0x09,
    RFM_REG_LNA             = 0x0C,
    RFM_REG_FIFO_ADDR_PTR   = 0x0D,
    RFM_REG_IRQ_FLAGS       = 0x12,
    RFM_REG_LAST_RSSI       = 0x1A,
    RFM_REG_MODEM_CONFIG1   = 0x1D,
    RFM_REG_MODEM_CONFIG2   = 0x1E,
    RFM_REG_SYM_TIMEOUT_LSB = 0x1F,
    RFM_REG_PREAMBLE_MSB    = 0x20,
    RFM_REG_PREAMBLE_LSB    = 0x21,
    RFM_REG_PAYLOAD_LENGTH  = 0x22,
    RFM_REG_MODEM_CONFIG3   = 0x26, 
    RFM_REG_INVERT_IQ       = 0x33,
    RFM_REG_INVERT_IQ2      = 0x3b,
    RFM_REG_SYNC_WORD       = 0x39,
    RFM_REG_DIO_MAPPING1    = 0x40,
    RFM_REG_DIO_MAPPING2    = 0x41,
    RFM_REG_PA_DAC          = 0x4d
    
    } rfm_register_t;

typedef enum {
    RFM_MODE_SLEEP      = 0b000,
    RFM_MODE_STANDBY    = 0b001,
    RFM_MODE_FSTX       = 0b010,
    RFM_MODE_TX         = 0b011,
    RFM_MODE_FSRX       = 0b100,
    RFM_MODE_RXCONT     = 0b101,
    RFM_MODE_RXSINGLE   = 0b110,
    RFM_MODE_CAD        = 0b111,
    RFM_MODE_LORA       = 0b10000000
    } frm_mode_t;


/*
*****************************************************************************************
* FUNCTION PROTOTYPES
*****************************************************************************************
*/

bool RFM_Init();
void RFM_Send_Package(sBuffer *RFM_Tx_Package, sSettings *LoRa_Settings);
message_t RFM_Single_Receive(sSettings *LoRa_Settings);
void RFM_Continuous_Receive(sSettings *LoRa_Settings);
message_t RFM_Get_Package(sBuffer *RFM_Rx_Package);
void RFM_Write(unsigned char RFM_Address, unsigned char RFM_Data);
void RFM_Switch_Mode(unsigned char Mode);
void RFM_Set_Tx_Power(int level, int outputPin);
void RFM_Set_OCP(unsigned char mA);

unsigned char RFM_Get_Rssi();


#endif

