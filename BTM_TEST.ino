//#include <SoftwareSerial.h>
#include <Wire.h>
#include <I2C_Uart.h>
#include <Sleep_n0m1.h>
#include <TimerOne.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>

#include "eeprom_manage.h"
#include "BTMGlobalDfs.h"
#include "SmartMultimeter.h"
#include "BlueTooth.h"
#include "SmartMultimeter_dfs.h"

// for loovee
int volX_t[8]       = {50, 1200, 1300, 3700, 3800, 13400, 13500, 30000};
float volY_t[8]     = {0, 0, 0, 0, 0, 0, 0, 0};
float volY_n_t[8]   = {0, 0, 0, 0, 0, 0, 0, 0};



unsigned char dsb[7];
unsigned char drb[11];
bool getBtDta               = false;
int rdl    = 0;
int error=0;
int n;
void setup()
{
    Serial.begin(38400);
    Serial1.begin(38400);
    mega_init();
    blueTooth_Init();
    delay(1000);
    
    EEPM.init();

}

void loop()
{
    error=0;
    OHMtest();
    delay(1000);
    VVVtest();
    delay(1000);
    CUR_Atest();
    delay(1000);

    // CUR_MAtest();
    delay(1000);
    VOLtest();
    delay(1000);

    Serial.print("error=");
    Serial.println(error);
}

void serialEventLvc()
{
    while (Serial1.available())
    {
        // get the new byte:
        unsigned char inChar = (unsigned char)Serial1.read();
        // add it to the inputString:
        drb[rdl++]= inChar;
        // if the incoming character is a newline, set a flag
        // so the main loop can do something about it:

    }
}



void OHMtest(void)
{
    unsigned char porta[8]= {0x00, 0x08, 0x20, 0x24, 0x40, 0x42, 0x50, 0x51};
    float r[8]    = {0.97, 110, 100, 11, 10, 1.1, 1, 29};

    dsb[3]=0x04;
    for(int i=0;i<8;i++)
    {
        PORTA|=porta[i];
        delay(300);
        compare(r[i]);
        PORTA&=~porta[i];
    }

}


void VOLtest(void)
{
    //Forward   TEST
    unsigned char portc[7]  ={0x01,0x21,0x09,0x49,0x05,0x85,0x15};
    float v[7]      ={37,1.2,1.5,3.6,3.7,13.4,13.5};
    int n;
    dsb[3]=0x01;
    PORTA&=~0x80;
    
    for(n=0;n<7;n++)
    {
        PORTC=portc[n];
        delay(200);
        
        float tmp = compare(v[n]);
        if(n>0)tmp *= 1000.0;
        Serial.print("LOOVEE: ");
        Serial.println(tmp);
        Serial.println();
    }
    //30v test
    PORTA=0x80;
    delay(200);
    compare(30);
    ///////////////////
    //Reverse TEST


    PORTA&=~0xff;
    PORTC&=~0xff;
    for(n=1;n<7;n++)
    {
        portc[n]+=1;
    }
    for(n=0;n<7;n++)
    {
        PORTC=portc[n];
        delay(200);
        compare(v[n]);
    }
    //30v test
    PORTA=0x80;
    delay(200);
    compare(30);

}


///////////////////////////
void CUR_Atest(void)
{

    PORTL&=~0xff;
    PORTG&=~0x07;
    PORTD&=~0x80;
    /////////////
    //Foward test
    dsb[3]=0x02;
    //2A test
    // PORTG|=0x02;
    PORTL|=0x10;
    delay(200);
    compare(2.1);
    //230ma tesst
    PORTL=0x30;
    delay(200);
    compare(240);
    //210ma test
    PORTG=0x04;
    delay(200);
    compare(220);

    ///////////////////
    //Reverse TEST
    PORTL&=~0xff;
    PORTG&=~0x07;
    PORTD&=~0x80;
    //2A test
    //PORTG|=0x02;
    PORTL|=0x08;
    delay(200);
    compare(2.0);
    //230ma tesst
    PORTL|=0x20;
    delay(200);
    compare(240);
    //210ma test
    PORTG=0x04;
    delay(200);
    compare(215);
}
///////////
void CUR_MAtest(void)
{
    PORTL&=~0xff;
    PORTG&=~0x07;
    PORTD&=~0x80;
    /////////////////
    //Forward test
    dsb[3]=0x03;
    //200ma test
    PORTL=0xb0;
    delay(100);
    compare(170);
    //30ma test
    PORTD=0x80;
    delay(100);
    compare(28.5);


    ///////////////////
    //Reverse TEST
    PORTL&=~0xff;
    PORTG&=~0x07;
    PORTD&=~0x80;
    //200ma test
    PORTL=0xa8;;
    delay(100);
    compare(170);
    //30ma test
    PORTD|=0x80;
    delay(100);
    compare(28.5);
    PORTL&=~0xff;
    PORTG&=~0x07;
    PORTD&=~0x80;
}
////////////////////
float  Vtest(int pin)
{
    float v;
    int n;
    for(n=0;n<20;n++)
    {
        v+=analogRead(pin);
    }
    v*=0.00025;
    return v;
}

