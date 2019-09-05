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
 *
 ***************************************************************************/
PUBLIC BOOL test_users(
    json_t *tranger,
    const char *treedb_name,
    int without_ok_tests,
    int without_bad_tests,
    int show_oks,
    int verbose
)
{
    BOOL ret = TRUE;

    const char *path =
        "/yuneta/development/yuneta/^gobj-ecosistema/tests-g/ghelpers/tests/tr_treedb/users.json";

    size_t flags = 0;
    json_error_t error;
    json_t *jn_users = json_load_file(path, flags, &error);
    if(!jn_users) {
        printf("Can't decode user.json\n");
        return FALSE;
    }

    const char *test = "Load users";
    set_expected_results(
        test,
        json_pack("[]"  // error's list
        )
    );

    if(!check_log_result(test)) {
        ret = FALSE;
    }

    /*-----------------------------*
     *  First step: load users
     *-----------------------------*/
    int idx; json_t *jn_user;
    json_array_foreach(jn_users, idx, jn_user) {

//         json_t *data = json_pack("{s:s, s:s}",
//             "id", "administration",
//             "name", "Gestión"
//         );

        json_incref(jn_user);
        treedb_create_node(
            tranger, treedb_name,
            "users",
            jn_user
        );

    }

    /*-----------------------------------*
     *  Second step: assign department
     *-----------------------------------*/
    json_array_foreach(jn_users, idx, jn_user) {

    }

    json_decref(jn_users);

    return ret;
}

