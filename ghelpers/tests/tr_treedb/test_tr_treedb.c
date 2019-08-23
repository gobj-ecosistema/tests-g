/****************************************************************************
 *          test.c
 *
 *          Copyright (c) 2018 Niyamaka.
 *          All Rights Reserved.
 ****************************************************************************/
#include <argp.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <locale.h>
#include <time.h>
#include <ghelpers.h>

#include "schema_sample.c"

/***************************************************************************
 *      Constants
 ***************************************************************************/
#define APP_NAME "test_tr_treedb"

/***************************************************************************
 *      Structures
 ***************************************************************************/

/*
 *  Used by main to communicate with parse_opt.
 */
#define MIN_ARGS 0
#define MAX_ARGS 0
struct arguments
{
    char *args[MAX_ARGS+1];     /* positional args */

    int without_ok_tests;
    int without_bad_tests;
};
PRIVATE int show_log_output;

/***************************************************************************
 *              Prototypes
 ***************************************************************************/
static error_t parse_opt (int key, char *arg, struct argp_state *state);

/***************************************************************************
 *      Data
 ***************************************************************************/
PRIVATE json_t *expected_log_messages = 0;
PRIVATE json_t *unexpected_log_messages = 0;

// Set by yuneta_entry_point()
const char *argp_program_version = APP_NAME;
const char *argp_program_bug_address = "";

/* Program documentation. */
static char doc[] = "";

/* A description of the arguments we accept. */
static char args_doc[] = "Test of TimeRanger TreeDb";

/*
 *  The options we understand.
 *  See https://www.gnu.org/software/libc/manual/html_node/Argp-Option-Vectors.html
 */
static struct argp_option options[] = {
/*-name-----------------key-----arg---------flags---doc-----------------group */
{"without-ok-tests",    1,      0,          0,      "Not execute ok tests", 1},
{"without-bad-tests",   2,      0,          0,      "Not execute bad tests", 1},
{"show-log-output",     3,      0,          0,      "Show log ouputs", 1},

{0}
};

/* Our argp parser. */
static struct argp argp = {
    options,
    parse_opt,
    args_doc,
    doc
};

/***************************************************************************
 *  Parse a single option
 ***************************************************************************/
