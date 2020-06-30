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
* File:     Conversions.cpp
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

#include "Conversions.h"

void Hex2ASCII(unsigned char ASCII, unsigned char *Upper_Nibble, unsigned char *Lower_Nibble)
{
  //Load upper nibble
  *Upper_Nibble = (ASCII >> 4) & 0x0F;

  //Load lower nibbel
  *Lower_Nibble = ASCII & 0x0F;

  //Convert upper nibble to ascii
  if(*Upper_Nibble > 0x09)
  {
    *Upper_Nibble = *Upper_Nibble + 0x37;
  }
  else
  {
    *Upper_Nibble = *Upper_Nibble + 0x30;
  }

  //Convert Lower nibble
  if(*Lower_Nibble > 0x09)
  {
    *Lower_Nibble = *Lower_Nibble + 0x37;
  }
  else
  {
    *Lower_Nibble = *Lower_Nibble + 0x30;
  }
}
    
unsigned char ASCII2Hex(unsigned char Upper_Nibble, unsigned char Lower_Nibble)
{

  unsigned char ASCII;

    // High Nibble
    if(Upper_Nibble >= '0' && Upper_Nibble <= '9')
    {
        ASCII =(Upper_Nibble - '0') * 16;
    }
    
    if(Upper_Nibble >= 'A' && Upper_Nibble <= 'F')
    {
        ASCII = (Upper_Nibble - 'A' + 10) * 16;
    }

    if(Upper_Nibble >= 'a' && Upper_Nibble <= 'f')
    {
        ASCII = (Upper_Nibble - 'a' + 10) * 16;
    }
    
    // Low Nibble
    if(Lower_Nibble >= '0' && Lower_Nibble <= '9')
    {
        ASCII = ASCII + (Lower_Nibble - '0');
    }
    
    if(Lower_Nibble >= 'A' && Lower_Nibble <= 'F')
    {
        ASCII = ASCII + (Lower_Nibble - 'A' + 10);
    }

    if(Lower_Nibble >= 'a' && Lower_Nibble <= 'f')
    {
        ASCII = ASCII + (Lower_Nibble - 'a' + 10);
    } 

  return ASCII;
  
}



