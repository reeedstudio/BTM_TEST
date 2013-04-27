/*
  BTMGlobalDfs.h
  2013 Copyright (c) Seeed Technology Inc.  All right reserved.
 
  Author: 
  Hardware: Albert Miao 
  Firmware: Loovee
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef __BTMGLOBALDFS_H__
#define __BTMGLOBALDFS_H__

#include <Arduino.h>

// frame form i2c
#define START1          0x53
#define START2          0x19
#define END1            0x2f
#define END2            0x45

//
#define PSIGN           A6      // Positive sign
#define NSIGN           A7      // Negative sign

// value of adjust standard
#define VALVOL0         50  
#define VALVOL1         1200
#define VALVOL2         1300
#define VALVOL3         3600
#define VALVOL4         3700
#define VALVOL5         13400
#define VALVOL6         13500
#define VALVOL7         30000

//data in eeprom
#define EEPADDRIFSET    1
#define EEPADDRVOLAB    20
#define EEPADDRVOLAB_N  EEPADDRVOLAB+32
#define EEPADDRVOLX     EEPADDRVOLAB_N+32
#define EEPADDRVOLY     EEPADDRVOLX+16
#define EEPADDRVOLY_N   EEPADDRVOLY+32

#endif
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
