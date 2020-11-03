#if MODE == 1

// ********************************************************************* 
// includes
// *********************************************************************

#include "SevenSegmentTM1637.h"


const uint8_t digitToSegment[] = {
 // XGFEDCBA
  0b00111111,    // 0
  0b00000110,    // 1
  0b01011011,    // 2
  0b01001111,    // 3
  0b01100110,    // 4
  0b01101101,    // 5
  0b01111101,    // 6
  0b00000111,    // 7
  0b01111111,    // 8
  0b01101111,    // 9
  0b01110111,    // A
  0b01111100,    // b
  0b00111001,    // C
  0b01011110,    // d
  0b01111001,    // E
  0b01110001     // F
  };

class TM1637Display : public SevenSegmentTM1637
{
   public :
     TM1637Display(uint8_t pinClk, uint8_t pinDIO);
     void setBrightness(uint8_t brightness, bool on = true);
     void showNumberDec(int num, bool leading_zero = false, uint8_t length = 4, uint8_t pos = 0);
     void setSegments(const uint8_t segments[], uint8_t length = 4, uint8_t pos = 0);
     uint8_t encodeDigit(uint8_t digit);
     void showNumberDecEx(int num, uint8_t dots = 0, bool leading_zero = false, uint8_t length = 4, uint8_t pos = 0);
  };

     TM1637Display::TM1637Display(uint8_t pinClk, uint8_t pinDIO) : SevenSegmentTM1637(pinClk, pinDIO)
     {
        SevenSegmentTM1637(pinClk, pinDIO);
     }

     void TM1637Display::setBrightness(uint8_t brightness, bool on)
      {
          uint8_t setDisplayCmd   = B10000000; // bit 6,7
          uint8_t brightnessBits  =      B111; // bit 0,1,2 (7 = max)
          uint8_t displayOnBit    =     B1000; // bit 3
          // construct complete command
          uint8_t command = setDisplayCmd | brightnessBits | displayOnBit;

         // turn display on and set brightness to max (7)
         bool ack = SevenSegmentTM1637::command(command);

         // write init to display using automatic address
         uint8_t setDataCmd      = B01000000;  // bit 6,7
         ack = SevenSegmentTM1637::command(setDataCmd);

        // setBacklight(brightness);

      }
     void TM1637Display::showNumberDec(int num, bool leading_zero, uint8_t length, uint8_t pos)
      {
        showNumberDecEx(num, 0, leading_zero, length, pos);
      }

     void TM1637Display::showNumberDecEx(int num, uint8_t dots, bool leading_zero, uint8_t length, uint8_t)
     {
     
      bool ack;
       
       uint8_t digits[4];
       const static int divisors[] = { 1, 10, 100, 1000 };
       bool leading = true;

       for(int8_t k = 0; k < 4; k++) {
      int divisor = divisors[4 - 1 - k];
      int d = num / divisor;
      uint8_t digit = 0;

      if (d == 0) {
        if (leading_zero || !leading || (k == 3))
           digit = encodeDigit(d);
        else
           digit = 0;
        }
     else {
       digit = encodeDigit(d);
       num -= d * divisor;
       leading = false;
     }
   
     // Add the decimal point/colon to the digit
     digit |= (dots & 0x80);
     dots <<= 1;
   
     digits[k] = digit;
   }

   uint8_t commands[5];
   commands[0] = TM1637_COM_SET_ADR|B0000000;
   commands[1] = digits[0];
   if(dots!=0) {
     commands[2] = digits[1]|B10000000;
   } else {
     commands[2] = digits[1];   
   }
   commands[3] = digits[2];
   commands[4] = digits[3];

   ack = SevenSegmentTM1637::command(commands, 5);

     }

      void TM1637Display::setSegments(const uint8_t segments[], uint8_t length, uint8_t pos)
      {
        uint8_t commands[5];
        commands[0] = B11000000+pos;
        commands[1] = segments[0];
        commands[2] = segments[1];
        commands[3] = segments[2];
        commands[4] = segments[3];


        TM1637Display::command(commands,length+1);
      }
     
uint8_t TM1637Display::encodeDigit(uint8_t digit)
{
  //return SevenSegmentTM1637::encode((int16_t) digit)
  return digitToSegment[digit & 0x0f];
}

