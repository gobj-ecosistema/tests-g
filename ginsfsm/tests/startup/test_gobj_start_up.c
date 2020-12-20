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

/***************************************************************************
 *      Constants
 ***************************************************************************/

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

    int repeat;
    int caso;
};

/***************************************************************************
 *              Prototypes
 ***************************************************************************/
static error_t parse_opt (int key, char *arg, struct argp_state *state);

/***************************************************************************
 *      Data
 ***************************************************************************/

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
{"repeat",          'r',    "TIMES",    0,      "Repeat execution 'repeat' times. Set -1 to infinite loop. Default: 1000000", 1},
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
    case 'r':
        if(arg) {
            arguments->repeat = atoi(arg);
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

    /*-------------------------------------*
     *  Loop
     *-------------------------------------*/
    switch(flags) {
    case 0:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);
            gobj_start_up(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            for (i = 0; i < cnt; i++) {
            }
            gobj_shutdown();
            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;
    case 1:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);
            for (i = 0; i < cnt; i++) {
                gobj_start_up(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                gobj_shutdown();
            }
            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;
    }


    /*-------------------------------------*
     *  Your end code
     *-------------------------------------*/

    /*-------------------------------------*
     *  Print times
     *-------------------------------------*/
    char bf[128];
    pretty_bytes(bf, sizeof(bf), cnt);

    dt = ts_diff (st, et);
    printf("# test(%s): %lu.%lus\n",
          bf,
          dt.tv_sec, dt.tv_nsec);
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
    arguments.repeat = 100000;
    /*
     *  Parse arguments
     */
    argp_parse (&argp, argc, argv, 0, 0, &arguments);
    test(arguments.caso, arguments.repeat);

    return 0;
}

