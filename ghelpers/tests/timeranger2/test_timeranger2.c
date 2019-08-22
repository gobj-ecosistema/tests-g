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
#include <time.h>
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
static int test_result(json_t * list, uint64_t *result, int max)
{
    int ret = 0;

    int count;
    json_t *jn_record;
    json_array_foreach(list, count, jn_record) {
        if(count >= max) {
            ret = -1;
            printf("ERROR count >= max, count %d, max %d\n", count, max);
        }
        json_int_t rowid = kw_get_int(jn_record, "__md_tranger__`__rowid__", 0, KW_REQUIRED);
        if(result[count] != rowid) {
            ret = -1;
            printf("ERROR count rowid not match, count %d, wait rowid %d, found rowid %d\n",
                count,
                (int)(result[count]),
                (int)(rowid)
            );
        }
    }

    if(count != max) {
        ret = -1;
        printf("ERROR count != max, count %d, max %d\n", count, max);
    }
    if(json_array_size(list) != max) {
        ret = -1;
        printf("ERROR array_size != max, array_size %d, max %d\n", (int)json_array_size(list), max);
    }
    return ret;
}

/***************************************************************************
 *
 ***************************************************************************/
int leidos = 0;
int load_all_records_callback(
    json_t *tranger,
    json_t *list,
    md_record_t *md_record,
    json_t *jn_record
)
{
    leidos++;
    if(leidos != md_record->__rowid__) {
        printf("ERROR en rowid, leidos %'d, rowid %'d\n", leidos, (int)md_record->__rowid__);
        //exit(-1);
    }
    JSON_DECREF(jn_record);
    return 0;
}

int load_rango_callback(
    json_t *tranger,
    json_t *list,
    md_record_t *md_record,
    json_t *jn_record
)
{
    leidos++;
    if(pinta_rows) {
        printf("rowid = %"PRIu64"\n", (uint64_t)md_record->__rowid__);
    }
    JSON_DECREF(jn_record);
    return 1; // add to returned list
}

int load_record_callback3(
    json_t *tranger,
    json_t *topic,
    json_t *list,
    md_record_t *md_record,
    json_t *jn_record
)
{
    leidos++;
    json_int_t id = kw_get_int((json_t *)jn_record, "id", 0, 0);
    int id2 = md_record->key.i;
    if(pinta_rows) {
        printf("Leo %'d, rowid %'d, id %'d, id %'d\n",
            leidos,
            (int)md_record->__rowid__,
            (int)id,
            (int)id2
        );
    }
    JSON_DECREF(jn_record);
    return 1; // add to returned list
}

/***************************************************************************
 *
 ***************************************************************************/
