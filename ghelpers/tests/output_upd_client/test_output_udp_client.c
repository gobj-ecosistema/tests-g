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
#include <03_output_udp_client.h>

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
    char bf[3000];
    memset(bf, 0, sizeof(bf));
    int len = sizeof(bf);
    switch(flags) {
    case 0:
        len = 1499;
        break;
    case 1:
        len = 1500;
        break;
    case 2:
        len = 3000;
        break;
    }
    memset(bf, 'a', len);

    /*-------------------------------------*
     *  Your start code
     *-------------------------------------*/
    udpc_start_up(0);
    //udpc_t udpc = udpc_open("udp://127.0.0.1:1000", 0, 0, 0);
    //udpc_t udpc = udpc_open("udp://46.105.122.160:1000", 0, 0, 0);


    /*-------------------------------------*
     *  Loop
     *-------------------------------------*/
    switch(flags) {
    case 0:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);
            udpc_t udpc = udpc_open("udp://127.0.0.1:1000", 0, 0, 0, 0, 0);
            for (i = 0; i < cnt; i++) {
                udpc_write(udpc, LOG_INFO, bf, len);
            }
            udpc_close(udpc);
            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;
    case 1:
    case 2:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);
            for (i = 0; i < cnt; i++) {
                udpc_t udpc = udpc_open("udp://127.0.0.1:1000", 0, 0, 0, 0, 0);
                udpc_write(udpc, LOG_INFO, bf, len);
                udpc_close(udpc);
            }
            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;
    }



    /*-------------------------------------*
     *  Your end code
     *-------------------------------------*/
    udpc_end();

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
        test(0, 1LL);
        test(1, 1LL);
        test(2, 1LL);
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