////////////////
void VVVtest(void)
{
    float VA;
    float VB;
    float VRE;
    VA=Vtest(A13);
    if(VA>3.45)
    {
        Serial.print("3.3V test fail 1......");
        Serial.println(VA);
        error=1;
    }
    if(VA<3.15)
    {
        Serial.print("3.3V test fail 2......");
        Serial.println(VA);
        error=1;
    }
    VB=Vtest(A15);

    if(VB<3.0)
    {
        Serial.print("-3.3V test fail......");
        Serial.println(VB);
        error=1;
    }
    VRE=Vtest(A14);
    VRE=VRE/2;
    if(VRE>1.275)
    {
        Serial.print("1.25V test fail......");
        Serial.println(VRE);
        error=1;
    }
    if(VRE<1.225)
    {
        Serial.print("1.25V test fail......");
        Serial.println(VRE);
        error=1;
    }

    Serial.println(error);
}


//////////////////////////
void swhch(void)
{
    float v;
    v=Vtest(A11);
    v=v/2;
    Serial.println(v);
}


void getdata(void)
{
    // loovee add here
    rdl = 0;     // loovee change to here
    getBtDta = false;
    
    while(1)
    {
        blueToothSend(7,dsb);
        delay(250);

        serialEventLvc();

        if(drb[rdl-1] == 0x45 && drb[rdl-2] == 0x2f)
        {
            getBtDta = true;
        }

        if(rdl > 20)
        {
            rdl = 0;
            getBtDta = false;
        }

        if(getBtDta)
        {

            if(  drb[0] ==  0x53 && drb[1] == 0x00 &&drb[2] == 5 )
            {
                // Serial.println("get good data from btmeter!!");
                break;
            }
            else
            {
                // Serial.println("get bad data from btm");
                rdl = 0;
                getBtDta = 0;
                blueToothSend(7,dsb);
                delay(50);
            }

        }
    }
}

//////////////////////
bool checkGoodDta(unsigned char *dta)
{

    if( rdl == 11 && dta[0] ==  0x53 && dta[1] == 0x00 &&dta[2] == 5  &&dta[10] == 0x2f && dta[11] == 0x45)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

float disdata(  )
{
    int data;
    data  = drb[4];
    data  = data<<8;
    data +=drb[5];
    float tmp = (float)data + (float)drb[6]/100.0;
    return tmp;
}
/////////////////////
float compare(float data)
{
    float revdata;
    float revdataH;
    float revdataL;
    //Serial.println("in compare");
    getdata();                              // wait data from bluetooth meter
    revdata =(float) disdata();
    
    float tmp = revdata;
    
    if(revdata>900)
    {
        revdata/=1000;
        if(revdata>900)
        {
            revdata/=1000;
        }
    }
    // Serial.println("dis ok");

    if(dsb[3]==0x04)
    {
        revdataH=data*1.05;
        revdataL=data*0.95;
    }
    else
    {
        revdataH=data*1.05;
        revdataL=data*0.95;
    }
    if(revdata>revdataH)
    {
        Serial.print(data);
        if(dsb[3]==0x04)
        {

            Serial.print("R test fail.....");
            Serial.println(revdata);
        }
        if(dsb[3]==0x01)
        {
            Serial.print("V test fail.....");
            Serial.println(revdata);
        }
        if(dsb[3]==0x02)
        {
            Serial.print("A test fail.....");
            Serial.println(revdata);
        }
        if(dsb[3]==0x03)
        {
            Serial.print("mA test fail.....");
            Serial.println(revdata);
        }
        error=1;
    }
    if(revdata<revdataL)
    {
        Serial.print(data);
        if(dsb[3]==0x04)
        {

            Serial.print("R test fail.....");
            Serial.println(revdata);
        }
        if(dsb[3]==0x01)
        {
            Serial.print("V test fail.....");
            Serial.println(revdata);
        }
        if(dsb[3]==0x02)
        {
            Serial.print("A test fail.....");
            Serial.println(revdata);
        }
        if(dsb[3]==0x03)
        {
            Serial.print("mA test fail.....");
            Serial.println(revdata);
        }
        error=1;
    }

    return tmp;
}

void mega_init(void)
{
    analogReference(INTERNAL2V56);
    DDRA|=0xff;
    DDRC|=0xff;
    DDRL|=0xff;
    DDRG|=0x07;
    DDRD|=0x80;
    PORTA&=~0xff;
    PORTC&=~0xff;
    PORTL&=~0xff;
    PORTL|=0x08;
    PORTG&=~0x07;
    PORTG|=0x01;
    PORTD&=~0x80;
    dsb[0]=0x53;
    dsb[1]=0x00;
    dsb[2]=0x01;
    dsb[4]=0x00;

    dsb[5]=0x2F;
    dsb[6]=0x45;

}

