/****************************************************************************
 *          test.c
 *
 *          Copyright (c) 2015 Niyamaka.
 *          All Rights Reserved.
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <ghelpers.h>

/***************************************************************************
 *      Constants
 ***************************************************************************/

/***************************************************************************
 *      Structures
 ***************************************************************************/

/***************************************************************************
 *      Prototypes
 ***************************************************************************/

/***************************************************************************
 *      Data
 ***************************************************************************/

/***************************************************************************
 *
 ***************************************************************************/
static inline struct timespec ts_diff (struct timespec start, struct timespec end)
{
    struct timespec temp;
    if ((end.tv_nsec - start.tv_nsec) < 0) {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}

/***************************************************************************


 ***************************************************************************/
static inline void test(int flags, uint64_t cnt)
{
#define DUMP_SIZE 512

    struct timespec st, et, dt;
    char mensaje[256];

    switch(flags) {
    case 0:
    default:
        strcpy(mensaje, "test");
        break;
    }

    /*-------------------------------------*
     *  Your start code
     *-------------------------------------*/
    log_startup(
        "test",             // application name
        "1.0.0",            // applicacion version
        "test_glogger"     // executable program, to can trace stack
    );
    log_add_handler("test_stdout", "stdout", LOG_OPT_LOGGER|LOG_HND_OPT_TRACE_STACK, 0);


    /*-------------------------------------*
     *  Loop
     *-------------------------------------*/
    switch(flags) {
    case 0:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);

            for(int i=0; i<cnt; i++) {
                GBUFFER *gbuf = gbuf_create(15, 600, 0, 0);
                gbuf_printf(gbuf, "1234567890\n");
                if(0) log_debug_gbuf(0, gbuf, 0);
                gbuf_printf(gbuf, "1234567890\n");
                if(0) log_debug_gbuf(0, gbuf, 0);
                gbuf_printf(gbuf, "1234567890123456789012345678901234567890123456789012345678901234567890\n");
                if(0) log_debug_gbuf(0, gbuf, 0);
                gbuf_printf(gbuf, "1234567890123456789012345678901234567890123456789012345678901234567890\n");
                if(0) log_debug_gbuf(0, gbuf, 0);
                gbuf_printf(gbuf, "1234567890123456789012345678901234567890123456789012345678901234567890\n");
                if(0) log_debug_gbuf(0, gbuf, 0);
                if(0) {
                    char *bf = gbuf_cur_rd_pointer(gbuf);
                    printf("%s", bf);
                }
                gbuf_decref(gbuf);
            }

            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;

    case 1:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);

            GBUFFER *unmasked;

            unmasked = gbuf_create(8, 32, 0, 0);
            char temp[4]={'a','b','c','d'};
            //char h_mask[4]={1,2,3,4};
            char *p;
            size_t ln = 32;
            for(size_t i=0; i<ln; i++) {
                p = &temp[i % 4];
                if(p) {
//                     *p = (*p) ^ h_mask[i % 4];
                    gbuf_append(unmasked, "x", 1);
                    log_debug_gbuf(0, unmasked, 0);
                } else {
                    log_error(0,
                        "gobj",         "%s",__FILE__,
                        "function",     "%s", __FUNCTION__,
                        "msgset",       "%s", MSGSET_INTERNAL_ERROR,
                        "msg",          "%s", "gbuf_get() return NULL",
                        NULL
                    );
                }
            }
            gbuf_decref(unmasked);

            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;

    }


    /*-------------------------------------*
     *  Your end code
     *-------------------------------------*/

    /*-------------------------------------*
     *  Print times
     *-------------------------------------*/
    dt = ts_diff (st, et);

    printf("# %-36s (%12" PRIu64 "): %lu.%lus\n",
        mensaje,
        cnt,
        dt.tv_sec, dt.tv_nsec);
}


/***************************************************************************
 *                      Main
 ***************************************************************************/
int main(int argc, char *argv[])
{
    if(argc == 1) {
        test(0, 1000000LL);
    } else if(argc == 2) {
        test(1, 1LL);

    } else if(argc == 3) {
        test(0, -1LL);
    }
    return 0;
}

