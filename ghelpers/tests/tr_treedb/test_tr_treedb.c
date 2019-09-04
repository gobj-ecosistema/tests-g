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
    int show_oks;
    int print_tranger;
    int print_treedb;
    int verbose;
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
{"show-oks",            4,      0,          0,      "Show log ouputs (verbose required)", 1},
{"print-tranger",       5,      0,          0,      "Print tranger json", 1},
{"print-treedb",        6,      0,          0,      "Print treedb json", 1},
{"verbose",             7,      0,          0,      "Verbose", 1},

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

    case 4:
        arguments->show_oks = 1;
        break;

    case 5:
        arguments->print_tranger = 1;
        break;

    case 6:
        arguments->print_treedb = 1;
        break;

    case 7:
        arguments->verbose = 1;
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
PRIVATE BOOL check_log_result(const char *test)
{
    if(json_array_size(unexpected_log_messages)>0) {

        printf("%s  --> ERROR %s\n", On_Red BWhite,Color_Off);
        int idx; json_t *value;
        printf("      Unexpected error:\n");
        json_array_foreach(unexpected_log_messages, idx, value) {
            printf("          '%s'\n", kw_get_str(value, "msg", "?", 0));
        }
        return FALSE;
    }

    if(json_array_size(expected_log_messages)>0) {
        printf("%s  --> ERROR %s\n", On_Red BWhite, Color_Off);
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
PRIVATE BOOL match_record(
    json_t *record_, // NOT owned
    json_t *expected_ // NOT owned
)
{
    BOOL ret = TRUE;
    json_t *record = json_deep_copy(record_);
    json_t *expected = json_deep_copy(expected_);
    if(!record) {
        return FALSE;
    }
    if(!expected) {
        return FALSE;
    }

    if(json_typeof(record) != json_typeof(expected)) { // json_typeof CONTROLADO
        ret = FALSE;
    } else {
        switch(json_typeof(record)) {
            case JSON_ARRAY:
                {
                    int idx; json_t *v1;
                    json_array_foreach(record, idx, v1) {
                        json_t *v2 = json_array_get(expected, idx);
                        if(!match_record(v1, v2)) {
                            ret = FALSE;
                            break;
                        }

                    }
                }
                break;
            case JSON_OBJECT:
                {
                    json_object_del(record, "__md_treedb__");
                    json_object_del(expected, "__md_treedb__");
                    void *n; const char *key; json_t *value;
                    json_object_foreach_safe(record, n, key, value) {
                        if(!kw_has_key(expected, key)) {
                            ret = FALSE;
                            break;
                        }
                        if(json_typeof(value)==JSON_OBJECT) {
                            if(!match_record(value, json_object_get(expected, key))) {
                                ret = FALSE;
                                break;
                            } else {
                                json_object_del(record, key);
                                json_object_del(expected, key);
                            }
                        } else {
                            if(!kw_is_identical(value, json_object_get(expected, key))) {
                                ret = FALSE;
                                break;
                            } else {
                                json_object_del(record, key);
                                json_object_del(expected, key);
                            }
                        }
                    }

                    if(json_object_size(record)>0) {
                        ret = FALSE;
                    }
                    if(json_object_size(expected)>0) {
                        ret = FALSE;
                    }
                }
                break;
            default:
                ret = FALSE;
                break;
        }

    }

    JSON_DECREF(record);
    JSON_DECREF(expected);
    return ret;
}

/***************************************************************************
 *
 ***************************************************************************/
PRIVATE BOOL test_treedb_schema(
    json_t *tranger,
    json_t *topic_cols_desc,
    int without_ok_tests,
    int without_bad_tests,
    int show_oks,
    int verbose
)
{
    BOOL ret = TRUE;
    const char *test;

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
        parse_schema_cols(
            topic_cols_desc,
            kwid_new_list("verbose", topic, "cols")
        );
        if(!check_log_result(test)) {
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
        parse_schema_cols(
            topic_cols_desc,
            kwid_new_list("verbose", topic, "cols")
        );
        if(!check_log_result(test)) {
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

        parse_schema_cols(
            topic_cols_desc,
            kwid_new_list("verbose", topic, "cols")
        );
        if(!check_log_result(test)) {
            ret = FALSE;
        }
        tranger_delete_topic(tranger, test);
    }

    return ret;
}

/***************************************************************************
 *
 ***************************************************************************/
PRIVATE BOOL test_schema(
    json_t *tranger,
    json_t *topic_cols_desc,
    const char *treedb_name,
    int verbose
)
{
    BOOL ret = TRUE;

    const char *topic_name; json_t *topic;
    json_object_foreach(kw_get_dict(tranger, "topics", 0, KW_REQUIRED), topic_name, topic)
    {
        set_expected_results(
            topic_name,
            json_pack("[]"  // error's list
            )
        );
        parse_schema_cols(
            topic_cols_desc,
            kwid_new_list("verbose", topic, "cols")
        );
        if(!check_log_result(topic_name)) {
            ret = FALSE;
        }
    }

    return ret;
}

/***************************************************************************
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
 ***************************************************************************/
PRIVATE BOOL test_departments_data(
    json_t *tranger,
    const char *treedb_name,
    int without_ok_tests,
    int without_bad_tests,
    int show_oks,
    int verbose
)
{
    BOOL ret = TRUE;
    json_t *direction = 0;
    json_t *administration = 0;
    json_t *management = 0;
    json_t *microinformatics = 0;
    json_t *network = 0;
    json_t *systems = 0;
    json_t *development = 0;

/**------------------------------------------------------------*
    Dirección
        |
         -> Administración
*------------------------------------------------------------*/

    json_t *data = 0;
    json_t *found = 0;
    json_t *expected = 0;

    /*-----------------------------------*
     *  Dirección
     *-----------------------------------*/
    if(!without_ok_tests) {
        const char *test = "Create direction, good";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );

        data = json_pack("{s:s, s:s}",
            "id", "1",
            "name", "Dirección"
        );
        expected = json_pack("{s:s, s:s, s:s, s:{}, s:{}, s:[]}",
            "id", "1",
            "name", "Dirección",
            "department_id", "",
            "departments",
            "managers",
            "users"

        );

        direction = treedb_create_node(
            tranger, treedb_name,       // treedb
            "departments",              // topic_name
            data                        // data
        );
        if(!match_record(direction, expected)) {
            ret = FALSE;
            printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            if(verbose) {
                log_debug_json(0, direction, "Record found");
                log_debug_json(0, expected, "Record expected");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }
        if(show_oks) {
            log_debug_json(0, direction, "Record found");
            log_debug_json(0, expected, "Record expected");
        }
        JSON_DECREF(expected);
    }

    /*-----------------------------------*
     *
     *-----------------------------------*/
    if(!without_bad_tests) {
        const char *test = "Create administration, wrong, no id";
        set_expected_results(
            test,
            json_pack("[{s:s}]",  // error's list
                "msg", "Field 'id' required"
            )
        );

        data = json_pack("{s:s}",
            "name", "Administración"
        );
        expected = 0;

        found = treedb_create_node( // Must return 0
            tranger, treedb_name,
            "departments",
            data
        );

        if(found) {
            ret = FALSE;
            printf("%s  --> ERROR Administracion %s\n", On_Red BWhite, Color_Off);
            if(verbose) {
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }

        if(show_oks) {
            log_debug_json(0, found, "Record found");
            log_debug_json(0, expected, "Record expected");
        }
        JSON_DECREF(found);
        JSON_DECREF(expected);
    }

    /*-----------------------------------*
     *  Administración
     *-----------------------------------*/
    if(!without_ok_tests) {
        const char *test = "Create administration, good";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );

        data = json_pack("{s:s, s:s}",
            "id", "2",
            "name", "Administración"
        );
        expected = json_pack("{s:s, s:s, s:s, s:{}, s:{}, s:[]}",
            "id", "2",
            "name", "Administración",
            "department_id", "",
            "departments",
            "managers",
            "users"
        );

        administration = treedb_create_node(
            tranger, treedb_name,
            "departments",
            data
        );
        if(!match_record(administration, expected)) {
            ret = FALSE;
            printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            if(verbose) {
                log_debug_json(0, administration, "Record found");
                log_debug_json(0, expected, "Record expected");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }
        if(show_oks) {
            log_debug_json(0, administration, "Record found");
            log_debug_json(0, expected, "Record expected");
        }
        JSON_DECREF(expected);
    }

    /*-----------------------------------*
     *
     *-----------------------------------*/
    if(!without_ok_tests) {
        const char *test = "Get administration, good";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );

        expected = json_pack("{s:s, s:s, s:s, s:{}, s:{}, s:[]}",
            "id", "2",
            "name", "Administración",
            "department_id", "",
            "departments",
            "managers",
            "users"
        );

        found = treedb_get_node(
            tranger, treedb_name,
            "departments",
            json_string("2")
        );
        if(!match_record(found, expected)) {
            ret = FALSE;
            printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            if(verbose) {
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }
        if(show_oks) {
            log_debug_json(0, found, "Record found");
            log_debug_json(0, expected, "Record expected");
        }
        JSON_DECREF(expected);
    }

    /*-------------------------------------*
     *      Dirección->Administración
     *------------------------------------*/
    if(!without_bad_tests) {
        const char *test = "link direction->administration: wrong hook";
        set_expected_results(
            test,
            json_pack("[{s:s}]",  // error's list
                "msg", "hook field not found"
            )
        );
        treedb_link_nodes(
            tranger,
            "departmentsx",
            direction,
            administration
        );
        if(!check_log_result(test)) {
            ret = FALSE;
        }
    }

    if(!without_ok_tests) {
        const char *test = "link direction->administration, good";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );
        treedb_link_nodes(
            tranger,
            "departments",
            direction,
            administration
        );

        expected = json_pack("{s:s, s:s, s:s, s:{s:{s:s, s:s, s:s, s:{}, s:{}, s:[]}}, s:{}, s:[]}",
            "id", "1",
            "name", "Dirección",
            "department_id", "",
            "departments",
                "2",
                    "id", "2",
                    "name", "Administración",
                    "department_id", "1",
                    "departments",
                    "managers",
                    "users",
            "managers",
            "users"
        );

        found = treedb_get_node(
            tranger, treedb_name,
            "departments",
            json_string("1")
        );
        if(!match_record(found, expected)) {
            ret = FALSE;
            printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            if(verbose) {
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }
        if(show_oks) {
            log_debug_json(0, found, "Record found");
            log_debug_json(0, expected, "Record expected");
        }
        JSON_DECREF(expected);
    }

    if(!without_ok_tests) {
        // Comprueba con timeranger
        const char *test = "match treedb with timeranger rowid 1, id 1 Dirección";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );
        md_record_t md_record;
        tranger_get_record(
            tranger,
            tranger_topic(tranger, "departments"),
            1, //__rowid__
            &md_record,
            TRUE
        );
        found = tranger_read_record_content(
            tranger,
            tranger_topic(tranger, "departments"),
            &md_record
        );
        expected = json_pack("{s:s, s:s, s:s, s:{}, s:{}, s:[]}",
            "id", "1",
            "name", "Dirección",
            "department_id", "",
            "departments",
            "managers",
            "users"
        );
        if(!expected) {
            printf("%s  --> expected NULL in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
        }
        if(!match_record(found, expected)) {
            ret = FALSE;
            printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            if(verbose) {
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }
        if(show_oks) {
            log_debug_json(0, found, "Record found");
            log_debug_json(0, expected, "Record expected");
        }
        JSON_DECREF(found);
        JSON_DECREF(expected);
    }

    if(!without_ok_tests) {
        // Comprueba con timeranger
        const char *test = "match treedb with timeranger rowid 3, id 2 Administración";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );
        md_record_t md_record;
        tranger_get_record(
            tranger,
            tranger_topic(tranger, "departments"),
            3, //__rowid__
            &md_record,
            TRUE
        );
        found = tranger_read_record_content(
            tranger,
            tranger_topic(tranger, "departments"),
            &md_record
        );
        expected = json_pack("{s:s, s:s, s:s, s:{}, s:{}, s:[]}",
            "id", "2",
            "name", "Administración",
            "department_id", "1",
            "departments",
            "managers",
            "users"
        );
        if(!expected) {
            printf("%s  --> expected NULL in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
        }

        if(!match_record(found, expected)) {
            ret = FALSE;
            printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            if(verbose) {
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }
        if(show_oks) {
            log_debug_json(0, found, "Record found");
            log_debug_json(0, expected, "Record expected");
        }
        JSON_DECREF(found);
        JSON_DECREF(expected);
    }

/*------------------------------------------------------------*
         -> Administración
                |
                |-> Gestión
*------------------------------------------------------------*/
    /*-----------------------------------*
     *  Gestión
     *-----------------------------------*/
    if(!without_bad_tests) {
        const char *test = "Create management, wrong, duplicated key";
        set_expected_results(
            test,
            json_pack("[{s:s}]",  // error's list
                "msg", "Node already exists"
            )
        );

        data = json_pack("{s:s, s:s}",
            "id", "2",
            "name", "Gestión"
        );

        management = treedb_create_node(
            tranger, treedb_name,
            "departments",
            data
        );
        if(management) {
            ret = FALSE;
            printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            if(verbose) {
                log_debug_json(0, management, "Record found");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }
    }

    if(!without_ok_tests) {
        const char *test = "Create management, good";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );

        data = json_pack("{s:s, s:s}",
            "id", "3",
            "name", "Gestión"
        );
        expected = json_pack("{s:s, s:s, s:s, s:{}, s:{}, s:[]}",
            "id", "3",
            "name", "Gestión",
            "department_id", "",
            "departments",
            "managers",
            "users"
        );

        management = treedb_create_node(
            tranger, treedb_name,
            "departments",
            data
        );
        if(!match_record(management, expected)) {
            ret = FALSE;
            printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            if(verbose) {
                log_debug_json(0, management, "Record found");
                log_debug_json(0, expected, "Record expected");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }
        if(show_oks) {
            log_debug_json(0, management, "Record found");
            log_debug_json(0, expected, "Record expected");
        }
        JSON_DECREF(expected);
    }

    /*-------------------------------------*
     *      Administración -> Gestión
     *------------------------------------*/
    if(!without_ok_tests) {
        const char *test = "link administration->management, good";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );
        treedb_link_nodes(
            tranger,
            "departments",
            administration,
            management
        );

        expected = json_pack("{s:s, s:s, s:s, s:{s:{s:s, s:s, s:s, s:{}, s:{}, s:[]}}, s:{}, s:[]}",
            "id", "2",
            "name", "Administración",
            "department_id", "1",
            "departments",
                "3",
                    "id", "3",
                    "name", "Gestión",
                    "department_id", "2",
                    "departments",
                    "managers",
                    "users",
            "managers",
            "users"
        );

        found = treedb_get_node(
            tranger, treedb_name,
            "departments",
            json_string("2")
        );
        if(!match_record(found, expected)) {
            ret = FALSE;
            printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            if(verbose) {
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }
        if(show_oks) {
            log_debug_json(0, found, "Record found");
            log_debug_json(0, expected, "Record expected");
        }
        JSON_DECREF(expected);
    }

    if(!without_ok_tests) {
        // Comprueba con timeranger
        const char *test = "match treedb with timeranger rowid 5, id 3 Gestión";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );
        md_record_t md_record;
        tranger_get_record(
            tranger,
            tranger_topic(tranger, "departments"),
            5, //__rowid__
            &md_record,
            TRUE
        );
        found = tranger_read_record_content(
            tranger,
            tranger_topic(tranger, "departments"),
            &md_record
        );
        expected = json_pack("{s:s, s:s, s:s, s:{}, s:{}, s:[]}",
            "id", "3",
            "name", "Gestión",
            "department_id", "2",
            "departments",
            "managers",
            "users"
        );
        if(!expected) {
            printf("%s  --> expected NULL in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
        }

        if(!match_record(found, expected)) {
            ret = FALSE;
            printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            if(verbose) {
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }
        if(show_oks) {
            log_debug_json(0, found, "Record found");
            log_debug_json(0, expected, "Record expected");
        }
        JSON_DECREF(found);
        JSON_DECREF(expected);
    }

/*------------------------------------------------------------**
         -> Administración
                |
                |-> Microinformática
*------------------------------------------------------------*/
    /*-----------------------------------*
     *  Microinformática
     *-----------------------------------*/
    if(!without_ok_tests) {
        const char *test = "Create microinformatics, good";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );

        data = json_pack("{s:s, s:s}",
            "id", "4",
            "name", "Microinformática"
        );
        expected = json_pack("{s:s, s:s, s:s, s:{}, s:{}, s:[]}",
            "id", "4",
            "name", "Microinformática",
            "department_id", "",
            "departments",
            "managers",
            "users"
        );

        microinformatics = treedb_create_node(
            tranger, treedb_name,
            "departments",
            data
        );
        if(!match_record(microinformatics, expected)) {
            ret = FALSE;
            printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            if(verbose) {
                log_debug_json(0, microinformatics, "Record found");
                log_debug_json(0, expected, "Record expected");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }
        if(show_oks) {
            log_debug_json(0, microinformatics, "Record found");
            log_debug_json(0, expected, "Record expected");
        }
        JSON_DECREF(expected);
    }

    /*---------------------------------------------*
     *      Administración -> Microinformática
     *---------------------------------------------*/
    if(!without_ok_tests) {
        const char *test = "link administration->microinformatics, good";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );
        treedb_link_nodes(
            tranger,
            "departments",
            administration,
            microinformatics
        );

        expected = json_pack("{s:s, s:s, s:s, s:"
                "{s:{s:s, s:s, s:s, s:{}, s:{}, s:[]}, s:{s:s, s:s, s:s, s:{}, s:{}, s:[]}},"
                " s:{}, s:[]}",
            "id", "2",
            "name", "Administración",
            "department_id", "1",
            "departments",
                "3",
                    "id", "3",
                    "name", "Gestión",
                    "department_id", "2",
                    "departments",
                    "managers",
                    "users",
                "4",
                    "id", "4",
                    "name", "Microinformática",
                    "department_id", "2",
                    "departments",
                    "managers",
                    "users",
            "managers",
            "users"
        );

        found = treedb_get_node(
            tranger, treedb_name,
            "departments",
            json_string("2")
        );
        if(!match_record(found, expected)) {
            ret = FALSE;
            printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            if(verbose) {
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }
        if(show_oks) {
            log_debug_json(0, found, "Record found");
            log_debug_json(0, expected, "Record expected");
        }
        JSON_DECREF(expected);
    }

    if(!without_ok_tests) {
        // Comprueba con timeranger
        const char *test = "match treedb with timeranger rowid 7, id 4 Microinformática";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );
        md_record_t md_record;
        tranger_get_record(
            tranger,
            tranger_topic(tranger, "departments"),
            7, //__rowid__
            &md_record,
            TRUE
        );
        found = tranger_read_record_content(
            tranger,
            tranger_topic(tranger, "departments"),
            &md_record
        );
        expected = json_pack("{s:s, s:s, s:s, s:{}, s:{}, s:[]}",
            "id", "4",
            "name", "Microinformática",
            "department_id", "2",
            "departments",
            "managers",
            "users"
        );
        if(!expected) {
            printf("%s  --> expected NULL in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
        }

        if(!match_record(found, expected)) {
            ret = FALSE;
            printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            if(verbose) {
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }
        if(show_oks) {
            log_debug_json(0, found, "Record found");
            log_debug_json(0, expected, "Record expected");
        }
        JSON_DECREF(found);
        JSON_DECREF(expected);
    }

/*------------------------------------------------------------**
         -> Administración
                |
                |-> Redes
*------------------------------------------------------------*/
    /*-----------------------------------*
     *  Redes
     *-----------------------------------*/
    if(!without_ok_tests) {
        const char *test = "Create network, good";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );

        data = json_pack("{s:s, s:s}",
            "id", "5",
            "name", "Redes"
        );
        expected = json_pack("{s:s, s:s, s:s, s:{}, s:{}, s:[]}",
            "id", "5",
            "name", "Redes",
            "department_id", "",
            "departments",
            "managers",
            "users"
        );

        network = treedb_create_node(
            tranger, treedb_name,
            "departments",
            data
        );
        if(!match_record(network, expected)) {
            ret = FALSE;
            printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            if(verbose) {
                log_debug_json(0, network, "Record found");
                log_debug_json(0, expected, "Record expected");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }
        if(show_oks) {
            log_debug_json(0, network, "Record found");
            log_debug_json(0, expected, "Record expected");
        }
        JSON_DECREF(expected);
    }

    /*---------------------------------------------*
     *      Administración -> Redes
     *---------------------------------------------*/
    if(!without_ok_tests) {
        const char *test = "link administration->network, good";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );
        treedb_link_nodes(
            tranger,
            "departments",
            administration,
            network
        );

        expected = json_pack("{s:s, s:s, s:s, s:"
                "{s:{s:s, s:s, s:s, s:{}, s:{}, s:[]}, "
                "s:{s:s, s:s, s:s, s:{}, s:{}, s:[]}, "
                "s:{s:s, s:s, s:s, s:{}, s:{}, s:[]}},"
                " s:{}, s:[]}",
            "id", "2",
            "name", "Administración",
            "department_id", "1",
            "departments",
                "3",
                    "id", "3",
                    "name", "Gestión",
                    "department_id", "2",
                    "departments",
                    "managers",
                    "users",
                "4",
                    "id", "4",
                    "name", "Microinformática",
                    "department_id", "2",
                    "departments",
                    "managers",
                    "users",
                "5",
                    "id", "5",
                    "name", "Redes",
                    "department_id", "2",
                    "departments",
                    "managers",
                    "users",
            "managers",
            "users"
        );

        found = treedb_get_node(
            tranger, treedb_name,
            "departments",
            json_string("2")
        );
        if(!match_record(found, expected)) {
            ret = FALSE;
            printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            if(verbose) {
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }
        if(show_oks) {
            log_debug_json(0, found, "Record found");
            log_debug_json(0, expected, "Record expected");
        }
        JSON_DECREF(expected);
    }

    if(!without_ok_tests) {
        // Comprueba con timeranger
        const char *test = "match treedb with timeranger rowid 9, id 5 Redes";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );
        md_record_t md_record;
        tranger_get_record(
            tranger,
            tranger_topic(tranger, "departments"),
            9, //__rowid__
            &md_record,
            TRUE
        );
        found = tranger_read_record_content(
            tranger,
            tranger_topic(tranger, "departments"),
            &md_record
        );
        expected = json_pack("{s:s, s:s, s:s, s:{}, s:{}, s:[]}",
            "id", "5",
            "name", "Redes",
            "department_id", "2",
            "departments",
            "managers",
            "users"
        );
        if(!expected) {
            printf("%s  --> expected NULL in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
        }

        if(!match_record(found, expected)) {
            ret = FALSE;
            printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            if(verbose) {
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }
        if(show_oks) {
            log_debug_json(0, found, "Record found");
            log_debug_json(0, expected, "Record expected");
        }
        JSON_DECREF(found);
        JSON_DECREF(expected);
    }

/*------------------------------------------------------------**
         -> Administración
                |
                |-> Sistemas
*------------------------------------------------------------*/
    /*-----------------------------------*
     *  Sistemas
     *-----------------------------------*/
    if(!without_ok_tests) {
        const char *test = "Create systems, good";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );

        data = json_pack("{s:s, s:s}",
            "id", "6",
            "name", "Sistemas"
        );
        expected = json_pack("{s:s, s:s, s:s, s:{}, s:{}, s:[]}",
            "id", "6",
            "name", "Sistemas",
            "department_id", "",
            "departments",
            "managers",
            "users"
        );

        systems = treedb_create_node(
            tranger, treedb_name,
            "departments",
            data
        );
        if(!match_record(systems, expected)) {
            ret = FALSE;
            printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            if(verbose) {
                log_debug_json(0, systems, "Record found");
                log_debug_json(0, expected, "Record expected");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }
        if(show_oks) {
            log_debug_json(0, systems, "Record found");
            log_debug_json(0, expected, "Record expected");
        }
        JSON_DECREF(expected);
    }

    /*---------------------------------------------*
     *      Administración -> Sistemas
     *---------------------------------------------*/
    if(!without_ok_tests) {
        const char *test = "link administration->systems, good";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );
        treedb_link_nodes(
            tranger,
            "departments",
            administration,
            systems
        );

        expected = json_pack("{s:s, s:s, s:s, s:"
                "{s:{s:s, s:s, s:s, s:{}, s:{}, s:[]}, "
                "s:{s:s, s:s, s:s, s:{}, s:{}, s:[]}, "
                "s:{s:s, s:s, s:s, s:{}, s:{}, s:[]}, "
                "s:{s:s, s:s, s:s, s:{}, s:{}, s:[]}},"
                " s:{}, s:[]}",
            "id", "2",
            "name", "Administración",
            "department_id", "1",
            "departments",
                "3",
                    "id", "3",
                    "name", "Gestión",
                    "department_id", "2",
                    "departments",
                    "managers",
                    "users",
                "4",
                    "id", "4",
                    "name", "Microinformática",
                    "department_id", "2",
                    "departments",
                    "managers",
                    "users",
                "5",
                    "id", "5",
                    "name", "Redes",
                    "department_id", "2",
                    "departments",
                    "managers",
                    "users",
                "6",
                    "id", "6",
                    "name", "Sistemas",
                    "department_id", "2",
                    "departments",
                    "managers",
                    "users",
            "managers",
            "users"
        );

        found = treedb_get_node(
            tranger, treedb_name,
            "departments",
            json_string("2")
        );
        if(!match_record(found, expected)) {
            ret = FALSE;
            printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            if(verbose) {
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }
        if(show_oks) {
            log_debug_json(0, found, "Record found");
            log_debug_json(0, expected, "Record expected");
        }
        JSON_DECREF(expected);
    }

    if(!without_ok_tests) {
        // Comprueba con timeranger
        const char *test = "match treedb with timeranger rowid 11, id 6 Sistemas";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );
        md_record_t md_record;
        tranger_get_record(
            tranger,
            tranger_topic(tranger, "departments"),
            11, //__rowid__
            &md_record,
            TRUE
        );
        found = tranger_read_record_content(
            tranger,
            tranger_topic(tranger, "departments"),
            &md_record
        );
        expected = json_pack("{s:s, s:s, s:s, s:{}, s:{}, s:[]}",
            "id", "6",
            "name", "Sistemas",
            "department_id", "2",
            "departments",
            "managers",
            "users"
        );
        if(!expected) {
            printf("%s  --> expected NULL in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
        }

        if(!match_record(found, expected)) {
            ret = FALSE;
            printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            if(verbose) {
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }
        if(show_oks) {
            log_debug_json(0, found, "Record found");
            log_debug_json(0, expected, "Record expected");
        }
        JSON_DECREF(found);
        JSON_DECREF(expected);
    }

/*------------------------------------------------------------**
         -> Administración
                |
                 -> Desarrollo
*------------------------------------------------------------*/
    /*-----------------------------------*
     *  Desarrollo
     *-----------------------------------*/
    if(!without_ok_tests) {
        const char *test = "Create development, good";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );

        data = json_pack("{s:s, s:s}",
            "id", "7",
            "name", "Desarrollo"
        );
        expected = json_pack("{s:s, s:s, s:s, s:{}, s:{}, s:[]}",
            "id", "7",
            "name", "Desarrollo",
            "department_id", "",
            "departments",
            "managers",
            "users"
        );

        development = treedb_create_node(
            tranger, treedb_name,
            "departments",
            data
        );
        if(!match_record(development, expected)) {
            ret = FALSE;
            printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            if(verbose) {
                log_debug_json(0, development, "Record found");
                log_debug_json(0, expected, "Record expected");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }
        if(show_oks) {
            log_debug_json(0, development, "Record found");
            log_debug_json(0, expected, "Record expected");
        }
        JSON_DECREF(expected);
    }

    /*---------------------------------------------*
     *      Administración -> Desarrollo
     *---------------------------------------------*/
    if(!without_ok_tests) {
        const char *test = "link administration->development, good";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );
        treedb_link_nodes(
            tranger,
            "departments",
            administration,
            development
        );

        expected = json_pack("{s:s, s:s, s:s, s:"
                "{s:{s:s, s:s, s:s, s:{}, s:{}, s:[]}, "
                "s:{s:s, s:s, s:s, s:{}, s:{}, s:[]}, "
                "s:{s:s, s:s, s:s, s:{}, s:{}, s:[]}, "
                "s:{s:s, s:s, s:s, s:{}, s:{}, s:[]}, "
                "s:{s:s, s:s, s:s, s:{}, s:{}, s:[]}},"
                " s:{}, s:[]}",
            "id", "2",
            "name", "Administración",
            "department_id", "1",
            "departments",
                "3",
                    "id", "3",
                    "name", "Gestión",
                    "department_id", "2",
                    "departments",
                    "managers",
                    "users",
                "4",
                    "id", "4",
                    "name", "Microinformática",
                    "department_id", "2",
                    "departments",
                    "managers",
                    "users",
                "5",
                    "id", "5",
                    "name", "Redes",
                    "department_id", "2",
                    "departments",
                    "managers",
                    "users",
                "6",
                    "id", "6",
                    "name", "Sistemas",
                    "department_id", "2",
                    "departments",
                    "managers",
                    "users",
                "7",
                    "id", "7",
                    "name", "Desarrollo",
                    "department_id", "2",
                    "departments",
                    "managers",
                    "users",
            "managers",
            "users"
        );

        found = treedb_get_node(
            tranger, treedb_name,
            "departments",
            json_string("2")
        );
        if(!match_record(found, expected)) {
            ret = FALSE;
            printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            if(verbose) {
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }
        if(show_oks) {
            log_debug_json(0, found, "Record found");
            log_debug_json(0, expected, "Record expected");
        }
        JSON_DECREF(expected);
    }

    if(!without_ok_tests) {
        // Comprueba con timeranger
        const char *test = "match treedb with timeranger rowid 13, id 7 Desarrollo";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );
        md_record_t md_record;
        tranger_get_record(
            tranger,
            tranger_topic(tranger, "departments"),
            13, //__rowid__
            &md_record,
            TRUE
        );
        found = tranger_read_record_content(
            tranger,
            tranger_topic(tranger, "departments"),
            &md_record
        );
        expected = json_pack("{s:s, s:s, s:s, s:{}, s:{}, s:[]}",
            "id", "7",
            "name", "Desarrollo",
            "department_id", "2",
            "departments",
            "managers",
            "users"
        );
        if(!expected) {
            printf("%s  --> expected NULL in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
        }

        if(!match_record(found, expected)) {
            ret = FALSE;
            printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            if(verbose) {
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }
        if(show_oks) {
            log_debug_json(0, found, "Record found");
            log_debug_json(0, expected, "Record expected");
        }
        JSON_DECREF(found);
        JSON_DECREF(expected);
    }

    /*------------------------------------------------------------*
     *          Foto final
     *------------------------------------------------------------*/
char foto_final[]= "\
{                                                                           \n\
    'treedb_test': {                                                        \n\
        '__tags__': {},                                                     \n\
        'users': {},                                                        \n\
        'departments': {                                                    \n\
            '1': {                                                          \n\
                'id': '1',                                                  \n\
                'name': 'Dirección',                                        \n\
                'department_id': '',                                        \n\
                'departments': {                                            \n\
                    '2': {                                                  \n\
                        'id': '2',                                          \n\
                        'name': 'Administración',                           \n\
                        'department_id': '1',                               \n\
                        'departments': {                                    \n\
                            '3': {                                          \n\
                                'id': '3',                                  \n\
                                'name': 'Gestión',                          \n\
                                'department_id': '2',                       \n\
                                'departments': {},                          \n\
                                'managers': {},                             \n\
                                'users': [],                                \n\
                                '__md_treedb__': {                          \n\
                                }                                           \n\
                            },                                              \n\
                            '4': {                                          \n\
                                'id': '4',                                  \n\
                                'name': 'Microinformática',                 \n\
                                'department_id': '2',                       \n\
                                'departments': {},                          \n\
                                'managers': {},                             \n\
                                'users': [],                                \n\
                                '__md_treedb__': {                          \n\
                                }                                           \n\
                            },                                              \n\
                            '5': {                                          \n\
                                'id': '5',                                  \n\
                                'name': 'Redes',                            \n\
                                'department_id': '2',                       \n\
                                'departments': {},                          \n\
                                'managers': {},                             \n\
                                'users': [],                                \n\
                                '__md_treedb__': {                          \n\
                                }                                           \n\
                            },                                              \n\
                            '6': {                                          \n\
                                'id': '6',                                  \n\
                                'name': 'Sistemas',                         \n\
                                'department_id': '2',                       \n\
                                'departments': {},                          \n\
                                'managers': {},                             \n\
                                'users': [],                                \n\
                                '__md_treedb__': {                          \n\
                                }                                           \n\
                            },                                              \n\
                            '7': {                                          \n\
                                'id': '7',                                  \n\
                                'name': 'Desarrollo',                       \n\
                                'department_id': '2',                       \n\
                                'departments': {},                          \n\
                                'managers': {},                             \n\
                                'users': [],                                \n\
                                '__md_treedb__': {                          \n\
                                }                                           \n\
                            }                                               \n\
                        },                                                  \n\
                        'managers': {},                                     \n\
                        'users': [],                                        \n\
                        '__md_treedb__': {                                  \n\
                        }                                                   \n\
                    }                                                       \n\
                },                                                          \n\
                'managers': {},                                             \n\
                'users': [],                                                \n\
                '__md_treedb__': {                                          \n\
                }                                                           \n\
            },                                                              \n\
            '2': {                                                          \n\
                'id': '2',                                                  \n\
                'name': 'Administración',                                   \n\
                'department_id': '1',                                       \n\
                'departments': {                                            \n\
                    '3': {                                                  \n\
                        'id': '3',                                          \n\
                        'name': 'Gestión',                                  \n\
                        'department_id': '2',                               \n\
                        'departments': {},                                  \n\
                        'managers': {},                                     \n\
                        'users': [],                                        \n\
                        '__md_treedb__': {                                  \n\
                        }                                                   \n\
                    },                                                      \n\
                    '4': {                                                  \n\
                        'id': '4',                                          \n\
                        'name': 'Microinformática',                         \n\
                        'department_id': '2',                               \n\
                        'departments': {},                                  \n\
                        'managers': {},                                     \n\
                        'users': [],                                        \n\
                        '__md_treedb__': {                                  \n\
                        }                                                   \n\
                    },                                                      \n\
                    '5': {                                                  \n\
                        'id': '5',                                          \n\
                        'name': 'Redes',                                    \n\
                        'department_id': '2',                               \n\
                        'departments': {},                                  \n\
                        'managers': {},                                     \n\
                        'users': [],                                        \n\
                        '__md_treedb__': {                                  \n\
                        }                                                   \n\
                    },                                                      \n\
                    '6': {                                                  \n\
                        'id': '6',                                          \n\
                        'name': 'Sistemas',                                 \n\
                        'department_id': '2',                               \n\
                        'departments': {},                                  \n\
                        'managers': {},                                     \n\
                        'users': [],                                        \n\
                        '__md_treedb__': {                                  \n\
                        }                                                   \n\
                    },                                                      \n\
                    '7': {                                                  \n\
                        'id': '7',                                          \n\
                        'name': 'Desarrollo',                               \n\
                        'department_id': '2',                               \n\
                        'departments': {},                                  \n\
                        'managers': {},                                     \n\
                        'users': [],                                        \n\
                        '__md_treedb__': {                                  \n\
                        }                                                   \n\
                    }                                                       \n\
                },                                                          \n\
                'managers': {},                                             \n\
                'users': [],                                                \n\
                '__md_treedb__': {                                          \n\
                }                                                           \n\
            },                                                              \n\
            '3': {                                                          \n\
                'id': '3',                                                  \n\
                'name': 'Gestión',                                          \n\
                'department_id': '2',                                       \n\
                'departments': {},                                          \n\
                'managers': {},                                             \n\
                'users': [],                                                \n\
                '__md_treedb__': {                                          \n\
                }                                                           \n\
            },                                                              \n\
            '4': {                                                          \n\
                'id': '4',                                                  \n\
                'name': 'Microinformática',                                 \n\
                'department_id': '2',                                       \n\
                'departments': {},                                          \n\
                'managers': {},                                             \n\
                'users': [],                                                \n\
                '__md_treedb__': {                                          \n\
                }                                                           \n\
            },                                                              \n\
            '5': {                                                          \n\
                'id': '5',                                                  \n\
                'name': 'Redes',                                            \n\
                'department_id': '2',                                       \n\
                'departments': {},                                          \n\
                'managers': {},                                             \n\
                'users': [],                                                \n\
                '__md_treedb__': {                                          \n\
                }                                                           \n\
            },                                                              \n\
            '6': {                                                          \n\
                'id': '6',                                                  \n\
                'name': 'Sistemas',                                         \n\
                'department_id': '2',                                       \n\
                'departments': {},                                          \n\
                'managers': {},                                             \n\
                'users': [],                                                \n\
                '__md_treedb__': {                                          \n\
                }                                                           \n\
            },                                                              \n\
            '7': {                                                          \n\
                'id': '7',                                                  \n\
                'name': 'Desarrollo',                                       \n\
                'department_id': '2',                                       \n\
                'departments': {},                                          \n\
                'managers': {},                                             \n\
                'users': [],                                                \n\
                '__md_treedb__': {                                          \n\
                }                                                           \n\
            }                                                               \n\
        },                                                                  \n\
        'attributes': {},                                                   \n\
        'roles': {}                                                         \n\
    }                                                                       \n\
}                                                                           \n\
";

    if(!without_ok_tests) {
        const char *test = "departments: foto final";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            )
        );
        helper_quote2doublequote(foto_final);
        expected = legalstring2json(foto_final, TRUE);
        json_t *treedb = kw_get_dict(tranger, "treedbs", 0, 0);

        if(!match_record(treedb, expected)) {
            ret = FALSE;
            printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            if(verbose) {
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            }
        } else {
            if(!check_log_result(test)) {
                ret = FALSE;
            }
        }
        JSON_DECREF(expected);
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
    int without_bad_tests,
    int show_oks
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

    // TODO en verbose pintar OK\n, en no-verbose pintar ..... y punto rojo si error.

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

    static uint32_t mem_list[] = {0, 0};
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
    log_add_handler("test_capture", "testing", LOG_OPT_UP_WARNING, 0);
    log_add_handler(
        "test_stdout",
        "stdout",
        arguments.verbose?LOG_OPT_ALL:LOG_OPT_UP_WARNING|LOG_HND_OPT_BEATIFUL_JSON,
        0
    );

    expected_log_messages = json_array();
    unexpected_log_messages = json_array();

    /*------------------------------*
     *  La bbddd de pruebas
     *------------------------------*/
    char *path = "/test/treedb/db_test1";

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
    json_t *topic_cols_desc =_treedb_create_topic_cols_desc();
    if(!test_treedb_schema(
        tranger,
        topic_cols_desc,
        arguments.without_ok_tests,
        arguments.without_bad_tests,
        arguments.show_oks,
        arguments.verbose
    )) {
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
    treedb_open_db(
        tranger,  // owned
        treedb_name,
        jn_schema_sample,  // owned
        0
    );

    if(!check_log_result("clean start")) { // Be sure there is no previous error
        ret = -1;
    }

    /*------------------------------*
     *  Ejecuta los tests
     *------------------------------*/
    if(!test_schema(tranger, topic_cols_desc, treedb_name, arguments.verbose)) {
        ret = -1;
    }

    if(!test_departments_data(
            tranger,
            treedb_name,
            arguments.without_ok_tests,
            arguments.without_bad_tests,
            arguments.show_oks,
            arguments.verbose
        )) {
        ret = -1;
    }

    if(arguments.print_tranger) {
        print_json(tranger);
    } else if(arguments.print_treedb) {
        print_json(kw_get_dict(tranger, "treedbs", 0, 0));
    }

    /*------------------------------*
     *  Cierra la bbdd
     *------------------------------*/
    treedb_close_db(tranger, treedb_name);

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

