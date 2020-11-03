#if MODE == 0

// ********************************************************************* 
// includes
// *********************************************************************
#include <Wire.h>



 
void setup_display()
{
  Wire.begin(2);    // join i2c bus with address #2
  Wire.onRequest(requestEvent); // register event
}


// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() 
{
  byte rc = track;

  if(halfTrack < 0)
  {
    rc |= 0x80;
  }
  else if(halfTrack > 0)
  {
    rc |= 0x40;
  }
  Wire.write(rc);      // respond with message of 1 bytes
}



void loop_display()
{
}


#endif

