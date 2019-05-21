//#include <Python.h>
#define _GNU_SOURCE 1

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <glib0.h>
#include "tests.h"

#define CLOCKTYPE CLOCK_MONOTONIC
/* this one should be appropriate to avoid errors on multiprocessors systems */

static inline double ts_diff(struct timespec tsi, struct timespec tsf)
{
    double elaps_s = difftime(tsf.tv_sec, tsi.tv_sec);
    long elaps_ns = tsf.tv_nsec - tsi.tv_nsec;
    return elaps_s + ((double)elaps_ns) / 1.0e9;
}


void perf_glog(int options, unsigned long cnt)
{
    unsigned long i;
    struct timespec st, et;
    double dt;
    const char *fls;

    clock_gettime (CLOCK_MONOTONIC, &st);
    for (i = 0; i < cnt; i++) {
        log_info(1,
            "info-key", "%s", "Hi! info",
            "info-key", "%s", "Hi! info",
            "info-key", "%s", "Hi! info",
            "info-key", "%s", "Hi! info",
            "info-key", "%s", "Hi! info",
            NULL
        );
    }
    clock_gettime (CLOCK_MONOTONIC, &et);

    dt = ts_diff (st, et);

    if (options & LOG_OPT_NODISCOVER)
        fls = "no-discover";
    else if (options & LOG_OPT_NOTIME)
        fls = "no-time";
    else
        fls = "discover";

    printf ("# test_glog(%s, %lu): %lfs\n", fls, cnt, dt);
}

