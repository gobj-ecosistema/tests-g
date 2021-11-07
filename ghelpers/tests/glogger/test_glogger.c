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
#include <10_glogger.h>

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

  EN CASA
  ====================
# test a udp,  log_info/log_debug_printf     (      100000): 0.967024537s
# test a file, log_info/log_debug_printf     (      100000): 0.909249377s
# test a udp, dump 512                 (      100000): 8.936258756s
# test a file, dump 512                (      100000): 8.843534469s

# test a udp, dump 256                 (      100000): 4.738673599s
# test a file, dump 256                (      100000): 4.571734411s

# test a udp, dump 64                  (      100000): 1.420870350s
# test a file, dump 64                 (      100000): 1.365519198s

# test a udp, dump 32                  (      100000): 0.902837215s
# test a file, dump 32                 (      100000): 0.867679965s

DESPUES de modificar tdump

# test a udp, dump 32                  (      100000): 0.717428920s
# test a file, dump 32                 (      100000): 0.667940219s

# test a udp, dump 512                 (      100000): 2.58713829s
# test a file, dump 512                (      100000): 1.954705226s

# test a udp, dump 1500                (      100000): 6.100389326s
# test a file, dump 1500               (      100000): 4.581253871s

EN IB
# test a udp,  log_info/log_debug_printf     (      100000): 1.700266385s
# test a file, log_info/log_debug_printf     (      100000): 1.270764286s
# test a udp, dump 1500                (      100000): 7.503579283s
# test a file, dump 1500               (      100000): 4.679109202s

# test a udp, dump 512                 (      100000): 3.270722683s
# test a file, dump 512                (      100000): 2.73603194s

 ***************************************************************************/