static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
    /*
     *  Get the input argument from argp_parse,
     *  which we know is a pointer to our arguments structure.
     */
    struct arguments *arguments = state->input;

    switch (key) {
    case 1:
        arguments->without_ok_tests = 1;
        break;

    case 2:
        arguments->without_bad_tests = 1;
        break;

    case 3:
        show_log_output = 1;
        break;

    case ARGP_KEY_ARG:
        if (state->arg_num >= MAX_ARGS) {
            /* Too many arguments. */
            argp_usage (state);
        }
        arguments->args[state->arg_num] = arg;
        break;

    case ARGP_KEY_END:
        if (state->arg_num < MIN_ARGS) {
            /* Not enough arguments. */
            argp_usage (state);
        }
        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/***************************************************************************
 *  Prints to the provided buffer a nice number of bytes (KB, MB, GB, etc)
 *  https://www.mbeckler.org/blog/?p=114
 ***************************************************************************/
PRIVATE void pretty_bytes(char* bf, int bfsize, uint64_t bytes)
{
    const char* suffixes[7];
    suffixes[0] = "B";
    suffixes[1] = "Miles";
    suffixes[2] = "Millones";
    suffixes[3] = "GB";
    suffixes[4] = "TB";
    suffixes[5] = "PB";
    suffixes[6] = "EB";
    uint s = 0; // which suffix to use
    double count = bytes;
    while (count >= 1000 && s < 7)
    {
        s++;
        count /= 1000;
    }
    if (count - floor(count) == 0.0)
        snprintf(bf, bfsize, "%d %s", (int)count, suffixes[s]);
    else
        snprintf(bf, bfsize, "%.1f %s", count, suffixes[s]);
}

/***************************************************************************
 *
 ***************************************************************************/
PRIVATE inline double ts_diff2 (struct timespec start, struct timespec end)
{
    uint64_t s, e;
    s = ((uint64_t)start.tv_sec)*1000000 + ((uint64_t)start.tv_nsec)/1000;
    e = ((uint64_t)end.tv_sec)*1000000 + ((uint64_t)end.tv_nsec)/1000;
    return ((double)(e-s))/1000000;
}

/***************************************************************************
 *
 ***************************************************************************/
PRIVATE inline struct timespec ts_diff (struct timespec start, struct timespec end)
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
PRIVATE int capture_log_write(void* v, int priority, const char* bf, size_t len)
{
    json_t *msg = legalstring2json(bf, FALSE);
    if(show_log_output) {
        print_json(msg);
    }

    json_t *expected_msg = kw_get_list_value(expected_log_messages, 0, KW_DONT_LOG);

    if(expected_msg) {
        JSON_INCREF(expected_msg);
        if(kw_match_simple(msg, expected_msg)) {
            kw_get_list_value(expected_log_messages, 0, KW_EXTRACT);
            JSON_DECREF(expected_msg);
            JSON_DECREF(msg);
            return -1; // It's only mine
        }
    }
    json_array_append_new(unexpected_log_messages, msg);
    return 0;  // Continue with next output handlers
}

/***************************************************************************
 *
 ***************************************************************************/
PRIVATE void set_expected_results(
    const char *name,
    json_t *errors_list
)
{
    printf("Test '%s'\n", name);
    JSON_DECREF(expected_log_messages);
    json_array_clear(unexpected_log_messages);
    expected_log_messages = errors_list;
}

/***************************************************************************
 *  Return TRUE if all is ok.
 ***************************************************************************/
PRIVATE BOOL show_results(const char *test, int errors)
{
    if(json_array_size(unexpected_log_messages)>0) {

        printf("%s  --> ERROR %d fields%s\n", On_Red BWhite, -errors, Color_Off);
        int idx; json_t *value;
        printf("      Unexpected error:\n");
        json_array_foreach(unexpected_log_messages, idx, value) {
            printf("          '%s'\n", kw_get_str(value, "msg", "?", 0));
        }
        return FALSE;
    }

    if(json_array_size(expected_log_messages)>0) {
        printf("%s  --> ERROR %d fields%s\n", On_Red BWhite, -errors, Color_Off);
        int idx; json_t *value;
        printf("      Expected error not consumed:\n");
        json_array_foreach(expected_log_messages, idx, value) {
            printf("          '%s'\n", kw_get_str(value, "msg", "?", 0));
        }
        return FALSE;
    }

    printf("  --> OK\n");
    return TRUE;
}

/***************************************************************************
 *
 ***************************************************************************/
// PRIVATE json_t *__trtdb_create_topic_cols_desc(void)
// {
//     json_t *topic_cols_desc = json_array();
//     json_array_append_new(
//         topic_cols_desc,
//         json_pack("{s:s, s:s, s:[s,s], s:[s,s]}",
//             "id", "id",
//             "header", "Id",
//             "type",
//                 "string", "integer",
//             "flag",
//                 "required", "pkey"
//         )
//     );
//     json_array_append_new(
//         topic_cols_desc,
//         json_pack("{s:s, s:s, s:s, s:s}",
//             "id", "header",
//             "header", "Header",
//             "type",
//                 "string",
//             "flag",
//                 ""
//         )
//     );
//     json_array_append_new(
//         topic_cols_desc,
//         json_pack("{s:s, s:s, s:s, s:[s,s,s,s,s,s,s,s], s:s}",
//             "id", "type",
//             "header", "Type",
//             "type", "enum",
//             "enum",
//                 "string","integer","object","array","real","boolean","null",  "enum",
//             "flag",
//                 "required"
//         )
//     );
//     json_array_append_new(
//         topic_cols_desc,
//         json_pack("{s:s, s:s, s:s, s:[s,s,s,s,s,s,s,s,s], s:s}",
//             "id", "flag",
//             "header", "Flag",
//             "type", "enum",
//             "enum",
//                 "","persistent","required","volatil","pkey","uuid","link","reverse","include",
//             "flag",
//                 ""
//         )
//     );
//     return topic_cols_desc;
// }

/***************************************************************************
 *
 ***************************************************************************/
PRIVATE BOOL test_treedb_schema(
    json_t *tranger,
    json_t *topic_cols_desc,
    int without_ok_tests,
    int without_bad_tests
)
{
    BOOL ret = TRUE;
    const char *test;
    int errors = 0;

    /*-----------------------------------*
     *
     *-----------------------------------*/
    if(!without_ok_tests) {
        test = "good_desc_01";
        json_t *topic = tranger_create_topic(
            tranger,
            test,
            "id",
            "",
            sf_rowid_key,
            json_pack("[{s:s, s:s, s:s, s:s}]",
                "id", "id",
                "header", "Id",
                "type", "integer",
                "flag", "required"
            )
        );
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );
        errors = parse_schema_cols(
            test,
            topic_cols_desc,
            kw_get_list(topic, "cols", 0, KW_REQUIRED)
        );
        if(!show_results(test, errors)) {
            ret = FALSE;
        }
        tranger_delete_topic(tranger, test);
    }

    /*-----------------------------------*
     *
     *-----------------------------------*/
    if(!without_ok_tests) {
        test = "good_desc_02";
        json_t *topic = tranger_create_topic(
            tranger,
            test,
            "id",
            "",
            sf_rowid_key,
            json_pack("[{s:s, s:s, s:[s], s:[s,s]}]",
                "id", "name",
                "header", "Name",
                "type", "string",
                "flag",
                    "persistent", "required"
            )
        );
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );
        errors = parse_schema_cols(
            test,
            topic_cols_desc,
            kw_get_list(topic, "cols", 0, KW_REQUIRED)
        );
        if(!show_results(test, errors)) {
            ret = FALSE;
        }
        tranger_delete_topic(tranger, test);
    }

    /*-----------------------------------*
     *
     *-----------------------------------*/
    if(!without_bad_tests) {
        test = "bad_desc_01";
        json_t *topic = tranger_create_topic(
            tranger,
            test,
            "id",
            "",
            sf_rowid_key,
            json_pack("[{s:b, s:[s], s:s, s:s}]",
                "id", 1,
                "header", "Xx",
                "type", "xinteger",
                "flag", "xrequired"
            )
        );
        set_expected_results(
            test,
            json_pack("[{s:s, s:s}, {s:s, s:s}, {s:s, s:s}, {s:s, s:s}]",  // error's list
                "msg", "Wrong basic type",
                "field", "id",
                "msg", "Wrong basic type",
                "field", "header",
                "msg", "Wrong enum type",
                "field", "type",
                "msg", "Wrong enum type",
                "field", "flag"
            )
        );

        errors = parse_schema_cols(
            test,
            topic_cols_desc,
            kw_get_list(topic, "cols", 0, KW_REQUIRED)
        );
        if(!show_results(test, errors)) {
            ret = FALSE;
        }
        tranger_delete_topic(tranger, test);
    }

    return ret;
}

