/*
** File: coveragetest_common.h
**
** Purpose:
** Common definitions for all coverage tests
*/


#ifndef _ZMQ_IFACE_COVERAGETEST_COMMON_H_
#define _ZMQ_IFACE_COVERAGETEST_COMMON_H_

/*
 * Includes
 */

#include <utassert.h>
#include <uttest.h>
#include <utstubs.h>

#include <cfe.h>
#include <zmq_iface.h>

/*
 * Macro to call a function and check its int32 return code
 */
#define UT_TEST_FUNCTION_RC(func,exp)           \
{                                               \
    int32 rcexp = exp;                          \
    int32 rcact = func;                         \
    UtAssert_True(rcact == rcexp, "%s (%ld) == %s (%ld)",   \
        #func, (long)rcact, #exp, (long)rcexp);             \
}

/*
 * Macro to add a test case to the list of tests to execute
 */
#define ADD_TEST(test) UtTest_Add((Test_ ## test),ZMQ_IFACE_UT_Setup,ZMQ_IFACE_UT_TearDown, #test)

/*
 * Setup function prior to every test
 */
void ZMQ_IFACE_UT_Setup(void);

/*
 * Teardown function after every test
 */
void ZMQ_IFACE_UT_TearDown(void);

#endif
