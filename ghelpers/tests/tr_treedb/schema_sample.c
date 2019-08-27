#pragma once

static char schema_sample[]= "\
{                                                                   \n\
    'topics': [                                                     \n\
        {                                                           \n\
            'topic_name': 'users',                                  \n\
            'pkey': 'id',                                           \n\
            'system_flag': 'sf_string_key',                         \n\
            'cols': [                                               \n\
                {                                                   \n\
                    'id': 'id',                                     \n\
                    'header': 'Id',                                 \n\
                    'type': 'string',                               \n\
                    'flag': ['persistent','required','uuid']        \n\
                },                                                  \n\
                {                                                   \n\
                    'id': 'username',                               \n\
                    'header': 'User Name',                          \n\
                    'type': 'string',                               \n\
                    'flag': ['persistent','required']               \n\
                },                                                  \n\
                {                                                   \n\
                    'id': 'firstName',                              \n\
                    'header': 'First Name',                         \n\
                    'type': 'string',                               \n\
                    'flag': ['persistent','required']               \n\
                },                                                  \n\
                {                                                   \n\
                    'id': 'lastName',                               \n\
                    'header': 'Last Name',                          \n\
                    'type': 'string',                               \n\
                    'flag': ['persistent','required']               \n\
                },                                                  \n\
                {                                                   \n\
                    'id': 'email',                                  \n\
                    'header': 'Email',                              \n\
                    'type': 'string',                               \n\
                    'flag': ['persistent','required']               \n\
                },                                                  \n\
                {                                                   \n\
                    'id': 'emailVerified',                          \n\
                    'header': 'Email Verified',                     \n\
                    'type': 'boolean',                              \n\
                    'flag': 'persistent'                            \n\
                },                                                  \n\
                {                                                   \n\
                    'id': 'disabled',                               \n\
                    'header': 'Disabled',                           \n\
                    'type': 'boolean',                              \n\
                    'flag': 'persistent'                            \n\
                },                                                  \n\
                {                                                   \n\
                    'id': 'online',                                 \n\
                    'header': 'On Line',                            \n\
                    'type': 'boolean',                              \n\
                    'flag': 'volatil'                               \n\
                },                                                  \n\
                {                                                   \n\
                    'id': 'department',                             \n\
                    'header': 'Department',                         \n\
                    'type': 'array',                                \n\
                    'flag': ['persistent'],                         \n\
                    'link': 'departments$id',                       \n\
                    'reverse': 'departments$id.users'               \n\
                },                                                  \n\
                {                                                   \n\
                    'id': 'attributes',                             \n\
                    'header': 'Attributes',                         \n\
                    'type': 'array',                                \n\
                    'flag': ['persistent','include'],               \n\
                    'include': ['attributes$id.attribute']          \n\
                },                                                  \n\
                {                                                   \n\
                    'id': 'roles',                                  \n\
                    'header': 'Roles',                              \n\
                    'type': 'array',                                \n\
                    'flag': ['persistent','include'],               \n\
                    'include': ['roles$id.role']                    \n\
                }                                                   \n\
            ]                                                       \n\
        },                                                          \n\
                                                                    \n\
        {                                                           \n\
            'topic_name': 'departments',                            \n\
            'pkey': 'id',                                           \n\
            'system_flag': 'sf_string_key',                         \n\
            'cols': [                                               \n\
                {                                                   \n\
                    'id': 'id',                                     \n\
                    'header': 'Id',                                 \n\
                    'type': 'string',                               \n\
                    'flag': ['persistent','required','uuid']        \n\
                },                                                  \n\
                {                                                   \n\
                    'id': 'name',                                   \n\
                    'header': 'Name',                               \n\
                    'type': 'string',                               \n\
                    'flag': ['persistent','required']               \n\
                },                                                  \n\
                                                                    \n\
                {                                                   \n\
                    'id': 'department_id',                          \n\
                    'header': 'Top Department',                     \n\
                    'type': 'string',                               \n\
                    'flag': ['persistent', 'fkey']                  \n\
                },                                                  \n\
                {                                                   \n\
                    'id': 'departments',                            \n\
                    'header': 'Departments',                        \n\
                    'type': 'array',                                \n\
                    'flag': ['volatil'],                            \n\
                    'link': {                                       \n\
                        'departments': 'id'                         \n\
                    },                                              \n\
                    'reverse': {                                    \n\
                        'departments': 'department_id'              \n\
                    }                                               \n\
                },                                                  \n\
                                                                    \n\
                {                                                   \n\
                    'id': 'managers',                               \n\
                    'header': 'Managers',                           \n\
                    'type': 'array',                                \n\
                    'flag': ['persistent'],                         \n\
                    'link': [                                       \n\
                        'users`id',                                 \n\
                        'departments`users'                         \n\
                    ]                                               \n\
                },                                                  \n\
                {                                                   \n\
                    'id': 'users',                                  \n\
                    'header': 'Users',                              \n\
                    'type': 'array',                                \n\
                    'flag': ['volatil'],                            \n\
                    'link': 'users$id',                             \n\
                    'reverse': 'users$id.department'                \n\
                }                                                   \n\
            ]                                                       \n\
        },                                                          \n\
                                                                    \n\
        {                                                           \n\
            'topic_name': 'attributes',                             \n\
            'pkey': 'id',                                           \n\
            'system_flag': 'sf_string_key',                         \n\
            'cols': [                                               \n\
                {                                                   \n\
                    'id': 'id',                                     \n\
                    'header': 'Id',                                 \n\
                    'type': 'string',                               \n\
                    'flag': ['persistent','required','uuid']        \n\
                },                                                  \n\
                {                                                   \n\
                    'id': 'attribute',                              \n\
                    'header': 'Attribute',                          \n\
                    'type': 'object',                               \n\
                    'flag': 'persistent'                            \n\
                },                                                  \n\
                {                                                   \n\
                    'id': 'attributes',                             \n\
                    'header': 'Attributes',                         \n\
                    'type': 'array',                                \n\
                    'flag': ['persistent'],                         \n\
                    'link': 'attributes$id'                         \n\
                }                                                   \n\
            ]                                                       \n\
        },                                                          \n\
                                                                    \n\
        {                                                           \n\
            'topic_name': 'roles',                                  \n\
            'pkey': 'id',                                           \n\
            'system_flag': 'sf_string_key',                         \n\
            'cols': [                                               \n\
                {                                                   \n\
                    'id': 'id',                                     \n\
                    'header': 'Id',                                 \n\
                    'type': 'string',                               \n\
                    'flag': ['persistent','required','uuid']        \n\
                },                                                  \n\
                {                                                   \n\
                    'id': 'role',                                   \n\
                    'header': 'Role',                               \n\
                    'type': 'object',                               \n\
                    'flag': 'persistent'                            \n\
                },                                                  \n\
                {                                                   \n\
                    'id': 'roles',                                  \n\
                    'header': 'Roles',                              \n\
                    'type': 'array',                                \n\
                    'flag': ['persistent'],                         \n\
                    'link': 'roles$id'                              \n\
                }                                                   \n\
            ]                                                       \n\
        }                                                           \n\
    ],                                                              \n\
    'indexes': [                                                    \n\
    ]                                                               \n\
}                                                                   \n\
";