/***************************************************************************
 *
 ***************************************************************************/
PRIVATE BOOL match_record(
    json_t *record_, // NOT owned
    json_t *expected_, // owned
    BOOL verbose
)
{
    BOOL ret = TRUE;
    json_t *record = json_deep_copy(record_);
    json_t *expected = json_deep_copy(expected_);

    void *n; const char *key; json_t *value;
    json_object_foreach_safe(record, n, key, value) {
        if(!kw_has_key(expected, key)) {
            ret = FALSE;
        }
        if(!kw_is_identical(value, json_object_get(expected, key))) {
            ret = FALSE;
        }
        json_object_del(record, key);
        json_object_del(expected, key);
    }

    if(json_object_size(record)>0) {
        ret = FALSE;
    }
    if(json_object_size(expected)>0) {
        ret = FALSE;
    }

    if(!ret && verbose) {
        log_debug_json(0, record, "record");
        log_debug_json(0, expected, "expected");
    }

    JSON_DECREF(record);
    JSON_DECREF(expected);
    JSON_DECREF(expected_);
    return ret;
}

/***************************************************************************
 *
 ***************************************************************************/
PRIVATE BOOL test_schema(
    json_t *tranger,
    json_t *topic_cols_desc,
    const char *treedb_name
)
{
    BOOL ret = TRUE;
    int errors = 0;

    const char *topic_name; json_t *topic;
    json_object_foreach(kw_get_dict(tranger, "topics", 0, KW_REQUIRED), topic_name, topic)
    {
        set_expected_results(
            topic_name,
            json_pack("[]"  // error's list
            )
        );
        errors = parse_schema_cols(
            topic_name,
            topic_cols_desc,
            kw_get_list(topic, "cols", 0, KW_REQUIRED)
        );
        if(!show_results(topic_name, errors)) {
            ret = FALSE;
        }
    }

    return ret;
}

/***************************************************************************
 *
 ***************************************************************************/
