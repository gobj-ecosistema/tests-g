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
#include <locale.h>
#include <jansson.h>
#include <03_json_config.h>
#include <13_json_helper.h>

/***************************************************************************
 *      Constants
 ***************************************************************************/

/***************************************************************************
 *      Structures
 ***************************************************************************/
typedef struct dl_item {
    DL_ITEM_FIELDS

    char temp[32];
    float f;
} dl_item;

/***************************************************************************
 *      Prototypes
 ***************************************************************************/

/***************************************************************************
 *      Data
 ***************************************************************************/
json_t *jn_list;
json_t *jn_dict;
dl_list_t dl_list;

/***************************************************************************
 *
 ***************************************************************************/
static inline double ts_diff (struct timespec start, struct timespec end)
{
    uint64_t s, e;
    s = ((uint64_t)start.tv_sec)*1000000 + ((uint64_t)start.tv_nsec)/1000;
    e = ((uint64_t)end.tv_sec)*1000000 + ((uint64_t)end.tv_nsec)/1000;
    return ((double)(e-s))/1000000;
}

/***************************************************************************
 *
 ***************************************************************************/
static inline void test(int caso, char *scaso,uint64_t cnt)
{
    struct timespec st, et;
    double dt;
    uint64_t i;

    /*-------------------------------------*
     *  Loop
     *-------------------------------------*/

    switch(caso) {
    case 0:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);

            for (i = 0; i < cnt; i++) {
                if(i%2 == 0) {
                    json_t *jn_item = json_sprintf("Merde %d", (int)i);
                    json_array_append_new(jn_list, jn_item);
                } else {
                    json_t *jn_item = json_real(0.5);
                    json_array_append_new(jn_list, jn_item);
                }
            }
            if(json_array_size(jn_list)!=cnt) {
                printf("Merde, bad size\n");
            }

            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;
    case 1:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);

            for (i = 0; i < cnt; i++) {
                json_t *jn_value = json_array_get(jn_list, i);
                if(!jn_value) {
                    printf("Merde, %"PRIu64" no value\n", i);
                }
            }

            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;

    case 2:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);

            json_t *jn_value;
            int idx;
            json_array_foreach(jn_list, idx, jn_value) {
            }

            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;

    case 9:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);

            json_decref(jn_list);
            json_decref(jn_dict);

            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;

    case 10:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);

            for (i = 0; i < cnt; i++) {
                dl_item *item = malloc(sizeof(dl_item));
                if(item) {
                    memset(item, 0, sizeof(dl_item));
                    dl_add(&dl_list, item);
                }
            }
            if(dl_size(&dl_list)!=cnt) {
                printf("Merde, bad size\n");
            }

            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;

    case 11:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);

            for (i = 0; i < cnt; i++) {
                dl_item *item = dl_nfind(&dl_list, (int)i+1);
                if(!item) {
                    printf("Merde, %"PRIu64" no value\n", i);
                }
            }

            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;

    case 12:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);

            dl_item *item = dl_first(&dl_list);
            while(item) {
                item = dl_next(item);
            }

            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;

    case 19:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);

            dl_flush(&dl_list, free);

            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;

    default:
        printf("Merde");
}


    /*-------------------------------------*
     *  Print times
     *-------------------------------------*/
    dt = ts_diff (st, et);

    setlocale(LC_ALL, "");
    printf("\n# test %3d %12s ==> %'10"PRIu64" items, time %f, %'d op/sec\n",
        caso,
        scaso,
        cnt,
        dt,
        (int)(((double)cnt)/dt)
    );
}

/***************************************************************************
 *                      Main
 ***************************************************************************/
int main(int argc, char *argv[])
{
    /*-------------------------------------*
     *  Your start code
     *-------------------------------------*/
    log_startup(
        "test",             // application name
        "1.0.0",            // applicacion version
        "test_glogger"     // executable program, to can trace stack
    );
    log_add_handler("test_stdout", "stdout", LOG_OPT_ALL, 0);

    jn_list = json_array();
    jn_dict = json_object();
    uint64_t cnt = 0;
    if(argc == 1) {
        cnt = 100000LL;
    } else if(argc == 2) {
        cnt = 100000000LL;
    }
    test(0, "Alloc json", cnt);
    test(1, "Get json", cnt);
    test(2, "For json", cnt);
    test(9, "Free json", 1LL);
    test(10, "Alloc dl", cnt);
    test(11, "Get dl", cnt);
    test(12, "For dl", cnt);
    test(19, "Free dl", 1LL);

    /*-------------------------------------*
     *  Your end code
     *-------------------------------------*/
    log_end();

    return 0;
}

