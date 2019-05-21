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


void perf_gbmem(int options, unsigned long cnt)
{
    unsigned long i;
    struct timespec st, et;
    double dt;
    const char *fls;

    if(options) {
        gbmem_startup( /* Create memory core */
            32,                /* smaller memory block */
            2*1024,            /* largest memory block */
            32*1024L,          /* super-block size */
            2*1024L*1024L,     /* maximum core memory */
            NULL, //&c_fns              /* system memory functions */
            0
        );
        //gbmem_trace_alloc_free(TRUE);
    }

    clock_gettime (CLOCK_MONOTONIC, &st);

    for(i=0; i<cnt; i++) {
        PTR p;
        size_t size;

        //size = i % 1024;
        size = i % (2*1024);
        if(size == 0)
            size = 1;
        else if(size > 2*1024 - 2)
            size = 2*1024 - 2;
        //printf("size %ld\n", size);
        if (options) {
            p = gbmem_malloc(size);
            if(!p) {
                printf("ERROR GBMEM size=%ld\n", size);
            } else {
                gbmem_free(p);
            }
        } else {
            p = calloc(1, size);
            if(!p) {
                printf("ERROR CALLOC %ld\n", size);
            } else {
                free(p);
            }
        }
    }

    clock_gettime (CLOCK_MONOTONIC, &et);

    if(options)
        gbmem_shutdown();   /* Close memory core */

    dt = ts_diff (st, et);

    if (options)
        fls = "gbmem ";
    else
        fls = "malloc";

    printf ("# test_gbmem(%s, %lu): %lfs\n", fls, cnt, dt);
}
