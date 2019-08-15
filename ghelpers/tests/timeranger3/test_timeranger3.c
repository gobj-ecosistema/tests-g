/****************************************************************************
 *          test.c
 *
 *          Copyright (c) 2015 Niyamaka.
 *          All Rights Reserved.
 ****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <time.h>
#include <sys/resource.h>
#include <30_timeranger.h>
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
BOOL pinta_rows = 0;

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
int leidos = 0;

/***************************************************************************
 *
 ***************************************************************************/
static void test(json_t *tranger, const char *topic_name, int flags, uint64_t cnt)
{
    uint64_t i;
    struct timespec st, et;
    double dt;
    #define MAX_RECS 10

    /*-------------------------------------*
     *  Loop
     *-------------------------------------*/
    switch(flags) {
    case 0:
        {
            /*-------------------------------------*
             *      Add records
             *-------------------------------------*/
            printf("Case %d: tranger_append_record %'lu records\n", flags, (unsigned long)cnt);
            clock_gettime (CLOCK_MONOTONIC, &st);
            for(i=0; i<cnt/2; i++) {
                if(i % 2 == 0) {
                    json_t *jn_record1 = json_pack("{s:I, s:s}",
                        "id", (json_int_t)(i+1),
                        "content",
                        "Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el."
                        "Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el."
                        "Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el.x"
                    );
                    md_record_t md_record;
                    tranger_append_record(tranger, topic_name, 0, 0, &md_record, jn_record1);
                } else {
                    json_t *jn_record1 = json_pack("{s:I, s:s}",
                        "id", (json_int_t)(i+1),
                        "content",
                        "Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el."
                        "Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el alfa.Pepe el.x"
                    );
                    md_record_t md_record;
                    tranger_append_record(tranger, topic_name, 0, 0, &md_record, jn_record1);
                }
                //sleep(1);
            }

            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;
    default:
        printf("Merde\n");
    }

    /*-------------------------------------*
     *  Print times
     *-------------------------------------*/
    dt = ts_diff (st, et);

    printf("# test(%12" PRIu64 "): %f, %'ld op/sec\n\n",
        cnt,
        dt,
        (long)(((double)cnt)/dt)
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
    log_add_handler("test_stdout", "stdout", LOG_OPT_LOGGER|LOG_HND_OPT_TRACE_STACK, 0);

    static uint32_t mem_list[] = {0, 0};
    // You also need link with ghelpersd library
    gbmem_trace_alloc_free(1, mem_list);

    #define MEM_MIN_BLOCK   512
    uint64_t MEM_MAX_SYSTEM_MEMORY = free_ram_in_kb() * 1024LL;
    MEM_MAX_SYSTEM_MEMORY /= 100LL;
    MEM_MAX_SYSTEM_MEMORY *= 90LL;  // Coge el 90% de la memoria

    uint64_t MEM_MAX_BLOCK = (MEM_MAX_SYSTEM_MEMORY / sizeof(md_record_t)) * sizeof(md_record_t);
    MEM_MAX_BLOCK = MIN(1*1024*1024*1024LL, MEM_MAX_BLOCK);  // 1*G max

    uint64_t MEM_SUPERBLOCK = MEM_MAX_BLOCK;

    setlocale(LC_ALL, "");

    struct rlimit limit;
    getrlimit(RLIMIT_NOFILE, &limit);
    limit.rlim_cur = 25;
    if(setrlimit( RLIMIT_NOFILE, &limit)<0) {
        log_error(0,
            "gobj",         "%s", __FILE__,
            "function",     "%s", __FUNCTION__,
            "msgset",       "%s", MSGSET_SYSTEM_ERROR,
            "msg",          "%s", "setrlimit() FAILED",
            "errno",        "%d", errno,
            "strerror",     "%s", strerror(errno),
            NULL
        );
    }
    if(getrlimit( RLIMIT_NOFILE, &limit)<0) {
        log_error(0,
            "gobj",         "%s", __FILE__,
            "function",     "%s", __FUNCTION__,
            "msgset",       "%s", MSGSET_SYSTEM_ERROR,
            "msg",          "%s", "getrlimit() FAILED",
            "errno",        "%d", errno,
            "strerror",     "%s", strerror(errno),
            NULL
        );
    } else {
        log_info(0,
            "gobj",         "%s", __FILE__,
            "function",     "%s", __FUNCTION__,
            "msgset",       "%s", MSGSET_INFO,
            "msg",          "%s", "rlimits",
            "rlim_cur",     "%d", (int)limit.rlim_cur,
            "rlim_max",     "%d", (int)limit.rlim_max,
            NULL
        );
    }

    if(0) {
        gbmem_startup(
            MEM_MIN_BLOCK,
            MEM_MAX_BLOCK,
            MEM_SUPERBLOCK,
            MEM_MAX_SYSTEM_MEMORY,
            NULL,
            0
        );
    } else {
        gbmem_startup_system(
            MEM_MAX_BLOCK,
            MEM_MAX_SYSTEM_MEMORY
        );
    }

    /*
     *  WARNING now all json is gbmem allocated
     */
    json_set_alloc_funcs(
        gbmem_malloc,
        gbmem_free
    );
    uv_replace_allocator(
        gbmem_malloc,
        gbmem_realloc,
        gbmem_calloc,
        gbmem_free
    );

    char *path = "/test/timeranger3";
    rmrdir(path);
    printf("\n============> Creating '%s' tranger db\n", path);

    json_t *jn_tranger = json_pack("{s:s, s:s, s:s, s:b}",
        "path", path,
        "database", "database",
        "filename_mask", "%M-%S",
        "master", 1
    );
    json_t *tranger = tranger_startup(jn_tranger);

    if(tranger_open_topic(tranger, "topic", FALSE)) {
        tranger_delete_topic(tranger, "topic");
    }

    tranger_create_topic(
        tranger,
        "topic",    // topic name
        "id",       // pkey
        "",         // tkey
        0,          // system_latch
        json_pack("{s:s, s:s}", // jn_cols, owned
            "id", "",
            "address", ""
        )
    );
    tranger_close_topic(tranger, "topic");

    unsigned long count = 0;
    if(argc == 1) {
        count = 100000;
    } else if(argc == 2) {
        count = 1000000;
    } else if(argc == 3) {
        count = 10000000;
    } else if(argc == 4) {
        count = 100000000;
    }

    test(tranger, "topic", 0, count);
    printf("====> mem %'lu\n", gbmem_mem_in_use());

    /*-------------------------------------*
     *  Your end code
     *-------------------------------------*/
    tranger_shutdown(tranger);
    gbmem_shutdown();
    end_ghelpers_library();

    return 0;
}

