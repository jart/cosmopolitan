#ifndef COSMOPOLITAN_THIRD_PARTY_MBEDTLS_TEST_LIB_H_
#define COSMOPOLITAN_THIRD_PARTY_MBEDTLS_TEST_LIB_H_
#include "libc/str/str.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"
#include "third_party/mbedtls/config.h"
#include "third_party/mbedtls/platform.h"

#define DEPENDENCY_SUPPORTED    0
#define KEY_VALUE_MAPPING_FOUND 0
#define DISPATCH_TEST_SUCCESS   0

#define KEY_VALUE_MAPPING_NOT_FOUND -1
#define DEPENDENCY_NOT_SUPPORTED    -2
#define DISPATCH_TEST_FN_NOT_FOUND  -3
#define DISPATCH_INVALID_TEST_DATA  -4
#define DISPATCH_UNSUPPORTED_SUITE  -5

#define PSA_ECC_FAMILY_SECP_R1        0x12
#define PSA_ECC_FAMILY_SECP_K1        0x17
#define PSA_ECC_FAMILY_BRAINPOOL_P_R1 0x30

#if defined(MBEDTLS_TEST_CONSTANT_FLOW_MEMSAN)
#include <sanitizer/msan_interface.h>
#define TEST_CF_SECRET __msan_allocated_memory
#define TEST_CF_PUBLIC __msan_unpoison
#elif defined(MBEDTLS_TEST_CONSTANT_FLOW_VALGRIND)
#include <valgrind/memcheck.h>
#define TEST_CF_SECRET VALGRIND_MAKE_MEM_UNDEFINED
#define TEST_CF_PUBLIC VALGRIND_MAKE_MEM_DEFINED
#else
#define TEST_CF_SECRET(ptr, size)
#define TEST_CF_PUBLIC(ptr, size)
#endif

#if defined(MBEDTLS_TEST_NULL_ENTROPY) ||                                   \
    (!defined(MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES) &&                        \
     (!defined(MBEDTLS_NO_PLATFORM_ENTROPY) || defined(MBEDTLS_HAVEGE_C) || \
      defined(MBEDTLS_ENTROPY_HARDWARE_ALT) || defined(ENTROPY_NV_SEED)))
#define ENTROPY_HAVE_STRONG
#else
#error oh no entropy
#endif

COSMOPOLITAN_C_START_

#define WRITE mbedtls_test_write

