/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "test_common.h"

#include <test/constant_flow.h>
#include <test/helpers.h>
#include <test/macros.h>
#include <errno.h>
#include <string.h>

#if defined(MBEDTLS_PSA_INJECT_ENTROPY)
#include <psa/crypto.h>
#include <test/psa_crypto_helpers.h>
#endif

#if defined(MBEDTLS_TEST_HOOKS) && defined(MBEDTLS_PSA_CRYPTO_C)
#include <test/psa_memory_poisoning_wrappers.h>
#endif
#if defined(MBEDTLS_TEST_HOOKS) && !defined(MBEDTLS_THREADING_C)
#include <test/bignum_codepath_check.h>
#endif
#if defined(MBEDTLS_THREADING_C)
#include "mbedtls/threading.h"
#endif

/*----------------------------------------------------------------------------*/
/* Static global variables */

#if defined(MBEDTLS_PLATFORM_C)
static mbedtls_platform_context platform_ctx;
#endif

static mbedtls_test_info_t mbedtls_test_info;

#ifdef MBEDTLS_THREADING_C
mbedtls_threading_mutex_t mbedtls_test_info_mutex;
#endif /* MBEDTLS_THREADING_C */

/*----------------------------------------------------------------------------*/
/* Mbedtls Test Info accessors
 *
 * NOTE - there are two types of accessors here: public accessors and internal
 * accessors. The public accessors have prototypes in helpers.h and lock
 * mbedtls_test_info_mutex (if mutexes are enabled). The _internal accessors,
 * which are expected to be used from this module *only*, do not lock the mutex.
 * These are designed to be called from within public functions which already
 * hold the mutex. The main reason for this difference is the need to set
 * multiple test data values atomically (without releasing the mutex) to prevent
 * race conditions. */

mbedtls_test_result_t mbedtls_test_get_result(void)
{
    mbedtls_test_result_t result;

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_lock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    result =  mbedtls_test_info.result;

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_unlock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    return result;
}

static void mbedtls_test_set_result_internal(mbedtls_test_result_t result, const char *test,
                                             int line_no, const char *filename)
{
    /* Internal function only - mbedtls_test_info_mutex should be held prior
     * to calling this function. */

    mbedtls_test_info.result = result;
    mbedtls_test_info.test = test;
    mbedtls_test_info.line_no = line_no;
    mbedtls_test_info.filename = filename;
}

const char *mbedtls_test_get_test(void)
{
    const char *test;

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_lock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    test = mbedtls_test_info.test;

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_unlock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    return test;
}
const char *mbedtls_get_test_filename(void)
{
    const char *filename;

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_lock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    /* It should be ok just to pass back the pointer here, as it is going to
     * be a pointer into non changing data. */
    filename = mbedtls_test_info.filename;

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_unlock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    return filename;
}

int mbedtls_test_get_line_no(void)
{
    int line_no;

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_lock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    line_no = mbedtls_test_info.line_no;

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_unlock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    return line_no;
}

void mbedtls_test_increment_step(void)
{
#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_lock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    ++mbedtls_test_info.step;

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_unlock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */
}

unsigned long mbedtls_test_get_step(void)
{
    unsigned long step;

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_lock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    step = mbedtls_test_info.step;

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_unlock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    return step;
}

static void mbedtls_test_reset_step_internal(void)
{
    /* Internal function only - mbedtls_test_info_mutex should be held prior
     * to calling this function. */

    mbedtls_test_info.step = (unsigned long) (-1);
}

void mbedtls_test_set_step(unsigned long step)
{
#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_lock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    mbedtls_test_info.step = step;

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_unlock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */
}

void mbedtls_test_get_line1(char *line)
{
#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_lock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    memcpy(line, mbedtls_test_info.line1, MBEDTLS_TEST_LINE_LENGTH);

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_unlock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */
}

