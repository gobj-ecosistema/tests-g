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
#include <03_rotatory.h>

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
    uint64_t i;
    struct timespec st, et, dt;
    char mask[256];
    char mensaje[1500+1];
    memset(mensaje, 'a', sizeof(mensaje));
    mensaje[1500] = 0;

    switch(flags) {
    case 0:
        strcpy(mask, "/test/pp-DDMM.log");
        break;
    case 1:
        strcpy(mask, "/test/pp.txt");
        break;
    case 2:
        strcpy(mask, "/test/pp-DDMM.log");
        strcpy(mask, "/test/pp-ss.log");
        //strcpy(mask, "/test/pp-hhmm.log");
        break;
    }

    /*-------------------------------------*
     *  Your start code
     *-------------------------------------*/
    rotatory_start_up();

    /*-------------------------------------*
     *  Loop
     *-------------------------------------*/
    switch(flags) {
    case 0:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);
            hrotatory_t hr = rotatory_open(mask, 0, 0, 0, 0, 0, 0);
            for (i = 0; i < cnt; i++) {
                if(hr) {
                    rotatory_fwrite(hr, LOG_INFO, "%s", mensaje);
                }
            }
            rotatory_close(hr);
            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;
    case 1:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);
            for (i = 0; i < cnt; i++) {
                hrotatory_t hr = rotatory_open(mask, 0, 0, 0, 0, 0, 0);
                if(hr) {
                    rotatory_fwrite(hr, LOG_INFO, "%s", mensaje);
                    rotatory_close(hr);
                }
            }
            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;
    case 2:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);
            hrotatory_t hr = rotatory_open(mask, 0, 0, 0, 0, 0, 0);
            for (i = 0; i < cnt; i++) {
                if(hr) {
                    rotatory_fwrite(hr, LOG_INFO, "%s", mensaje);
                }
            }
            rotatory_close(hr);
            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;
    }


    /*-------------------------------------*
     *  Your end code
     *-------------------------------------*/
    rotatory_end();

    /*-------------------------------------*
     *  Print times
     *-------------------------------------*/
    dt = ts_diff (st, et);

    printf("# test(%12" PRIu64 "): %lu.%lus\n",
          cnt,
          dt.tv_sec, dt.tv_nsec);
}

/***************************************************************************
 *                      Main
 ***************************************************************************/
int main(int argc, char *argv[])
{
    if(argc == 1) {
        test(0, 10LL);
        test(1, 10LL);
    } else if(argc == 2) {
        test(0, 100000LL);
        test(1, 100000LL);

        test(0, 1000000LL);
        test(1, 1000000LL);

    } else if(argc == 3) {
        test(2, -1LL);
    }
    return 0;
}

