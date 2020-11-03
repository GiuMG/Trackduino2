#if MODE == 99

// ********************************************************************* 
// includes
// *********************************************************************
#include <TM1637Display.h> //Einbinden der Bibliothekt zum ansteuern des Displays

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
  else
  {
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
