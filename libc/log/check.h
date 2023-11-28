#ifndef COSMOPOLITAN_LIBC_LOG_CHECK_H_
#define COSMOPOLITAN_LIBC_LOG_CHECK_H_
#include "libc/dce.h"
#include "libc/macros.internal.h"
COSMOPOLITAN_C_START_

#define CHECK(X, ...)         __CHK(ne, !=, false, "false", !!(X), #X, "" __VA_ARGS__)
#define CHECK_EQ(Y, X, ...)   __CHK(eq, ==, Y, #Y, X, #X, "" __VA_ARGS__)
#define CHECK_NE(Y, X, ...)   __CHK(ne, !=, Y, #Y, X, #X, "" __VA_ARGS__)
#define CHECK_LE(Y, X, ...)   __CHK(le, <=, Y, #Y, X, #X, "" __VA_ARGS__)
#define CHECK_LT(Y, X, ...)   __CHK(lt, <, Y, #Y, X, #X, "" __VA_ARGS__)
#define CHECK_GE(Y, X, ...)   __CHK(ge, >=, Y, #Y, X, #X, "" __VA_ARGS__)
#define CHECK_GT(Y, X, ...)   __CHK(gt, >, Y, #Y, X, #X, "" __VA_ARGS__)
#define CHECK_NOTNULL(X, ...) __CHK(ne, !=, NULL, "NULL", X, #X, "" __VA_ARGS__)

#define DCHECK(X, ...)       __DCHK(ne, !=, false, "false", !!(X), #X, "" __VA_ARGS__)
#define DCHECK_EQ(Y, X, ...) __DCHK(eq, ==, Y, #Y, X, #X, "" __VA_ARGS__)
#define DCHECK_NE(Y, X, ...) __DCHK(ne, !=, Y, #Y, X, #X, "" __VA_ARGS__)
#define DCHECK_LE(Y, X, ...) __DCHK(le, <=, Y, #Y, X, #X, "" __VA_ARGS__)
#define DCHECK_LT(Y, X, ...) __DCHK(lt, <, Y, #Y, X, #X, "" __VA_ARGS__)
#define DCHECK_GE(Y, X, ...) __DCHK(ge, >=, Y, #Y, X, #X, "" __VA_ARGS__)
#define DCHECK_GT(Y, X, ...) __DCHK(gt, >, Y, #Y, X, #X, "" __VA_ARGS__)
#define DCHECK_NOTNULL(X, ...) \
  __DCHK(ne, !=, NULL, "NULL", X, #X, "" __VA_ARGS__)

#define CHECK_ALIGNED(BYTES, VAR, ...)                                \
  do {                                                                \
    if (((uintptr_t)VAR & ((BYTES)-1u))) {                            \
      __check_fail_aligned(BYTES, (uintptr_t)VAR, __FILE__, __LINE__, \
                           "" __VA_ARGS__);                           \
      __builtin_unreachable();                                        \
    }                                                                 \
    VAR = (typeof(VAR))__builtin_assume_aligned(VAR, BYTES);          \
  } while (0)

#define DCHECK_ALIGNED(BYTES, VAR, ...)                      \
  do {                                                       \
    if (((uintptr_t)VAR & ((BYTES)-1u))) {                   \
      __DCHK_ALIGNED(BYTES, (uintptr_t)VAR, "" __VA_ARGS__); \
      __builtin_unreachable();                               \
    }                                                        \
    VAR = (typeof(VAR))__builtin_assume_aligned(VAR, BYTES); \
  } while (0)

#define __CHK(SUFFIX, OP, WANT, WANTSTR, GOT, GOTSTR, ...)                   \
  do {                                                                       \
    autotype(GOT) Got = (GOT);                                               \
    autotype(WANT) Want = (WANT);                                            \
    if (!(Want OP Got)) {                                                    \
      if (!NoDebug()) {                                                      \
        __check_fail(#SUFFIX, #OP, (uint64_t)Want, (WANTSTR), (uint64_t)Got, \
                     (GOTSTR), __FILE__, __LINE__, __VA_ARGS__);             \
      } else {                                                               \
        __check_fail_##SUFFIX((uint64_t)Want, (uint64_t)Got, __FILE__,       \
                              __LINE__, 0, __VA_ARGS__);                     \
      }                                                                      \
      __builtin_unreachable();                                               \
    }                                                                        \
  } while (0)

#ifdef NDEBUG
#define __DCHK(SUFFIX, OP, WANT, WANTSTR, GOT, ...) \
  do {                                              \
    autotype(GOT) Got = (GOT);                      \
    autotype(WANT) Want = (WANT);                   \
    if (!(Want OP Got)) {                           \
      __builtin_unreachable();                      \
    }                                               \
  } while (0)
#else
#define __DCHK(SUFFIX, OP, WANT, WANTSTR, GOT, GOTSTR, ...) \
  __CHK(SUFFIX, OP, WANT, WANTSTR, GOT, GOTSTR, __VA_ARGS__)
#endif /* NDEBUG */

#ifdef NDEBUG
#define __DCHK_ALIGNED(BYTES, VAR, ...)
#else
#define __DCHK_ALIGNED(BYTES, VAR, ...) \
  __check_fail_aligned(BYTES, VAR, __FILE__, __LINE__, __VA_ARGS__)
#endif

void __check_fail(const char *, const char *, uint64_t, const char *, uint64_t,
                  const char *, const char *, int, const char *,
                  ...) relegated wontreturn;

void __check_fail_eq(uint64_t, uint64_t, const char *, int, const char *,
                     const char *, ...) relegated wontreturn;
void __check_fail_ne(uint64_t, uint64_t, const char *, int, const char *,
                     const char *, ...) relegated wontreturn;
void __check_fail_le(uint64_t, uint64_t, const char *, int, const char *,
                     const char *, ...) relegated wontreturn;
void __check_fail_lt(uint64_t, uint64_t, const char *, int, const char *,
                     const char *, ...) relegated wontreturn;
void __check_fail_ge(uint64_t, uint64_t, const char *, int, const char *,
                     const char *, ...) relegated wontreturn;
void __check_fail_gt(uint64_t, uint64_t, const char *, int, const char *,
                     const char *, ...) relegated wontreturn;
void __check_fail_aligned(unsigned, uint64_t, const char *, int, const char *,
                          ...) relegated wontreturn;

#ifdef __VSCODE_INTELLISENSE__
#undef __CHK
#define __CHK(...)
#undef __DCHK
#define __DCHK(...)
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_LOG_CHECK_H_ */
