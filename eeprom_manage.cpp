/*
  eeprom_manage.cpp
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
#include <EEPROM.h>
#include <Wire.h>

#include "eeprom_manage.h"
#include "BTMGlobalDfs.h"


/*********************************************************************************************************
** Function name: init
** Descriptions:  init 
*********************************************************************************************************/
unsigned char eeprom_manage::init()
{

    
    for(int i=0; i<512; i++)
    {
        EEPROM.write(i, 0xff);
    }
    
    EEPROM.write(EEPADDRIFSET, 0x55);
    
    Wire.begin();               // init i2c
    
    volX[0] = VALVOL0;
    volX[1] = VALVOL1;
    volX[2] = VALVOL2;
    volX[3] = VALVOL3;
    volX[4] = VALVOL4;
    volX[5] = VALVOL5;
    volX[6] = VALVOL6;
    volX[7] = VALVOL7;
    
    return 0;

}

/*********************************************************************************************************
** Function name: e2prom_set
** Descriptions:  write a byte to eeprom
*********************************************************************************************************/
unsigned char eeprom_manage::e2prom_set(int addr,unsigned char dat)
{
    if(addr>=512 || addr<0)
        return 0;
    EEPROM.write(addr, dat);
    return 1;
}

/*********************************************************************************************************
** Function name: e2prom_get
** Descriptions:  get a byte from eeprom
*********************************************************************************************************/
unsigned char eeprom_manage::e2prom_get(int addr)
{
    char dat;
    if(addr>=512 || addr<0)
        return 0;
    return EEPROM.read(addr);
}

/*********************************************************************************************************
** Function name: write
** Descriptions:  write a float(int ...) to eeprom
*********************************************************************************************************/
unsigned char eeprom_manage::write(int addr,void *buffer,int len)
{
    int sum = 0;
    unsigned char *ch = (unsigned char*)buffer;
    for(int i=0; i<len; i++)
    {
        sum += e2prom_set(addr+i,ch[i]);
    }
    return sum;
}

/*********************************************************************************************************
** Function name: read
** Descriptions:  read a float(int...) from eeprom
*********************************************************************************************************/
unsigned char eeprom_manage::read(int addr,void *buffer,int len)
{

    unsigned char *ch;
    if(addr>=512 || addr<0)
        return 0;
    if(addr+len>=512)
        len = 512-addr;
    ch = (unsigned char*)buffer;
    for(int i=0;i<len;i++)
        ch[i] = e2prom_get(addr+i);
    return len;
    
}

/*********************************************************************************************************
** Function name: putDta
** Descriptions:  putDta
*********************************************************************************************************/
unsigned char eeprom_manage::putDta(int addrs, int len, unsigned char *dta)
{
    if(len <= 0)return 0;
    
    for(int i = 0; i<=len; i++)
    {
        EEPROM.write(addrs+i, dta[i]);
    }
    return 1;
}

/*********************************************************************************************************
** Function name: putVolX
** Descriptions:  putVolX
*********************************************************************************************************/
unsigned char eeprom_manage::putVolX()
{
    int addr = EEPADDRVOLX;
    for(int i = 0; i<8; i++)
    {
        addr += write(addr, &volX[i], 2);
    }
}

/*********************************************************************************************************
** Function name: putVolY
** Descriptions:  putVolY
*********************************************************************************************************/
unsigned char eeprom_manage::putVolY()
{
    int addr = EEPADDRVOLY;
    for(int i = 0; i<8; i++)
    {
        addr += write(addr, &volY[i], 4);
    }
}

/*********************************************************************************************************
** Function name: putVolY_n
** Descriptions:  putVolY_n
*********************************************************************************************************/
unsigned char eeprom_manage::putVolY_n()
{
    int addr = EEPADDRVOLY_N;
    for(int i = 0; i<8; i++)
    {
        addr += write(addr, &volY_n[i], 4);
    }
    return addr;
}

