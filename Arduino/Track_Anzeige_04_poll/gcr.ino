/*
     *      This program is free software; you can redistribute it and/or
     *      modify it under the terms of the GNU General Public License
     *      as published by the Free Software Foundation; either version
     *      2 of the License, or (at your option) any later version.
     *
     *  Copyright 2006 Wolfgang Moser (http://d81.de)
     *
     */



 void gcr_5_to_4_decode(const unsigned char *source, unsigned char *dest)
 {            
      /* 255 denotes illegal GCR bytes, for error checking extensions */
     static const unsigned char decodeGCR[32] =
         {255,255,255,255,255,255,255,255,255,  8,  0,  1,255, 12,  4,  5,
          255,255,  2,  3,255, 15,  6,  7,255,  9, 10, 11,255, 13, 14,255 };
 
         /* at least 24 bits for shifting into bits 16...20 */
     register unsigned long tdest, nybble;
     int rv = 0;
     int i;
   
     tdest   = *source;
     tdest <<= 13;
   
    for(i = 5; (i < 13); i += 2, dest++)
    {
        source++;
        tdest  |= ((unsigned long)(*source)) << i;
   
       // "tdest >> 16" could be optimized to a word
       // aligned access, hopefully the compiler does
       // this for us (in a portable way)
       nybble  = decodeGCR[ (tdest >> 16) & 0x1f ];
       if(nybble > 15) rv |= 2;    // invalid GCR detected
       *dest   = nybble << 4;
       tdest <<= 5;
   
       nybble  = decodeGCR[ (tdest >> 16) & 0x1f ];
       if(nybble > 15) rv |= 1;    // invalid GCR detected
       *dest  |= (nybble & 0x0f);
       tdest <<= 5;
   
       //rv    <<= 2;    // mark invalid GCR codes, make room for new
   }
 }
