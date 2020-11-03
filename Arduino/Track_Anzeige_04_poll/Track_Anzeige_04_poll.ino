#include <SevenSegmentExtended.h>
#include <SevenSegmentFun.h>
#include <SevenSegmentTM1637.h>

// =====================================================================
// MODE:
// (0) track number send as I2C slave device 2
// (1) track number display over TM1637 display
// *********************************************************************
#define MODE    1



//#define TEST_LED              // internal LED!!!

#define BYTELIST_SIZE   360


// ********************************************************************* 
// includes
// *********************************************************************
#include <Arduino.h>                //Einbinden der Arduino Bibliothekt
#include <digitalWriteFast.h>
#include <TimerOne.h>



#if MODE == 0
  #define NODISPLAY
#else
  #define DISPLAY
#endif



// ********************************************************************* 
// prototype
// *********************************************************************
void setup_display(void);
void loop_display(void);




// ********************************************************************* 
// IO
// *********************************************************************
#define IO_DATA0  8
#define IO_DATA1  9
#define IO_DATA2  10
#define IO_DATA3  11
#define IO_DATA4  4
#define IO_DATA5  5
#define IO_DATA6  6
#define IO_DATA7  7

#define IO_MTR    12
#define IO_STEP0  A0
#define IO_STEP1  A1

#define IO_BYRDY  2
#define IO_SYNC   3
 

#define  interrupt0Pin  2     // byte ready
#define  interrupt1Pin  3     // sync



//
// for UNO and NANO
//
#define GetByteFromDrive()    ((PINB & 0xF) |  (PIND & 0xF0))
#define IsSync()              (!(PIND & (1 << PD3)))
#define IsByteReady()         (!(PIND & (1 << PD2)))

//#define IsSync()              (!digitalRead(IO_SYNC))
//#define IsByteReady()         (!digitalRead(IO_BYRDY))

//#define IsSync()              (!digitalReadFast(IO_SYNC))
//#define IsByteReady()         (!digitalReadFast(IO_BYRDY))

#define IsMotor()             (digitalReadFast(IO_MTR))
#define GetStepper()          ((digitalReadFast(IO_STEP0)?1:0)|(digitalReadFast(IO_STEP1)?2:0))

#ifdef TEST_LED
  #define SetLED(p)      digitalWrite(LED_BUILTIN, p)  
#else  
  #define SetLED(p)      ;
#endif  

 

// display vars
int dispStat = 0;
bool dispMotor = false;


// comamnds
int doTrackAnalyze = 1;


int track = -1;
int halfTrack = 0;
int dir = 1;


byte volatile byteList[BYTELIST_SIZE];
byte volatile doRead = 0;
int volatile ptRead = 0;


long volatile cntTim1 = 0;
byte volatile f1s = 0;


 
void setup()
{
  Serial.begin(38400);
  Serial.println("## 1541 track display ##");

    
  // initialize the digital pin as an output.
#ifdef TEST_LED
  pinMode(LED_BUILTIN, OUTPUT);     
#endif  

  // turn on pullup
  digitalWrite(IO_MTR, HIGH);
  digitalWrite(IO_SYNC, HIGH);
  digitalWrite(IO_BYRDY, HIGH);
  digitalWrite(IO_DATA0, HIGH);
  digitalWrite(IO_DATA1, HIGH);
  digitalWrite(IO_DATA2, HIGH);
  digitalWrite(IO_DATA3, HIGH);
  digitalWrite(IO_DATA4, HIGH);
  digitalWrite(IO_DATA5, HIGH);
  digitalWrite(IO_DATA6, HIGH);
  digitalWrite(IO_DATA7, HIGH);
  digitalWrite(IO_STEP0, HIGH);
  digitalWrite(IO_STEP1, HIGH);
  

  setup_display();
  
  //TestTim0();

  Timer1.initialize(1000);            // initialize timer1, and set a 1 ms  period
  Timer1.attachInterrupt(MsInt);      // attaches callback() as a timer overflow interrupt
}


//
// runs every ms
//
void MsInt()
{
  static long div = 0;

  DoStepper();
  /*
  if(++cntTim1 > div)
  {
    div = cntTim1 +1000;
    f1s = 1;
  }
  */
}
 


void inline IncTrack()
{
  halfTrack++;
  if(halfTrack > 1)
  {
    halfTrack = 0;
    track++;
  }
}

void inline DecTrack()
{
  halfTrack--;
  if(halfTrack < -1)
  {
    halfTrack = 0;
    track--;
  }
}






