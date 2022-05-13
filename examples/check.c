#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/log/check.h"

/**
 * @fileoverview Check Macros
 *
 * The simplest assertion is:
 *
 *     assert(123 == x);
 *
 * This has some downsides:
 *
 * 1. It's nice to know what `x` is when it crashes
 * 2. It's sometimes nice to have the check always take effect.
 * 3. It's problematic that assert() can't do __builtin_assume()
 *
 * Cosmopolitan provides alternative macros like:
 *
 * - `CHECK(EXPR, ...)`
 * - `CHECK_EQ(WANT, GOT, ...)`
 * - `CHECK_NE(WANT, GOT, ...)`
 * - `CHECK_GT(WANT, GOT, ...)`
 * - `CHECK_LT(WANT, GOT, ...)`
 * - `CHECK_NOTNULL(EXPR, ...)`
 *
 * The CHECK macros always happen. They always kill the process when
 * they fail. Printf formatting information may be provided as extra
 * arguments. On the other hand, there exists:
 *
 * - `DCHECK(EXPR, ...)`
 * - `DCHECK_EQ(WANT, GOT, ...)`
 * - `DCHECK_NE(WANT, GOT, ...)`
 * - `DCHECK_GT(WANT, GOT, ...)`
 * - `DCHECK_LT(WANT, GOT, ...)`
 * - `DCHECK_NOTNULL(EXPR, ...)`
 *
 * The DCHECK macros always happen when NDEBUG isn't defined. When
 * NDEBUG is defined, they still happen, but in a special way that
 * causes the compiler to recognize their failure as undefined behavior.
 * What this means is that, if the provided expressions are pure without
 * side-effects, then the code compiles down to nothing and the compiler
 * may be able to use the knowledge of something being the case in order
 * to optimize other code adjacent to your DCHECK.
 *
 * In the default build modes, this prints lots of information:
 *
 *     error:examples/check.c:23:check.com: check failed on nightmare pid 15412
 *         CHECK_EQ(123, some_source_code);
 *     		 → 0x7b (123)
 *     		== 0x64 (some_source_code)
 *         extra info: hello
 *         EUNKNOWN/0/No error information
 *         ./o//examples/check.com
 *     0x0000000000407404: __die at libc/log/die.c:42
 *     0x0000000000407340: __check_fail at libc/log/checkfail.c:73
 *     0x00000000004071d0: main at examples/check.c:23
 *     0x000000000040256e: cosmo at libc/runtime/cosmo.S:69
 *     0x000000000040217d: _start at libc/crt/crt.S:85
 *
 * In NDEBUG mode (e.g. MODE=rel, MODE=tiny, etc.) this prints a much
 * simpler message that, most importantly, doesn't include any source
 * code, although it still includes the file name for reference.
 *
 *     error:examples/check.c:14: check failed: 123 == 100: extra info: hello
 *
 * That way your release binaries won't leak CI. You may optionally link
 * the following functions to further expand the information shown by
 * the NDEBUG check failure:
 *
 *     STATIC_YOINK("__die");
 *     STATIC_YOINK("strerror");
 *
 * Please note that backtraces aren't ever available in MODE=tiny.
 */

int main(int argc, char *argv[]) {
  int some_source_code = 100;
  CHECK_EQ(123, some_source_code, "extra info: %s", "hello");
  return 0;
}
