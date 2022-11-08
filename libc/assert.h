#ifndef COSMOPOLITAN_LIBC_ASSERT_H_
#define COSMOPOLITAN_LIBC_ASSERT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern bool __assert_disable;
void __assert_fail(const char *, const char *, int) _Hide relegated;

#ifdef NDEBUG
#define assert(x) ((void)0)
#else
#define assert(x) ((void)((x) || (__assert_fail(#x, __FILE__, __LINE__), 0)))
#endif

#ifndef __cplusplus
#define static_assert _Static_assert
#endif

#ifdef __GNUC__
#if !defined(TINY) && !defined(MODE_DBG)
/**
 * Specifies expression can't possibly be false.
 *
 * This macro uses the `notpossible` keyword and is intended for things
 * like systems integration, where we know for a fact that something is
 * never going to happen, but there's no proof. So we don't want to add
 * extra bloat for filenames and line numbers, since `ShowCrashReports`
 * can print a backtrace if we just embed the UD2 instruction to crash.
 * That's useful for systems code, for the following reason. Invariants
 * make sense with _unassert() since they usually get placed at the top
 * of functions. But if you used _unassert() to test a system call does
 * not fail, then check happens at the end of your function usually and
 * is therefore likely to result in a failure condition where execution
 * falls through into a different function, which is shocking to debug.
 *
 * In `MODE=tiny` these assertions are redefined as undefined behavior.
 */
#define _npassert(x)                 \
  ({                                 \
    if (__builtin_expect(!(x), 0)) { \
      notpossible;                   \
    }                                \
    (void)0;                         \
  })
#else
#define _npassert(x) _unassert(x)
#endif
#endif

#ifdef __GNUC__
/**
 * Specifies expression being false is undefined behavior.
 *
 * This is a good way to tell the compiler about your invariants, which
 * leads to smaller faster programs. The expression is never removed by
 * the preprocessor so it's recommended that it be free of side-effects
 * if you intend for it to be removed. At the same time, this guarantee
 * makes this assertion useful for things like system calls, since they
 * won't be removed by `NDEBUG` mode.
 *
 * If this assertion fails, the worst possible things can happen unless
 * you've built your binary in `MODE=dbg` since UBSAN is the only thing
 * that's capable of debugging this macro.
 */
#define _unassert(x)                 \
  ({                                 \
    if (__builtin_expect(!(x), 0)) { \
      unreachable;                   \
    }                                \
    (void)0;                         \
  })
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ASSERT_H_ */
