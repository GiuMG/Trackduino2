# Trackduino2
Trackduino is Trackdisplay reader for Commodore 1541 Drives. It's based on an original project from oe7twj (don't know his real name) 

# What does it do ? 
It is able ro read the Track position of the read/write head and display it on a 4 digit LED Display along with the motor rotation animation and the half track indication.
This particulary version includes also the feature to be used with a LCD 16x2 display as well. 
Trackduino2 can be easily plugged into the MOS 6522 VIA Chip socket of the 1541 mainboard without any other internal connections, since it already includes the Arduino Nano, acting as an 6522 adapter board. It can be easily removed to get back the Drives stock condition. 

# How it works 
Once the Drive is powered on, the Arduino waits for the Track information given from the read/write head, until then the Trackposition is still unknown and the Display stays on stand-by.


