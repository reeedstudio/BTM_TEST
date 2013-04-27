/*
  SmartMultimeter.cpp
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
#include <Wire.h>
#include <Sleep_n0m1.h>
#include <I2C_Uart.h>

#include "SmartMultimeter.h"

#if SLEEP_MODE
Sleep sleep;
#endif
/*********************************************************************************************************
** Function name: init
** Descriptions:  Initialize all variables and I/O pin
*********************************************************************************************************/
void SmartMultimeter::init()
{
    dtaRevBt[20];

    cmdType = RETA;

    dtaSendBt[DTASENDBTSTART1]  = DTASTART1;
    dtaSendBt[DTASENDBTSTART2]  = DTASTART2;
    dtaSendBt[DTASENDBTEND1]    = DTASEND1;
    dtaSendBt[DTASENDBTEND2]    = DTASEND2;
    dtaSendBt[DTAREVBTLEN]      = 5;

    pinMode(PINADCINV, INPUT);
    pinMode(PINSWS0, OUTPUT);
    pinMode(PINSWS1, OUTPUT);
    pinMode(PINSWS2, OUTPUT);
    pinMode(13, OUTPUT);
    for(int i = PINR1; i<=PINR3; i++)
    {
        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
    }
    int xV[8] = {100, 1200, 1300, 3700, 3800, 13700, 14000, 30000};
    float ynV[8] = {106, 1210, 1220, 3660, 3480, 13360, 13480, 29480};
    float yV[8]  = { 88, 1180, 1380, 3800, 3900, 13800, 14100, 30100};
    
    int xnI2A[4]    = {50, 230, 250, 2000};
    float ynI2A[4]  = {45, 236, 225, 2070};
    
    int xI2A[4]     = {50, 200, 250, 2000};
    float yI2A[4]   = {55, 212, 259, 2090};

    for(int i = 0; i<2; i++)
    {
        lsline(2, &xnI2A[2*i], &ynI2A[2*i], nIAB[i]);
        lsline(2, &xI2A[2*i],  &yI2A[2*i],  IAB[i]);
    }
    for(int i = 0; i<4; i++)
    {
        lsline(2, &xV[2*i], &ynV[2*i], nvAB[i]);
        lsline(2, &xV[2*i], &yV[2*i], vAB[i]);
    }

}

/*********************************************************************************************************
** Function name: adcIsInv
** Descriptions:  Just for vol mode, when vol < 0, return 1
*********************************************************************************************************/
bool SmartMultimeter::adcIsInv()
{
    if(digitalRead(PINADCINV))
    {
        digitalWrite(13, LOW);
        return 0;
    }
    digitalWrite(13, HIGH);
    return 1;
}

/*********************************************************************************************************
** Function name: genDtaBt
** Descriptions:  generate data to send to bluetooth, except for the start and end
*********************************************************************************************************/
void SmartMultimeter::genDtaBt(float dta, unsigned char rType, unsigned char unit)
{
    dta = abs(dta);
    unsigned int iR =   (unsigned int)dta;          // Integer
    float fR = (float)(dta - iR);
    unsigned char ifR = (unsigned char)(100*fR);    // float
   // PRINT("unit = ");
	//PRINTLN(unit);
    dtaSendBt[DTASENDBTLEN]     =   1;
    dtaSendBt[DTASENDBTTYPE]    =   rType;
    //dtaSendBt[DTASENDBTN1]      =   iR>>8;
    //dtaSendBt[DTASENDBTN2]      =   iR&0xff;
    //dtaSendBt[DTASENDBTF]       =   ifR;
    //dtaSendBt[DTASENDBTUNIT]    =   unit;
}

/*********************************************************************************************************
** Function name: setR
** Descriptions:  when measure resistance, choose the dividing resistor
*********************************************************************************************************/
void SmartMultimeter::setR(unsigned char r)
{
    for(int i = 8; i<=11; i++)               // All pin floating
    {
        pinMode(i, INPUT);
    }
    pinMode(r+8, OUTPUT);                       // set r to high
    digitalWrite(r+8, HIGH);
}

/*********************************************************************************************************
** Function name: Partition
** Descriptions:  just use for quickSort
*********************************************************************************************************/
inline int SmartMultimeter::Partition(int a,int b, int *N)
{
    int i=a,j=b,tmp = 0;
    while(i<j)
    {
        while((i<j)&&(N[i]<=N[j]))
        j--;
        if(i<j)
        {
            tmp = N[i];
            N[i] = N[j];
            N[j] = tmp;
            i++;
        }
        while((i<j)&&(N[i]<=N[j]))
        i++;
        if(i<j)
        {
            tmp = N[i];
            N[i] = N[j];
            N[j] = tmp;
            j--;
        }
    }
    return i;
}

