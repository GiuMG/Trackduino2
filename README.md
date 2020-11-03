------
# Trackduino2
Trackduino is Track display-reader for Commodore 1541 Drives. It's based on an original project from oe7twj (don't know his real name) 
-------
## What does it do ? 
- It is able to read the Track position of the read/write head and display it on a 4 digit LED Display along with the motor rotation animation and the half track indication.
- It also works as an I2C slave device sending the current track to an external device.
- This particularly version includes also the feature to be used with a LCD 16x2 display as well. 
- Trackduino2 can be easily plugged into the MOS 6522 VIA Chip socket of the 1541 mainboard without any other internal connections or modifications, since it acts as an 6522 adapter board including the Arduino Nano.
- It can be easily removed to get back the Drives stock condition. 

-----
## How it works 
When the Drive is powered on, the Arduino resets and waits for the Track information given from the read/write head, until then the Track number is still unknown and the Display stays on stand-by.
Once the head reads data from the current track of the inserted disc, the Arduino peeks information from the 6522 VIA chip. Each track contains some data sectors, and each sector has an sector-header. The sector-header contains information about the current track and sector number, which is then driven to the display. 
The data that follows is then read the get the change of track by reading the data for the stepper motor.
## What do you need  

-----
 ### Hardware: (You'll find all these components very cheap on Aliexpress)
- My Trackduino Adapter PCB (you can find the Gerber files below) 

![PCB_top](https://user-images.githubusercontent.com/64856480/98012570-8c218000-1df9-11eb-8c60-df49d0f7fd19.png)

- Arduino Nano 
- TM1637 4 digit LED Display 
- 16x2 LCD Display (for LCD version) 
- I2C LCD module (only for LCD version)
- 1x 40pin Turned Pinheader SIL Strip
- 20-30cm 4 pole jumper wire female-female
- 40pin DIL Socket (optional)

 ### Necessary Software:
- Arduino IDE 1.8 or later versions  https://www.arduino.cc/en/software
- Arduino Libraries: (downloadable from Arduino IDE)
- DigitalWriteFast 
- TimerOne
- TM1637
- LiquidCrystal_I2C 
  
------
## Assembling the whole stuff
Just program the Trackduino Sketch into the Arduino Nano and solder it on the adapter PCB using the strip line headers included. 
Use the following Sketches:  
"Track_anzeige_05_lcd" for LCD mode, "Track_anzeige_04_poll" for LED display mode and "Track_anzeige_04_poll_Nano3" only for Arduino Nano3 versions(Atmega168) 
    and only with LED display. 
Cut the turned 40pin strip line in two 20pin strips and solder them on the bottom side of the adapter. Once done, solder the socket (but you can directly mount the MOS6522 chip on it) on the upper side of the adapter board and the 4 pole header for the display. Connect the Jumper wires from the adapter to the Display, following the pin signals printed on the silkscreen.

![Trackduino_Led](https://user-images.githubusercontent.com/64856480/98007882-372f3b00-1df4-11eb-935e-172dbc5e25f2.JPG)

You can now check if the Arduino works correcty just plugging in a mini-USB B cable. The display will initializze and short after showing random tracks, since it's not connected to the Drive, but this is ok, it means it does work properly. 

-----
## Final Installation 
Put the whole adapter into the 6522 socket of you 1541 (UC2) or 1541-II (U8) Drive and turn it on. 
After power on init, the Display will now show 2 horizontal lines which means it is waiting for data being read. 
![IMG_1308](https://user-images.githubusercontent.com/64856480/98009658-17991200-1df6-11eb-8337-33479454991c.JPG)
Insert a Floppy Disk and call the directory on your C64. You should be able to see the spinning animation and the track number 18.

![Install_1541](https://user-images.githubusercontent.com/64856480/98008239-97be7800-1df4-11eb-98c0-5ccf88a54802.JPG)

For now on it will display each track read by the head.
You're done ! 
![874B7316-8C54-4DD7-89A9-5FF7E9D7A396](https://user-images.githubusercontent.com/64856480/98009782-3bf4ee80-1df6-11eb-8d7c-7a40bb7fe68c.JPG)

