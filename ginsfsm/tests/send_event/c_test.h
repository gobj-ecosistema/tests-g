/****************************************************************************
 *          C_TEST.H
 *          Test GClass.
 *
 *          Test
 *
 *          Copyright (c) 2016 Niyamaka.
 *          All Rights Reserved.
 ****************************************************************************/
#ifndef _C_TEST_H
#define _C_TEST_H 1

#include <yuneta.h>

#ifdef __cplusplus
extern "C"{
#endif

/**rst**
.. _test-gclass:

**"Test"** :ref:`GClass`
================================

Test

``GCLASS_TEST_NAME``
   Macro of the gclass string name, i.e **"Test"**.

``GCLASS_TEST``
   Macro of the :func:`gclass_test()` function.

**rst**/
PUBLIC GCLASS *gclass_test(void);

#define GCLASS_TEST_NAME "Test"
#define GCLASS_TEST gclass_test()


#ifdef __cplusplus
}
#endif

#endif
