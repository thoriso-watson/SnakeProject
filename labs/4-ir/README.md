### Overview: reverse engineering a infrared remote control.

Today we're going to use an IR sensor --- [the Vishay TSOP4838](https://www.newark.com/webapp/wcs/stores/servlet/ProductDisplay?catalogId=15003&productSeoURL=vishay&partNumber=60K6999) to reverse engineer the
key presses from a remote so that you can control your pi using one.

Where stuff is:

  - The datasheet is in `4-ir/docs/tsop4838.pdf`.  You should read
    this to make sure (1) you know which pins to connect to and (2)
    what min and max power they expect.

  - The code is in `4-ir/code/ir-tsop.c`.

  - Amazon lied about delivery times so we only have a 7 remotes.   If
    you have your phone, you should be able to download a "universal remote"
    app and use that.  A cool side-effect is having the infrastructure
    to control your pi from your phone.   

    Note: many (most?) remote apps suck, so it makes sense to poke around
    on reddit or stackoverflow for a simple one.

*Avoid this major, common mistake*:

  - Do not print values *while* recording times, since that messes up
	the timing.  Instead when the IR has been low for "a long time",
	emit all the timings you've seen.


First steps:
  1. Write the code to read from the input pin.  

  2. Remotes communicate by sending values for a speicific amount of
     time.  So to start off record the value sent and the time it was sent.

  3. A key press transition is typically signaled with a "very long"
     on-value.	Then a set of timed off/on pulses.    You should be
     able to see this pattern in the values you print and it should be
     "somewhat" stable.

### Checkoff

Checkoff:
  1. You should pass the few tests.
  2. You should show that pressing four keys on your remote causes your
     pi to print the key value.

--------------------------------------------------------------------
### Background: how an IR remote sends values

On the remotes I used for class (but not this year: we have different
remotes) it looks like they send a 0 or 1 bit by always sending 1 but
for differing amounts of time.   They seperate the bits by sending 0
for a set amount of time.  (You might read up on Manchester encoding.)

For these remotes:

    - skip: send 0 for about 600usec.
    - '0': send '1' for about 600usec.
    - '1': send '1' for about 1600 usec.

So to send 01 they would:

    1. Send a skip (0 for 600 usec).
    2. Send a 0 (1 for 600 usec);
    3. Send a skip (0 for 600 usec);
    4. Send a 1 (1 for 1600usec).
    5. (maybe) send a skip (0 for 600usec).

It also looks like they indicate the start of a transmission by sending
1 for about 40,000 usec and then an initial value.

So given this information you should be able to reverse engineer the
value for each key.
   1. pick up the header.
   2. read the values.
   3. convert the values to 0 and 1, shifting them into an unsigned
  
The main issue is handling error.  We'll just do something simple:
   1. When deciding if a value is a skip, return true if it's within
   a given number of usec (e.g., if its between 400 to 800usec).

   2. When deciding if transmission was a 1 or a 0, pick the value by 
   deciding if its above or below the halfway point between the two 
   values (e.g., if the timing is above (1600+600)/2 then its a 1 and
   is a 0 otherwise).

   3. If you are more clever than this somewhat brain-dead approach,
   be more clever!

--------------------------------------------------------------------
### What to do:

Go through `tsop322.c` and start implementing the code.  It's all in one file
to make it easier to see what is going on.
  1. If you scan through for `unimplemented` you will see the parts to implement.
  2. Start with `get_readings` which will need a modified version of your timeout
     from last time.
  3. Then implement `convert` to produce a 32-bit value from a set of readings.
  4. Then figure out what value each key on your remote gives and implement
     `key_to_str`.
  5. There are some helper functions that hopefully are self explanatory.

--------------------------------------------------------------------
### Extension: make a simple network.

We have emitters as well as receivers.  You should be able to send bits of information
from one pi to another using them.  First try on your own pi.

--------------------------------------------------------------------
### Extension: control a light

Now that you have an IR sensor, use it to control whether an LED goes more or less
bright.  Since we don't have interrupts, you'll have to PWM the LED
during the "wait" time in your timeout code.