/*********************************************************************************************************
** Function name: quickSort
** Descriptions:  quicksort algorithm
*********************************************************************************************************/
void SmartMultimeter::quickSort(int a,int b, int *N)
{
    int pivot;
    if(a<b)
    {
        pivot=Partition(a,b, N);
        quickSort(a,pivot-1, N);
        quickSort(pivot+1,b, N);
    }
}

/*********************************************************************************************************
** Function name: genVol
** Descriptions:  Calculate the voltage value and generate dtaSendBt[]
*********************************************************************************************************/
void SmartMultimeter::genVol()
{
    int pinAD = adcIsInv() ? A7 : A6;           // if > 0
    int ch    = ADCHV1;

    if(readADC(pinAD, ADCHV1) < 1000)           // choos chanel
    {
        ch = ADCHV1;
    }
    else if(readADC(pinAD, ADCHV2) < 1000)
    {
        ch = ADCHV2;
    }
    else if(readADC(pinAD, ADCHV3) < 1000)
    {
        ch = ADCHV3;
    }
    else 
    {
        ch = ADCHV4;
    }
    
    PRINTLN("**************");
    PRINT("ch = ");PRINTLN(ch);
    
    if(A6 == pinAD)
    {
        PRINTLN("A6");
    }
    else
    {
        PRINTLN(A7);
    }
    setSwitch(ch);                                  // set chanel

    int valAD = readADC(pinAD);

    float tch[4] = {3.0, 24.3, 1.0, 11.0};          // multiple, {3.0, 26.8, 1.0, 8.0}

    float vol = valAD*1.25/1023;
    
    vol *= tch[ch];

    float er = 0.00001;
    
    vol *= 1000.0;

    unsigned char adchvNum[4] = {1, 3, 0, 2};
    
    if(A7 == pinAD)
    {
        vol = (vol - nvAB[adchvNum[ch]][0])/nvAB[adchvNum[ch]][1];
    }
    else
    {
        vol = (vol - vAB[adchvNum[ch]][0])/vAB[adchvNum[ch]][1];
    }
   
    vol /= 1000.0;

    vol = abs(vol);
    unsigned char unit = (vol - 1.0)<er ? UNITMV : UNITV;
    unit = (pinAD == A7) ? unit + 128 : unit;
    vol = (vol - 1.0)<er ? vol*1000.0 : vol;
    genDtaBt(vol, RETV, unit);
    
}

/*********************************************************************************************************
** Function name: genAmp
** Descriptions:  Calculate the current value and generate dtaSendBt[]
*********************************************************************************************************/
void SmartMultimeter::genAmp()
{
    unsigned char ch = 0;

    int pinAD = adcIsInv() ? A7 : A6;           // if > 0

    float cht[3] = {1, 10, 100};
    for(int i = ADCHA2; i>=ADCHA0; i--)
    {
        int vadc = readADC(pinAD, i);
        if(vadc < 1010)
        {
            ch = i;
            break;
        }
    }

    setSwitch(ch);

    int valADC = readADC(pinAD);

    float v1 = valADC*1.25/1023;

    v1 = v1/cht[ch-ADCHA0];
    float iGet = 0;

    if(cmdType == GETMA)
    {
        iGet = v1/(4.399);
        
        if(A7 == pinAD)
        {
            iGet += 0.003;
        }
    }
    else
    {
        iGet = v1/0.499;
        
        iGet *= 1000.0;
        
        if(A6 == pinAD)
        {
            switch(ch)
            {
                case ADCHA0:
                
                iGet = (iGet - IAB[1][0])/IAB[1][1];
                break;
                
                case ADCHA1:
                iGet = (iGet - IAB[0][0])/IAB[0][1];
                break;
                
                case ADCHA2:
                iGet = (iGet - IAB[0][0])/IAB[0][1];
                
                break;
                
                default:
                
                ;
            }
        }
        else if(A7 == pinAD)
        {
            switch(ch)
            {
                case ADCHA0:
                
                iGet = (iGet - nIAB[1][0])/nIAB[1][1];
                break;
                
                case ADCHA1:
                iGet = (iGet - nIAB[0][0])/nIAB[0][1];
                break;
                
                case ADCHA2:
                iGet = (iGet - nIAB[0][0])/nIAB[0][1];
                
                break;
                
                default:
                
                ;
            }
        }
        
        iGet /= 1000.0;
    }

    float er = 0.00001;

    if(A6 == pinAD)
    {
        if((iGet - 1.0) < er)
        {
            iGet *= 1000.0;
            genDtaBt(iGet, RETA, UNITMA);
        }
        else
        {
            genDtaBt(iGet, RETA, UNITA);
        }
    }
    else
    {
        iGet = abs(iGet);
        if((iGet - 1.0) < er)
        {
            iGet *= 1000.0;
            genDtaBt(iGet, RETA, UNITNMA);
        }
        else
        {
            genDtaBt(iGet, RETA, UNITNA);
        }
    }

}