static void test(json_t *tranger, const char *topic_name, int flags, uint64_t cnt)
{
    uint64_t i;
    struct timespec st, et;
    double dt;
    static uint64_t appends = 0;
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
            appends = cnt/2;
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
            }

            tranger_backup_topic(
                tranger,
                topic_name,
                "",
                "",
                TRUE,
                0
            );

            for(; i<cnt; i++) {
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
            }

            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;
    case 4:
        {
            /*-------------------------------------*
             *  Busca relative foreward from_rowid
             *-------------------------------------*/
            json_int_t from_rowid = -10;
            printf("Case %d: tranger_open_list, relative, foreward, from_rowid %ld, to_rowid\n",
                flags, (unsigned long)from_rowid
            );
            clock_gettime (CLOCK_MONOTONIC, &st);

            leidos = 0;
            json_t *match_record = json_pack("{s:I}",
                "from_rowid", (json_int_t)from_rowid
            );

            json_t *jn_list = json_pack("{s:s, s:o, s:I}",
                "topic_name", topic_name,
                "match_cond", match_record,
                "load_record_callback", (json_int_t)(size_t)load_rango_callback
            );
            json_t *tr_list = tranger_open_list(
                tranger,
                jn_list
            );

            uint64_t result[MAX_RECS];
            int max = sizeof(result)/sizeof(result[0]);
            for(int ii=0; ii<max; ii++) result[ii] = appends - MAX_RECS + ii + 1;

            test_result(kw_get_list(tr_list, "data", 0, KW_REQUIRED), result, max);

            tranger_close_list(
                tranger,
                tr_list
            );

            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;
    case 5:
        {
            /*-------------------------------------*
             *  Busca relative backward from_rowid
             *-------------------------------------*/
            json_int_t from_rowid = -10;
            printf("Case %d: tranger_open_list, relative, backward, from_rowid %ld, to_rowid\n",
                flags, (unsigned long)from_rowid
            );
            clock_gettime (CLOCK_MONOTONIC, &st);

            leidos = 0;
            json_t *match_record = json_pack("{s:b, s:I}",
                "backward", 1,
                "from_rowid", (json_int_t)from_rowid
            );
            json_t *jn_list = json_pack("{s:s, s:o, s:I}",
                "topic_name", topic_name,
                "match_cond", match_record,
                "load_record_callback", (json_int_t)(size_t)load_rango_callback
            );
            json_t *tr_list = tranger_open_list(
                tranger,
                jn_list
            );

            uint64_t result[MAX_RECS];
            int max = sizeof(result)/sizeof(result[0]);
            for(int ii=0; ii<max; ii++) result[ii] = appends-ii;

            test_result(kw_get_list(tr_list, "data", 0, KW_REQUIRED), result, max);

            tranger_close_list(
                tranger,
                tr_list
            );

            clock_gettime (CLOCK_MONOTONIC, &et);

        }
        break;
    case 6:
        {
            /*------------------------------------------------*
             *  Busca relative foreward from_rowid, to_rowid
             *------------------------------------------------*/
            json_int_t from_rowid = -20;
            json_int_t to_rowid = -10;
            printf("Case %d: tranger_open_list, relative, foreward, from_rowid %ld, to_rowid %ld\n",
                flags, (unsigned long)from_rowid, (unsigned long)to_rowid
            );
            clock_gettime (CLOCK_MONOTONIC, &st);

            leidos = 0;
            json_t *match_record = json_pack("{s:I, s:I}",
                "from_rowid", (json_int_t)from_rowid,
                "to_rowid", (json_int_t)to_rowid
            );

            json_t *jn_list = json_pack("{s:s, s:o, s:I}",
                "topic_name", topic_name,
                "match_cond", match_record,
                "load_record_callback", (json_int_t)(size_t)load_rango_callback
            );
            json_t *tr_list = tranger_open_list(
                tranger,
                jn_list
            );

            uint64_t result[MAX_RECS];
            int max = sizeof(result)/sizeof(result[0]);
            for(int ii=0; ii<max; ii++) result[ii] = appends + from_rowid + ii + 1;

            test_result(kw_get_list(tr_list, "data", 0, KW_REQUIRED), result, max);

            tranger_close_list(
                tranger,
                tr_list
            );

            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;
    case 7:
        {
            /*------------------------------------------------*
             *  Busca relative backward from_rowid, to_rowid
             *------------------------------------------------*/
            json_int_t from_rowid = -20;
            json_int_t to_rowid = -10;
            printf("Case %d: tranger_open_list, relative, backward, from_rowid %ld, to_rowid %ld\n",
                flags, (unsigned long)from_rowid, (unsigned long)to_rowid
            );
            clock_gettime (CLOCK_MONOTONIC, &st);

            leidos = 0;
            json_t *match_record = json_pack("{s:b, s:I, s:I}",
                "backward", 1,
                "from_rowid", (json_int_t)from_rowid,
                "to_rowid", (json_int_t)to_rowid
            );
            json_t *jn_list = json_pack("{s:s, s:o, s:I}",
                "topic_name", topic_name,
                "match_cond", match_record,
                "load_record_callback", (json_int_t)(size_t)load_rango_callback
            );
            json_t *tr_list = tranger_open_list(
                tranger,
                jn_list
            );

            uint64_t result[MAX_RECS];
            int max = sizeof(result)/sizeof(result[0]);
            for(int ii=0; ii<max; ii++) result[ii] = appends + to_rowid - ii;

            test_result(kw_get_list(tr_list, "data", 0, KW_REQUIRED), result, max);

            tranger_close_list(
                tranger,
                tr_list
            );

            clock_gettime (CLOCK_MONOTONIC, &et);

        }
        break;
    case 8:
        {
            /*------------------------------------------------*
             *  Busca relative from_rowid, to_rowid
             *------------------------------------------------*/
            json_int_t from_rowid = -10;
            json_int_t to_rowid = -20;
            printf("Case %d: tranger_open_list, relative, backward, from_rowid %ld, to_rowid %ld\n",
                flags, (unsigned long)from_rowid, (unsigned long)to_rowid
            );
            clock_gettime (CLOCK_MONOTONIC, &st);

            leidos = 0;
            json_t *match_record = json_pack("{s:I, s:I}",
                "from_rowid", (json_int_t)from_rowid,
                "to_rowid", (json_int_t)to_rowid
            );
            json_t *jn_list = json_pack("{s:s, s:o, s:I}",
                "topic_name", topic_name,
                "match_cond", match_record,
                "load_record_callback", (json_int_t)(size_t)load_rango_callback
            );
            json_t *tr_list = tranger_open_list(
                tranger,
                jn_list
            );

            uint64_t result[1] = {0};
            int max = 0;

            test_result(kw_get_list(tr_list, "data", 0, KW_REQUIRED), result, max);

            tranger_close_list(
                tranger,
                tr_list
            );

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

    char *path = "/test/timeranger2";
    //rmrdir(path);
    printf("\n============> Creating '%s' tranger db\n", path);

    json_t *jn_tranger = json_pack("{s:s, s:s, s:b}",
        "path", path,
        "database", "database",
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
    test(tranger, "topic", 4, count);
    test(tranger, "topic", 5, count);
    test(tranger, "topic", 6, count);
    test(tranger, "topic", 7, count);
    test(tranger, "topic", 8, count);
    //test(tranger, "topic", 9, count);
    test(tranger, "topic", 10, count);
    test(tranger, "topic", 11, count);
    printf("====> mem %'lu\n", gbmem_mem_in_use());

    /*-------------------------------------*
     *  Your end code
     *-------------------------------------*/
    tranger_shutdown(tranger);
    gbmem_shutdown();
    end_ghelpers_library();

    return 0;
}

