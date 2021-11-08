#include "rpi.h"
#include "interleave.h"
#include "measure-overheads.h"

static void nop_fn(void * arg) { }

void measure_wait_until_fn(fn_t fn, unsigned budget) {
    th_t th = th_mk(fn, 0, budget);
    uint32_t t = 
       TIME_CYC_DEADLINE("\tchecking indirect routine:", 
        wait_until_ncycles(th, cycle_cnt_read(), budget), budget);
}

void measure_wait_until_overhead(void) {
    th_t nop_th = th_mk(nop_fn, 0, 200);
    uint32_t t = 
       TIME_CYC_DEADLINE("\tcalling with 0 budget: this will give overhead:", 
        wait_until_ncycles(nop_th, cycle_cnt_read(), 0), 200);
}

// wait: this is literally not a deadline.
void measure_fn(fn_t fn, unsigned budget) {
    th_t th = th_mk(fn, 0, budget);
    TIME_CYC_PRINT("\tcost:", run_fn(th));
}

// some simple basic operations.
void measure_primitive_ops(unsigned in, unsigned out) {
    TIME_CYC_PRINT("\tcost :", gpio_read(in));
    TIME_CYC_PRINT("\tcost :", gpio_write(out,0));

    TIME_CYC_PRINT("\tcost :", dev_barrier());
    TIME_CYC_PRINT("\tcost :", dmb());
    TIME_CYC_PRINT("\tcost :", dsb());
    TIME_CYC_PRINT("\tcost cycle_cnt_read:", cycle_cnt_read());
    TIME_CYC_PRINT("\tshould take about 7000 cycles:", delay_us(10));
}