#define TEST_ASSERT(TEST)                           \
  do {                                              \
    if (!(TEST)) {                                  \
      mbedtls_test_fail(#TEST, __LINE__, __FILE__); \
      goto exit;                                    \
    }                                               \
  } while (0)

#define TEST_ASSERT_STREQ(A, B)                                              \
  do {                                                                       \
    const char *StrA = (A);                                                  \
    const char *StrB = (B);                                                  \
    if (strcmp(StrA, StrB)) {                                                \
      mbedtls_test_fail(                                                     \
          xasprintf("!strcmp(%`'s,\n          %`'s)", StrA, StrB), __LINE__, \
          __FILE__);                                                         \
      goto exit;                                                             \
    }                                                                        \
  } while (0)

#define TEST_EQUAL(A, B)                                                     \
  do {                                                                       \
    long Ax = (long)(A);                                                     \
    long Bx = (long)(B);                                                     \
    if (Ax != Bx) {                                                          \
      mbedtls_test_fail(xasprintf("TEST_EQUAL(%s, %s)\n"                     \
                                  "  Wanted: %,ld (-0x%04lx %s)\n"           \
                                  "  Got:    %,ld (-0x%04lx %s)",            \
                                  #A, #B, Ax, -Ax, GetTlsError(Ax), Bx, -Bx, \
                                  GetTlsError(Bx)),                          \
                        __LINE__, __FILE__);                                 \
      goto exit;                                                             \
    }                                                                        \
  } while (0)

#define ASSERT_ALLOC(pointer, length)                           \
  do {                                                          \
    TEST_ASSERT((pointer) == NULL);                             \
    if ((length) != 0) {                                        \
      (pointer) = mbedtls_calloc(sizeof(*(pointer)), (length)); \
      TEST_ASSERT((pointer) != NULL);                           \
    }                                                           \
  } while (0)

#define ASSERT_ALLOC_WEAK(pointer, length)                      \
  do {                                                          \
    TEST_ASSERT((pointer) == NULL);                             \
    if ((length) != 0) {                                        \
      (pointer) = mbedtls_calloc(sizeof(*(pointer)), (length)); \
      TEST_ASSUME((pointer) != NULL);                           \
    }                                                           \
  } while (0)

#define ASSERT_COMPARE(p1, size1, p2, size2)                         \
  do {                                                               \
    TEST_ASSERT((size1) == (size2));                                 \
    if ((size1) != 0) TEST_ASSERT(memcmp((p1), (p2), (size1)) == 0); \
  } while (0)

#define TEST_ASSUME(TEST)                           \
  do {                                              \
    if (!(TEST)) {                                  \
      mbedtls_test_skip(#TEST, __LINE__, __FILE__); \
      goto exit;                                    \
    }                                               \
  } while (0)

#define TEST_INVALID_PARAM_RET(PARAM_ERR_VALUE, TEST)             \
  do {                                                            \
    mbedtls_test_param_failed_expect_call();                      \
    if (((TEST) != (PARAM_ERR_VALUE)) ||                          \
        (mbedtls_test_param_failed_check_expected_call() != 0)) { \
      mbedtls_test_fail(#TEST, __LINE__, __FILE__);               \
      goto exit;                                                  \
    }                                                             \
    mbedtls_test_param_failed_check_expected_call();              \
  } while (0)

#define TEST_INVALID_PARAM(TEST)                                  \
  do {                                                            \
    memcpy(jmp_tmp, mbedtls_test_param_failed_get_state_buf(),    \
           sizeof(jmp_tmp));                                      \
    if (setjmp(mbedtls_test_param_failed_get_state_buf()) == 0) { \
      TEST;                                                       \
      mbedtls_test_fail(#TEST, __LINE__, __FILE__);               \
      goto exit;                                                  \
    }                                                             \
    mbedtls_test_param_failed_reset_state();                      \
  } while (0)

#define TEST_VALID_PARAM(TEST) TEST_ASSERT((TEST, 1));

#define ASSERT_ALLOC(pointer, length)                           \
  do {                                                          \
    TEST_ASSERT((pointer) == NULL);                             \
    if ((length) != 0) {                                        \
      (pointer) = mbedtls_calloc(sizeof(*(pointer)), (length)); \
      TEST_ASSERT((pointer) != NULL);                           \
    }                                                           \
  } while (0)

#define TEST_HELPER_ASSERT(a)                                       \
  if (!(a)) {                                                       \
    WRITE("%s:%d: assertion failed: %s\n", __FILE__, __LINE__, #a); \
    exit(1);                                                        \
  }

#define ARRAY_LENGTH_UNSAFE(array) (sizeof(array) / sizeof(*(array)))

#ifdef __GNUC__
#define IS_ARRAY_NOT_POINTER(arg) \
  (!__builtin_types_compatible_p(__typeof__(arg), __typeof__(&(arg)[0])))
#define STATIC_ASSERT_EXPR(const_expr) \
  (0 && sizeof(struct { unsigned int STATIC_ASSERT : 1 - 2 * !(const_expr); }))
#define STATIC_ASSERT_THEN_RETURN(condition, value) \
  (STATIC_ASSERT_EXPR(condition) ? 0 : (value))
#define ARRAY_LENGTH(array)                               \
  (STATIC_ASSERT_THEN_RETURN(IS_ARRAY_NOT_POINTER(array), \
                             ARRAY_LENGTH_UNSAFE(array)))
#else
#define ARRAY_LENGTH(array) ARRAY_LENGTH_UNSAFE(array)
#endif

#define USE_PSA_INIT() ((void)0)
#define USE_PSA_DONE() ((void)0)

typedef enum {
  MBEDTLS_TEST_RESULT_SUCCESS = 0,
  MBEDTLS_TEST_RESULT_FAILED,
  MBEDTLS_TEST_RESULT_SKIPPED
} mbedtls_test_result_t;

typedef struct {
  mbedtls_test_result_t result;
  const char *test;
  const char *filename;
  int line_no;
  unsigned long step;
} mbedtls_test_info_t;

extern mbedtls_test_info_t mbedtls_test_info;

typedef struct {
  const char *failure_condition;
  const char *file;
  int line;
} mbedtls_test_param_failed_location_record_t;

typedef struct {
  unsigned char *buf;
  size_t length;
} mbedtls_test_rnd_buf_info;

typedef struct data_tag {
  uint8_t *x;
  uint32_t len;
} data_t;

/**
 * Info structure for the pseudo random function
 *
 * Key should be set at the start to a test-unique value.
 * Do not forget endianness!
 * State( v0, v1 ) should be set to zero.
 */
typedef struct {
  uint32_t key[16];
  uint32_t v0, v1;
} mbedtls_test_rnd_pseudo_info;

extern jmp_buf jmp_tmp;

int mbedtls_test_platform_setup(void);
void mbedtls_test_platform_teardown(void);
int mbedtls_test_hexcmp(uint8_t *, uint8_t *, uint32_t, uint32_t);
void mbedtls_test_fail(const char *, int, const char *);
void mbedtls_test_skip(const char *, int, const char *);
void mbedtls_test_set_step(unsigned long);
void mbedtls_test_info_reset(void);
int mbedtls_test_unhexify(unsigned char *, size_t, const char *, size_t *);
void mbedtls_test_hexify(unsigned char *, const unsigned char *, int);
unsigned char *mbedtls_test_zero_alloc(size_t);
unsigned char *mbedtls_test_unhexify_alloc(const char *, size_t *);
void mbedtls_test_param_failed_get_location_record(
    mbedtls_test_param_failed_location_record_t *);
void mbedtls_test_param_failed_expect_call(void);
int mbedtls_test_param_failed_check_expected_call(void);
void *mbedtls_test_param_failed_get_state_buf(void);
void mbedtls_test_param_failed_reset_state(void);
int mbedtls_test_rnd_std_rand(void *, unsigned char *, size_t);
int mbedtls_test_rnd_zero_rand(void *, unsigned char *, size_t);
int mbedtls_test_rnd_buffer_rand(void *, unsigned char *, size_t);
int mbedtls_test_rnd_pseudo_rand(void *, unsigned char *, size_t);
int mbedtls_test_write(const char *, ...);
int execute_tests(int, const char **, const char *);
int get_expression(int32_t, int32_t *);
int dispatch_test(size_t, void **);
int dep_check(int);
int check_test(size_t);
char *GetTlsError(long);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_MBEDTLS_TEST_LIB_H_ */
