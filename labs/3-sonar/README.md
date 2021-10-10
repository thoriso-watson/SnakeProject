### Measuring distance with a cheap sonar device and cross-checking results.
	
For this lab you're going to implement and a distance measuring device
that uses sonar using the HC-SR04 device.  At a high level it sends a
high frequency pulse (if you have a cat, it may get annoyed) and tells
you when/if it receives it back.  You use the time between pulse and
receive to compute distance using the speed of sound.

<table><tr><td>
  <img src="images/hc-sr04.jpg"/>
</td></tr></table>

### Check-off

For lab:
   1. Use a ruler (we'll have one) to check that your sonar measures distance accurately.
   2. Show that you can handle when your sonar does not hear an echo back.
   3. Compute checksums and compare to everyone else.
   4. Start doing one of the extensions so we have an idea of what's best to do for
      next lab.

----------------------------------------------------------------------
### Part 1: Implementing the HC-SR04 sonar driver.

First, get the datasheet.  The first one is a 3-page sheet that isn't
super helpful (common); there's a longer one if you keep digging.
Also, the sparkfun comments for the device have useful factoids (but
also wrong ones, which --- you guessed it --- is common).

*Note: to save more time for an extension, I checked in a bunch of
datasheets in the `docs/` directory in this lab.***

Again, you're going to be skimming through the data sheet looking
for the information you need.

Second, implement three functions:
 1. `hc_sr04_init(trigger, echo)`: called with the GPIO pins to use for 
    `trigger` and `echo`.  It sets these up and initializes the device.
 2. `hc_sr04_get_distance`: compute the distance in inches using the sonar
     and returns it.
 3. `read_while_eq(pin,v,timeout)`, a helper function that blocks reading `pin`
     until either (1) `gpio_read(pin)` is not equal to `v` or (2) `timeout`
     microseconds have expired.   

     This is a handy function for this lab and later.  In general, when
     we do measurements in the real world the measurement can fail.
     For example, for today's lab, if the sonar pulse never echos back
     (because it got absorbed or because it hit a surface at a sharp
     enough  angle that it ricocheted off) then the naive code will just
     lock up.  `read_while_eq` handles this case by returning an error
     if it times out.

     You should look in `libpi/cs140e-src/timer.c` to see how to handle
     the case that time wraps around.

Some additional hints, you'll need to:

 0. Determine if any pin need to be set as `pulldown`.  If you don't 
    the device will often (maybe always) work, but could easily not.
 1. Initialize the device (pay attention to settling time delays here!).
 2. Do a send (again, pay attention to any needed time delays).
 3. Measure how long it takes to receive the echo and compute round trip
    by converting that time to distance (in inches, very crude) using
    the datasheet formula.  Note, we do not have floating point!  So the
    way you compute this must use integer arithmetic.  Note, too, that
    we don't even have division!

Trouble-shooting.

  0. Sonar uses reflection: if the device is too close to a surface on any side (typically
     its bottom) it can get garbage readings.  If you use a breadboard put the sensor so the emitters
     (the two silver cones) are as close to the edge as you can make them.
  1. Readings can be noisy --- in the real world you may need to require multiple
     high (or low) readings before you decide to trust the signal.  We ignore this problem today.
  2. There are conflicting accounts of what value voltage you need for `Vcc`.
  3. The initial 3-page data sheet you'll find sucks; look for a longer one. 

Expected tests and their results:
  1. If you aim the device where it can't get a reflection, it should return `-1` --- in 
     particular do not deadlock!
  2. If you use a ruler and a hard surface (the meat of your hand is probably not sufficiently
     reflective) you should get reasonable readings.
  3. If you come up with addition tests, please suggest them.

----------------------------------------------------------------------
### Extension 1: use two sonar's to more accurately estimate position.

I have not done this. It could be nonsense.

However, *in theory* you can use the readings from two sonar to accurately
estimate position (just as you do with your eyes).  If you track position
over time, you can get velocity, and predicted movement.  Try doing this!
I'm very curious how accurate this can be made.

----------------------------------------------------------------------
### Extension 2: write a simple event system and pwm multiple LED lights.

This will have some words.