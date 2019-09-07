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

    const char *operation = "create"; // "update" TODO

    /*--------------------------------------------------------*
     *  Firt step:  Check file json
     *      first key level must be the treedb name
     *      second key level must be the topic_name
     *      third level are the rows, can be a list or a dict.
     *--------------------------------------------------------*/
    json_t *jn_treedbs = kw_get_dict(file_json, "treedbs", 0, 0);
    if(!jn_treedbs) {
        printf("First level keyword 'treedbs' not found in this file\n  %s\n", path);
        JSON_DECREF(file_json);
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
                int idx; json_t *new_record;
                json_array_foreach(new_data, idx, new_record) {
                    JSON_INCREF(new_record);
                    json_t *record = treedb_create_node( // Return is NOT YOURS
                        tranger,
                        treedb_name_,
                        topic_name,
                        new_record,
                        ""
                    );
                    if(!record) {
                        printf("Cannot load (create) topic data to topic '%s' of the Treedb '%s': FOUND DATA!\n  %s\n",
                            topic_name,
                            treedb_name_,
                            path
                        );
                        ret = FALSE;
                    }
                    // Overrride loading id by id set by treedb
                    if(record) {
                        json_object_set(new_record, "id", json_object_get(record, "id"));
                    } else {
                        json_object_set_new(new_record, "id", json_null()); // Mark wrong record
                    }
                }
                JSON_DECREF(new_data);
            }
        }

        /*--------------------------------------------------------------*
         *  Third step: load topic hooks
         *      assign users to their departments
         *--------------------------------------------------------------*/
        json_object_foreach(jn_treedb, topic_name, jn_new_data) {
            json_t *topic_desc = kwid_get("", tranger, "topics`%s", topic_name);
            if(!topic_desc) {
                // Error already logged
                continue;
            }

            if(strcmp(operation, "update")==0) {
                printf("update operation not implemented\n");
                ret = FALSE;
                continue;

            } else { // "create"
                /*
                 *  Loop desc cols searching fkey
                 */
                json_t *cols= kwid_new_dict("", tranger, "topics`%s`cols", topic_name);
                json_t *new_data = kwid_new_list("", jn_treedb, topic_name);

                const char *col_name; json_t *col;
                json_object_foreach(cols, col_name, col) {
                    json_t *fkey = kwid_get("", col, "fkey");
                    if(!fkey) {
                        continue;
                    }
print_json(col);
                    /*
                     *  Loop desc fkey searching fkeys
                     */
                    const char *parent_topic_name; json_t *jn_parent_field_name;
                    json_object_foreach(fkey, parent_topic_name, jn_parent_field_name) {
                        const char *hook_name = json_string_value(jn_parent_field_name);

                        /*
                         *  Loop new data searching links
                         */
                        int new_record_idx; json_t *new_record;
                        json_array_foreach(new_data, new_record_idx, new_record) {
// print_json(new_record);

                            /*
                             *  In "id" of new_record there we put the "true" id of child record
                             */
                            json_t *child_id = json_object_get(new_record, "id");
                            if(!child_id || json_is_null(child_id)) {
                                continue;
                            }

                            /*
                             *  Get child node
                             */
                            json_t *child_record = treedb_get_node( // Return is NOT YOURS
                                tranger,
                                treedb_name_,
                                topic_name,
                                child_id
                            );
                            if(!child_record) {
                                printf("Cannot load child topic '%s' id '%s' of the Treedb '%s'\n  %s\n",
                                    topic_name,
                                    json_string_value(child_id),
                                    treedb_name_,
                                    path
                                );
                                ret = FALSE;
                                continue;
                            }

                            /*
                             *  Get ids from new_record fkey field
                             */
                            json_t *ids = kwid_get_new_ids(new_record, col_name);
// print_json(ids);
                            int ids_idx; json_t *jn_id;
                            json_array_foreach(ids, ids_idx, jn_id) {
                                json_t *parent_record = treedb_get_node( // Return is NOT YOURS
                                    tranger,
                                    treedb_name_,
                                    parent_topic_name,
                                    jn_id
                                );
                                if(!parent_record) {
                                    printf("Cannot get parent topic '%s' id '%s' of the Treedb '%s'\n  %s\n",
                                        parent_topic_name,
                                        json_string_value(jn_id),
                                        treedb_name_,
                                        path
                                    );
                                    ret = FALSE;
                                    continue;
                                }

// print_json(parent_record);
// print_json(child_record);
                                if(treedb_link_nodes(
                                    tranger,
                                    hook_name,
                                    parent_record,    // not owned
                                    child_record      // not owned
                                )<0) {
                                    printf("Cannot link nodes topic '%s' id '%s' of the Treedb '%s'\n  %s\n",
                                        topic_name,
                                        json_string_value(child_id),
                                        treedb_name_,
                                        path
                                    );
                                    ret = FALSE;
                                }
                            }

                            JSON_DECREF(ids);
                        }
                    }
                }
                JSON_DECREF(cols);
                JSON_DECREF(new_data);
            }
        }
    }

    JSON_DECREF(file_json);
    return ret;
}