static void mbedtls_test_set_line1_internal(const char *line)
{
    /* Internal function only - mbedtls_test_info_mutex should be held prior
     * to calling this function. */

    if (line == NULL) {
        memset(mbedtls_test_info.line1, 0, MBEDTLS_TEST_LINE_LENGTH);
    } else {
        memcpy(mbedtls_test_info.line1, line, MBEDTLS_TEST_LINE_LENGTH);
    }
}

void mbedtls_test_get_line2(char *line)
{
#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_lock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    memcpy(line, mbedtls_test_info.line2, MBEDTLS_TEST_LINE_LENGTH);

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_unlock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */
}

static void mbedtls_test_set_line2_internal(const char *line)
{
    /* Internal function only - mbedtls_test_info_mutex should be held prior
     * to calling this function. */

    if (line == NULL) {
        memset(mbedtls_test_info.line2, 0, MBEDTLS_TEST_LINE_LENGTH);
    } else {
        memcpy(mbedtls_test_info.line2, line, MBEDTLS_TEST_LINE_LENGTH);
    }
}


#if defined(MBEDTLS_TEST_MUTEX_USAGE)
const char *mbedtls_test_get_mutex_usage_error(void)
{
    const char *usage_error;

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_lock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    usage_error = mbedtls_test_info.mutex_usage_error;

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_unlock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    return usage_error;
}

void mbedtls_test_set_mutex_usage_error(const char *msg)
{
#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_lock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    if (mbedtls_test_info.mutex_usage_error == NULL || msg == NULL) {
        mbedtls_test_info.mutex_usage_error = msg;
    }

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_unlock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */
}
#endif // #if defined(MBEDTLS_TEST_MUTEX_USAGE)

int mbedtls_test_buffer_is_all_zero(const uint8_t *buf, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        if (buf[i] != 0) {
            return 0;
        }
    }
    return 1;
}

#if defined(MBEDTLS_BIGNUM_C)

unsigned mbedtls_test_get_case_uses_negative_0(void)
{
    unsigned test_case_uses_negative_0 = 0;
#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_lock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */
    test_case_uses_negative_0 = mbedtls_test_info.case_uses_negative_0;

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_unlock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    return test_case_uses_negative_0;
}

static void mbedtls_test_set_case_uses_negative_0_internal(unsigned uses)
{
    /* Internal function only - mbedtls_test_info_mutex should be held prior
     * to calling this function. */

    mbedtls_test_info.case_uses_negative_0 = uses;
}

void mbedtls_test_increment_case_uses_negative_0(void)
{
#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_lock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    ++mbedtls_test_info.case_uses_negative_0;

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_unlock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */
}

#endif /* MBEDTLS_BIGNUM_C */

#ifdef MBEDTLS_TEST_MUTEX_USAGE
mbedtls_threading_mutex_t *mbedtls_test_get_info_mutex(void)
{
    return &mbedtls_test_info_mutex;
}

#endif /* MBEDTLS_TEST_MUTEX_USAGE */

/*----------------------------------------------------------------------------*/
/* Helper Functions */