PRIVATE BOOL test_load_data(json_t *tranger, const char *treedb_name, BOOL verbose)
{
    BOOL ret = TRUE;
    const char *test = "load_data";
    set_expected_results(
        test,
        json_pack("[]"  // error's list
        )
    );

/*
    Dirección
        |
         -> Administración
                |
                |-> Gestión
                |
                |-> Microinformática
                |
                |-> Redes
                |
                |-> Sistemas
                |
                 -> Desarrollo
*/

/*
    Dirección
        |
         -> Administración
*/

    json_t *id = 0;
    json_t *data = 0;
    json_t *expected = 0;

    /*--------------------------*
     *      Dirección
     *--------------------------*/
    id = json_string("1");
    data = json_pack("{s:s}",
        "name", "Dirección"
    );
    expected = json_deep_copy(data);
    json_object_set_new(expected, "id", json_deep_copy(id));

    json_t *direction = trtdb_read_node(
        tranger, treedb_name,       // treedb
        "departments",              // topic_name
        id,                         // id
        0,                          // fields
        data,                       // data
        "create|verbose"            // options
    );
    if(!match_record(direction, expected, verbose)) {
        ret = FALSE;
        printf("%s  --> ERROR Dirección %s\n", On_Red BWhite, Color_Off);
    }

    /*--------------------------*
     *      Administración
     *--------------------------*/
    data = json_pack("{s:s, s:s}",
        "id", "2",
        "name", "Administración"
    );
    expected = json_deep_copy(data);

    json_t *administration = trtdb_read_node(
        tranger, treedb_name,
        "departments",
        0,
        0,
        data,
        "create|verbose"
    );
    if(!match_record(administration, expected, verbose)) {
        ret = FALSE;
        printf("%s  --> ERROR Administracion %s\n", On_Red BWhite, Color_Off);
    }

    /*------------------------------*
     *  Dirección->Administración
     *------------------------------*/
//     {
//         trtdb_link_node(tranger, direction, administration, 0);
//     }

#ifdef PEPE

/*
         -> Administración
                |
                |-> Gestión
*/
    trtdb_link_node(tranger,
        administration,
        trtdb_read_node(
            tranger, treedb_name,
            "departments",
            0,
            json_pack("{s:s}",
                "name", "Gestión"
            ),
            "create|verbose"
        ),
        0
    );

/*
         -> Administración
                |
                |-> Microinformática
*/
    trtdb_link_node(tranger,
        administration,
        trtdb_read_node(
            tranger, treedb_name,
            "departments",
            0,
            json_pack("{s:s}",
                "name", "Microinformática"
            ),
            "create|verbose"
        ),
        0
    );

/*
         -> Administración
                |
                |-> Redes
*/
    trtdb_link_node(tranger,
        administration,
        trtdb_read_node(
            tranger, treedb_name,
            "departments",
            0,
            json_pack("{s:s}",
                "name", "Redes"
            ),
            "create|verbose"
        ),
        0
    );

/*
         -> Administración
                |
                |-> Sistemas
*/
    trtdb_link_node(tranger,
        administration,
        trtdb_read_node(
            tranger, treedb_name,
            "departments",
            0,
            json_pack("{s:s}",
                "name", "Sistemas"
            ),
            "create|verbose"
        ),
        0
    );

/*
         -> Administración
                |
                 -> Desarrollo
*/
    trtdb_link_node(tranger,
        administration,
        trtdb_read_node(
            tranger, treedb_name,
            "departments",
            0,
            json_pack("{s:s}",
                "name", "Desarrollo"
            ),
            "create|verbose"
        ),
        0
    );
#endif

    if(ret) {
        if(!show_results(test, 0)) {
            ret = FALSE;
        }
    }

    return ret;
}

/***************************************************************************
 *
 ***************************************************************************/
void test_performance(
    json_t *rc2,
    int caso,
    const char *desc,
    int without_ok_tests,
    int without_bad_tests
)
{
    uint64_t cnt;
    struct timespec st, et;
    double dt;

    cnt = without_ok_tests * without_bad_tests;

    /*-------------------------------------*
     *  Loop
     *-------------------------------------*/
    switch(caso) {
    case 1:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);


            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;

    default:
        printf("MIERDA\n");
    }


    /*-------------------------------------*
     *  Your end code
     *-------------------------------------*/

    /*-------------------------------------*
     *  Print times
     *-------------------------------------*/
    char bf[128];
    pretty_bytes(bf, sizeof(bf), cnt);

    dt = ts_diff2(st, et);

    setlocale(LC_ALL, "");
    printf("# test %d %-16s ==> without_ok_tests %'d; without_bad_tests %'d; %'"PRIu64" items; %f seconds; %'lu op/sec\n\n",
        caso,
        desc,
        without_ok_tests,
        without_bad_tests,
        cnt,
        dt,
        (unsigned long)(((double)cnt)/dt)
    );
}

