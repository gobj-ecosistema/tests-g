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
#include "tests.h"

#define CLOCKTYPE CLOCK_MONOTONIC
/* this one should be appropriate to avoid errors on multiprocessors systems */

static inline double ts_diff(struct timespec tsi, struct timespec tsf)
{
    double elaps_s = difftime(tsf.tv_sec, tsi.tv_sec);
    long elaps_ns = tsf.tv_nsec - tsi.tv_nsec;
    return elaps_s + ((double)elaps_ns) / 1.0e9;
}

/*---------------------------------------------*
 *              Actions
 *---------------------------------------------*/
PRIVATE int trace = 0;

PRIVATE int ac_switch_on(
    void *self, const char *event, json_t *kw, void *src)
{
    if(trace)
        printf("===> switch ON\n");
    return 1;
}
PRIVATE int ac_switch_off(
    void *self, const char *event, json_t *kw, void *src)
{
    if(trace)
        printf("<=== switch OFF\n");
    return 1;
}

/*---------------------------------------------*
 *              FSM
 *---------------------------------------------*/
PRIVATE const char *event_names[] = {
    "EV_PUSHED_BUTTON",
    NULL
};
PRIVATE const char *output_event_list[] = {
    "EV_PUSHED_BUTTON",
    NULL
};
PRIVATE const char *state_names[] = {
    "ST_OFF",
    "ST_ON",
    NULL
};

PRIVATE EV_ACTION st_OFF[] = {
    {"EV_PUSHED_BUTTONX",  ac_switch_on,  "ST_ON"},
    {"EV_PUSHED_BUTTONX",  ac_switch_on,  "ST_ON"},
    {"EV_PUSHED_BUTTON",   ac_switch_on,  "ST_ON"},
    {0,0,0}
};

PRIVATE EV_ACTION st_ON[] = {
    {"EV_PUSHED_BUTTONX",  ac_switch_off,  "ST_OFF"},
    {"EV_PUSHED_BUTTONX",  ac_switch_off,  "ST_OFF"},
    {"EV_PUSHED_BUTTON",   ac_switch_off,  "ST_OFF"},
    {0,0,0}
};

PRIVATE EV_ACTION *states[] = {
    st_OFF,
    st_ON,
    NULL
};

PRIVATE FSM fsm = {
    "PowerSwitch",
    event_names,
    output_event_list,
    state_names,
    states,
};


/****************************************************************************
 *  Trace machine function
 ****************************************************************************/
void perf_smachine(unsigned long cnt)
{
//     unsigned long i;
//     struct timespec st, et;
//     double dt;
//     hmach mach;
//
//     mach = smachine_create(
//         "perf",
//         &fsm,
//         0
//     );
//
//     trace = 0;
//     smachine_set_global_trace(0);
//     clock_gettime (CLOCK_MONOTONIC, &st);
//     for (i = 0; i < cnt; i++) {
//         smachine_inject_event(mach, "EV_PUSHED_BUTTON", 0, 0);
//     }
//     clock_gettime (CLOCK_MONOTONIC, &et);
//
//     dt = ts_diff (st, et);
//
//     printf ("# test_smachine(%lu): %lfs\n", cnt, dt);
}