int mbedtls_test_platform_setup(void)
{
    int ret = 0;

#if defined(MBEDTLS_TEST_HOOKS) && defined(MBEDTLS_PSA_CRYPTO_C) \
    && !defined(MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS) \
    && defined(MBEDTLS_TEST_MEMORY_CAN_POISON)
    mbedtls_poison_test_hooks_setup();
#endif

#if defined(MBEDTLS_PSA_INJECT_ENTROPY)
    /* Make sure that injected entropy is present. Otherwise
     * psa_crypto_init() will fail. This is not necessary for test suites
     * that don't use PSA, but it's harmless (except for leaving a file
     * behind). */
    ret = mbedtls_test_inject_entropy_restore();
    if (ret != 0) {
        return ret;
    }
#endif

#if defined(MBEDTLS_PLATFORM_C)
    ret = mbedtls_platform_setup(&platform_ctx);
#endif /* MBEDTLS_PLATFORM_C */

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_init(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */


#if defined(MBEDTLS_TEST_HOOKS) && !defined(MBEDTLS_THREADING_C)
    mbedtls_codepath_test_hooks_setup();
#endif /* MBEDTLS_TEST_HOOKS && !MBEDTLS_THREADING_C */

    return ret;
}

void mbedtls_test_platform_teardown(void)
{
#if defined(MBEDTLS_TEST_HOOKS) && defined(MBEDTLS_PSA_CRYPTO_C) \
    && !defined(MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS) \
    &&  defined(MBEDTLS_TEST_MEMORY_CAN_POISON)
    mbedtls_poison_test_hooks_teardown();
#endif
#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_free(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

#if defined(MBEDTLS_PLATFORM_C)
    mbedtls_platform_teardown(&platform_ctx);
#endif /* MBEDTLS_PLATFORM_C */

#if defined(MBEDTLS_TEST_HOOKS) && !defined(MBEDTLS_THREADING_C)
    mbedtls_codepath_test_hooks_teardown();
#endif /* MBEDTLS_TEST_HOOKS && !MBEDTLS_THREADING_C */
}

int mbedtls_test_ascii2uc(const char c, unsigned char *uc)
{
    if ((c >= '0') && (c <= '9')) {
        *uc = c - '0';
    } else if ((c >= 'a') && (c <= 'f')) {
        *uc = c - 'a' + 10;
    } else if ((c >= 'A') && (c <= 'F')) {
        *uc = c - 'A' + 10;
    } else {
        return -1;
    }

    return 0;
}

static void mbedtls_test_fail_internal(const char *test, int line_no, const char *filename)
{
    /* Internal function only - mbedtls_test_info_mutex should be held prior
     * to calling this function. */

    /* Don't use accessor, we already hold mutex. */
    if (mbedtls_test_info.result != MBEDTLS_TEST_RESULT_FAILED) {
        /* If we have already recorded the test as having failed then don't
         * overwrite any previous information about the failure. */
        mbedtls_test_set_result_internal(MBEDTLS_TEST_RESULT_FAILED, test, line_no, filename);
    }
}

void mbedtls_test_fail(const char *test, int line_no, const char *filename)
{
#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_lock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    mbedtls_test_fail_internal(test, line_no, filename);

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_unlock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */
}

void mbedtls_test_skip(const char *test, int line_no, const char *filename)
{
#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_lock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    mbedtls_test_set_result_internal(MBEDTLS_TEST_RESULT_SKIPPED, test, line_no, filename);

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_unlock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */
}

void mbedtls_test_info_reset(void)
{
#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_lock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    mbedtls_test_set_result_internal(MBEDTLS_TEST_RESULT_SUCCESS, 0, 0, 0);
    mbedtls_test_reset_step_internal();
    mbedtls_test_set_line1_internal(NULL);
    mbedtls_test_set_line2_internal(NULL);

#if defined(MBEDTLS_BIGNUM_C)
    mbedtls_test_set_case_uses_negative_0_internal(0);
#endif

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_unlock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */
}

void mbedtls_test_info_save(mbedtls_test_info_t *out)
{
#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_lock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */
    memcpy(out, &mbedtls_test_info, sizeof(mbedtls_test_info));
#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_unlock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */
}

void mbedtls_test_info_overwrite(const mbedtls_test_info_t *replacement)
{
#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_lock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */
    memcpy(&mbedtls_test_info, replacement, sizeof(mbedtls_test_info));
#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_unlock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */
}

int mbedtls_test_equal(const char *test, int line_no, const char *filename,
                       unsigned long long value1, unsigned long long value2)
{
    TEST_CF_PUBLIC(&value1, sizeof(value1));
    TEST_CF_PUBLIC(&value2, sizeof(value2));

    if (value1 == value2) {
        return 1;
    }

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_lock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    /* Don't use accessor, as we already hold mutex. */
    if (mbedtls_test_info.result != MBEDTLS_TEST_RESULT_FAILED) {
        /* If we've already recorded the test as having failed then don't
         * overwrite any previous information about the failure. */

        char buf[MBEDTLS_TEST_LINE_LENGTH];
        mbedtls_test_fail_internal(test, line_no, filename);
        (void) mbedtls_snprintf(buf, sizeof(buf),
                                "lhs = 0x%016llx = %lld",
                                value1, (long long) value1);
        mbedtls_test_set_line1_internal(buf);
        (void) mbedtls_snprintf(buf, sizeof(buf),
                                "rhs = 0x%016llx = %lld",
                                value2, (long long) value2);
        mbedtls_test_set_line2_internal(buf);
    }

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_unlock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    return 0;
}

int mbedtls_test_le_u(const char *test, int line_no, const char *filename,
                      unsigned long long value1, unsigned long long value2)
{
    TEST_CF_PUBLIC(&value1, sizeof(value1));
    TEST_CF_PUBLIC(&value2, sizeof(value2));

    if (value1 <= value2) {
        return 1;
    }

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_lock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    /* Don't use accessor, we already hold mutex. */
    if (mbedtls_test_info.result != MBEDTLS_TEST_RESULT_FAILED) {
        /* If we've already recorded the test as having failed then don't
         * overwrite any previous information about the failure. */

        char buf[MBEDTLS_TEST_LINE_LENGTH];
        mbedtls_test_fail_internal(test, line_no, filename);
        (void) mbedtls_snprintf(buf, sizeof(buf),
                                "lhs = 0x%016llx = %llu",
                                value1, value1);
        mbedtls_test_set_line1_internal(buf);
        (void) mbedtls_snprintf(buf, sizeof(buf),
                                "rhs = 0x%016llx = %llu",
                                value2, value2);
        mbedtls_test_set_line2_internal(buf);
    }

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_unlock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    return 0;
}

int mbedtls_test_le_s(const char *test, int line_no, const char *filename,
                      long long value1, long long value2)
{
    TEST_CF_PUBLIC(&value1, sizeof(value1));
    TEST_CF_PUBLIC(&value2, sizeof(value2));

    if (value1 <= value2) {
        return 1;
    }

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_lock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    /* Don't use accessor, we already hold mutex. */
    if (mbedtls_test_info.result != MBEDTLS_TEST_RESULT_FAILED) {
        /* If we've already recorded the test as having failed then don't
         * overwrite any previous information about the failure. */

        char buf[MBEDTLS_TEST_LINE_LENGTH];
        mbedtls_test_fail_internal(test, line_no, filename);
        (void) mbedtls_snprintf(buf, sizeof(buf),
                                "lhs = 0x%016llx = %lld",
                                (unsigned long long) value1, value1);
        mbedtls_test_set_line1_internal(buf);
        (void) mbedtls_snprintf(buf, sizeof(buf),
                                "rhs = 0x%016llx = %lld",
                                (unsigned long long) value2, value2);
        mbedtls_test_set_line2_internal(buf);
    }

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_unlock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    return 0;
}

void mbedtls_test_fail_errno(const char *test,
                             int line_no, const char *filename)
{
#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_lock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */

    /* Don't use accessor, we already hold mutex. */
    if (mbedtls_test_info.result != MBEDTLS_TEST_RESULT_FAILED) {
        /* If we've already recorded the test as having failed then don't
         * overwrite any previous information about the failure. */

        char buf[MBEDTLS_TEST_LINE_LENGTH];
        mbedtls_test_fail_internal(test, line_no, filename);
        (void) mbedtls_snprintf(buf, sizeof(buf),
                                "errno = %d (%s)",
                                errno, strerror(errno));
        mbedtls_test_set_line1_internal(buf);
    }

#ifdef MBEDTLS_THREADING_C
    mbedtls_mutex_unlock(&mbedtls_test_info_mutex);
#endif /* MBEDTLS_THREADING_C */
}

int mbedtls_test_unhexify(unsigned char *obuf,
                          size_t obufmax,
                          const char *ibuf,
                          size_t *len)
{
    unsigned char uc, uc2;

    *len = strlen(ibuf);

    /* Must be even number of bytes. */
    if ((*len) & 1) {
        return -1;
    }
    *len /= 2;

    if ((*len) > obufmax) {
        return -1;
    }

    while (*ibuf != 0) {
        if (mbedtls_test_ascii2uc(*(ibuf++), &uc) != 0) {
            return -1;
        }

        if (mbedtls_test_ascii2uc(*(ibuf++), &uc2) != 0) {
            return -1;
        }

        *(obuf++) = (uc << 4) | uc2;
    }

    return 0;
}

void mbedtls_test_hexify(unsigned char *obuf,
                         const unsigned char *ibuf,
                         int len)
{
    unsigned char l, h;

    while (len != 0) {
        h = *ibuf / 16;
        l = *ibuf % 16;

        if (h < 10) {
            *obuf++ = '0' + h;
        } else {
            *obuf++ = 'a' + h - 10;
        }

        if (l < 10) {
            *obuf++ = '0' + l;
        } else {
            *obuf++ = 'a' + l - 10;
        }

        ++ibuf;
        len--;
    }
}

unsigned char *mbedtls_test_zero_alloc(size_t len)
{
    void *p;
    size_t actual_len = (len != 0) ? len : 1;

    p = mbedtls_calloc(1, actual_len);
    TEST_HELPER_ASSERT(p != NULL);

    memset(p, 0x00, actual_len);

    return p;
}

unsigned char *mbedtls_test_unhexify_alloc(const char *ibuf, size_t *olen)
{
    unsigned char *obuf;
    size_t len;

    *olen = strlen(ibuf) / 2;

    if (*olen == 0) {
        return mbedtls_test_zero_alloc(*olen);
    }

    obuf = mbedtls_calloc(1, *olen);
    TEST_HELPER_ASSERT(obuf != NULL);
    TEST_HELPER_ASSERT(mbedtls_test_unhexify(obuf, *olen, ibuf, &len) == 0);

    return obuf;
}

int mbedtls_test_hexcmp(uint8_t *a, uint8_t *b,
                        uint32_t a_len, uint32_t b_len)
{
    int ret = 0;
    uint32_t i = 0;

    if (a_len != b_len) {
        return -1;
    }

    for (i = 0; i < a_len; i++) {
        if (a[i] != b[i]) {
            ret = -1;
            break;
        }
    }
    return ret;
}

#if defined(MBEDTLS_TEST_HOOKS)
void mbedtls_test_err_add_check(int high, int low,
                                const char *file, int line)
{
    /* Error codes are always negative (a value of zero is a success) however
     * their positive opposites can be easier to understand. The following
     * examples given in comments have been made positive for ease of
     * understanding. The structure of an error code is such:
     *
     *                                                shhhhhhhhlllllll
     *
     * s = sign bit.
     * h = high level error code (includes high level module ID (bits 12..14)
     *     and module-dependent error code (bits 7..11)).
     * l = low level error code.
     */
    if (high > -0x1000 && high != 0) {
        /* high < 0001000000000000
         * No high level module ID bits are set.
         */
        mbedtls_test_fail("'high' is not a high-level error code",
                          line, file);
    } else if (high < -0x7F80) {
        /* high > 0111111110000000
         * Error code is greater than the largest allowed high level module ID.
         */
        mbedtls_test_fail("'high' error code is greater than 15 bits",
                          line, file);
    } else if ((high & 0x7F) != 0) {
        /* high & 0000000001111111
         * Error code contains low level error code bits.
         */
        mbedtls_test_fail("'high' contains a low-level error code",
                          line, file);
    } else if (low < -0x007F) {
        /* low >  0000000001111111
         * Error code contains high or module level error code bits.
         */
        mbedtls_test_fail("'low' error code is greater than 7 bits",
                          line, file);
    } else if (low > 0) {
        mbedtls_test_fail("'low' error code is greater than zero",
                          line, file);
    }
}

void (*mbedtls_test_hook_error_add)(int, int, const char *, int);

#endif /* MBEDTLS_TEST_HOOKS */