#define SEG_A   0b00000001
#define SEG_B   0b00000010
#define SEG_C   0b00000100
#define SEG_D   0b00001000
#define SEG_E   0b00010000
#define SEG_F   0b00100000
#define SEG_G   0b01000000


//      A
//   F     B
//      G
//   E     C
//      D


 


// ********************************************************************* 
// IO
// *********************************************************************
#define IO_CLK A5     //PIN 2 für den CLK PIN
#define IO_DIO A4     //PIN3 für den DIO 



//erzeugen des Display Objektes mit den Parametern für die PINs
TM1637Display display(IO_CLK, IO_DIO); 



 
int delayVal = 800;
int pause = 125;


 
void setup_display()
{
  //Setzt die Helligkeit des Displays mögliche Werte sind 0 bis 15
  //Wobei 0 dunkel und 15 hell ist.
  SetLED(HIGH);
  display.setBrightness(15);
  uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
  uint8_t data2[] = { 0x00, 0x00, 0x00, 0x00 };
  //Setzt die Anzahl der möglichen Segmente.
  display.setSegments(data);
  //Pause
  delay(delayVal);

  SetLED(LOW);
  display.setBrightness(10);
  display.setSegments(data2);
}



#define ANI_1   (        SEG_B | SEG_G | SEG_F)
#define ANI_2   (SEG_A |         SEG_G | SEG_F)
#define ANI_3   (SEG_A | SEG_B |         SEG_F)
#define ANI_4   (SEG_A | SEG_B | SEG_G        )

#define ANI_E   (SEG_A | SEG_F | SEG_G | SEG_E | SEG_D )


//
// show motor animation
// 
void DispMotorAnim()
{
  static unsigned long timer1 = 0;
  static int offs = 0;
  static uint8_t data[] = { ANI_1, ANI_2, ANI_3, ANI_4, 0 };

  if(millis() > timer1)
  {
    timer1 = millis() + 210;
  
    if(dispMotor)
    {
      display.setSegments(data + offs, 1, 0);
      offs = (offs +1) & 0x3;
    }
    else
      display.setSegments(data + 4, 1, 0);
  }
}




void DispTrack()
{
  static int oldTrack = -1;
  static int oldHalfTrack = -2;
  static uint8_t dataO[] = { SEG_A };
  static uint8_t dataM[] = { SEG_G };
  static uint8_t dataU[] = { SEG_D };
  static uint8_t dataErr[] = { ANI_E  };

  if(track < 1 || track > 42)
  {
    if(dispStat == 0)
    {
      display.setSegments(dataM, 1, 2);
      display.setSegments(dataM, 1, 3);
    }
    else if(dispStat < 90)
    {
      display.setSegments(dataM, 1, 2);
      display.showNumberDec(dispStat & 15, false, 1, 3);
    }
    else
    {
      display.setSegments(dataErr, 1, 2);
      display.showNumberDec(dispStat & 15, false, 1, 3);
    }
  }
  else if(track != oldTrack || halfTrack != oldHalfTrack)
  {
    oldTrack = track;
    oldHalfTrack = halfTrack;
    
    display.showNumberDec(track, false, 2, 2);  //Zeigt die Nummer an
  
    if(halfTrack == 0)
    {
      display.setSegments(dataM, 1, 1);
    }
    else if(halfTrack < 0)
    {
      display.setSegments(dataU, 1, 1);
    }
    else
    {
      display.setSegments(dataO, 1, 1);
    }
  }
}



void TrackDisplay()
{
  DispMotorAnim();
  DispTrack();
}


 
void display_demo()
{
  static unsigned long timer1 = 0;
  static unsigned long timer2 = 0;

  if(millis() > timer2)
  {
    timer2 = millis() + 210;
    DispMotorAnim();
  }

  if(millis() > timer1)
  {
    timer1 = millis() + 1000;

    if(dir > 0)
    {
      if(track < 22)
        IncTrack();
      else
        dir = -1;
    }
    else
    {
      if(track > 14)
        DecTrack();
      else
        dir = 1;
    }
    DispTrack();
  }
}


void loop_display()
{
  //display_demo();
  TrackDisplay();
}

#endif

