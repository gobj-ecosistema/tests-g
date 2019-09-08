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
PUBLIC int load_topic_new_data(
    json_t *tranger,
    const char *treedb_name,
    const char *topic_name,
    json_t *new_data // owned
)
{
    int ret = 0;

    /*--------------------------------------------------------------*
     *  load topic records except hook fields content
     *--------------------------------------------------------------*/
    int idx; json_t *new_record;
    json_array_foreach(new_data, idx, new_record) {
        JSON_INCREF(new_record);
        json_t *record = treedb_create_node( // Return is NOT YOURS
            tranger,
            treedb_name,
            topic_name,
            new_record,
            ""
        );
        if(!record) {
            log_error(0,
                "gobj",         "%s", __FILE__,
                "function",     "%s", __FUNCTION__,
                "msgset",       "%s", MSGSET_PARAMETER_ERROR,
                "msg",          "%s", "treedb_create_node() FAILED",
                "treedb_name",  "%s", treedb_name,
                "topic_name",   "%s", topic_name,
                NULL
            );
            ret += -1;
        }

        // Override loading id by id set by treedb
        if(record) {
            json_object_set_new(
                new_record,
                "id",
                json_string(json_string_value(json_object_get(record, "id")))
            );
        } else {
            json_object_set_new(new_record, "id", json_null()); // Mark wrong record
        }
    }

    /*--------------------------------------------------------------*
     *  load topic hooks
     *--------------------------------------------------------------*/
    /*
     *  Loop desc cols searching fkey
     */
    json_t *cols= kwid_new_dict("", tranger, "topics`%s`cols", topic_name);

    const char *col_name; json_t *col;
    json_object_foreach(cols, col_name, col) {
        json_t *fkey = kwid_get("", col, "fkey");
        if(!fkey) {
            continue;
        }
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
                /*
                 *  In "id" of new_record there we put the "true" id of child record
                 */
                json_t *child_id = json_object_get(new_record, "id");
                if(!child_id || json_is_null(child_id)) {
                    log_error(0,
                        "gobj",         "%s", __FILE__,
                        "function",     "%s", __FUNCTION__,
                        "msgset",       "%s", MSGSET_PARAMETER_ERROR,
                        "msg",          "%s", "Record without id",
                        "treedb_name",  "%s", treedb_name,
                        "topic_name",   "%s", topic_name,
                        "new_record",   "%j", new_record,
                        NULL
                    );
                    ret += -1;
                    continue;
                }

                /*
                 *  Get child node
                 */
                JSON_INCREF(child_id);
                json_t *child_record = treedb_get_node( // Return is NOT YOURS
                    tranger,
                    treedb_name,
                    topic_name,
                    child_id
                );
                if(!child_record) {
                    log_error(0,
                        "gobj",         "%s", __FILE__,
                        "function",     "%s", __FUNCTION__,
                        "msgset",       "%s", MSGSET_PARAMETER_ERROR,
                        "msg",          "%s", "Node not found",
                        "treedb_name",  "%s", treedb_name,
                        "topic_name",   "%s", topic_name,
                        "id",           "%j", child_id,
                        NULL
                    );
                    ret += -1;
                    continue;
                }

                /*
                 *  Get ids from new_record fkey field
                 */
                json_t *ids = kwid_get_new_ids(new_record, col_name);

                int ids_idx; json_t *jn_id;
                json_array_foreach(ids, ids_idx, jn_id) {
                    JSON_INCREF(jn_id);
                    json_t *parent_record = treedb_get_node( // Return is NOT YOURS
                        tranger,
                        treedb_name,
                        parent_topic_name,
                        jn_id
                    );
                    if(!parent_record) {
                        log_error(0,
                            "gobj",         "%s", __FILE__,
                            "function",     "%s", __FUNCTION__,
                            "msgset",       "%s", MSGSET_PARAMETER_ERROR,
                            "msg",          "%s", "Node not found",
                            "treedb_name",  "%s", treedb_name,
                            "topic_name",   "%s", parent_topic_name,
                            "id",           "%j", jn_id,
                            NULL
                        );
                        ret += -1;
                        continue;
                    }

                    if(treedb_link_nodes(
                        tranger,
                        hook_name,
                        parent_record,    // not owned
                        child_record      // not owned
                    )<0) {
                        log_error(0,
                            "gobj",         "%s", __FILE__,
                            "function",     "%s", __FUNCTION__,
                            "msgset",       "%s", MSGSET_PARAMETER_ERROR,
                            "msg",          "%s", "treedb_link_nodes() FAILED",
                            "treedb_name",  "%s", treedb_name,
                            "hook_name",    "%s", hook_name,
                            "parent_record","%j", parent_record,
                            "child_record", "%j", child_record,
                            NULL
                        );
                        ret += -1;
                    }
                }

                JSON_DECREF(ids);
            }
        }
    }
    JSON_DECREF(cols);
    JSON_DECREF(new_data);

    return ret;
}

