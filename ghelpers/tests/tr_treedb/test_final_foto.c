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
 *
 ***************************************************************************/
PUBLIC BOOL test_final_foto(
    json_t *tranger,
    const char *treedb_name,
    int without_ok_tests,
    int without_bad_tests,
    int show_oks,
    int verbose
)
{
    BOOL ret = TRUE;
    json_t *expected = 0;
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
                log_debug_json(0, treedb, "Record found");
                log_debug_json(0, expected, "Record expected");
            } else {
                printf("%sX%s", On_Red BWhite, Color_Off);
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

