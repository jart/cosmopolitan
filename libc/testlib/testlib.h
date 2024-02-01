#ifndef COSMOPOLITAN_LIBC_TESTLIB_H_
#define COSMOPOLITAN_LIBC_TESTLIB_H_
#include "libc/stdbool.h"
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § testing library                                           ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

/**
 * Declares test case function.
 *
 * Test cases are guaranteed by the linker to be run in order, sorted by
 * the (SUITE, NAME) tuple passed here.
 */
#define TEST(SUITE, NAME)             \
  __static_yoink("__testcase_start"); \
  __TEST_PROTOTYPE(SUITE, NAME, __TEST_ARRAY, )

/**
 * Declares function that globally modifies program state.
 *
 * All tests will be run an additional time, for each fixture. Fixtures
 * are useful, for example, when multiple implementations of a function
 * exist. Memory protections, on sections such as .initbss, are removed
 * temorarilly by the runtime while calling fixture functions. Fixtures
 * are also guaranteed by the linker to be run in sorted order.
 */
#define FIXTURE(SUITE, NAME)         \
  __static_yoink("__fixture_start"); \
  __FIXTURE("fixture", SUITE, NAME)

/**
 * Declares benchmark function.
 *
 * These only run if (1) the -b flag is passed to the FOO_test.com; and
 * (2) the unit tests passed. It's just an ordinary function, that gets
 * registered with the linker. It should print things to stdout.
 *
 * @see EZBENCH()
 */
#define BENCH(SUITE, NAME)         \
  __static_yoink("__bench_start"); \
  __TEST_PROTOTYPE(SUITE, NAME, __BENCH_ARRAY, optimizespeed)

#define ASSERT_GE(C, X) _TEST2("ASSERT_GE", C, >=, (X), #C, " ≥ ", #X, 1)
#define ASSERT_GT(C, X) _TEST2("ASSERT_GT", C, >, (X), #C, " > ", #X, 1)
#define ASSERT_LE(C, X) _TEST2("ASSERT_LE", C, <=, (X), #C, " ≤ ", #X, 1)
#define ASSERT_LT(C, X) _TEST2("ASSERT_LT", C, <, (X), #C, " < ", #X, 1)
#define EXPECT_GE(C, X) _TEST2("EXPECT_GE", C, >=, (X), #C, " ≥ ", #X, 0)
#define EXPECT_GT(C, X) _TEST2("EXPECT_GT", C, >, (X), #C, " > ", #X, 0)
#define EXPECT_LE(C, X) _TEST2("EXPECT_LE", C, <=, (X), #C, " ≤ ", #X, 0)
#define EXPECT_LT(C, X) _TEST2("EXPECT_LT", C, <, (X), #C, " < ", #X, 0)

#ifdef __aarch64__
#define _TESTLIB_ASM_COMMENT "//"
#else
#define _TESTLIB_ASM_COMMENT "#"
#endif

#define __TEST_ARRAY(S)                     \
  _Section(".piro.relo.sort.testcase.2." #S \
           ",\"aw\",@init_array "_TESTLIB_ASM_COMMENT)

#define __BENCH_ARRAY(S)                 \
  _Section(".piro.relo.sort.bench.2." #S \
           ",\"aw\",@init_array "_TESTLIB_ASM_COMMENT)

#define __TEST_PROTOTYPE(S, N, A, K)               \
  void S##_##N(void);                              \
  testfn_t S##_##N##_ptr[] A(S##_##N) = {S##_##N}; \
  K void S##_##N(void)

#define __TEST_SECTION(NAME, CONTENT) \
  ".section " NAME "\n" CONTENT "\n\t.previous\n"

#define __RELOSECTION(NAME, CONTENT) \
  __TEST_SECTION(".piro.relo.sort" NAME ",\"aw\",@progbits", CONTENT)

#define __ROSTR(STR) __TEST_SECTION(".rodata.str1.1,\"aSM\",@progbits,1", STR)

#define TESTLIB_STRINGIFY(A)  _TESTLIB_STRINGIFY(A)
#define _TESTLIB_STRINGIFY(A) #A

