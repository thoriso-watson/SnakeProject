### Overview: interleaving multiple computations.

***This is a too-short description: sorry!***

As you learn more things, it's natural do want to do more than one
at once.  For example you might want to PWM an LED while you do sonar.
Our problem is that we only have one CPU and one thread of control,
so it's not clear how this can work.

The common (naive) view is that you *must* use threads or interrupts
to interleave computation.  Doing so adds alot of complexity.  And,
in fact, may well be impossible depending on their timing requirements.
(For example: if you're using interrupts to PWM an LED but the second
thing you want to do has very tight timing requirements, and cannot be
interrupted, you're out of luck.)

This lab has a simple example of how to interleave two pieces of code. The basic idea is
pretty simple: whenever the first piece of code busy-waits or calls a delay, we run the
second.  This depends on:
  1. The second piece of code finishing "fast enough."
  2. The first bit of code running the second "often enough".

We give you an example interface `code/interleave.h` and an example bit
of code `code/example-ir-send-recv.c` that will send a signal from an
IR led and *simultaneously* receiving this signal on your TSOP.

If you hook up your TSOP to pin 21 and an IR led to pin 20, the code should
"just work." You can then extend it or use it to do other things.
