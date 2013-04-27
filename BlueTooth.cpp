/*
  Bluetooth.cpp
  2012 Copyright (c) Seeed Technology Inc.  All right reserved.
 
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
#include <Arduino.h>
#include "BlueTooth.h"

char strAddr[30];
char name[20] = "BT MULTIMETER";


void blueTooth_Init()
{
    //Serial.begin(38400); //Set BluetoothBee BaudRate to default baud rate 38400
    Serial1.begin(38400);
    delay(500);
    setupBlueToothConnection();
}

// SEND
void blueToothSend(INT8U len, INT8U *dataSend)
{
    // data head
    for(int i = 0; i<len; i++)
    {
        Serial1.write(dataSend[i]);
    }
    
}

static void setupBlueToothConnection()
{
    sendBlueToothCommand("\r\n+STWMOD=1\r\n");
    CheckOK();
   // sendBlueToothCommand("\r\n+STNA=BT MULTIMETER\r\n");
   // CheckOK();
    sendBlueToothCommand("\r\n+STAUTO=0\r\n");
    CheckOK();
    sendBlueToothCommand("\r\n+STOAUT=1\r\n");
    CheckOK();
    sendBlueToothCommand("\r\n +STPIN=0000\r\n");
    checkBluetoothState('1');
    sendBlueToothCommand("\r\n+INQ=1\r\n");
    //checkBluetoothState('4');
	getaddr();
	 Serial1.print("\r\n+CONN=");
	 Serial1.print(strAddr);
	sendBlueToothCommand("\r\n");
	 CheckOK();
	 Serial.println("BT CONN ok");
	 // sendBlueToothCommand("\r\n+RTPIN=0000\r\n");
	  	// delay(200);

}

//Checks if the response "OK" is received
static void CheckOK()
{
    char a,b;
    while(1)
    {
        if(Serial1.available())
        {
            a = Serial1.read();
            if('O' == a)
            {
                // Wait for next character K. available() is required in some cases, as K is not immediately available.
                while(Serial1.available())
                {
                    b = Serial1.read();
                    break;
                }
                if('K' == b)
                {
                    break;
                }
            }
        }
        delay(1);
    }
}

static void sendBlueToothCommand(char command[])
{
    Serial1.print(command);
}

static void checkBluetoothState(char state)
{
    unsigned char temp;
    while(1)  
    {
        while(!Serial1.available());
        temp=Serial1.read();
        if(temp=='+')  
        {
            while(temp!=state)  
            {
                if(Serial1.available())
                temp=Serial1.read();
            }
            if(temp==state)  
            {
                break;
            }
        }
        delay(1);
    }
}


int findString(char *sfind, char *dta)
{
    char *ptr = strstr(dta, sfind);
    if(ptr)
    {
        return int(ptr-dta)+1;
    }
    else
    {
        return 0;
    }
}


bool blueProc(char *dtaIn, char *dtaOut)
{
       
	if(findString(name, dtaIn)==0)
    {
        return 0;
    }
    

    if(findString("+RTINQ=", dtaIn) == 0)
    {
        return 0;
    }
	
	int offset = 0;
	for(int i=0; i<strlen(dtaIn); i++)
	{
		if(dtaIn[i] == '=')
		{
			offset = i+1;
			break;
		}
	}

	int i=0;
	for(i = 0; dtaIn[i+offset] != ';'; i++)
	{

		dtaOut[i] = dtaIn[i+offset];
	}
	dtaOut[i] = '\0';
	return 1;
}
  
  void getaddr(void)
  {
  int n=0;
  //char strAddr[30];
  //char addr[30];
  char dtaUart[100];
  int dtaUartLen = 0;
unsigned char getDta = 0;
//while(!Serial1.available())
 while (!n) 
  {
  // Serial.println("start1");
  
	if(Serial1.available())
	{
		dtaUart[dtaUartLen++] = (char)Serial1.read();
		//Serial.print(dtaUart[dtaUartLen-1]);
	}

    if (dtaUart[dtaUartLen-1] == '\n') 
	{
      getDta = true;
      dtaUart[dtaUartLen] = '\0';
    } 
	if(getDta)
    {

		//Serial.println("str");
		blueProc(dtaUart, strAddr);
		delay(100);
		n=blueProc(dtaUart, strAddr);
		if(n==1)
		{
		Serial.println("strAddr:");
		Serial.println(strAddr);
		}
		//Serial.println(strAddr);
        //addr=strAddr;
		//n=1;
        getDta = 0;
        dtaUartLen = 0;
    }
  }
 
	
  }


/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
