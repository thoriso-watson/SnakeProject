#ifndef __INTERLEAVE_H__
#define __INTERLEAVE_H__
/* 
 * engler: cs49n.  tl;dr --- an interface:
 *  - wait_until_ncycles
 *  - write_for_ncycles
 * that can be used to interleave independent computations that have 
 * nanosecond granularity, hard-realtime requirements but ***without*** 
 * the use of either interrupts or real threads.  massive simpification.  
 * massively easier to get right.
 *
 *  - current error rate seems to range from 2-30ns  i think can be 
 *    improved some.
 *
 *
 * longer description:
 *
 * given a single, bounded routine <fn> that takes argument <arg> and runs 
 * for at most <budget> cycles, interleave <fn(arg)> with the <wait_until_ncycle>
 * cycle-delay calls used by a second timing critical task.
 *
 * you can view <fn> routine as either a hard-realtime, run-to-completion 
 * thread (in that it does not block) or a hard-realtime event.
 *
 * note: we currently only interlace a single routine.   you could imagine
 * adding a queue of routines.  you could also imagine this adding a 
 * noticeable amount of nanosec.   if you really care about hard-realtime,
 * and want multiple routines a(), b(), ..., IMO it's better to keep the 
 * current interface and write a single routine fn() that calls a(), b(), ...
 * internally.  in a hard-realtime, care-about-nanosec world, you likely 
 * have some application-specific scheduling requirements / aspirations, which 
 * are easily encoded in the if-statements of fn() and, conversely, easily 
 * screwed up by an exogenous scheduler.
 *
 * improvements:
 *  - add a <read_until> version.
 *  - currently pass a <start> everywhere, which makes it easier to do
 *    better accuracy in some cases.   however it does make the code 
 *    more complex: add routines that call internally.
 *  - clever tricks to reduce error further.
 *  - easy trick to reduce error: inling gpio_write
 *  - derive the slop dynamically.
 */
#include "cycle-count.h"

/*
 * configuration options.
 *
 * --- <wait_until_slop> can be dynamically derived if that 
 *     becomes interesting.
 */
enum {
    // = 1 if the code runs with the icache enabled: this changes
    // our overhead calculation.
    enable_cache_p = 1,

    // if cache is enabled overhead seems to be 26 cycles
    // if cache not enabled: about 80
    wait_until_slop = enable_cache_p ? 26 : 80,

    // if = 1: the system checks at runtime if given <fn> exceeds 
    // its alleged bound <budget>.  this check is cheap, but if you want
    // the absolute lowest overhead set to 0 so the compiler removes it.
    check_broken_budget_p = 1,
};

// the routine type.
typedef void (*fn_t)(void *arg);

// a bounded, run-to-completion thread.
//  - <fn(arg)> will complete in at most <budget> cycles.
typedef struct {
    // should we allow them to return a value?  <TH_STOP, TH_AGAIN>?
    const fn_t fn;
    void *arg;
    const uint32_t budget;
} th_t;
static inline th_t th_mk(fn_t fn, void *arg, unsigned budget) {
    return (th_t) { .fn = fn, .arg = arg, .budget = budget };
}

// run <th.fn>, panic if exceeds <th.budget>.
//  - some extra cycle_cnt_read() calls.  maybe should optimize?
static inline void run_fn(th_t th) {
    // no safety net!   slightly faster.
    if(!check_broken_budget_p) {
        th.fn(th.arg);
    // check that <fn> does not exceed <budget>
    } else {
        uint32_t s = cycle_cnt_read();
        th.fn(th.arg);
        uint32_t tot = cycle_cnt_read() - s;
        if(tot > th.budget)
            panic("exceeded budget: had %d, took %d\n", th.budget, tot);
    }
}

// run until <ncycles> have passed since <start>, interleaving
// calls to <th>
static inline uint32_t
wait_until_ncycles(th_t th, unsigned start, unsigned ncycles) {
    // <wait_until_slop> should give how many cycles this routine
    // consumes when nothing happens (i.e., it's overhead).
    // [should just return.]
    if(ncycles >= wait_until_slop)
        ncycles -= wait_until_slop;

    uint32_t n = 0;
    // this might matter for variance:
    //      asm_align(3);
    while(1) {
        n = cycle_cnt_read() - start;
        if(n >= ncycles)
            break;

        if((n + th.budget) < ncycles)
            run_fn(th);
    }
    return n;
}

// set GPIO <pin> = <v> and wait <ncycles> from <start>, 
// interleaving calls to <th>
static inline void
write_for_ncycles_inc(th_t th, unsigned pin, unsigned v,
                    unsigned start, unsigned ncycles) {
    gpio_write(pin, v);     // inline this?
    wait_until_ncycles(th, start, ncycles);
}

static inline void
write_for_ncycles(th_t th, 
    unsigned pin, unsigned v, unsigned ncycles) {
    write_for_ncycles_inc(th, pin, v, 
                cycle_cnt_read(), ncycles);
}

#endif
