/****************************************************************************
 *          test.c
 *
 *          Copyright (c) 2015 Niyamaka.
 *          All Rights Reserved.
 ****************************************************************************/
#include <argp.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <ginsfsm.h>
#include "c_test.h"

/***************************************************************************
 *      Constants
 ***************************************************************************/
#define MAX_CASE 4

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

    int seconds;
    int caso;
};

/***************************************************************************
 *              Prototypes
 ***************************************************************************/
static error_t parse_opt (int key, char *arg, struct argp_state *state);

/***************************************************************************
 *      Data
 ***************************************************************************/
/*---------------------------------------------*
 *      Attributes - order affect to oid's
 *---------------------------------------------*/
PRIVATE sdata_desc_t tattr_desc[] = {
/*-ATTR-type------------name----------------flag----------------default---------description---------- */
SDATA (ASN_OCTET_STR,   "database",         SDF_REQUIRED,       "agent.db",     "Database name"),
SDATA (ASN_INTEGER,     "timerStBoot",      0,                  4*1000,         "Timer to run yunos on boot"),
SDATA_END()
};


// Set by yuneta_entry_point()
// const char *argp_program_version = APP_NAME " " APP_VERSION;
// const char *argp_program_bug_address = APP_SUPPORT;

/* Program documentation. */
static char doc[] = "";

/* A description of the arguments we accept. */
static char args_doc[] = "";

/*
 *  The options we understand.
 *  See https://www.gnu.org/software/libc/manual/html_node/Argp-Option-Vectors.html
 */
static struct argp_option options[] = {
{"seconds",         's',    "SECONDS",    0,    "Repeat loop 's' seconds. Default: 1 ", 1},
{"caso",            'c',    "CASE",     0,      "Test case.", 1},
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
    case 's':
        if(arg) {
            arguments->seconds = atoi(arg);
        }
        break;

    case 'c':
        arguments->caso = atoi(arg);
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
void pretty_bytes(char* bf, int bfsize, uint64_t bytes)
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
static inline void test(int caso, int seconds)
{
    uint64_t st, et;

    /*-------------------------------------*
     *  Your start code
     *-------------------------------------*/
    log_startup("app", "1.0", "app");
    log_add_handler("test_stdout", "stdout", LOG_OPT_ALL|LOG_HND_OPT_TRACE_STACK, 0);
    gobj_start_up(0, 0, 0, 0, 0, 0, 0);

    // TODO need update to yuneta 3
    hgobj gobj = gobj_create_yuno("test", GCLASS_TEST, 0);

    uint64_t t = start_msectimer(seconds*1000);
    uint64_t count = 0;

    /*-------------------------------------*
     *  Loop
     *-------------------------------------*/
    char *scaso;
    if(caso > MAX_CASE) {
        caso = 1;
    }
    switch(caso) {
    default:
    case 1:
        {
            scaso = "gobj_send_event(gobj, \"EV_SAMPLE\", 0, gobj)";
            st = time_in_miliseconds();
            while(1) {
                gobj_send_event(gobj, "EV_SAMPLE", 0, gobj);
                count++;
                if(test_msectimer(t)) {
                    break;
                }
            }
            et = time_in_miliseconds();
        }
        break;
    case 2:
        {
            scaso = "gobj_send_event(gobj, \"EV_SAMPLE\", kw(sin pack), gobj)";
            st = time_in_miliseconds();
            while(1) {
                count++;
                json_t *kw = json_object();
                json_object_set_new(kw, "sample_str", json_string("agent.db"));
                json_object_set_new(kw, "sample_int", json_integer(4*1000));
                gobj_send_event(gobj, "EV_SAMPLE", kw, gobj);
                if(test_msectimer(t)) {
                    break;
                }
            }
            et = time_in_miliseconds();
        }
        break;
    case 3:
        {
            scaso = "gobj_send_event(gobj, \"EV_SAMPLE\", kw(con pack), gobj)";
            st = time_in_miliseconds();
            while(1) {
                count++;
                json_t *kw = json_pack("{s:s, s:I}",
                    "sample_str", "agent.db",
                    "sample_int", (json_int_t)4000
                );
                gobj_send_event(gobj, "EV_SAMPLE", kw, gobj);
                if(test_msectimer(t)) {
                    break;
                }
            }
            et = time_in_miliseconds();
        }
        break;
    case 4:
        {
            scaso = "gobj_send_event(gobj, \"EV_SAMPLE\", kw(con sdata), gobj)";
            st = time_in_miliseconds();
            while(1) {
                count++;
                hsdata hs = sdata_create(tattr_desc, 0, 0, 0, 0, 0);
                gobj_send_event(gobj, "EV_SAMPLE", 0, gobj);
                sdata_destroy(hs);
                if(test_msectimer(t)) {
                    break;
                }
            }
            et = time_in_miliseconds();
        }
        break;
    }

    /*-------------------------------------*
     *  Your end code
     *-------------------------------------*/
    gobj_destroy(gobj);
    gobj_shutdown();
    log_end();

    /*-------------------------------------*
     *  Print times
     *-------------------------------------*/
    char bf[128];
    pretty_bytes(bf, sizeof(bf), count);

    printf("test '%s' =>\n    transactions: %s/%.2f sec\n",
        scaso,
        bf,
        (float)(et-st)/1000
    );
}

/***************************************************************************
 *                      Main
 ***************************************************************************/
int main(int argc, char *argv[])
{
    struct arguments arguments;
    /*
     *  Default values
     */
    memset(&arguments, 0, sizeof(arguments));
    arguments.seconds = 1;
    /*
     *  Parse arguments
     */
    argp_parse (&argp, argc, argv, 0, 0, &arguments);
    if(arguments.caso==0) {
        for(int i=1; i<= MAX_CASE; i++) {
            test(i, arguments.seconds);
        }
    } else {
        test(arguments.caso, arguments.seconds);
    }

    return 0;
}

