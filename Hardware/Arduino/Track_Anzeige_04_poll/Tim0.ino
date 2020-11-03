



//
// T0 tick :: 4µs
//
unsigned long inline hpticks(void)
{
  extern volatile unsigned long timer0_overflow_count;
  return (timer0_overflow_count << 8) + TCNT0;
}



//
// T0 tick :: 4µs
//
unsigned long CliTicks(void)
{
  static unsigned long timer0_overflow_count = 0;
  static byte lastTCNT = 0;
  byte tcnt;

  if(lastTCNT != (tcnt = TCNT0))
  {
    if(lastTCNT > tcnt)
      timer0_overflow_count++;
    
    lastTCNT = tcnt;
  }
  return (timer0_overflow_count << 8) + TCNT0;
}


//
// T0 tick :: 4µs
//
void CliDelayTicks(int pDelay)
{
  byte lastTCNT = TCNT0;
  byte tcnt;

  while(pDelay-- > 0)
  {
    while(lastTCNT == (tcnt = TCNT0)) ;

    lastTCNT = tcnt;
  }
}



void DisableIntTimer0()
{
  TIMSK0 &= ~_BV(TOIE0); // disable timer0 overflow interrupt
}
void EnableIntTimer0()
{
  TIMSK0 |= _BV(TOIE0); // enable timer0 overflow interrupt
}



/*
void TestTim0()
{
  unsigned long hp1;
  unsigned long hp2;
  unsigned long hp3;
  unsigned long cl1;
  unsigned long cl2;
  unsigned long cl3;

  hp1 = hpticks();
  cl1 = CliTicks();
  delay(2);
  hp2 = hpticks();
  cl2 = CliTicks();
  CliDelayTicks(100);
  hp3 = hpticks();
  cl3 = CliTicks();

  
  Serial.print("hpticks: ");
  Serial.print(hp1);
  Serial.print(", CliTicks: ");
  Serial.print(cl1);
  Serial.println();
  Serial.print("hpticks: ");
  Serial.print(hp2);
  Serial.print(", CliTicks: ");
  Serial.print(cl2);
  Serial.println();
  Serial.print("hpticks: ");
  Serial.print(hp3);
  Serial.print(", CliTicks: ");
  Serial.print(cl3);
  Serial.println();
}
*/

