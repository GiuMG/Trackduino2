/*void Test()
{
  long cnt=0;
  long cnt2=0;
  byte to;

  //
  // wait for ByRdy
  //
  cli();
  activateFallingEdge0();
  
  to = TCNT0 +80;
  cnt2 = 100000;
  while(1)
  {
    if(isFallingEdge0())
    {
      clearFallingEdge0();
      cnt++;
    }
    if(--cnt2 < 0)
    //if(to == TCNT0)
      break;
  }
  sei();
  Serial.print("count byte ready: ");
  Serial.println(cnt);
  return;
}*/

