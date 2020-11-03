# Trackduino2
Trackduino is Trackdisplay reader for Commodore 1541 Drives. It's based on an original project from oe7twj (don't know his real name) 

## What does it do ? 
--------------------
- It is able ro read the Track position of the read/write head and display it on a 4 digit LED Display along with the motor rotation animation and the half track indication.
- It also works as an I2C slave device sending the current track to an external device.
- This particulary version includes also the feature to be used with a LCD 16x2 display as well. 
- Trackduino2 can be easily plugged into the MOS 6522 VIA Chip socket of the 1541 mainboard without any other internal connections, since it already includes the Arduino Nano, acting as an 6522 adapter board. 
- It can be easily removed to get back the Drives stock condition. 

## How it works 
--------------
When the Drive is powered on, the Arduino resets and waits for the Track information given from the read/write head, until then the Track number is still unknown and the Display stays on stand-by.
Once the head reads data from the current track of the inserted disc, the Arduino peeks information from the 6522 VIA chip. Each track contains some data sectors, and each sector has an sector-header. The sector-header contains information about the current track and sector number, which is then driven to the display. 
The data that follows is then read the get the change of track by reading the data for the stepper motor.
## What do you need  
-------------------
 ### Hardware: (You'll find all these components very cheap on Aliexpress)
- My Trackduino Adapter PCB (you can find the Gerber files below) 
- Arduino Nano 
- TM1637 4 digit LED Display 
- 16x2 LCD Display (for LCD version) 
- I2C LCD module (only for LCD version)
- 1x 40pin Turned Pinheader SIL Strip
- 20-30cm 4 pole jumper wire female-female
- 40pin DIL Socket (optional)

-----
### Necessary Software:
- Arduino IDE 1.8 or later versions  https://www.arduino.cc/en/software
- Arduino Libraries: (downloadable from Arduino IDE)
   - DigitalWriteFast 
   - TimerOne
   - TM1637
   - LiquidCrystal_I2C 
   -------------
  ## Assembling the whole stuff
  ------------------
   Just program the Trackduino Sketch into the Arduino Nano and solder it on the adapter PCB using the strip line headers included. 
   Use the following Sketches:  
      "Track_anzeige_05_lcd" for LCD mode, "Track_anzeige_o4_poll" for LED display mode and "Track_anzeige_04_poll_Nano3" only for Arduino Nano3 versions(Atmega168) and only with LED display mode. 
      
Cut the turned 40pin strip line in two 20pin strips and solder them on the bottom side of the adapter. Once done, solder the socket (but you can directly mount the MOS6522 chip on it) on the upper side of the adapter board and the 4 pole header for the display. Connect the Jumper wires from the adapter to the Display, following the pins printed on the silkscreen. You can now connect a mini-USB B cable to the Arduino and you shuld be able to see it working correctly, displaying random tracks, since it's not connected to the Drive, but it's the way it works correctly.
Put the whole adapter into the 6522 socket of you 1541 (U  ) or 1541-II (U  ) Drive and turn it on. 
After initialization, the Display will shoy two horizontal lines, it means it's on stand-by. 
Insert a Floppy Disk and call directory. You'll see the spinning animation and the track number on 18. You're done ! 
