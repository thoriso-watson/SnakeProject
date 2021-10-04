We're writing out the prelab.  For the momemt, you should look at:
  
   1. `code` : trivial user-level emulator for your pi code.
      You will use this approach to cross-check your implementation
      against everyone else's to ensure everyone is equivalant.  Thus,
      if one person is correct, everyone is correct.

   3. A quick and dirty `Makefile` [tutorial](http://nuclear.mutantstargoat.com/articles/make/).

If you're going to modify these, please copy the directory --- I'm
still modifying and playing around with the code, so you'd get conflicts
otherwise.

To pop up a few levels, the raw tracing of PUT/GET enables some
interesting tricks:

  1. Check fidelity of your fake-pi emulator:  You can store the PUT/GET
     calls emitted from a raw run to a file (a log) and then change the
     fake pi so it optionally reads from this log: when you replay them
     the result should be identical.  (Including when you log the PUT/GET
     done during the replay, emit that to a log, replay that, etc.)

  2. Partial evaluation: You can also write a program that reads this
     log and emits a pi program that executes just these exact calls ---
     you can use this trick to replace the original program them with this
     (often much simpler) low-level sequence of PUTs and GETs.

