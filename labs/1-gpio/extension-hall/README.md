### Extension: Detecting magnet contact with a hall effect sensor
	
For this extension  you're going to implement the sensor driver on your own
for a simple a simple magnetic sensing device (a "hall effect" sensor).

<table><tr><td>
  <img src="images/hall.jpg"/>
</td></tr></table>

There's some simple skeleton code, but you're going to do all the work
yourself, including getting the data sheets and anything else you need.
This process will foreshadow what you'll have to do when you write drivers
for devices on your own (with the simplification that in these two cases
we know its possible to get the needed information!).

### Check-off

 1. Show that your hall effect sensor works.
 2. They are really cheap, so hook up 3-4 of them, and use them to control some
    number of LEDs.

----------------------------------------------------------------------
#### Part 1: Implementing code for the hall effect Allegro A1104 sensor

First, find the data sheet.   Google search for 'allegro A1104 datasheet'
or some variation of that.  You're looking for the manufacturer's doc
for what to do to use the sensor.

When you find it, you'll notice it's fairly confusing.  This is the norm.
The game is to skim around ignoring the vast majority of information
(which will be irrelevant to you) and find the few bits of information
you need:

  1. What the pins do (input power, output, ground).
  2. What voltage you need for power (`Vcc`).
  3. What reading you get when the device is closed / signaling (`Vout`).

If any of these values are out of range of the pi, you can't connect
directly.

Now, we write the code.
   1. Copy `3-input.c` to `4-hall.c` and and modify the makefile to 
      compile it.
   2. Write the code to configure the GPIO pins in `4-hall.c`.
   3. Test with a magnet and an LED.

Troubleshooting:
  1. Check wiring.   Check if sensor is reversed.
  2. Where you touch the sensor matters.
  3. The magnet orientation matters.
  4. The input voltage matters.

