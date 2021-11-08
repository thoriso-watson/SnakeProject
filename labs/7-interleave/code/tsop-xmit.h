#ifndef __TSOP_XMIT_H__
#define __TSOP_XMIT_H__


// can either use usec or cycles.
// 38khz cycles --- so full transition up and down 38*1000 times per sec
//
// usec = (1000*1000usec) / (38*1000) = 26.31
// cycles = (700*1000*1000) / (38*1000) = 18421.0 / 2 = 9210
//
// we go with cycles just b/c
enum { tsop_hold_cyc = 9210, };

// send a 38khz signal to the tsop by flashing the led attached to 
// <pin> for <usec>, running <th> during the delays.
//
static inline void tsop_write_cyc(unsigned pin, unsigned usec, th_t th) {
    // XXX: we should check that we aren't called "too soon" and spin if so.
    unsigned start = cycle_cnt_read();
    unsigned n = 0;

    unsigned end = usec_to_cycles(usec);

    // tsop expects a 38khz signal --- this means 38,000 transitions
    // between hi (1) and lo (0) per second.
    //
    // common mistake: setting to hi or lo 38k times per sec rather 
    // than both hi+low (which is 2x as frequent).
    //
    // we use cycles rather than usec for increased precision, but 
    // at the rate the IR works, usec is likely fine.
    do {

        write_for_ncycles_inc(th, pin, 1, start, n += tsop_hold_cyc);
        write_for_ncycles_inc(th, pin, 0, start, n += tsop_hold_cyc);

    } while((cycle_cnt_read() - start) < end);
}

// our semi-arbitrary definitions for on and off --- the main limit
// is that they cannot be "too short".   see the datasheet for what
// this is.
enum { 
    // these won't be the exact observed values coming out.
    // what to do:
    //  1. tune the wait code to give as low error as you can
    //  2. can subtract a slop amount off of these: this is not
    //     ideal b/c if the <write_for_ncycle> improves it will
    //     be off.
    //  3. more fun: assuming there is low variance, you can derive 
    //     the amount of time at runtime*
    on_usec = 1200*2,       // 1: RF picks up 1 for 2400usec.
    off_usec = 600*2,       // 0: RF picks up 1 for 1200usec
    gap_usec = 600          // LED has to be down for at least 600usec
};

// turn led off for <gap_usec> --- the datasheet states there must
// be "long enough" 0 gaps b/n the signals.
//
// one improvement: change <tsop_write_cyc> so it just delays til the
// next time.
static void tsop_turn_off(unsigned pin, th_t th) {
    write_for_ncycles(th, pin, 0, usec_to_cycles(gap_usec));
}

// send a 0 by flashing the LED for <off_usec> microsec.
// <th> is invoked during the deadtime.
static void tsop_send_0(unsigned pin, th_t th) 
    { tsop_write_cyc(pin, off_usec, th); }

// send a 1 by flashing the LED for <off_usec> microsec.
// <th> is invoked during the deadtime.
static void tsop_send_1(unsigned pin, th_t th) 
    { tsop_write_cyc(pin, on_usec, th); }


static int tsop_val(unsigned usec) {
    assert(on_usec> off_usec);
    unsigned mid = (on_usec + off_usec) / 2;
    return (usec > mid) ? 1 : 0;
}
#endif
