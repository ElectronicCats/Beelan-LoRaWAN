/******************************************************************************************
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
* Author:   Eduardo Contreras
* Company:  Beelan
****************************************************************************************/
/****************************************************************************************
* Created on:		  23-02-2020
****************************************************************************************/


#ifndef CONVERSIONS_H
#define CONVERSIONS_H

/*
*****************************************************************************************
* FUNCTION PROTOTYPES
*****************************************************************************************
*/

void Hex2ASCII(unsigned char ASCII, unsigned char *Upper_Nibble, unsigned char *Lower_Nibble);
unsigned char ASCII2Hex(unsigned char Upper_Nibble, unsigned char Lower_Nibble);

#endif