/*
//
// read a byte
//
int ReadByte()
{
  uint8_t oldSREG = SREG;
  static unsigned long laTim = 0;
  //unsigned long tim = CliTicks();
  byte to;
  byte by;

  //
  // wait for next ByRdy
  //
  while(CliTicks() - laTim < 2);
  laTim = CliTicks();

  //
  // wait for ByRdy
  //
  cli();
  clearFallingEdge0();
  to = TCNT0 +180;
  while(1)
  {
    //if(IsByteReady())
    //{
    if(isFallingEdge0())
    {
      by = GetByteFromDrive();
      clearFallingEdge0();
      SREG = oldSREG;
      return by;
    }
    if(to == TCNT0)
      // timeout - 7*4µs
      break;
  }

  SREG = oldSREG;
  return -1;
} */

//
// read a byte
// MODDED
int inline ReadByte()
{
       uint8_t oldSREG = SREG;
     //  static unsigned long laTim = 0;
  static byte laTim = 0;
  //unsigned long tim = CliTicks();
  byte to;
  byte by;

  //
  // wait for next ByRdy
  //
  //while(CliTicks() - laTim < 1);
  //laTim = CliTicks();
  while(laTim == TCNT0);
  laTim = TCNT0;

  //
  // wait for ByRdy
  //
  //noInterrupts();
  to = TCNT0 +50;
  while(1)
  {
    if(IsByteReady())
    {
      by = GetByteFromDrive();
      //SREG = oldSREG;
      return by;
    }
/*    if(IsSync())
      // SYNC ...
      to = TCNT0 +20;
    else*/ if(to == TCNT0)
      // timeout - 20*4µs
      break;
  }

  //SREG = oldSREG;
  return -1;
}






//
// read a block
//
int inline SearchHeader()
{
  int cnt = 600;
  int by;

  while(1)
  {
    by = ReadByte();
    if(by < 0)  return -1;

    if(by == 0xff)
    {
      by = ReadByte();
      if(by < 0)  return -2;

      if(by == 0x52)
      {
        StartReadSector();
        byteList[ptRead++] = 0xff;
        byteList[ptRead++] = 0x52;
        return 0;
      }
    }
    if(--cnt <= 0)
      return -4;
  }
}



//
// read a block
//
int ReadBlock()
{
  int rc;
  int by;

  if((rc = SearchHeader()) < 0)
    return rc;
  
  while(1)
  {
    by = ReadByte();
    if(by < 0)  return -5;

    if(ptRead >= BYTELIST_SIZE)
      return 0;
      
    byteList[ptRead++] = by;
  } 
}





/*
void InitPinChange()
{
  PCMSK1  =  (1 << PCINT8);
}*/


//
int StartReadSector()
{
  activateFallingEdge0();
  doRead = 0;               // stop
  ptRead = 0;
  doRead = 1;               // read sector
  return 0;
}



//
int ReadSector()
{
  int rc;
  
  noInterrupts();
  rc = ReadBlock();
  interrupts();
  if(rc == 0) rc = ptRead;
  return rc;

  /*
  if(doRead == 3)
  {
    return ptRead;
  }

  while(ptRead > 1)
  {
    if(byteList[1] != 0x52)
    {
      Serial.println("2");
      StartReadSector();
      break;
    }
    if(ptRead > 10)
    {
      int i;
      byte c;

      for(i=9; i<ptRead; i++)
      {
        c = byteList[i];
        if(c != 0x55)
        {
          if(i < 12)
          {
            Serial.println("3");
            StartReadSector();
            return i+1;
            //break;
          }
          if(c != 0xff)
          {
            Serial.println("4");
            StartReadSector();
            return i+1;
            //break;
          }
          break;
        }
      }
    }
    break;
  }
  return 0;
  */
}




