//#include <Python.h>
#define _GNU_SOURCE 1

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <glib0.h>
#include <gobj.h>
#include "c_power_switch.h"
#include "tests.h"

#define CLOCKTYPE CLOCK_MONOTONIC
/* this one should be appropriate to avoid errors on multiprocessors systems */

static inline double ts_diff(struct timespec tsi, struct timespec tsf)
{
    double elaps_s = difftime(tsf.tv_sec, tsi.tv_sec);
    long elaps_ns = tsf.tv_nsec - tsi.tv_nsec;
    return elaps_s + ((double)elaps_ns) / 1.0e9;
}


/****************************************************************************
 *  Trace machine function
 ****************************************************************************/
void perf_gobj(unsigned long cnt)
{
    unsigned long i;
    struct timespec st, et;
    double dt;
    hgobj gobj;

    gobj = gobj_create_yuno(
        "room",         // name
        GCLASS_POWER_SWITCH,    // gclass
        RUN_AS_MAIN_UV_PROCESS,
        0,              // main yuno
        0               // kw
    );

    gobj_enable_trace_machine(1);
    gobj_trace_all_machines(1);
    clock_gettime (CLOCK_MONOTONIC, &st);
    for (i = 0; i < cnt; i++) {
        gobj_send_event(gobj, "EV_PUSHED_BUTTON", 0, 0);
    }
    clock_gettime (CLOCK_MONOTONIC, &et);

    dt = ts_diff (st, et);

    printf ("# test_gobj(%lu): %lfs\n", cnt, dt);
}

