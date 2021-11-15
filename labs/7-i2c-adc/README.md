## Using the I2C protocol to use an analog-to-digital converter (ADC)

So far this quarter we've interacted with digital devices that give a
0 or a 1 as output, possibly with a time delay (e.g., sonar, ws2812b).
Many devices (especially the cheaper, simpler ones) convey information
by outputing a varying voltage instead.  In addition to cost, often an
analog device has a significantly simpler datasheet and is easier to
get working than a digital one.

The downside of the R/PI is that it has no way
to read these directly.  Instead we have to use an
[ADC](https://en.wikipedia.org/wiki/Analog-to-digital_converter) to
convert the voltage to a number.

You're going to do two things:
  1. Write a quick driver for the `ads1115` analog-to-digital converter.
     This will give us a way to get a known signal and see that you 
     are correct.

  2. Use it to get the reading from a potentiometer.  At the lowest setting
     it should be around 1, at the highest around 16k.

  3. Extension: add a microphone or temperature input readings.  

     A good final project is making a pretty equalizer using the ws2812b
     by using the adc to read from a microphone, doing an FFT on the
     data and display the results in a nice way (e.g., seperating the
     bass, mid and treble; changing color based on magnitude and rate
     of change, etc.)

---------------------------------------------------------------------------
### Part0: hook up your potentiometer and ADC.

First off to make sure your basic hardware is working, use the breadboard
to hook up the potentiometer given in class (the little knob) to the
power and ground from the pi and a middle leg controlling an LED.
The LED should go bright and low as you turn the knob.  You'll keep
this setup even when you're using the ADC so that you can quickly check
visually that the ADC readings make sense.

<table><tr><td>
  <img src="images/pot-back.jpg"/>
</td></tr></table>


Next hook up your ADC to the breadboard as well, with:
  - `a0` connected to the middle of the potentiometer (and LED).
  - `g` to ground
  - `v` to 3v.
  - the two i2c pins: `sda` and `scl`.

<table><tr><td>
  <img src="images/pot-adc.jpg"/>
</td></tr></table>

You should be able to run the `staff-binary/ads1115.bin` and get output.

---------------------------------------------------------------------------
### Part 1: write the i2c driver for the adc

We want to known signal to check your i2c.  Using the i2c makes this 
relatively easy.  It also re-enforces that after awhile, datasheets
look very similar and they often aren't that bad.

This code will closely mirror your last lab.  To save time, the 
starter code has some (but not all) page numbers.  There's
an annotated datasheet in `./docs/ads1114.annoted.pdf`).

For this part:
  1. Implement `adc_write16` and `adc_read16` to read and write the 
     16-bit registers on the ads1115 using `i2c_write` and `i2c_read`.
  2. Figure out how to set the configuration register to have 860 samples per
     second and a +/- 4v range.
  3. Make sure your readings make sense.

This shouldn't take that long.  And having an ADC is super useful ---
there's a huge number of cheap analogue sensors and devices you can
now use.

---------------------------------------------------------------------------
### Part 2: use your adc to decode readings from a mic or temperature sensor 

We have both microphones (`docs/max4466-ebay.pdf`) and temperature sensors
in class.  You should get one, download the datasheet, and get make sure
you get some sensible data.

---------------------------------------------------------------------------
### Exension: hook your light strip up to the microphone output.

It's pretty easy to use the ADC to hook it up to your light strip and
display the results.

I'll have a floating-point version of libpi checked in pretty soon.

---------------------------------------------------------------------------
### Exension: implement your own i2c driver.

There are annotated documents in the `docs` directory that decribe

You can flip between our implementation and yours by modifying the `Makefile`.
The annotated broadcom has the information you'l need.

Note:
  1. You'll have to set the pins 2  (`SDA`) and 3 (`SCL`) pins to be used
     for i2c using `gpio_set_function` and the information on page 98.
  2. You'll want to use `dev_barriers()` here.
  3. Use the flags in the i2c fields!  Make sure don't start sending
     when the i2c hardware is still busy or when there is no space on
     the fifo.  Make sure if you read there is data there.  Make sure
     you check if there are errors.

As a first step, do the structure layout so that the field fields pass the
assertions.

Use the `get32_T` and `put32_T` macros as discussed in the prelab.
So your code will look something like:

    c_register c = get32_T(&i2c->control);
        c.clear = 0b11; // shouldn't need, but hygenic(?)
        c.st = 1;
        c.read = read_p;
    put32_T(i2c->control, c);

`i2c_init`:
    1. Setup the pins
    2. Enable the i2c device.
    3. Reset the `done` flag and any errors.
    4. At the end: Assert anything useful.  For example, that a transfer is not active.

On both `i2c_read` and `i2c_write` make sure you start by reseting the
`done` flag and clearing any errors.
