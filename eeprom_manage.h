/*
  eeprom_manage.h
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
#ifndef __EEPROM_MANAGE_H__
#define __EEPROM_MANAGE_H__

class eeprom_manage
{
private:
    
    int volX[8];
    float volY[8];
    float volY_n[8];
    
private:
    unsigned char e2prom_set(int addr, unsigned char dat);
    unsigned char e2prom_get(int addr);
    unsigned char write(int addr, void *buffer, int len);
    unsigned char read(int addr, void *buffer, int len);
    unsigned char sendDtaI2c(int addrs, int len);               // send data to i2c
    unsigned char putVolX();
    unsigned char putVolY();
    unsigned char putVolY_n();
    
    unsigned char putDta(int addrs, int len, unsigned char *dta);
    unsigned char sendVolX();
    unsigned char sendVolY();
    unsigned char sendVolY_n();
    
    unsigned char checkBTMOK();
    
public:

    unsigned char setVolX(int *prt);
    unsigned char setVolY(float *ptr);
    unsigned char setVolY_n(float *ptr);
    unsigned char testI2C();
    unsigned char sendClear();
    
    unsigned char init();
    unsigned char Adjust();

    
};

extern eeprom_manage  EEPM;
#endif
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
