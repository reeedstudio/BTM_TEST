/*
  SmartMultimeter.h
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

#ifndef _SMARTMULTIMETER_H_
#define _SMARTMULTIMETER_H_

#include "SmartMultimeter_dfs.h"

class SmartMultimeter
{
    public:
    unsigned char cmdType;                  // cmd type

    public:
    unsigned char dtaRevBt[11] ;            // data read from bluetooth
    unsigned char dtaSendBt[7];            // data send to bluetooth
    
    private:
    float nvAB[4][2];
    float vAB[4][2];
    
    float nIAB[2][2];
    float  IAB[2][2];
    
    float nmIAB[2][2];
    float  mIAB[2][2];
    
    public:

    void setSwitch(unsigned char sw);       // switch adc read channel
    bool adcIsInv();                        // if adc value inverter
    void setR(unsigned char r);
    void genDtaBt(float dta, unsigned char rType, unsigned char unit);  // generate the dta to bluetooth
    inline int Partition(int a,int b, int *N);
    void quickSort(int a,int b, int *N);    // quick sort
    int readADC(int pinAD);
    int readADC(int pinAD, int ch);
    void lsline(int n, int *x, float *y, float a[2]);
    
    public:
    void init();
    void genVol();                          // generate voltage value, and save to dtaSendBt
    void genAmp();                          // generate ampl  value, V
    void genRes();                          // generate resistance value
    
    void genAVR();
#if SLEEP_MODE
    void sleepMode();
#endif

};

extern SmartMultimeter SmartVom;

#endif
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
