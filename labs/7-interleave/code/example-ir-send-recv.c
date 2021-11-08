// try to send and receive.
#include "rpi.h"
#include "interleave.h"
#include "measure-overheads.h"
#include "tsop-xmit.h"

enum { in = 21, out = 20 };

typedef struct {
    enum { RF_INVALID = 0, RF_LO, RF_HI } state; 
    unsigned start;     // cycles
    unsigned end;
    unsigned cnt;
} rf_val_t;

static inline rf_val_t rf_mk(void) 
    { return (rf_val_t){}; }

// returns usec of reading if valid, -1 otherwise.
static inline int rf_read_val(rf_val_t *r) {
    if(r->state == RF_INVALID)
        return -1;
    int v = r->end - r->start;
    assert(v>=0);
    return v;
}

// simple little run to completion routine that 
// attempts to record the number of usec from when
// the pin went lo (RF signal incoming) to hi (no RF).
enum { out_budget = 2000 };
void out_call(void *data) {
    unsigned t = timer_get_usec();
    unsigned v = !gpio_read(in);

    rf_val_t *r = data;
    switch(r->state) {
    case RF_INVALID:
    case RF_LO: 
        // transitioned from lo->hi?
        if(v) {
            r->state = RF_HI;
            r->end = r->start = t;
            r->cnt++;
        }
        break;
    case RF_HI:
        // hi->lo?
        if(!v) {
            r->state = RF_LO;
            r->cnt++;
        }
        r->end = t;
        break;
    default: panic("invalid state %d\n", r->state);
    }
}

// not sure what this is doing.
void tsop_send(rf_val_t *r, unsigned pin, unsigned val) {
    // reset state
    *r = rf_mk();
    th_t th = th_mk(out_call, r, out_budget);

    if(val)
            tsop_send_1(pin, th);
    else
            tsop_send_0(pin, th);
}

void do_measurements();

void notmain(void) {
    // gives more predictable timings: slop has to be doubled.
    if(enable_cache_p)
        enable_cache();

    // tsop receiver
    gpio_set_input(in);
    gpio_set_pullup(in);
    // led transmit.
    gpio_set_output(out);
    gpio_set_pulldown(out);

    do_measurements();

    for(unsigned i = 0; i < 10; i++) {
        unsigned val = i % 2;

        rf_val_t r = rf_mk();
        tsop_send(&r, out, val);
    
        int usec = rf_read_val(&r);
        int v = tsop_val(usec);
        output("received: %d usec, val=%d (expected = %d)\n", usec, v, val);
        assert(v == val);
    }
    output("SUCCESS!\n");
}

static void nop_fn(void *arg) { }

void do_measurements(void) {
    output("----- measuring overheads of different stuff --------------\n");

    output("timing test:\n");
    th_t out_th = th_mk(out_call, 0, out_budget);
    unsigned s = timer_get_usec();
    wait_until_ncycles(out_th, cycle_cnt_read(), 18421);
    unsigned tot_usec = timer_get_usec() - s;
    output("should be around 26: %d\n", tot_usec);
    assert(tot_usec >= 26 && tot_usec <= 28);

    output("----- measuring primitive ops --------------\n");
    measure_primitive_ops(in, out);

    measure_wait_until_overhead();

    output("------------ various nop overheads --------------------\n");
    measure_fn(nop_fn, 200);
    measure_fn(nop_fn, 200);
    measure_wait_until_fn(nop_fn, 200);
    measure_wait_until_fn(nop_fn, 200);
    
    output("------------ various out_call overheads --------------------\n");
    measure_fn(out_call, 2000);
    measure_fn(out_call, 2000);

    measure_wait_until_fn(out_call, 2000);
    measure_wait_until_fn(out_call, 2000);
    output("----------------------------------------------------------\n");
    
}