/***************************************************************************
 *                      Main
 ***************************************************************************/
int main(int argc, char *argv[])
{
    int ret = 0;

    struct arguments arguments;
    /*
     *  Default values
     */
    memset(&arguments, 0, sizeof(arguments));
    arguments.without_ok_tests = 0;
    arguments.without_bad_tests = 0;

    /*
     *  Parse arguments
     */
    argp_parse (&argp, argc, argv, 0, 0, &arguments);

    /*-------------------------------------*
     *  Your start code
     *-------------------------------------*/
    init_ghelpers_library(APP_NAME);
    log_startup(
        "test",             // application name
        "1.0.0",            // applicacion version
        "test_glogger"     // executable program, to can trace stack
    );

    static uint32_t mem_list[] = {2680, 0};
    gbmem_trace_alloc_free(0, mem_list);

    /*------------------------------------------------*
     *          Setup memory
     *------------------------------------------------*/
    #define MEM_MIN_BLOCK   512
    uint64_t MEM_MAX_SYSTEM_MEMORY = free_ram_in_kb() * 1024LL;
    MEM_MAX_SYSTEM_MEMORY /= 100LL;
    MEM_MAX_SYSTEM_MEMORY *= 90LL;  // Coge el 90% de la memoria

    uint64_t MEM_MAX_BLOCK = (MEM_MAX_SYSTEM_MEMORY / sizeof(md_record_t)) * sizeof(md_record_t);
    MEM_MAX_BLOCK = MIN(1*1024*1024*1024LL, MEM_MAX_BLOCK);  // 1*G max

    uint64_t MEM_SUPERBLOCK = MEM_MAX_BLOCK;

    if(1) {
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

    /*------------------------------*
     *  Captura salida logger
     *------------------------------*/
    log_register_handler(
        "testing",          // handler_name
        0,                  // close_fn
        capture_log_write,  // write_fn
        0                   // fwrite_fn
    );
    log_add_handler("test_capture", "testing", LOG_OPT_UP_INFO, 0);
    log_add_handler("test_stdout", "stdout", LOG_OPT_ALL, 0);

    expected_log_messages = json_array();
    unexpected_log_messages = json_array();

    /*------------------------------*
     *  La bbddd de pruebas
     *------------------------------*/
    char *path = "/test/trtdb/db_test1";

    /*------------------------------*
     *  Destruye la bbdd previa
     *------------------------------*/
    rmrdir(path);

    /*------------------------------*
     *      Startup tranger
     *------------------------------*/
    json_t *jn_tranger = json_pack("{s:s, s:b}",
        "path", path,
        "master", 1
    );
    json_t *tranger = tranger_startup(
        jn_tranger // owned
    );

    /*------------------------------*
     *  Check treedb internals
     *------------------------------*/
    json_t *topic_cols_desc =_trtdb_create_topic_cols_desc();
    if(!test_treedb_schema(
        tranger,
        topic_cols_desc,
        arguments.without_ok_tests,
        arguments.without_bad_tests)) {
            ret = -1;
    }

    /*------------------------------*
     *      Check treedb sample
     *------------------------------*/
    helper_quote2doublequote(schema_sample);
    json_t *jn_schema_sample = legalstring2json(schema_sample, TRUE);
    if(!jn_schema_sample) {
        exit(-1);
    }

    const char *treedb_name = "treedb_test";
    trtdb_open_db(
        tranger,  // owned
        treedb_name,
        jn_schema_sample,  // owned
        0
    );

    /*------------------------------*
     *  Ejecuta los tests
     *------------------------------*/
    if(!test_schema(tranger, topic_cols_desc, treedb_name)) {
        ret = -1;
    }

    if(!test_load_data(tranger, treedb_name, FALSE)) { // verbose a option
        ret = -1;
    }

    //print_json(kw_get_dict(tranger, "treedbs", 0, 0));
    //print_json(tranger);

    /*------------------------------*
     *  Cierra la bbdd
     *------------------------------*/
    trtdb_close_db(tranger, treedb_name);

    tranger_shutdown(tranger);

    /*---------------------------*
     *      Destroy all
     *---------------------------*/
    JSON_DECREF(topic_cols_desc);
    JSON_DECREF(expected_log_messages);
    JSON_DECREF(unexpected_log_messages);
    gbmem_shutdown();
    end_ghelpers_library();

    return ret;
}

