/****************************************************************************
 *          tests
 *          Copyright (c) 2019 Niyamaka.
 *          All Rights Reserved.
 ****************************************************************************/
#pragma once

#include <ghelpers.h>

#ifdef __cplusplus
extern "C"{
#endif

PUBLIC void set_expected_results(
    const char *name,
    json_t *errors_list,
    int verbose
);

PUBLIC BOOL match_record(
    json_t *record_, // NOT owned
    json_t *expected_ // NOT owned
);
PUBLIC BOOL check_log_result(const char *test, int verbose);

PUBLIC BOOL test_departments(
    json_t *tranger,
    const char *treedb_name,
    int without_ok_tests,
    int without_bad_tests,
    int show_oks,
    int verbose
);

PUBLIC BOOL test_users(
    json_t *tranger,
    const char *treedb_name,
    int without_ok_tests,
    int without_bad_tests,
    int show_oks,
    int verbose
);

#ifdef __cplusplus
}
#endif