/***************************************************************************
 *
 ***************************************************************************/
PUBLIC int load_treedbs(
    json_t *tranger,
    json_t *jn_treedbs, // owned
    const char *operation
)
{
    int ret = 0;

    /*--------------------------------------------------------*
     *  Firt step:  Check file json
     *      first key level must be the treedb name
     *      second key level must be the topic_name
     *      third level are the rows, can be a list or a dict.
     *--------------------------------------------------------*/
    const char *treedb_name; json_t *jn_treedb;
    json_object_foreach(jn_treedbs, treedb_name, jn_treedb) {
        json_t *treedb = kwid_get("", tranger, "treedbs`%s", treedb_name);
        if(!treedb) {
            log_error(0,
                "gobj",         "%s", __FILE__,
                "function",     "%s", __FUNCTION__,
                "msgset",       "%s", MSGSET_PARAMETER_ERROR,
                "msg",          "%s", "Treedb not found in tranger",
                "treedb_name",  "%s", treedb_name,
                NULL
            );
            ret += -1;
            continue;
        }

        const char *topic_name; json_t *jn_new_data;
        json_object_foreach(jn_treedb, topic_name, jn_new_data) {
            json_t *topic_desc = kwid_get("", tranger, "topics`%s", topic_name);
            if(!topic_desc) {
                log_error(0,
                    "gobj",         "%s", __FILE__,
                    "function",     "%s", __FUNCTION__,
                    "msgset",       "%s", MSGSET_PARAMETER_ERROR,
                    "msg",          "%s", "Topic not found in treedb",
                    "treedb_name",  "%s", treedb_name,
                    "topic_name",   "%s", topic_name,
                    NULL
                );
                ret += -1;
                continue;
            }

            json_t *current_data = treedb_get_index(tranger, treedb_name, topic_name, "id");
            json_int_t current_data_size = json_object_size(current_data);

            if(strcmp(operation, "update")==0) {
                log_error(0,
                    "gobj",         "%s", __FILE__,
                    "function",     "%s", __FUNCTION__,
                    "msgset",       "%s", MSGSET_PARAMETER_ERROR,
                    "msg",          "%s", "update operation not implemented",
                    "treedb_name",  "%s", treedb_name,
                    "topic_name",   "%s", topic_name,
                    NULL
                );
                ret += -1;
                continue;
            } else { // "create"
                if(current_data_size>0) {
                    log_error(0,
                        "gobj",         "%s", __FILE__,
                        "function",     "%s", __FUNCTION__,
                        "msgset",       "%s", MSGSET_PARAMETER_ERROR,
                        "msg",          "%s", "Topic data NOT EMPTY",
                        "treedb_name",  "%s", treedb_name,
                        "topic_name",   "%s", topic_name,
                        NULL
                    );
                    ret += -1;
                    continue;
                }
                json_t *new_data = kwid_new_list("", jn_treedb, topic_name);
                ret = load_topic_new_data(tranger, treedb_name, topic_name, new_data);
            }
        }
    }

    JSON_DECREF(jn_treedbs);
    return ret;
}

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
    "/yuneta/development/yuneta/^gobj-ecosistema/tests-g/ghelpers/tests/tr_treedb/users.one.json";
    //"/yuneta/development/yuneta/^gobj-ecosistema/tests-g/ghelpers/tests/tr_treedb/users.full.json";

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
        ),
        verbose
    );

    const char *operation = "create"; // "update" TODO

    json_t *jn_treedbs = kw_get_dict(file_json, "treedbs", 0, 0);
    if(!jn_treedbs) {
        printf("First level keyword 'treedbs' not found in this file\n  %s\n", path);
        JSON_DECREF(file_json);
        return FALSE;
    }

    JSON_INCREF(jn_treedbs);
    if(load_treedbs(tranger, jn_treedbs, operation)<0) {
        ret = FALSE;
    }

    kw_check_refcounts(file_json, 1000);
    kw_check_refcounts(tranger, 1000);

    JSON_DECREF(file_json);

    if(!check_log_result(test, verbose)) {
        ret = FALSE;
    }

    return ret;
}