//
// analyze byte stream
//
void DoTrackAnalyze()
{
  static unsigned long timer1 = 0;
  static int zst = 0;
  int len, bytesRead;


  switch(zst)
  {
    case 0:
      // wait for drive motor
      SetLED(0);
      dispStat = 0;
      if(IsMotor())
      {
        Serial.println("drive on.");
        
        timer1 = millis() + 450;
        zst = 10;
      }
      break;
    case 10:
      // wait for drive motor is up
      dispStat = 1;
      if(!IsMotor())
      {
        zst = 1;
        break;
      }
      if(millis() > timer1)
      {
        timer1 = millis() + 2000;
        zst = 20;
      }
      break;
    case 20:
      // wait sync 
      dispStat = 2;
      if(!IsMotor())
      {
        zst = 1;
        break;
      }
      if(IsSync())
      {
        zst = 30;
      }
      if(millis() > timer1)
      {
        zst = 800;
      }
      break;
    case 30:
      // wait !sync 
      dispStat = 3;
      if(!IsMotor())
      {
        zst = 1;
        break;
      }
      if(!IsSync())
      {
        zst = 40;
      }
      if(millis() > timer1)
      {
        zst = 800;
      }
      break;
    case 40:
      // start read sector
      dispStat = 4;
      StartReadSector();
      //Test();
      timer1 = millis() + 270;
      zst = 41;
      break;
      
    case 41:
      // read sector
      dispStat = 5 + doRead;
      if(!IsMotor())
      {
        zst = 1;
        break;
      }
      if(millis() > timer1)
      {
        //zst = 40;
        //break;
      }
      
      bytesRead = ReadSector();
      if(bytesRead > 0)
      {
        if(bytesRead > BYTELIST_SIZE) bytesRead = BYTELIST_SIZE;

        if(bytesRead < 50)
        {
          zst = 41;
          break;
        }
        if(byteList[1] != 0x52)
        {
          zst = 40;
          break;
        }

        Serial.println("found sector header!");
        zst = 50;
        break;
      }
      else
      {
        Serial.print("-- ");  
        Serial.print(bytesRead);  
        Serial.println("");
      }
      break;
    case 50:
      // sector read
      byte header[8]; 
      byte crc; 

      gcr_5_to_4_decode(byteList +1, header +0);
      gcr_5_to_4_decode(byteList +6, header +4);
      Serial.print("header: ");
      Serial.print(header[0]);
      Serial.print("  CS: ");
      Serial.print(header[1]);
      Serial.print("  Sector: ");
      Serial.print(header[2]);
      Serial.print("  Track: ");
      Serial.print(header[3]);
      Serial.print("  ID1: ");
      Serial.print(header[4]);
      Serial.print("  ID2: ");
      Serial.print(header[5]);
      for(int k=1, crc=0; k < 6; k++)
      {
        crc ^= header[k];
      }
      Serial.print("  CRC: ");
      Serial.println(crc);
      if(crc == 0)
      {
        track = header[3];
        zst = 51;
      }
      else
      {
        Serial.println("CRC error ...");
        zst = 0;
      }

      len = 16;
      for(int k=0; k < bytesRead; ++k)
      {
        //DoStepper();
        Serial.print(" ");
        Serial.print(byteList[k], HEX);
        if (--len == 0)
        {
          Serial.println("");
          len = 16;
        }
      }
      Serial.println("");
      break;
    case 51:
      dispStat = 9;
      if(!IsMotor())
      {
        Serial.println("drive off.");
        zst = 52;
        break;
      }
      break;
    case 52:
      dispStat = 9;
      if(IsMotor())
      {
        Serial.println("drive on.");
        zst = 51;
        break;
      }
      break;
    case 800:
      Serial.println("error ...");
      zst = 801;
    case 801:
      // wait for drive motor stops
      dispStat = 91;
      if(!IsMotor())
      {
        zst = 1;
        break;
      }
      break;
    default:
      // something bad happens ...
      Serial.println("drive off.");
      zst = 0;
      break;
  }
}



//
// watch stepper
//
void DoStepper()
{
  static byte oldStep = 0xff;
  //static byte oldTim;
  byte step = GetStepper();
  byte oldTim = TCNT0;
  //int diff;
  
  if(track < 1 || track > 42)
    oldStep = GetStepper();
  else
  {
    if(step != oldStep)
    {
      while(oldTim == TCNT0);
      step = GetStepper();
      if(step != oldStep)
      {
        if (((oldStep+1) & 0x3) == step)
          IncTrack();
        else if (((oldStep-1) & 0x3) == step)  
          DecTrack();
          
        oldStep = step;
      }
    }
  }
}



//
// print stepper
//
void PrintTrack()
{
  static byte oldTrack = 0;

  if(track < 1 || track > 42)
    ;
  else if(oldTrack != track)
  {
    oldTrack = track;
    Serial.print("track: ");
    Serial.print(track);
    Serial.print("  ht: ");
    Serial.print(halfTrack);
    Serial.print("  stp: ");
    Serial.print(GetStepper());
    Serial.println();
  }
/*  if(f1s)
  {
    f1s = 0;
    Serial.println(cntTim1);
  }*/
}


 
void loop()
{
  dispMotor = IsMotor();
  
  DoTrackAnalyze();
  //DoStepper();
  PrintTrack();
  
  loop_display();
}
