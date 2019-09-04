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
    json_t *errors_list
);

PUBLIC BOOL match_record(
    json_t *record_, // NOT owned
    json_t *expected_ // NOT owned
);
PUBLIC BOOL check_log_result(const char *test);

PUBLIC BOOL test_departments_data(
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

