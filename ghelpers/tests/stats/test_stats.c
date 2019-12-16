/****************************************************************************
 *          test.c
 *
 *          Copyright (c) 2015 Niyamaka.
 *          All Rights Reserved.
 ****************************************************************************/
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <20_stats.h>
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
static inline double ts_diff2 (struct timespec start, struct timespec end)
{
    uint64_t s, e;
    s = ((uint64_t)start.tv_sec)*1000000 + ((uint64_t)start.tv_nsec)/1000;
    e = ((uint64_t)end.tv_sec)*1000000 + ((uint64_t)end.tv_nsec)/1000;
    return ((double)(e-s))/1000000;
}

/***************************************************************************
 *
    Added 63072000 metrics; 2,031028 seconds; 31.054.224 op/sec VACIO!
    Added 63072000 metrics; 13,410204 seconds; 4.703.284 op/sec con kw
    Added 63072000 metrics; 23,524317 seconds; 2.681.140 op/sec con kw
    Added 63072000 metrics; 5,635651 seconds; 11.191.608 op/sec sin kw
    Added 63072000 metrics; 6,325518 seconds; 9.971.041 op/sec
    Added 63072000 metrics; 11,299417 seconds; 5.581.880 op/sec
    Added 63072000 metrics; 27,499870 seconds; 2.293.538 op/sec
    Added 63072000 metrics; 302,690851 seconds; 208.371 op/sec
    Added 63072000 metrics; 282,003643 seconds; 223.656 op/sec Con FILE
    Added 63072000 metrics; 282,787915 seconds; 223.036 op/sec
    Added 63072000 metrics; 388,638642 seconds; 162.289 op/sec Con open()
    Added 63072000 metrics; 285,234070 seconds; 221.123 op/sec Con FILE y flush
    Added 63072000 metrics; 142,562835 seconds; 442.415 op/sec con strftime!!!
    Added 63072000 metrics; 98,807086 seconds; 638.334 op/sec con fwrite en vez de fprintf
    Added 63072000 metrics; 161,381560 seconds; 390.825 op/sec con 4 métricas
    Added 63072000 metrics; 214,934838 seconds; 293.447 op/sec con render_name
    Added 63072000 metrics; 217,029063 seconds; 290.615 op/sec
    Added 63072000 metrics; 157,008789 seconds; 401.709 op/sec sacando fuera file_mask
    Added 63072000 metrics; 151,229412 seconds; 417.061 op/sec
    Added 63072000 metrics; 159,903024 seconds; 394.439 op/sec
    Added 63072000 metrics; 188,729432 seconds; 334.192 op/sec con years_in_years
 ***************************************************************************/
PRIVATE int test_stats(json_t *stats)
{
    struct timespec st, et;
    double dt;
    timestamp_t timestamp_from;
    timestamp_t timestamp_to;
    int offset;

    parse_date_basic("1970-01-01T00:00:01Z", &timestamp_from, &offset);
    parse_date_basic("1972-01-01T00:00:01Z", &timestamp_to, &offset);

    clock_gettime (CLOCK_MONOTONIC, &st);

    for(uint64_t i=timestamp_from; i<=timestamp_to; i++) {

        struct tm *tm = gmtime((time_t*)&i);
        //int tm_sec;   /* Seconds. [0-60] (1 leap second) */
        //int tm_min;   /* Minutes. [0-59] */
        //int tm_hour;  /* Hours.   [0-23] */
        //int tm_mday;  /* Day.     [1-31] */
        //int tm_mon;   /* Month.   [0-11] */
        //int tm_year;  /* Year - 1900.  */
        //float v = (tm->tm_mday-1)*100 + tm->tm_hour*10 + tm->tm_min;
        float v = (tm->tm_mday-1)*1000 + tm->tm_hour*100 + tm->tm_min*10 + tm->tm_sec;

        if(1) { // TEST
            if(i && (i % 10000 == 0)) {
                wstats_save(stats);
                wstats_restore(stats);
            }
        }

        wstats_add_value(
            stats,
            "flow_rate",
            i,
            json_real(v)
        );
    }

    clock_gettime (CLOCK_MONOTONIC, &et);
    dt = ts_diff2(st, et);
    setlocale(LC_ALL, "");

    trace_msg("Added %"PRIu64" metrics; %'f seconds; %'lu op/sec\n",
        timestamp_to - timestamp_from,
        dt,
        (unsigned long)(((double)(timestamp_to - timestamp_from))/dt)
    );
    return 0;
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

    static uint32_t mem_list[] = {3731, 0};
    gbmem_trace_alloc_free(0, mem_list);

    #define MEM_MIN_BLOCK   512
    uint64_t MEM_MAX_SYSTEM_MEMORY = free_ram_in_kb() * 1024LL;
    MEM_MAX_SYSTEM_MEMORY /= 100LL;
    MEM_MAX_SYSTEM_MEMORY *= 90LL;  // Coge el 90% de la memoria

    uint64_t MEM_MAX_BLOCK = (MEM_MAX_SYSTEM_MEMORY / sizeof(md_record_t)) * sizeof(md_record_t);
    MEM_MAX_BLOCK = MIN(1*1024*1024*1024LL, MEM_MAX_BLOCK);  // 1*G max

    uint64_t MEM_SUPERBLOCK = MEM_MAX_BLOCK;

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

    char *path = "/yuneta/store/stats/test_stats";
    rmrdir(path);

    json_t *jn_config = json_pack(
        "{s:s, s:s, s:i, s:i, s:i, s:{"
            "s:["
                "{s:s, s:s, s:f, s:s, s:s},"
                "{s:s, s:s, s:f, s:s, s:s},"
                "{s:s, s:s, s:f, s:s, s:s},"
                "{s:s, s:s, s:f, s:s, s:s},"
                "{s:s, s:s, s:f, s:s, s:s}"
            "],"
            "s:[{s:n, s:s, s:I, s:s, s:s}]"
        "}}",
        "path", "/yuneta/store/stats/",
        "group", "test_stats",
        "xpermission", 02770,
        "rpermission", 0660,
        "on_critical_error", 2,
        "metrics",
            "flow_rate",
                "id", "last_week_in_seconds",
                "metric_type", "",
                "value_type", (double)0,
                "filename_mask", "WDAY",
                "time_mask", "SEC",

                "id", "years_in_hours",
                "metric_type", "",
                "value_type", (double)0,
                "filename_mask", "YEAR",
                "time_mask", "HOUR",

                "id", "years_in_days",
                "metric_type", "",
                "value_type", (double)0,
                "filename_mask", "YEAR",
                "time_mask", "YDAY",

                "id", "years_in_months",
                "metric_type", "",
                "value_type", (double)0,
                "filename_mask", "YEAR",
                "time_mask", "MON",

                "id", "years_in_years",
                "metric_type", "",
                "value_type", (double)0,
                "filename_mask", "years",
                "time_mask", "YEAR",

            "velocity",
                "id",
                "metric_type", "",
                "value_type", (json_int_t)0,
                "filename_mask", "WDAY",
                "time_mask", "SEC"
    );

    json_t *stats = wstats_open(jn_config);

    test_stats(stats);

    /*-------------------------------------*
     *  Your end code
     *-------------------------------------*/
    wstats_close(stats);

    gbmem_shutdown();
    end_ghelpers_library();

    return 0;
}

