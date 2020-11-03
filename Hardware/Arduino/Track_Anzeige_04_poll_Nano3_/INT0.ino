//#include <digitalWriteFast.h>


//volatile int cnt0 = 0;
//volatile int cnt1 = 0;
//volatile bool fByteReady = 0;
volatile byte dByteReady = 0;
//volatile bool fSync = 0;




void activateFallingEdge0()
{
  EICRA &= ~(bit(ISC00) | bit (ISC01));  // clear existing flags
  EICRA |=  bit (ISC01);    // set wanted flags (falling level interrupt)
  EIFR   =  bit (INTF0);    // clear flag for interrupt 0
}  // end of activateInterrupt0

void clearFallingEdge0()
{
  EIFR   =  bit (INTF0);    // clear flag for interrupt 0
}  // end of activateInterrupt0

bool isFallingEdge0()
{
  return EIFR & bit (INTF0);    // test flag for interrupt 0
}  // end of activateInterrupt0


void activateInterrupt0 ()
{
  EICRA &= ~(bit(ISC00) | bit (ISC01));  // clear existing flags
  EICRA |=  bit (ISC01);    // set wanted flags (falling level interrupt)
  EIFR   =  bit (INTF0);    // clear flag for interrupt 0
  EIMSK |=  bit (INT0);     // enable it
}  // end of activateInterrupt0

void deactivateInterrupt0 ()
{
  EIMSK &= ~bit (INT0);     // disable it
}  // end of deactivateInterrupt0


// ISR for when to fire
ISR (INT0_vect)
{
  register byte b, d;

  CliDelayTicks(5);
  
  b = PINB;
  d = PIND;
  dByteReady = ((b & 0xF) |  (d & 0xF0));

  if(doRead == 1)
  {
    if(dByteReady == 0xff)   
       doRead = 2;
  }
  if(doRead == 2)
  {
    if(ptRead < BYTELIST_SIZE)
    {
      byteList[ptRead++] = dByteReady;
    }
    else
    {
       doRead = 3;
    }
  }
  
  //deactivateInterrupt0 ();          // no more interrupts yet
} // end of ISR (INT0_vect)