static inline void test(int flags, uint64_t cnt)
{
#define DUMP_SIZE 512

    uint64_t i;
    struct timespec st, et, dt;
    char mensaje[256];

    switch(flags) {
    case 0:
        strcpy(mensaje, "test a udp,  log_info/log_debug_printf");
        break;
    case 1:
        strcpy(mensaje, "test a file, log_info/log_debug_printf");
        break;
    case 2:
        snprintf(mensaje, sizeof(mensaje), "test a udp,  dump %d", DUMP_SIZE);
        break;
    case 3:
        snprintf(mensaje, sizeof(mensaje), "test a file, dump %d", DUMP_SIZE);
        break;
    case 4:
        strcpy(mensaje, "test infinito para chequear la perdida de memoria");
        break;
    }

    /*-------------------------------------*
     *  Your start code
     *-------------------------------------*/
    udpc_start_up(0);
    rotatory_start_up();

    /*-------------------------------------*
     *  Loop
     *-------------------------------------*/
    switch(flags) {
    case 0:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);

            log_startup(
                "test",             // application name
                "1.0.0",            // applicacion version
                "test_glogger"     // executable program, to can trace stack
            );
            udpc_t udpc = udpc_open("udp://127.0.0.1:1000", 0, 0, 0, 0, 0);
            log_add_handler("test_upd", "udp", LOG_OPT_ALL, udpc);

            for (i = 0; i < cnt; i++) {
                log_info(0,
                    "gobj",         "%s", __FILE__,
                    "function",     "%s", __FUNCTION__,
                    "counter",      "%d", (int) i,
                    NULL
                );
                log_debug_printf(
                    "printf con info",
                    "kaka de %s",
                    "vaca"
                );
                log_debug_printf(
                    "",
                    "kaka de %s",
                    "vaca"
                );
                log_debug_printf(
                    0,
                    "kaka de %s",
                    "vaca"
                );
            }
            log_end();

            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;
    case 1:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);

            log_startup(
                "test",             // application name
                "1.0.0",            // applicacion version
                "test_glogger"     // executable program, to can trace stack
            );
            hrotatory_t hr = rotatory_open("/test/pp-ss.log", 0, 0, 0, 0, 0, 0);
            log_add_handler("test_file", "file", LOG_OPT_ALL, hr);

            for (i = 0; i < cnt; i++) {
                log_info(0,
                    "gobj",         "%s", __FILE__,
                    "function",     "%s", __FUNCTION__,
                    "counter",      "%d", (int) i,
                    NULL
                );
                log_debug_printf(
                    "printf con info",
                    "kaka de %s",
                    "vaca"
                );
                log_debug_printf(
                    "",
                    "kaka de %s",
                    "vaca"
                );
                log_debug_printf(
                    0,
                    "kaka de %s",
                    "vaca"
                );
            }
            log_del_handler("test_file");
            log_end();

            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;
    case 2:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);

            log_startup(
                "test",             // application name
                "1.0.0",            // applicacion version
                "test_glogger"     // executable program, to can trace stack
            );
            udpc_t udpc = udpc_open("udp://127.0.0.1:1000", 0, 0, 0, 0, 0);
            log_add_handler("test_upd", "udp", LOG_OPT_ALL, udpc);

            for (i = 0; i < cnt; i++) {
                char bf[DUMP_SIZE] = {0};
                log_debug_dump(0,
                    bf,
                    sizeof(bf),
                    "dump"
                );
            }
            log_end();

            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;
    case 3:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);

            log_startup(
                "test",             // application name
                "1.0.0",            // applicacion version
                "test_glogger"     // executable program, to can trace stack
            );
            hrotatory_t hr = rotatory_open("/test/pp-ss.log", 0, 0, 0, 0, 0, 0);
            log_add_handler("test_file", "file", LOG_OPT_ALL, hr);

            for (i = 0; i < cnt; i++) {
                char bf[DUMP_SIZE] = {0};
                log_debug_dump(0,
                    bf,
                    sizeof(bf),
                    "dump"
                );
            }
            log_del_handler("test_file");
            log_end();

            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;
    case 4:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);

            for (i = 0; i < cnt; i++) {
                log_startup(
                    "test",             // application name
                    "1.0.0",            // applicacion version
                    "test_glogger"     // executable program, to can trace stack
                );
                hrotatory_t hr = rotatory_open("/test/pp-ss.log", 0, 0, 0, 0, 0, 0);
                log_add_handler("test_file", "file", LOG_OPT_ALL, hr);
                udpc_t udpc = udpc_open("udp://127.0.0.1:1000", 0, 0, 0, 0, 0);
                log_add_handler("test_upd", "udp", LOG_OPT_ALL, udpc);
                log_add_handler("test_stdout", "stdout", LOG_OPT_ALL, 0);

                log_info(0,
                    "gobj",         "%s", __FILE__,
                    "function",     "%s", __FUNCTION__,
                    "counter",      "%d", (int) i,
                    NULL
                );

                char bf[256] = {0};
                memset(bf, 'a', sizeof(bf) - 1);
                bf[sizeof(bf) - 1] = 0;
                log_debug_dump(0,
                    bf,
                    sizeof(bf),
                    "dump"
                );
                log_debug_dump(0,
                    bf,
                    sizeof(bf),
                    "dump"
                );
                log_debug_printf(
                    "printf con",
                    "kaka de %s",
                    "vaca"
                );
                log_debug_printf(
                    "",
                    "kaka de %s",
                    "vaca"
                );
                log_debug_printf(
                    0,
                    "kaka de %s",
                    "vaca"
                );
                log_debug_bf(0,
                    bf,
                    strlen(bf),
                    "bf"
                );
                json_t *jn = json_object();
                json_object_set_new(jn, "kk", json_string("vaca"));
                log_debug_json(0, jn, "debug");

                log_monitor(0,
                    "gobj",         "%s", __FILE__,
                    "function",     "%s", __FUNCTION__,
                    "counter",      "%d", (int) i,
                    NULL
                );
                json_decref(jn);

                log_del_handler("test_file");
                log_end();
            }

            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;
    }


    /*-------------------------------------*
     *  Your end code
     *-------------------------------------*/
    udpc_end();
    rotatory_end();

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
 *  test jansson vs json_buffer
 ***************************************************************************/
static inline void test2(int flags, uint64_t cnt)
{
    extern void jsonp_free(void *ptr);

    uint64_t i;
    struct timespec st, et, dt;
    char *mensaje = "";
    clock_gettime (CLOCK_MONOTONIC, &st);

    switch(flags) {
    case 0:
        {
            mensaje = "jansson";
            json_t *jn = json_object();
            for (i = 0; i < cnt; i++) {
                json_object_set_new(jn, "kaka", json_string("vaca"));
                json_object_set_new(jn, "double", json_real(10.00));
                json_object_set_new(jn, "integer", json_integer(10000));
                char *s = json_dumps(jn,0);
                jsonp_free(s);
            }
        }
        break;
    case 1:
        {
            mensaje = "json_buffer";
            hgen_t hgen = json_dict();
            for (i = 0; i < cnt; i++) {
                json_reset(hgen, 0);
                json_add_string(hgen, "kaka", "vaca");
                json_add_double(hgen, "double", 10.00);
                json_add_integer(hgen, "int", 10000);
                char *s = json_get_buf(hgen);
                if(s) ;
            }
        }
        break;
    }

    clock_gettime (CLOCK_MONOTONIC, &et);

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
        test(0, 100000LL);
        test(1, 100000LL);
        test(2, 100000LL);
        test(3, 100000LL);
    } else if(argc == 2) {
        test(0, 1000000LL);
        test(1, 1000000LL);
        test(2, 1000000LL);
        test(3, 1000000LL);

    } else if(argc == 3) {
        test(4, -1LL);
    } else if(argc == 4) {
        test2(0, 1000000LL);
        test2(1, 1000000LL);
    }
    return 0;
}

