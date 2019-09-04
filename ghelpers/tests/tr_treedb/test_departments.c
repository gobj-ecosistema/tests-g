/****************************************************************************
 *          test.c
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
PUBLIC BOOL test_departments_data(
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

