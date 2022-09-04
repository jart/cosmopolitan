#ifndef COSMOPOLITAN_LIBC_TESTLIB_SUBPROCESS_H_
#define COSMOPOLITAN_LIBC_TESTLIB_SUBPROCESS_H_
#include "libc/calls/calls.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/testlib/testlib.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
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
 */

#define SPAWN(METHOD)                     \
  {                                       \
    int _pid, _failed = g_testlib_failed; \
    ASSERT_NE(-1, (_pid = METHOD()));     \
    if (!_pid) {

#define EXITS(rc)                                         \
  _Exit(MAX(0, MIN(255, g_testlib_failed - _failed)));    \
  }                                                       \
  testlib_waitforexit(__FILE__, __LINE__, #rc, rc, _pid); \
  }

#define TERMS(sig)                                          \
  _Exit(MAX(0, MIN(255, g_testlib_failed - _failed)));      \
  }                                                         \
  testlib_waitforterm(__FILE__, __LINE__, #sig, sig, _pid); \
  }

void testlib_waitforexit(const char *, int, const char *, int, int);
void testlib_waitforterm(const char *, int, const char *, int, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TESTLIB_SUBPROCESS_H_ */
