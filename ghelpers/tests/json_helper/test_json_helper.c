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
#include <jansson.h>
#include <03_json_config.h>
#include <13_json_helper.h>

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
 *
 ***************************************************************************/
static inline void test(int flags, uint64_t cnt)
{
    struct timespec st, et, dt;
    uint64_t i;

    /*-------------------------------------*
     *  Your start code
     *-------------------------------------*/
    log_startup(
        "test",             // application name
        "1.0.0",            // applicacion version
        "test_glogger"     // executable program, to can trace stack
    );
    log_add_handler("test_stdout", "stdout", LOG_OPT_ALL, 0);

    /*-------------------------------------*
     *  Loop
     *-------------------------------------*/
    switch(flags) {
    case 0:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);

            json_t *jn_list = json_pack("[s, s, s, i, i, i, s]",
                "c", "b", "a", 1, 0, 2, "1.5"
            );
            print_json(jn_list);
            json_incref(jn_list);
            json_t *jn_sorted = sort_json_list_by_string(jn_list);
            print_json(jn_sorted);
            json_decref(jn_sorted);
            jn_sorted = sort_json_list_by_number(jn_list);
            print_json(jn_sorted);
            json_decref(jn_sorted);

            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;
    case 1:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);

            for (i = 0; i < cnt; i++) {
                json_t *jn_list = json_pack("[s, s, s, i, i, i, s]",
                    "c", "b", "a", 1, 0, 2, "1.5"
                );
//                 print_json(jn_list);
                json_incref(jn_list);
                json_t *jn_sorted = sort_json_list_by_string(jn_list);
//                 print_json(jn_sorted);
                json_decref(jn_sorted);
                jn_sorted = sort_json_list_by_number(jn_list);
//                 print_json(jn_sorted);
                json_decref(jn_sorted);
            }

            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;
    }


    /*-------------------------------------*
     *  Your end code
     *-------------------------------------*/
    log_end();

    /*-------------------------------------*
     *  Print times
     *-------------------------------------*/
    dt = ts_diff (st, et);

    printf("# %-36s (%12" PRIu64 "): %lu.%lus\n",
        "", //mensaje,
        cnt,
        dt.tv_sec, dt.tv_nsec);
}

/***************************************************************************
 *                      Main
 ***************************************************************************/
int main(int argc, char *argv[])
{
    if(argc == 1) {
        test(0, 100000LL);
    } else if(argc == 2) {
        test(1, 1000000LL);
    }
    return 0;
}

