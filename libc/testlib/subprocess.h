#ifndef COSMOPOLITAN_LIBC_TESTLIB_SUBPROCESS_H_
#define COSMOPOLITAN_LIBC_TESTLIB_SUBPROCESS_H_
#include "libc/calls/calls.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/testlib/testlib.h"
COSMOPOLITAN_C_START_

/**
 * @fileoverview Test Subprocess Helper Macros
 *
 * These macros may be used to run unit tests in a subprocess. It's
 * particularly useful when testing code that causes the process to die:
 *
 *     TEST(term, test) {
 *       SPAWN(fork);
 *         raise(SIGUSR1);
 *       TERMS(SIGUSR1);
 *     }
 *
 * The default action for both EXITS() and TERMS() is _Exit(0), unless
 * an EXPECT_FOO() macro failed, in which case the status is non-zero.
 *
 *     TEST(my, test) {
 *       SPAWN(fork);
 *       // do stuff
 *       EXITS(0);
 *     }
 *
 * You can also nest subprocesses:
 *
 *     TEST(exit, test) {
 *       SPAWN(fork);
 *         SPAWN(vfork);
 *           _Exit(2);
 *         EXITS(2);
 *         _Exit(1);
 *       EXITS(1);
 *     }
 *
 * The above are shorthand for:
 *
 *     TEST(my, test) {
 *       SPAWN(fork);
 *       // communicate with parent
 *       PARENT();
 *       // communicate with child
 *       WAIT(exit, 0)
 *     }
 *
 * These macros cause a local variable named `child` with the child pid
 * to be defined.
 */

#define SPAWN(METHOD)                      \
  {                                        \
    int child, _failed = g_testlib_failed; \
    ASSERT_NE(-1, (child = METHOD()));     \
    if (!child) {

#define EXITS(CODE) \
  PARENT()          \
  WAIT(exit, CODE)

#define TERMS(SIG) \
  PARENT()         \
  WAIT(term, SIG)

#define PARENT()                                       \
  _Exit(MAX(0, MIN(255, g_testlib_failed - _failed))); \
  }

#define WAIT(KIND, CODE)                                         \
  testlib_waitfor##KIND(__FILE__, __LINE__, #CODE, CODE, child); \
  }

void testlib_waitforexit(const char *, int, const char *, int, int);
void testlib_waitforterm(const char *, int, const char *, int, int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TESTLIB_SUBPROCESS_H_ */
