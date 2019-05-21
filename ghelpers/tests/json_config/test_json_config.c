/****************************************************************************
 *          TEST_JSON_CONFIG.C
 *
 *          Copyright (c) 2015 Niyamaka.
 *          All Rights Reserved.
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include <jansson.h>
#include <03_json_config.h>

/***************************************************************************
 *      Constants
 ***************************************************************************/
#define VERSION "1.0"

/***************************************************************************
 *      Structures
 ***************************************************************************/
/* Used by main to communicate with parse_opt. */
struct arguments
{
    int log_oids;
    int start;
    int stop;
    int install;
    int use_config_file;
    int use_extra_config_file;
    int print_verbose_config;
    int print_final_config;
    int print_role;
    const char *config_json_file;
    const char *parameter_config;
};

/***************************************************************************
 *      Prototypes
 ***************************************************************************/
static error_t parse_opt(int key, char *arg, struct argp_state *state);

/***************************************************************************
 *                      Default config
 ***************************************************************************/
PRIVATE char fixed_config[]= "\
{                                                       \n\
    'fixed_config': {                                   \n\
        'key_not_writable1': 'you cannot modify this'   \n\
    }                                                   \n\
}                                                       \n\
";
PRIVATE char variable_config[]= "\
{                                                       \n\
    'variable_config': {                                \n\
        'list': [1,2,3,4,5],                            \n\
        'key_writable1': 'you can- modify this',        \n\
        'key_writable2': {                              \n\
            'key_writable3': 'you can- modify this'     \n\
        }                                               \n\
    }                                                   \n\
}                                                       \n\
";

/***************************************************************************
 *      Data
 ***************************************************************************/
const char *argp_program_version = "test " VERSION;
const char *argp_program_bug_address = "<niyamaka@yuneta.io>";

/* Program documentation. */
static char doc[] =
  "test -- testing json_config";

/* A description of the arguments we accept. */
static char args_doc[] = "[{json config}]";

/* The options we understand. */
static struct argp_option options[] = {
{"start",               'S',    0,      0,  "start the program (as daemon)"},
{"stop",                'K',    0,      0,  "stop the program (as daemon)"},
{"install",             'i',    0,      0,  "install the yuno in the yuneta deploy realm"},
{"config-file",         'f',    "FILE", 0,  "load settings from json config file or [files]"},
{"extra-config-file",   'e',    "FILE", 0,  "load extra settings json config or [files]"},
{"print-config",        'p',    0,      0,  "print the final json config"},
{"print-verbose-config",'P',    0,      0,  "print verbose json config"},
{"log-oids",            'o',    0,      0,  "log the mib oid's"},
{"version",             'v',    0,      0,  "print program version"},
{"role",                'r',    0,      0,  "print role name"},
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
 *  Parser
 ***************************************************************************/
static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
    /*
     *  Get the input argument from argp_parse,
     *  which we know is a pointer to our arguments structure.
     */
    struct arguments *arguments = state->input;

    switch (key) {
    case 'o':
        arguments->log_oids = 1;
        break;
    case 'S':
        arguments->start = 1;
        break;
    case 'K':
        arguments->stop = 1;
        break;
    case 'i':
        arguments->install = 1;
        break;
    case 'v':
        //printf("%s %s\n", app_version, app_datetime);
        exit(0);
        break;
    case 'r':
        arguments->print_role = 1;
        break;
    case 'f':
        arguments->config_json_file = arg;
        arguments->use_config_file = 1;
        break;
    case 'p':
        arguments->print_final_config = 1;
        break;

    case 'P':
        arguments->print_verbose_config = 1;
        break;

    case ARGP_KEY_ARG:
        if (state->arg_num >= 1) {
            /* Too many arguments. */
            argp_usage (state);
        }
        arguments->parameter_config = arg;
        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
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

    /*
     *  Parse arguments
     */
    argp_parse (&argp, argc, argv, 0, 0, &arguments);

    helper_quote2doublequote(fixed_config);
    helper_quote2doublequote(variable_config);

    while(1) {
        char *final_sconfig = json_config(
            arguments.print_verbose_config,     // WARNING if true will exit(0)
            arguments.print_final_config,       // WARNING if true will exit(0)
            fixed_config,
            variable_config,
            arguments.use_config_file? arguments.config_json_file: 0,
            arguments.parameter_config,
            0
        );
        if(final_sconfig) {
            free(final_sconfig);
        } else {
            printf("ERROR json_config()\n");
            exit(-1);
        }
    }

    exit(0);
}
/*
void log_error(int opt, ...)
{
    printf("ERROR\n");
}*/
