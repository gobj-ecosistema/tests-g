/****************************************************************************
 *          test.c
 *
 *          Load topic with code.
 *
 *          Copyright (c) 2019 Niyamaka.
 *          All Rights Reserved.
 ****************************************************************************/
#include <argp.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <locale.h>
#include <time.h>

#include "test_tr_treedb.h"


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
PUBLIC BOOL test_departments(
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
    json_t *operation = 0;
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
            ),
            verbose
        );

        data = json_pack("{s:s, s:s}",
            "id", "direction",
            "name", "Dirección"
        );
        expected = json_pack("{s:s, s:s, s:s, s:{}, s:{}, s:[]}",
            "id", "direction",
            "name", "Dirección",
            "department_id", "",
            "departments",
            "managers",
            "users"

        );

        direction = treedb_create_node(
            tranger, treedb_name,       // treedb
            "departments",              // topic_name
            data,                       // data
            "strict"
        );
        if(!match_record(direction, expected)) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
                log_debug_json(0, direction, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
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
            ),
            verbose
        );

        data = json_pack("{s:s}",
            "name", "Administración"
        );
        expected = 0;

        found = treedb_create_node( // Must return 0
            tranger, treedb_name,
            "departments",
            data,                       // data
            "strict"
        );

        if(found) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR Administracion %s\n", On_Red BWhite, Color_Off);
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
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
            ),
            verbose
        );

        data = json_pack("{s:s, s:s}",
            "id", "administration",
            "name", "Administración"
        );
        expected = json_pack("{s:s, s:s, s:s, s:{}, s:{}, s:[]}",
            "id", "administration",
            "name", "Administración",
            "department_id", "",
            "departments",
            "managers",
            "users"
        );

        administration = treedb_create_node(
            tranger, treedb_name,
            "departments",
            data,
            "strict"
        );
        if(!match_record(administration, expected)) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
                log_debug_json(0, administration, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
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
            ),
            verbose
        );

        expected = json_pack("{s:s, s:s, s:s, s:{}, s:{}, s:[]}",
            "id", "administration",
            "name", "Administración",
            "department_id", "",
            "departments",
            "managers",
            "users"
        );

        found = treedb_get_node(
            tranger, treedb_name,
            "departments",
            json_string("administration")
        );
        if(!match_record(found, expected)) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
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
            ),
            verbose
        );
        treedb_link_nodes(
            tranger,
            "departmentsx",
            direction,
            administration
        );
        if(!check_log_result(test, verbose)) {
            ret = FALSE;
        }
    }

    if(!without_ok_tests) {
        const char *test = "link direction->administration, good";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            ),
            verbose
        );
        treedb_link_nodes(
            tranger,
            "departments",
            direction,
            administration
        );

        expected = json_pack("{s:s, s:s, s:s, s:{s:{s:s, s:s, s:s, s:{}, s:{}, s:[]}}, s:{}, s:[]}",
            "id", "direction",
            "name", "Dirección",
            "department_id", "",
            "departments",
                "administration",
                    "id", "administration",
                    "name", "Administración",
                    "department_id", "direction",
                    "departments",
                    "managers",
                    "users",
            "managers",
            "users"
        );

        found = treedb_get_node(
            tranger, treedb_name,
            "departments",
            json_string("direction")
        );
        if(!match_record(found, expected)) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
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
            ),
            verbose
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
            "id", "direction",
            "name", "Dirección",
            "department_id", "",
            "departments",
            "managers",
            "users"
        );
        if(!expected) {
            if(verbose) {
                printf("%s  --> expected NULL in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        }
        if(!match_record(found, expected)) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
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
            ),
            verbose
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
            "id", "administration",
            "name", "Administración",
            "department_id", "direction",
            "departments",
            "managers",
            "users"
        );
        if(!expected) {
            if(verbose) {
                printf("%s  --> expected NULL in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        }

        if(!match_record(found, expected)) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
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
        const char *test = "Create operation, wrong, duplicated key";
        set_expected_results(
            test,
            json_pack("[{s:s}]",  // error's list
                "msg", "Node already exists"
            ),
            verbose
        );

        data = json_pack("{s:s, s:s}",
            "id", "administration",
            "name", "Gestión"
        );

        operation = treedb_create_node(
            tranger, treedb_name,
            "departments",
            data,
            "strict"
        );
        if(operation) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
                log_debug_json(0, operation, "Record found");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
                ret = FALSE;
            }
        }
    }

    if(!without_ok_tests) {
        const char *test = "Create operation, good";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            ),
            verbose
        );

        data = json_pack("{s:s, s:s}",
            "id", "operation",
            "name", "Gestión"
        );
        expected = json_pack("{s:s, s:s, s:s, s:{}, s:{}, s:[]}",
            "id", "operation",
            "name", "Gestión",
            "department_id", "",
            "departments",
            "managers",
            "users"
        );

        operation = treedb_create_node(
            tranger, treedb_name,
            "departments",
            data,
            "strict"
        );
        if(!match_record(operation, expected)) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
                log_debug_json(0, operation, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
                ret = FALSE;
            }
        }
        if(show_oks) {
            log_debug_json(0, operation, "Record found");
            log_debug_json(0, expected, "Record expected");
        }
        JSON_DECREF(expected);
    }

    /*-------------------------------------*
     *      Administración -> Gestión
     *------------------------------------*/
    if(!without_ok_tests) {
        const char *test = "link administration->operation, good";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            ),
            verbose
        );
        treedb_link_nodes(
            tranger,
            "departments",
            administration,
            operation
        );

        expected = json_pack("{s:s, s:s, s:s, s:{s:{s:s, s:s, s:s, s:{}, s:{}, s:[]}}, s:{}, s:[]}",
            "id", "administration",
            "name", "Administración",
            "department_id", "direction",
            "departments",
                "operation",
                    "id", "operation",
                    "name", "Gestión",
                    "department_id", "administration",
                    "departments",
                    "managers",
                    "users",
            "managers",
            "users"
        );

        found = treedb_get_node(
            tranger, treedb_name,
            "departments",
            json_string("administration")
        );
        if(!match_record(found, expected)) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
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
            ),
            verbose
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
            "id", "operation",
            "name", "Gestión",
            "department_id", "administration",
            "departments",
            "managers",
            "users"
        );
        if(!expected) {
            if(verbose) {
                printf("%s  --> expected NULL in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        }

        if(!match_record(found, expected)) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
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
            ),
            verbose
        );

        data = json_pack("{s:s, s:s}",
            "id", "microinformatics",
            "name", "Microinformática"
        );
        expected = json_pack("{s:s, s:s, s:s, s:{}, s:{}, s:[]}",
            "id", "microinformatics",
            "name", "Microinformática",
            "department_id", "",
            "departments",
            "managers",
            "users"
        );

        microinformatics = treedb_create_node(
            tranger, treedb_name,
            "departments",
            data,
            "strict"
        );
        if(!match_record(microinformatics, expected)) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
                log_debug_json(0, microinformatics, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
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
            ),
            verbose
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
            "id", "administration",
            "name", "Administración",
            "department_id", "direction",
            "departments",
                "operation",
                    "id", "operation",
                    "name", "Gestión",
                    "department_id", "administration",
                    "departments",
                    "managers",
                    "users",
                "microinformatics",
                    "id", "microinformatics",
                    "name", "Microinformática",
                    "department_id", "administration",
                    "departments",
                    "managers",
                    "users",
            "managers",
            "users"
        );

        found = treedb_get_node(
            tranger, treedb_name,
            "departments",
            json_string("administration")
        );
        if(!match_record(found, expected)) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
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
            ),
            verbose
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
            "id", "microinformatics",
            "name", "Microinformática",
            "department_id", "administration",
            "departments",
            "managers",
            "users"
        );
        if(!expected) {
            if(verbose) {
                printf("%s  --> expected NULL in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        }

        if(!match_record(found, expected)) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
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
            ),
            verbose
        );

        data = json_pack("{s:s, s:s}",
            "id", "network",
            "name", "Redes"
        );
        expected = json_pack("{s:s, s:s, s:s, s:{}, s:{}, s:[]}",
            "id", "network",
            "name", "Redes",
            "department_id", "",
            "departments",
            "managers",
            "users"
        );

        network = treedb_create_node(
            tranger, treedb_name,
            "departments",
            data,
            "strict"
        );
        if(!match_record(network, expected)) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
                log_debug_json(0, network, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
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
            ),
            verbose
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
            "id", "administration",
            "name", "Administración",
            "department_id", "direction",
            "departments",
                "operation",
                    "id", "operation",
                    "name", "Gestión",
                    "department_id", "administration",
                    "departments",
                    "managers",
                    "users",
                "microinformatics",
                    "id", "microinformatics",
                    "name", "Microinformática",
                    "department_id", "administration",
                    "departments",
                    "managers",
                    "users",
                "network",
                    "id", "network",
                    "name", "Redes",
                    "department_id", "administration",
                    "departments",
                    "managers",
                    "users",
            "managers",
            "users"
        );

        found = treedb_get_node(
            tranger, treedb_name,
            "departments",
            json_string("administration")
        );
        if(!match_record(found, expected)) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
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
            ),
            verbose
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
            "id", "network",
            "name", "Redes",
            "department_id", "administration",
            "departments",
            "managers",
            "users"
        );
        if(!expected) {
            if(verbose) {
                printf("%s  --> expected NULL in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        }

        if(!match_record(found, expected)) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
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
            ),
            verbose
        );

        data = json_pack("{s:s, s:s}",
            "id", "systems",
            "name", "Sistemas"
        );
        expected = json_pack("{s:s, s:s, s:s, s:{}, s:{}, s:[]}",
            "id", "systems",
            "name", "Sistemas",
            "department_id", "",
            "departments",
            "managers",
            "users"
        );

        systems = treedb_create_node(
            tranger, treedb_name,
            "departments",
            data,
            "strict"
        );
        if(!match_record(systems, expected)) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
                log_debug_json(0, systems, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
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
            ),
            verbose
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
            "id", "administration",
            "name", "Administración",
            "department_id", "direction",
            "departments",
                "operation",
                    "id", "operation",
                    "name", "Gestión",
                    "department_id", "administration",
                    "departments",
                    "managers",
                    "users",
                "microinformatics",
                    "id", "microinformatics",
                    "name", "Microinformática",
                    "department_id", "administration",
                    "departments",
                    "managers",
                    "users",
                "network",
                    "id", "network",
                    "name", "Redes",
                    "department_id", "administration",
                    "departments",
                    "managers",
                    "users",
                "systems",
                    "id", "systems",
                    "name", "Sistemas",
                    "department_id", "administration",
                    "departments",
                    "managers",
                    "users",
            "managers",
            "users"
        );

        found = treedb_get_node(
            tranger, treedb_name,
            "departments",
            json_string("administration")
        );
        if(!match_record(found, expected)) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
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
            ),
            verbose
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
            "id", "systems",
            "name", "Sistemas",
            "department_id", "administration",
            "departments",
            "managers",
            "users"
        );
        if(!expected) {
            if(verbose) {
                printf("%s  --> expected NULL in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        }

        if(!match_record(found, expected)) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
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
            ),
            verbose
        );

        data = json_pack("{s:s, s:s}",
            "id", "development",
            "name", "Desarrollo"
        );
        expected = json_pack("{s:s, s:s, s:s, s:{}, s:{}, s:[]}",
            "id", "development",
            "name", "Desarrollo",
            "department_id", "",
            "departments",
            "managers",
            "users"
        );

        development = treedb_create_node(
            tranger, treedb_name,
            "departments",
            data,
            "strict"
        );
        if(!match_record(development, expected)) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
                log_debug_json(0, development, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
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
            ),
            verbose
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
            "id", "administration",
            "name", "Administración",
            "department_id", "direction",
            "departments",
                "operation",
                    "id", "operation",
                    "name", "Gestión",
                    "department_id", "administration",
                    "departments",
                    "managers",
                    "users",
                "microinformatics",
                    "id", "microinformatics",
                    "name", "Microinformática",
                    "department_id", "administration",
                    "departments",
                    "managers",
                    "users",
                "network",
                    "id", "network",
                    "name", "Redes",
                    "department_id", "administration",
                    "departments",
                    "managers",
                    "users",
                "systems",
                    "id", "systems",
                    "name", "Sistemas",
                    "department_id", "administration",
                    "departments",
                    "managers",
                    "users",
                "development",
                    "id", "development",
                    "name", "Desarrollo",
                    "department_id", "administration",
                    "departments",
                    "managers",
                    "users",
            "managers",
            "users"
        );

        found = treedb_get_node(
            tranger, treedb_name,
            "departments",
            json_string("administration")
        );
        if(!match_record(found, expected)) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
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
            ),
            verbose
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
            "id", "development",
            "name", "Desarrollo",
            "department_id", "administration",
            "departments",
            "managers",
            "users"
        );
        if(!expected) {
            if(verbose) {
                printf("%s  --> expected NULL in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        }

        if(!match_record(found, expected)) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
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
{\n\
    'treedb_test': {\n\
        '__tags__': {\n\
            'id': {}\n\
        },\n\
        'users': {\n\
            'id': {}\n\
        },\n\
        'departments': {\n\
            'id': {\n\
                'direction': {\n\
                    'id': 'direction',\n\
                    'name': 'Dirección',\n\
                    'department_id': '',\n\
                    'departments': {\n\
                        'administration': {\n\
                            'id': 'administration',\n\
                            'name': 'Administración',\n\
                            'department_id': 'direction',\n\
                            'departments': {\n\
                                'operation': {\n\
                                    'id': 'operation',\n\
                                    'name': 'Gestión',\n\
                                    'department_id': 'administration',\n\
                                    'departments': {},\n\
                                    'managers': {},\n\
                                    'users': [],\n\
                                    '__md_treedb__': {\n\
                                    }\n\
                                },\n\
                                'microinformatics': {\n\
                                    'id': 'microinformatics',\n\
                                    'name': 'Microinformática',\n\
                                    'department_id': 'administration',\n\
                                    'departments': {},\n\
                                    'managers': {},\n\
                                    'users': [],\n\
                                    '__md_treedb__': {\n\
                                    }\n\
                                },\n\
                                'network': {\n\
                                    'id': 'network',\n\
                                    'name': 'Redes',\n\
                                    'department_id': 'administration',\n\
                                    'departments': {},\n\
                                    'managers': {},\n\
                                    'users': [],\n\
                                    '__md_treedb__': {\n\
                                    }\n\
                                },\n\
                                'systems': {\n\
                                    'id': 'systems',\n\
                                    'name': 'Sistemas',\n\
                                    'department_id': 'administration',\n\
                                    'departments': {},\n\
                                    'managers': {},\n\
                                    'users': [],\n\
                                    '__md_treedb__': {\n\
                                    }\n\
                                },\n\
                                'development': {\n\
                                    'id': 'development',\n\
                                    'name': 'Desarrollo',\n\
                                    'department_id': 'administration',\n\
                                    'departments': {},\n\
                                    'managers': {},\n\
                                    'users': [],\n\
                                    '__md_treedb__': {\n\
                                    }\n\
                                }\n\
                            },\n\
                            'managers': {},\n\
                            'users': [],\n\
                            '__md_treedb__': {\n\
                            }\n\
                        }\n\
                    },\n\
                    'managers': {},\n\
                    'users': [],\n\
                    '__md_treedb__': {\n\
                    }\n\
                },\n\
                'administration': {\n\
                    'id': 'administration',\n\
                    'name': 'Administración',\n\
                    'department_id': 'direction',\n\
                    'departments': {\n\
                        'operation': {\n\
                            'id': 'operation',\n\
                            'name': 'Gestión',\n\
                            'department_id': 'administration',\n\
                            'departments': {},\n\
                            'managers': {},\n\
                            'users': [],\n\
                            '__md_treedb__': {\n\
                            }\n\
                        },\n\
                        'microinformatics': {\n\
                            'id': 'microinformatics',\n\
                            'name': 'Microinformática',\n\
                            'department_id': 'administration',\n\
                            'departments': {},\n\
                            'managers': {},\n\
                            'users': [],\n\
                            '__md_treedb__': {\n\
                            }\n\
                        },\n\
                        'network': {\n\
                            'id': 'network',\n\
                            'name': 'Redes',\n\
                            'department_id': 'administration',\n\
                            'departments': {},\n\
                            'managers': {},\n\
                            'users': [],\n\
                            '__md_treedb__': {\n\
                            }\n\
                        },\n\
                        'systems': {\n\
                            'id': 'systems',\n\
                            'name': 'Sistemas',\n\
                            'department_id': 'administration',\n\
                            'departments': {},\n\
                            'managers': {},\n\
                            'users': [],\n\
                            '__md_treedb__': {\n\
                            }\n\
                        },\n\
                        'development': {\n\
                            'id': 'development',\n\
                            'name': 'Desarrollo',\n\
                            'department_id': 'administration',\n\
                            'departments': {},\n\
                            'managers': {},\n\
                            'users': [],\n\
                            '__md_treedb__': {\n\
                            }\n\
                        }\n\
                    },\n\
                    'managers': {},\n\
                    'users': [],\n\
                    '__md_treedb__': {\n\
                    }\n\
                },\n\
                'operation': {\n\
                    'id': 'operation',\n\
                    'name': 'Gestión',\n\
                    'department_id': 'administration',\n\
                    'departments': {},\n\
                    'managers': {},\n\
                    'users': [],\n\
                    '__md_treedb__': {\n\
                    }\n\
                },\n\
                'microinformatics': {\n\
                    'id': 'microinformatics',\n\
                    'name': 'Microinformática',\n\
                    'department_id': 'administration',\n\
                    'departments': {},\n\
                    'managers': {},\n\
                    'users': [],\n\
                    '__md_treedb__': {\n\
                    }\n\
                },\n\
                'network': {\n\
                    'id': 'network',\n\
                    'name': 'Redes',\n\
                    'department_id': 'administration',\n\
                    'departments': {},\n\
                    'managers': {},\n\
                    'users': [],\n\
                    '__md_treedb__': {\n\
                    }\n\
                },\n\
                'systems': {\n\
                    'id': 'systems',\n\
                    'name': 'Sistemas',\n\
                    'department_id': 'administration',\n\
                    'departments': {},\n\
                    'managers': {},\n\
                    'users': [],\n\
                    '__md_treedb__': {\n\
                    }\n\
                },\n\
                'development': {\n\
                    'id': 'development',\n\
                    'name': 'Desarrollo',\n\
                    'department_id': 'administration',\n\
                    'departments': {},\n\
                    'managers': {},\n\
                    'users': [],\n\
                    '__md_treedb__': {\n\
                    }\n\
                }\n\
            }\n\
        },\n\
        'attributes': {\n\
            'id': {}\n\
        },\n\
        'roles': {\n\
            'id': {}\n\
        }\n\
    }\n\
}\n\
";

    if(!without_ok_tests) {
        const char *test = "departments: foto final";
        set_expected_results(
            test,
            json_pack("[]"  // error's list
            ),
            verbose
        );
        helper_quote2doublequote(foto_final);
        expected = legalstring2json(foto_final, TRUE);
        json_t *treedb = kw_get_dict(tranger, "treedbs", 0, 0);

        if(!match_record(treedb, expected)) {
            ret = FALSE;
            if(verbose) {
                printf("%s  --> ERROR in test: '%s'%s\n", On_Red BWhite, test, Color_Off);
                log_debug_json(0, found, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%s.%s", On_Red BWhite, Color_Off);
            }
        } else {
            if(!check_log_result(test, verbose)) {
                ret = FALSE;
            }
        }
        JSON_DECREF(expected);
    }

    return ret;
}

