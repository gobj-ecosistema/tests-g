/****************************************************************************
 *          test.c
 *
 *          Load topic from external json file.
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
    json_t *file_json = json_load_file(path, flags, &error);
    if(!file_json) {
        printf("Can't decode %s json file\n", path);
        return FALSE;
    }

    const char *test = "Load users from a treedb-json-file";
    set_expected_results(
        test,
        json_pack("[]"  // error's list
        )
    );

    if(!check_log_result(test)) {
        ret = FALSE;
    }

    /*--------------------------------------------------------*
     *  Firt step:  Check file json
     *      first key level must be the treedb name
     *      second key level must be the topic_name
     *      third level are the rows, can be a list or a dict.
     *--------------------------------------------------------*/
    json_t *jn_treedbs = kw_get_dict(file_json, "treedbs", 0, 0);
    if(!jn_treedbs) {
        printf("First level keyword 'treedbs' not found in this file\n  %s\n", path);
        json_decref(file_json);
        return FALSE;
    }
    const char *treedb_name_; json_t *jn_treedb;
    json_object_foreach(jn_treedbs, treedb_name_, jn_treedb) {
        json_t *treedb = kwid_get("", tranger, "treedbs`%s", treedb_name_);
        if(!treedb) {
            printf("Treedb '%s' not exists in this file\n  %s\n", treedb_name_, path);
            ret = FALSE;
            continue;
        }

        /*--------------------------------------------------------------*
         *  Second step: load topic records except hook fields content
         *      load users
         *--------------------------------------------------------------*/
        const char *operation = "create"; // "update" TODO
        const char *topic_name; json_t *jn_new_data;
        json_object_foreach(jn_treedb, topic_name, jn_new_data) {
            json_t *topic_desc = kwid_get("", tranger, "topics`%s", topic_name);
            if(!topic_desc) {
                printf("Topic '%s' of the Treedb '%s' not found in this file\n  %s\n",
                    topic_name,
                    treedb_name_,
                    path
                );
                ret = FALSE;
                continue;
            }

            json_t *current_data = treedb_get_index(tranger, treedb_name, topic_name, "id");
            json_int_t current_data_size = json_object_size(current_data);

            if(strcmp(operation, "update")==0) {
                printf("update operation not implemented\n");
                ret = FALSE;
                continue;
            } else { // "create"
                if(current_data_size>0) {
                    printf("Cannot load (create) topic data to topic '%s' of the Treedb '%s': FOUND DATA!\n  %s\n",
                        topic_name,
                        treedb_name_,
                        path
                    );
                    ret = FALSE;
                    continue;
                }

                json_t *new_data = kwid_new_list("", jn_treedb, topic_name);
                int idx; json_t *record;
                json_array_foreach(new_data, idx, record) {
                    JSON_INCREF(record);
                    if(!treedb_create_node( // Return is NOT YOURS
                        tranger,
                        treedb_name_,
                        topic_name,
                        record
                    )) {
                        printf("Cannot load (create) topic data to topic '%s' of the Treedb '%s': FOUND DATA!\n  %s\n",
                            topic_name,
                            treedb_name_,
                            path
                        );
                        ret = FALSE;
                    }
                }
                JSON_DECREF(new_data);

            }
        }
    }

    json_decref(file_json);
    return ret;
}