/*********************************************************************************************************
** Function name: sendDtaI2c
** Descriptions:  send eeprom data to i2c, 
*********************************************************************************************************/
unsigned char eeprom_manage::sendDtaI2c(int addrs, int len)
{

    unsigned char tSend[32];
    for(int i = 0; i<len; i++)
    {
        tSend[i] = EEPROM.read(addrs+i);            // read data from eeprom
    }
    
    Wire.beginTransmission(5);                      // begin i2c
    Wire.write(START1);                             // send start
    Wire.write(START2);
    Wire.write(addrs);
    Wire.write(len);
    for(int i = 0; i<len; i++)
    {
        Wire.write(tSend[i]);
    }
    Wire.write(END1);                               // send end
    Wire.write(END2);
    
    Wire.endTransmission();
    
    delay(50);
    
    // wait for ok
    Serial.println("wait for ok from BTM:");
    Wire.requestFrom(5, 2);                         // request 6 bytes from slave device #2
    delay(20);
    
    
    
  
}

/*********************************************************************************************************
** Function name: sendDtaI2c
** Descriptions:  send eeprom data to i2c, 
*********************************************************************************************************/
unsigned char eeprom_manage::checkBTMOK()
{
    char a,b;
    int tout = 0;
    while(1)
    {
        
        if(Wire.available())
        {
            a = Wire.read();
            Serial.write(a);
            if('O' == a)
            {
                // Wait for next character K. available() is required in some cases, as K is not immediately available.
                while(Wire.available())
                {
                    b = Wire.read();
                    Serial.write(a);
                    break;
                }
                
                if('K' == b)
                {
                    break;
                }
            }
        }
        delay(1);
        tout++;
        if(tout == 3000)
        {
            Serial.println("time out");
            return 0;
        }
    }
    
    Serial.println("\r\nget OK\r\n");
    return 1;
}

/*********************************************************************************************************
** Function name: sendVolX
** Descriptions:  send volX to i2c
*********************************************************************************************************/
unsigned char eeprom_manage::sendVolX()
{
    putVolX();
    sendDtaI2c(EEPADDRVOLX, 16);
}

/*********************************************************************************************************
** Function name: sendVolY
** Descriptions:  send volY to i2c
*********************************************************************************************************/
unsigned char eeprom_manage::sendVolY()
{
    putVolY();
    sendDtaI2c(EEPADDRVOLY, 16);
    sendDtaI2c(EEPADDRVOLY+16, 16);
}

/*********************************************************************************************************
** Function name: sendVolY_n
** Descriptions:  send VolY_n to i2c
*********************************************************************************************************/
unsigned char eeprom_manage::sendVolY_n()
{
    putVolY_n();
    sendDtaI2c(EEPADDRVOLY_N, 16);
    sendDtaI2c(EEPADDRVOLY_N+16, 16);
}

/*********************************************************************************************************
** Function name: Adjust
** Descriptions:  Adjust all
*********************************************************************************************************/
unsigned char eeprom_manage::Adjust()
{
    sendVolX();
    sendVolY();
    sendVolY_n();
    sendDtaI2c(EEPADDRIFSET, 1);                // adjusted
    
}

/*********************************************************************************************************
** Function name: setVolX
** Descriptions:  setVolX
*********************************************************************************************************/
unsigned char eeprom_manage::setVolX(int *ptr)
{
    for(int i = 0; i<8; i++)
    {
        volX[i] = ptr[i];
    }
}

/*********************************************************************************************************
** Function name: setVolY
** Descriptions:  setVolY
*********************************************************************************************************/
unsigned char eeprom_manage::setVolY(float *ptr)
{
    for(int i = 0; i<8; i++)
    {
        volY[i] = ptr[i];
    }
}

/*********************************************************************************************************
** Function name: setVolY_n
** Descriptions:  setVolY_n
*********************************************************************************************************/
unsigned char eeprom_manage::setVolY_n(float *ptr)
{
    for(int i = 0; i<8; i++)
    {
        volY_n[i] = ptr[i];
    }
}

eeprom_manage  EEPM;
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