#define __FIXTURE(KIND, GROUP, ENTRY)                               \
  asm(__RELOSECTION("." KIND ".2." #GROUP #ENTRY,                   \
                    "\t.quad\t1f\n"                                 \
                    "\t.quad\t2f\n"                                 \
                    "\t.quad\t" TESTLIB_STRINGIFY(GROUP##_##ENTRY)) \
          __ROSTR("1:\t.asciz\t" TESTLIB_STRINGIFY(#GROUP))         \
              __ROSTR("2:\t.asciz\t" TESTLIB_STRINGIFY(#ENTRY)));   \
  void GROUP##_##ENTRY(void)

/**
 * Enables setup and teardown of test directories.
 *
 * These should be called from SetUpOnce().
 */
void testlib_enable_tmp_setup_teardown(void);
void testlib_enable_tmp_setup_teardown_once(void);

/**
 * User-defined test setup function.
 *
 * The test runner will call this function before each TEST() if it's
 * defined by the linkage.
 */
void SetUp(void);
void SetUpOnce(void);

/**
 * User-defined test cleanup function.
 *
 * The test runner will call this function after each TEST() if it's
 * defined by the linkage.
 */
void TearDown(void);
void TearDownOnce(void);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § testing library » assert or die                           ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define EXPECT_TRUE(X)  _TEST2("EXPECT_TRUE", true, ==, (X), #X, "", "", 0)
#define ASSERT_TRUE(X)  _TEST2("ASSERT_TRUE", true, ==, (X), #X, "", "", 1)
#define ASSERT_FALSE(X) _TEST2("ASSERT_FALSE", false, ==, (X), #X, "", "", 1)

#define ASSERT_EQ(WANT, GOT, ...)                                             \
  __TEST_EQ(assert, __FILE__, __LINE__, __FUNCTION__, #WANT, #GOT, WANT, GOT, \
            __VA_ARGS__)

#define ASSERT_NE(WANT, GOT, ...)                                             \
  __TEST_NE(assert, __FILE__, __LINE__, __FUNCTION__, #WANT, #GOT, WANT, GOT, \
            __VA_ARGS__)

#define ASSERT_SYS(ERRNO, WANT, GOT, ...)                                  \
  do {                                                                     \
    int e = testlib_geterrno();                                            \
    __TEST_EQ(assert, __FILE__, __LINE__, __FUNCTION__, #WANT, #GOT, WANT, \
              GOT, __VA_ARGS__);                                           \
    __TEST_EQ(assert, __FILE__, __LINE__, __FUNCTION__, #ERRNO,            \
              testlib_strerror(), ERRNO, testlib_geterrno(), __VA_ARGS__); \
    testlib_seterrno(e);                                                   \
  } while (0)

#define ASSERT_BETWEEN(BEG, END, GOT) \
  assertBetween(FILIFU BEG, END, GOT, #BEG " <= " #GOT " <= " #END, true)
#define ASSERT_STREQ(WANT, GOT) \
  assertStringEquals(FILIFU sizeof(*(WANT)), WANT, GOT, #GOT, true)
#define ASSERT_STRNE(NOPE, GOT) \
  assertStringNotEquals(FILIFU sizeof(*(NOPE)), NOPE, GOT, #GOT, true)
#define ASSERT_STREQN(WANT, GOT, N) \
  assertStrnEquals(FILIFU sizeof(*(WANT)), WANT, GOT, N, #GOT, true)
#define ASSERT_STRNEN(NOPE, GOT, N) \
  assertStrnNotEquals(FILIFU sizeof(*(NOPE)), NOPE, GOT, N, #GOT, true)
#define ASSERT_STRCASEEQ(WANT, GOT) \
  assertStringCaseEquals(FILIFU sizeof(*(WANT)), WANT, GOT, #GOT, true)
#define ASSERT_STRCASENE(NOPE, GOT) \
  assertStringCaseNotEquals(FILIFU sizeof(*(NOPE)), NOPE, GOT, #GOT, true)
#define ASSERT_STRNCASEEQ(WANT, GOT, N) \
  assertStrnCaseEquals(FILIFU sizeof(*(WANT)), WANT, GOT, N, #GOT, true)
#define ASSERT_STRNCASENE(NOPE, GOT, N) \
  assertStrnCaseNotEquals(FILIFU sizeof(*(NOPE)), NOPE, GOT, N, #GOT, true)
#define ASSERT_STARTSWITH(PREFIX, GOT) \
  assertStartsWith(FILIFU sizeof(*(PREFIX)), PREFIX, GOT, #GOT, true)
#define ASSERT_ENDSWITH(SUFFIX, GOT) \
  assertEndsWith(FILIFU sizeof(*(SUFFIX)), SUFFIX, GOT, #GOT, true)
#define ASSERT_IN(NEEDLE, GOT) \
  assertContains(FILIFU sizeof(*(NEEDLE)), NEEDLE, GOT, #GOT, true)

#define ASSERT_BINEQ(WANT, GOT)     \
  _Generic((WANT)[0],               \
      char: assertBinaryEquals_hex, \
      default: assertBinaryEquals_cp437)(FILIFU WANT, GOT, -1, #GOT, true)
#define ASSERT_BINNE(NOPE, GOT)        \
  _Generic((NOPE)[0],                  \
      char: assertBinaryNotEquals_hex, \
      default: assertBinaryNotEquals_cp437)(FILIFU NOPE, GOT, -1, #GOT, true)
#define ASSERT_BINEQN(WANT, GOT, N) \
  _Generic((WANT)[0],               \
      char: assertBinaryEquals_hex, \
      default: assertBinaryEquals_cp437)(FILIFU WANT, GOT, N, #GOT, true)
#define ASSERT_BINNEN(NOPE, GOT, N)    \
  _Generic((NOPE)[0],                  \
      char: assertBinaryNotEquals_hex, \
      default: assertBinaryNotEquals_cp437)(FILIFU NOPE, GOT, -1, #GOT, true)

#define ASSERT_FLOAT_EQ(WANT, GOT) \
  assertLongDoubleEquals(FILIFU WANT, GOT, #GOT, true)
#define ASSERT_DOUBLE_EQ(WANT, GOT) \
  assertLongDoubleEquals(FILIFU WANT, GOT, #GOT, true)
#define ASSERT_LDBL_EQ(WANT, GOT) \
  assertLongDoubleEquals(FILIFU WANT, GOT, #GOT, true)
#define ASSERT_LDBL_GT(VAL, GOT) \
  assertLongDoubleGreaterThan(VAL, GOT, #VAL " > " #GOT, true)
#define ASSERT_LDBL_LT(VAL, GOT) \
  assertLongDoubleLessThan(VAL, GOT, #VAL " < " #GOT, true)

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § testing library » assert or log                           ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define EXPECT_EQ(WANT, GOT, ...)                                             \
  __TEST_EQ(expect, __FILE__, __LINE__, __FUNCTION__, #WANT, #GOT, WANT, GOT, \
            __VA_ARGS__)

#define EXPECT_NE(WANT, GOT, ...)                                             \
  __TEST_NE(expect, __FILE__, __LINE__, __FUNCTION__, #WANT, #GOT, WANT, GOT, \
            __VA_ARGS__)

#define EXPECT_SYS(ERRNO, WANT, GOT, ...)                                  \
  do {                                                                     \
    int e = testlib_geterrno();                                            \
    __TEST_EQ(expect, __FILE__, __LINE__, __FUNCTION__, #WANT, #GOT, WANT, \
              GOT, __VA_ARGS__);                                           \
    __TEST_EQ(expect, __FILE__, __LINE__, __FUNCTION__, #ERRNO,            \
              testlib_strerror(), ERRNO, testlib_geterrno(), __VA_ARGS__); \
    testlib_seterrno(e);                                                   \
  } while (0)

#define EXPECT_FALSE(X) _TEST2("EXPECT_FALSE", false, ==, (X), #X, "", "", 0)
#define EXPECT_BETWEEN(BEG, END, GOT) \
  assertBetween(FILIFU BEG, END, GOT, #BEG " <= " #GOT " <= " #END, false)
#define EXPECT_STREQ(WANT, GOT) \
  assertStringEquals(FILIFU sizeof(*(WANT)), WANT, GOT, #GOT, false)
#define EXPECT_STRNE(NOPE, GOT) \
  assertStringNotEquals(FILIFU sizeof(*(NOPE)), NOPE, GOT, #GOT, false)
#define EXPECT_STREQN(WANT, GOT, N) \
  assertStrnEquals(FILIFU sizeof(*(WANT)), WANT, GOT, N, #GOT, false)
#define EXPECT_STRNEN(NOPE, GOT, N) \
  assertStrnNotEquals(FILIFU sizeof(*(NOPE)), NOPE, GOT, N, #GOT, false)
#define EXPECT_STRCASEEQ(WANT, GOT) \
  assertStringCaseEquals(FILIFU sizeof(*(WANT)), WANT, GOT, #GOT, false)
#define EXPECT_STRCASENE(NOPE, GOT) \
  assertStringCaseNotEquals(FILIFU sizeof(*(NOPE)), NOPE, GOT, #GOT, false)
#define EXPECT_STRNCASEEQ(WANT, GOT, N) \
  assertStrnCaseEquals(FILIFU sizeof(*(WANT)), WANT, GOT, N, #GOT, false)
#define EXPECT_STRNCASENE(NOPE, GOT, N) \
  assertStrnCaseNotEquals(FILIFU sizeof(*(NOPE)), NOPE, GOT, N, #GOT, false)
#define EXPECT_STARTSWITH(PREFIX, GOT) \
  assertStartsWith(FILIFU sizeof(*(PREFIX)), PREFIX, GOT, #GOT, false)
#define EXPECT_ENDSWITH(SUFFIX, GOT) \
  assertEndsWith(FILIFU sizeof(*(SUFFIX)), SUFFIX, GOT, #GOT, false)
#define EXPECT_IN(NEEDLE, GOT) \
  assertContains(FILIFU sizeof(*(NEEDLE)), NEEDLE, GOT, #GOT, false)

#define EXPECT_BINEQ(WANT, GOT)     \
  _Generic((WANT)[0],               \
      char: assertBinaryEquals_hex, \
      default: assertBinaryEquals_cp437)(FILIFU WANT, GOT, -1, #GOT, false)
#define EXPECT_BINNE(NOPE, GOT)        \
  _Generic((NOPE)[0],                  \
      char: assertBinaryNotEquals_hex, \
      default: assertBinaryNotEquals_cp437)(FILIFU NOPE, GOT, -1, #GOT, false)
#define EXPECT_BINEQN(WANT, GOT, N) \
  _Generic((WANT)[0],               \
      char: assertBinaryEquals_hex, \
      default: assertBinaryEquals_cp437)(FILIFU WANT, GOT, N, #GOT, false)
#define EXPECT_BINNEN(NOPE, GOT, N)    \
  _Generic((NOPE)[0],                  \
      char: assertBinaryNotEquals_hex, \
      default: assertBinaryNotEquals_cp437)(FILIFU NOPE, GOT, -1, #GOT, false)

#define EXPECT_FLOAT_EQ(WANT, GOT) \
  assertLongDoubleEquals(FILIFU WANT, GOT, #GOT, false)
#define EXPECT_DOUBLE_EQ(WANT, GOT) \
  assertLongDoubleEquals(FILIFU WANT, GOT, #GOT, false)
#define EXPECT_LDBL_EQ(WANT, GOT) \
  assertLongDoubleEquals(FILIFU WANT, GOT, #GOT, false)
#define EXPECT_LGBL_GT(VAL, GOT) \
  expectLongDoubleGreaterThan(VAL, GOT, #VAL " > " #GOT, false)
#define EXPECT_LGBL_LT(VAL, GOT) \
  expectLongDoubleLessThan(VAL, GOT, #VAL " < " #GOT, false)

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § testing library » implementation details                  ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define FILIFU           __FILE__, __LINE__, __FUNCTION__,
#define FILIFU_OBJ(...)  __FILE__, __LINE__, ##__VA_ARGS__
#define FILIFU_ARGS      const char *file, int line, const char *func,
#define FILIFU_FROM(OBJ) (OBJ)->file, (OBJ)->line, __FUNCTION__,
#define FILIFU_FIELDS \
  const char *file;   \
  int line

#define __TEST_EQ(KIND, FILE, LINE, FUNC, WANTCODE, GOTCODE, WANT, GOT, ...) \
  do {                                                                       \
    intptr_t Got, Want;                                                      \
    ++g_testlib_ran;                                                         \
    Got = (intptr_t)(GOT);                                                   \
    Want = (intptr_t)(WANT);                                                 \
    if (Want != Got) {                                                       \
      testlib_error_enter(FILE, FUNC);                                       \
      testlib_showerror_##KIND##_eq(LINE, WANTCODE, GOTCODE,                 \
                                    testlib_formatint(Want),                 \
                                    testlib_formatint(Got), "" __VA_ARGS__); \
      testlib_error_leave();                                                 \
    }                                                                        \
    (void)0;                                                                 \
  } while (0)

#define __TEST_NE(KIND, FILE, LINE, FUNC, WANTCODE, GOTCODE, WANT, GOT, ...) \
  do {                                                                       \
    intptr_t Got, Want;                                                      \
    ++g_testlib_ran;                                                         \
    Got = (intptr_t)(GOT);                                                   \
    Want = (intptr_t)(WANT);                                                 \
    if (Want == Got) {                                                       \
      testlib_error_enter(FILE, FUNC);                                       \
      testlib_showerror_##KIND##_ne(LINE, WANTCODE, GOTCODE,                 \
                                    testlib_formatint(Want),                 \
                                    testlib_formatint(Got), "" __VA_ARGS__); \
      testlib_error_leave();                                                 \
    }                                                                        \
  } while (0)

#define _TEST2(NAME, WANT, OP, GOT, WANTCODE, OPSTR, GOTCODE, ISFATAL)    \
  do {                                                                    \
    intptr_t Want = (intptr_t)(WANT);                                     \
    intptr_t Got = (intptr_t)(GOT);                                       \
    if (!(Want OP Got)) {                                                 \
      testlib_showerror(FILIFU NAME, OPSTR, WANTCODE OPSTR GOTCODE,       \
                        testlib_formatint(Want), testlib_formatint(Got)); \
      testlib_onfail2(ISFATAL);                                           \
    }                                                                     \
  } while (0)

typedef void (*testfn_t)(void);

struct TestFixture {
  const char *group;
  const char *name;
  testfn_t fn;
};

extern char g_fixturename[256];
extern bool g_testlib_shoulddebugbreak;     /* set by testmain */
extern _Atomic(unsigned) g_testlib_ran;     /* set by wrappers */
extern _Atomic(unsigned) g_testlib_failed;  /* set by wrappers */
extern const char *testlib_showerror_errno; /* set by macros */
extern const char *testlib_showerror_file;  /* set by macros */
extern const char *testlib_showerror_func;  /* set by macros */
extern const testfn_t __bench_start[], __bench_end[];
extern const testfn_t __testcase_start[], __testcase_end[];
extern const struct TestFixture __fixture_start[], __fixture_end[];

void testlib_showerror_assert_eq(int, const char *, const char *, char *,
                                 char *, const char *, ...) wontreturn;
void testlib_showerror_assert_false(int, const char *, const char *, char *,
                                    char *, const char *, ...) wontreturn;
void testlib_showerror_assert_ne(int, const char *, const char *, char *,
                                 char *, const char *, ...) wontreturn;
void testlib_showerror_assert_true(int, const char *, const char *, char *,
                                   char *, const char *, ...) wontreturn;
void testlib_showerror_expect_eq(int, const char *, const char *, char *,
                                 char *, const char *, ...);
void testlib_showerror_expect_false(int, const char *, const char *, char *,
                                    char *, const char *, ...);
void testlib_showerror_expect_ne(int, const char *, const char *, char *,
                                 char *, const char *, ...);
void testlib_showerror_expect_true(int, const char *, const char *, char *,
                                   char *, const char *, ...);

void testlib_error_leave(void);
void testlib_error_enter(const char *, const char *);
void testlib_showerror(const char *, int, const char *, const char *,
                       const char *, const char *, char *, char *);

void testlib_finish(void);
int testlib_geterrno(void);
void testlib_seterrno(int);
void testlib_runalltests(void);
const char *testlib_strerror(void);
void testlib_runallbenchmarks(void);
bool testlib_memoryexists(const void *);
void testlib_runtestcases(const testfn_t *, const testfn_t *, testfn_t);
void testlib_runfixtures(const testfn_t *, const testfn_t *,
                         const struct TestFixture *,
                         const struct TestFixture *);
int testlib_countfixtures(const struct TestFixture *,
                          const struct TestFixture *);
void testlib_abort(void) wontreturn relegated;
bool testlib_strequals(size_t, const void *, const void *) nosideeffect;
bool testlib_strnequals(size_t, const void *, const void *,
                        size_t) nosideeffect;
bool testlib_strcaseequals(size_t, const void *, const void *) nosideeffect;
bool testlib_strncaseequals(size_t, const void *, const void *,
                            size_t) nosideeffect;
void testlib_free(void *);
void testlib_extract(const char *, const char *, int);
bool testlib_binequals(const char16_t *, const void *, size_t) nosideeffect;
bool testlib_hexequals(const char *, const void *, size_t) nosideeffect;
bool testlib_startswith(size_t, const void *, const void *) nosideeffect;
bool testlib_endswith(size_t, const void *, const void *) nosideeffect;
bool testlib_contains(size_t, const void *, const void *) nosideeffect;
char *testlib_formatrange(intptr_t, intptr_t) mallocesque;
char *testlib_formatstr(size_t, const void *, int) mallocesque;
char *testlib_formatint(intptr_t) mallocesque;
char *testlib_formatbool(bool);
char *testlib_formatfloat(long double) mallocesque;
void testlib_formatbinaryashex(const char *, const void *, size_t, char **,
                               char **);
void testlib_formatbinaryasglyphs(const char16_t *, const void *, size_t,
                                  char **, char **);
bool testlib_almostequallongdouble(long double, long double);
void testlib_incrementfailed(void);
void testlib_clearxmmregisters(void);

forceinline void testlib_ontest() {
  ++g_testlib_ran;
}

forceinline void testlib_onfail2(bool isfatal) {
  testlib_incrementfailed();
  if (isfatal) {
    testlib_abort();
  }
}

forceinline void assertNotEquals(FILIFU_ARGS intptr_t donotwant, intptr_t got,
                                 const char *gotcode, bool isfatal) {
  ++g_testlib_ran;
  if (got != donotwant) return;
  if (g_testlib_shoulddebugbreak) DebugBreak();
  testlib_showerror(file, line, func, "assertNotEquals", "=", gotcode,
                    testlib_formatint(got), testlib_formatint(donotwant));
  testlib_onfail2(isfatal);
}

#define assertLongDoubleGreaterThan(a, b, code, isfatal)                   \
  do {                                                                     \
    autotype(a) a_ = (a);                                                  \
    autotype(b) b_ = (b);                                                  \
    if (a_ <= b_) {                                                        \
      testlib_showerror(FILIFU "assertLongDoubleGreaterThan", ">", code,   \
                        testlib_formatfloat(a_), testlib_formatfloat(b_)); \
      testlib_onfail2(isfatal);                                            \
    }                                                                      \
    (void)0;                                                               \
  } while (0)

#define assertLongDoubleLessThan(a, b, code, isfatal)                      \
  do {                                                                     \
    autotype(a) a_ = (a);                                                  \
    autotype(b) b_ = (b);                                                  \
    if (a_ >= b_) {                                                        \
      testlib_showerror(FILIFU "assertLongDoubleLessThan", "<", code,      \
                        testlib_formatfloat(a_), testlib_formatfloat(b_)); \
      testlib_onfail2(isfatal);                                            \
    }                                                                      \
    (void)0;                                                               \
  } while (0)

forceinline void assertBetween(FILIFU_ARGS intptr_t beg, intptr_t end,
                               intptr_t got, const char *gotcode,
                               bool isfatal) {
  ++g_testlib_ran;
  if (beg <= got && got <= end) return;
  if (g_testlib_shoulddebugbreak) DebugBreak();
  testlib_showerror(file, line, func, "assertBetween", "∉", gotcode,
                    testlib_formatint(got), testlib_formatrange(beg, end));
  testlib_onfail2(isfatal);
}

forceinline void assertStringEquals(FILIFU_ARGS size_t cw, const void *want,
                                    const void *got, const char *gotcode,
                                    bool isfatal) {
  ++g_testlib_ran;
  if (testlib_strequals(cw, want, got)) return;
  if (g_testlib_shoulddebugbreak) DebugBreak();
  testlib_showerror(file, line, func, "assertStringEquals", "≠", gotcode,
                    testlib_formatstr(cw, want, -1),
                    testlib_formatstr(cw, got, -1));
  testlib_onfail2(isfatal);
}

forceinline void assertStringNotEquals(FILIFU_ARGS size_t cw, const void *want,
                                       const void *got, const char *gotcode,
                                       bool isfatal) {
  ++g_testlib_ran;
  if (!testlib_strequals(cw, want, got)) return;
  if (g_testlib_shoulddebugbreak) DebugBreak();
  testlib_showerror(file, line, func, "assertStringNotEquals", "=", gotcode,
                    testlib_formatstr(cw, want, -1),
                    testlib_formatstr(cw, got, -1));
  testlib_onfail2(isfatal);
}

forceinline void assertStrnEquals(FILIFU_ARGS size_t cw, const void *want,
                                  const void *got, size_t n,
                                  const char *gotcode, bool isfatal) {
  ++g_testlib_ran;
  if (testlib_strnequals(cw, want, got, n)) return;
  if (g_testlib_shoulddebugbreak) DebugBreak();
  testlib_showerror(file, line, func, "assertStrnEquals", "≠", gotcode,
                    testlib_formatstr(cw, got, n),
                    testlib_formatstr(cw, want, n));
  testlib_onfail2(isfatal);
}

forceinline void assertStrnNotEquals(FILIFU_ARGS size_t cw, const void *want,
                                     const void *got, size_t n,
                                     const char *gotcode, bool isfatal) {
  ++g_testlib_ran;
  if (!testlib_strnequals(cw, want, got, n)) return;
  if (g_testlib_shoulddebugbreak) DebugBreak();
  testlib_showerror(file, line, func, "assertStrnNotEquals", "=", gotcode,
                    testlib_formatstr(cw, got, n),
                    testlib_formatstr(cw, want, n));
  testlib_onfail2(isfatal);
}

forceinline void assertStringCaseEquals(FILIFU_ARGS size_t cw, const void *want,
                                        const void *got, const char *gotcode,
                                        bool isfatal) {
  ++g_testlib_ran;
  if (testlib_strcaseequals(cw, want, got)) return;
  if (g_testlib_shoulddebugbreak) DebugBreak();
  testlib_showerror(file, line, func, "assertStringCaseEquals", "≠", gotcode,
                    testlib_formatstr(cw, got, -1),
                    testlib_formatstr(cw, want, -1));
  testlib_onfail2(isfatal);
}

forceinline void assertStringCaseNotEquals(FILIFU_ARGS size_t cw,
                                           const void *want, const void *got,
                                           const char *gotcode, bool isfatal) {
  ++g_testlib_ran;
  if (!testlib_strcaseequals(cw, want, got)) return;
  if (g_testlib_shoulddebugbreak) DebugBreak();
  testlib_showerror(file, line, func, "assertStringCaseNotEquals", "=", gotcode,
                    testlib_formatstr(cw, got, -1),
                    testlib_formatstr(cw, want, -1));
  testlib_onfail2(isfatal);
}

forceinline void assertStrnCaseEquals(FILIFU_ARGS size_t cw, const void *want,
                                      const void *got, size_t n,
                                      const char *gotcode, bool isfatal) {
  ++g_testlib_ran;
  if (testlib_strncaseequals(cw, want, got, n)) return;
  if (g_testlib_shoulddebugbreak) DebugBreak();
  testlib_showerror(file, line, func, "assertStrnCaseEquals", "≠", gotcode,
                    testlib_formatstr(cw, got, n),
                    testlib_formatstr(cw, want, n));
  testlib_onfail2(isfatal);
}

forceinline void assertStrnCaseNotEquals(FILIFU_ARGS size_t cw,
                                         const void *want, const void *got,
                                         size_t n, const char *gotcode,
                                         bool isfatal) {
  ++g_testlib_ran;
  if (!testlib_strncaseequals(cw, want, got, n)) return;
  if (g_testlib_shoulddebugbreak) DebugBreak();
  testlib_showerror(file, line, func, "assertStrnCaseNotEquals", "=", gotcode,
                    testlib_formatstr(cw, got, n),
                    testlib_formatstr(cw, want, n));
  testlib_onfail2(isfatal);
}

forceinline void assertStartsWith(FILIFU_ARGS size_t cw, const char *prefix,
                                  const char *s, const char *gotcode,
                                  bool isfatal) {
  ++g_testlib_ran;
  if (testlib_startswith(cw, s, prefix)) return;
  if (g_testlib_shoulddebugbreak) DebugBreak();
  testlib_showerror(file, line, func, "assertStartsWith", "≠", gotcode,
                    testlib_formatstr(1, prefix, -1),
                    testlib_formatstr(1, s, -1));
  testlib_onfail2(isfatal);
}

forceinline void assertEndsWith(FILIFU_ARGS size_t cw, const char *suffix,
                                const char *s, const char *gotcode,
                                bool isfatal) {
  ++g_testlib_ran;
  if (testlib_endswith(cw, s, suffix)) return;
  if (g_testlib_shoulddebugbreak) DebugBreak();
  testlib_showerror(file, line, func, "assertEndsWith", "≠", gotcode,
                    testlib_formatstr(1, s, -1),
                    testlib_formatstr(1, suffix, -1));
  testlib_onfail2(isfatal);
}

forceinline void assertContains(FILIFU_ARGS size_t cw, const char *needle,
                                const char *s, const char *gotcode,
                                bool isfatal) {
  ++g_testlib_ran;
  if (testlib_contains(cw, s, needle)) return;
  if (g_testlib_shoulddebugbreak) DebugBreak();
  testlib_showerror(file, line, func, "assertContains", "∉", gotcode,
                    testlib_formatstr(1, s, -1),
                    testlib_formatstr(1, needle, -1));
  testlib_onfail2(isfatal);
}

forceinline void assertBinaryEquals_cp437(FILIFU_ARGS const char16_t *want,
                                          const void *got, size_t n,
                                          const char *gotcode, bool isfatal) {
  ++g_testlib_ran;
  char *v1, *v2;
  if (testlib_binequals(want, got, n)) return;
  if (g_testlib_shoulddebugbreak) DebugBreak();
  testlib_formatbinaryasglyphs(want, got, n, &v1, &v2);
  testlib_showerror(file, line, func, "assertBinaryEquals", "≠", gotcode, v1,
                    v2);
  testlib_onfail2(isfatal);
}

forceinline void assertBinaryEquals_hex(FILIFU_ARGS const char *want,
                                        const void *got, size_t n,
                                        const char *gotcode, bool isfatal) {
  ++g_testlib_ran;
  char *v1, *v2;
  if (testlib_hexequals(want, got, n)) return;
  if (g_testlib_shoulddebugbreak) DebugBreak();
  testlib_formatbinaryashex(want, got, n, &v1, &v2);
  testlib_showerror(file, line, func, "assertBinaryEquals", "≠", gotcode, v1,
                    v2);
  testlib_onfail2(isfatal);
}

forceinline void assertBinaryNotEquals_cp437(FILIFU_ARGS const char16_t *want,
                                             const void *got, size_t n,
                                             const char *gotcode,
                                             bool isfatal) {
  ++g_testlib_ran;
  char *v1, *v2;
  if (!testlib_binequals(want, got, n)) return;
  if (g_testlib_shoulddebugbreak) DebugBreak();
  testlib_formatbinaryasglyphs(want, got, n, &v1, &v2);
  testlib_showerror(file, line, func, "assertBinaryNotEquals", "=", gotcode, v1,
                    v2);
  testlib_onfail2(isfatal);
}

forceinline void assertBinaryNotEquals_hex(FILIFU_ARGS const char *want,
                                           const void *got, size_t n,
                                           const char *gotcode, bool isfatal) {
  ++g_testlib_ran;
  char *v1, *v2;
  if (!testlib_hexequals(want, got, n)) return;
  if (g_testlib_shoulddebugbreak) DebugBreak();
  testlib_formatbinaryashex(want, got, n, &v1, &v2);
  testlib_showerror(file, line, func, "assertBinaryNotEquals", "=", gotcode, v1,
                    v2);
  testlib_onfail2(isfatal);
}

forceinline void assertLongDoubleEquals(FILIFU_ARGS long double want,
                                        long double got, const char *gotcode,
                                        bool isfatal) {
  ++g_testlib_ran;
  if (testlib_almostequallongdouble(want, got)) return;
  if (g_testlib_shoulddebugbreak) DebugBreak();
  testlib_showerror(file, line, func, "assertLongDoubleEquals", "≠", gotcode,
                    testlib_formatfloat(want), testlib_formatfloat(got));
  testlib_onfail2(isfatal);
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TESTLIB_H_ */