/*********************************************************************************************************
** Function name: genRes
** Descriptions:  Calculate the resistance value and generate dtaSendBt[]
*********************************************************************************************************/
void SmartMultimeter::genRes()
{
    long rCom = 0;
    long rAll[4] = {R4, R3, R2, R1};                // R1 - R3 : 1.8k 18k 180k 1.8M
    
    for(int i = 3; i>=0; i--)
    {
        setR(i);
        int vadc = readADC(A6, ADCHR);
        if(vadc < 1020)
        {
            rCom = rAll[i];
            break;
        }
        delay(1);
    }

    int valADC = readADC(A6);
    float uADC = (float)valADC*1.25/1023;
    float rTst = (rCom * uADC)/(3.3 - uADC);

    if(rTst < 1000.0)                   // R
    {
        genDtaBt(rTst, RETR, UNITR);
    }
    else                                // KR
    {
        rTst /= 1000;
        genDtaBt(rTst, RETR, UNITKR);
    }

}

/*********************************************************************************************************
** Function name: setSwitch
** Descriptions:  Set the ADC channels, from 0 - 7
*********************************************************************************************************/
void SmartMultimeter::genAVR()
{

    cmdType = dtaRevBt[3];

    switch(cmdType)
    {
        case GETV:
        genVol();
        break;

        case GETA:
        genAmp();

        break;

        case GETMA:
        genAmp();
        break;

        case GETR:
        genRes();
        
        default:
        ;
    }
#if SLEEP_MODE
    delay(1);
    sleepMode();
#endif
}
/*********************************************************************************************************
** Function name: setSwitch
** Descriptions:  Set the ADC channels, from 0 - 7
*********************************************************************************************************/
void SmartMultimeter::setSwitch(unsigned char sw)
{
    if(sw > 7)return ;

    if(sw & 0x01)digitalWrite(PINSWS0, HIGH);
    else    digitalWrite(PINSWS0, LOW);

    if(sw & 0x02)digitalWrite(PINSWS1, HIGH);
    else    digitalWrite(PINSWS1, LOW);

    if(sw & 0x04)digitalWrite(PINSWS2, HIGH);
    else    digitalWrite(PINSWS2, LOW);
}

/*********************************************************************************************************
** Function name: readADC
** Descriptions:  read adc value , pinAD: A6 or A7 only
*********************************************************************************************************/
int SmartMultimeter::readADC(int pinAD)
{

    int sAD[AVETIME];
    long sum = 0;
    for(int i = 0; i<AVETIME; i++)
    {
        sAD[i] = analogRead(pinAD);
    }
    quickSort(0, AVETIME-1, sAD);

    for(int i = AVETIME/4; i<(AVETIME-AVETIME/4); i++)       // remove the largest two and least tow
    {
        sum += sAD[i];
    }

    return sum/(AVETIME/2);
}

/*********************************************************************************************************
** Function name: readADC
** Descriptions:  read adc value, Just use for measure voltage
*********************************************************************************************************/
int SmartMultimeter::readADC(int pinAD, int ch)
{
    setSwitch(ch);
    delay(1);
    int sAD[50];
    long sum = 0;
    for(int i = 0; i<50; i++)               // read 50 data
    {
        sAD[i] = analogRead(pinAD);
    }
    quickSort(0, 49, sAD);

    for(int i = 20; i<40; i++)              // remove the largest two and least tow
    {
        sum += sAD[i];
    }

    return sum/20;
}

/*********************************************************************************************************
** Function name: lsline
** Descriptions:  The method of least squares linear fitting algorithm
*********************************************************************************************************/
void SmartMultimeter::lsline(int n, int *x, float *y, float a[2])
{
    float ave_x = 0;
    float ave_y = 0;
    float sumx2 = 0;
    float sumxy = 0;;

    for(int i = 0; i<n; i++)
    {
        ave_x += x[i];
        ave_y += y[i];
    }

    ave_x = ave_x/n;
    ave_y = ave_y/n;

    for(int i = 0; i<n; i++)
    {
        sumx2 += (x[i] - ave_x) * (x[i] - ave_x);
        sumxy += (y[i] - ave_y) * (x[i] - ave_x);
    }

    a[1] = (float)sumxy/(float)sumx2;
    a[0] = ave_y - a[1]*ave_x;
}

/*********************************************************************************************************
** Function name: getButton
** Descriptions:  getButton State
*********************************************************************************************************/
#if SLEEP_MODE
void SmartMultimeter::sleepMode()
{
    int sleepTime = FRESHRATE - 100;
    PRINTLN("goto sleep!");
    sleep.pwrDownMode();            //set sleep mode pwrDownMode idleMode
    sleep.sleepDelay(sleepTime);    //sleep for: sleepTime
    PRINTLN("awake!");
}
#endif

SmartMultimeter SmartVom;

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
