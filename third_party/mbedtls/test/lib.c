/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include "third_party/mbedtls/test/lib.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/timespec.h"
#include "libc/cxxabi.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/describebacktrace.internal.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/limits.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/check.h"
#include "libc/log/libfatal.internal.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/vendor.internal.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/stdio/append.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/nr.h"
#include "libc/temp.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"
#include "third_party/mbedtls/config.h"
#include "third_party/mbedtls/endian.h"
#include "third_party/mbedtls/error.h"
#include "libc/serialize.h"
#include "third_party/mbedtls/platform.h"

asm(".ident\t\"\\n\\n\
Mbed TLS (Apache 2.0)\\n\
Copyright ARM Limited\\n\
Copyright Mbed TLS Contributors\"");
asm(".include \"libc/disclaimer.inc\"");

__static_yoink("zipos");

#if defined(MBEDTLS_PLATFORM_C)
static mbedtls_platform_context platform_ctx;
#endif

#if defined(MBEDTLS_CHECK_PARAMS)
typedef struct {
  uint8_t expected_call;
  uint8_t expected_call_happened;
  jmp_buf state;
  mbedtls_test_param_failed_location_record_t location_record;
} param_failed_ctx_t;
static param_failed_ctx_t param_failed_ctx;
#endif

struct Buffer {
  size_t i, n;
  char *p;
};

char *output;
jmp_buf jmp_tmp;
int option_verbose;
mbedtls_test_info_t mbedtls_test_info;
static char tmpdir[PATH_MAX];
static char third_party[PATH_MAX];

int mbedtls_test_platform_setup(void) {
  int ret = 0;
  const char *s;
  static char mybuf[2][BUFSIZ];
  ShowCrashReports();
  if ((s = getenv("TMPDIR"))) {
    strlcpy(tmpdir, s, sizeof(tmpdir));
    if (makedirs(tmpdir, 0755)) {
      strcpy(tmpdir, "/tmp");
    }
  } else {
    strcpy(tmpdir, "/tmp");
  }
  s = realpath("third_party/", third_party);
  strlcat(tmpdir, "/mbedtls.XXXXXX", sizeof(tmpdir));
  if (!mkdtemp(tmpdir)) {
    perror(tmpdir);
    exit(1);
  }
  if (chdir(tmpdir)) {
    perror(tmpdir);
    exit(2);
  }
  if (s) symlink(s, "third_party");
  makedirs("o/tmp", 0755);
  setvbuf(stdout, mybuf[0], _IOLBF, BUFSIZ);
  setvbuf(stderr, mybuf[1], _IOLBF, BUFSIZ);
#if defined(MBEDTLS_PLATFORM_C)
  ret = mbedtls_platform_setup(&platform_ctx);
#endif /* MBEDTLS_PLATFORM_C */
  return ret;
}

void mbedtls_test_platform_teardown(void) {
  rmrf(tmpdir);
#if defined(MBEDTLS_PLATFORM_C)
  mbedtls_platform_teardown(&platform_ctx);
#endif /* MBEDTLS_PLATFORM_C */
}

wontreturn void exit(int rc) {
  if (rc) {
    fprintf(stderr, "mbedtls test exit() called with $?=%d bt %s\n", rc,
            DescribeBacktrace(__builtin_frame_address(0)));
  }
  if (rc) {
    xwrite(1, output, appendz(output).i);
  }
  free(output);
  output = 0;
  __cxa_finalize(0);
  _Exit(rc);
}

char *GetTlsError(long r) {
  char s[128];
  if (-0x10000 < r && r < 0) {
    mbedtls_strerror(r, s, sizeof(s));
    return xasprintf("-0x%04lx %s", -r, s);
  } else {
    return xasprintf("%#lx", r);
  }
}

int mbedtls_hardware_poll(void *wut, unsigned char *p, size_t n, size_t *olen) {
  uint64_t x;
  size_t i, j;
  unsigned char b[8];
  for (i = 0; i < n; ++i) {
    x = lemur64();
    WRITE64LE(b, x);
    for (j = 0; j < 8 && i + j < n; ++j) {
      p[i + j] = b[j];
    }
  }
  *olen = n;
  return 0;
}

int mbedtls_test_write(const char *fmt, ...) {
  int n;
  va_list va;
  va_start(va, fmt);
  if (option_verbose) {
    n = vfprintf(stderr, fmt, va);
  } else {
    char buf[512];
    vsnprintf(buf, 512, fmt, va);
    n = appends(&output, buf);
  }
  va_end(va);
  return n;
}

static int ascii2uc(const char c, unsigned char *uc) {
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

int mbedtls_test_hexcmp(uint8_t *a, uint8_t *b, uint32_t a_len,
                        uint32_t b_len) {
  int ret = 0;
  uint32_t i = 0;
  if (a_len != b_len) return -1;
  for (i = 0; i < a_len; i++) {
    if (a[i] != b[i]) {
      ret = -1;
      break;
    }
  }
  return ret;
}

/**
 * \brief           Record the current test case as a failure.
 *
 *                  This function can be called directly however it is usually
 *                  called via macros such as TEST_ASSERT, TEST_EQUAL,
 *                  PSA_ASSERT, etc...
 *
 * \note            If the test case was already marked as failed, calling
 *                  `mbedtls_test_fail( )` again will not overwrite any
 *                  previous information about the failure.
 *
 * \param test      Description of the failure or assertion that failed. This
 *                  MUST be a string literal.
 * \param line_no   Line number where the failure originated.
 * \param filename  Filename where the failure originated.
 */
void mbedtls_test_fail(const char *test, int line_no, const char *filename) {
  if (mbedtls_test_info.result == MBEDTLS_TEST_RESULT_FAILED) {
    /* We've already recorded the test as having failed. Don't
     * overwrite any previous information about the failure. */
    return;
  }
  mbedtls_test_info.result = MBEDTLS_TEST_RESULT_FAILED;
  mbedtls_test_info.test = test;
  mbedtls_test_info.line_no = line_no;
  mbedtls_test_info.filename = filename;
}

#ifdef MBEDTLS_CHECK_PARAMS
void mbedtls_param_failed(const char *msg, const char *file, int line) {
  /* Record the location of the failure */
  param_failed_ctx.location_record.failure_condition = msg;
  param_failed_ctx.location_record.file = file;
  param_failed_ctx.location_record.line = line;
  /* If we are testing the callback function...  */
  if (param_failed_ctx.expected_call != 0) {
    param_failed_ctx.expected_call = 0;
    param_failed_ctx.expected_call_happened = 1;
  } else {
    /* ...else try a long jump. If the execution state has not been set-up
     * or reset then the long jump buffer is all zero's and the call will
     * with high probability fault, emphasizing there is something to look
     * at.
     */
    longjmp(param_failed_ctx.state, 1);
  }
}
#endif

/**
 * \brief           Record the current test case as skipped.
 *
 *                  This function can be called directly however it is usually
 *                  called via the TEST_ASSUME macro.
 *
 * \param test      Description of the assumption that caused the test case to
 *                  be skipped. This MUST be a string literal.
 * \param line_no   Line number where the test case was skipped.
 * \param filename  Filename where the test case was skipped.
 */
void mbedtls_test_skip(const char *test, int line_no, const char *filename) {
  mbedtls_test_info.result = MBEDTLS_TEST_RESULT_SKIPPED;
  mbedtls_test_info.test = test;
  mbedtls_test_info.line_no = line_no;
  mbedtls_test_info.filename = filename;
}

/**
 * \brief       Set the test step number for failure reports.
 *
 *              Call this function to display "step NNN" in addition to the
 *              line number and file name if a test fails. Typically the "step
 *              number" is the index of a for loop but it can be whatever you
 *              want.
 *
 * \param step  The step number to report.
 */
void mbedtls_test_set_step(unsigned long step) {
  mbedtls_test_info.step = step;
}

/**
 * \brief       Reset mbedtls_test_info to a ready/starting state.
 */
void mbedtls_test_info_reset(void) {
  mbedtls_test_info.result = MBEDTLS_TEST_RESULT_SUCCESS;
  mbedtls_test_info.step = -1;
  mbedtls_test_info.test = 0;
  mbedtls_test_info.line_no = 0;
  mbedtls_test_info.filename = 0;
}

/**
 * \brief          This function decodes the hexadecimal representation of
 *                 data.
 *
 * \note           The output buffer can be the same as the input buffer. For
 *                 any other overlapping of the input and output buffers, the
 *                 behavior is undefined.
 *
 * \param obuf     Output buffer.
 * \param obufmax  Size in number of bytes of \p obuf.
 * \param ibuf     Input buffer.
 * \param len      The number of unsigned char written in \p obuf. This must
 *                 not be \c NULL.
 *
 * \return         \c 0 on success.
 * \return         \c -1 if the output buffer is too small or the input string
 *                 is not a valid hexadecimal representation.
 */
int mbedtls_test_unhexify(unsigned char *obuf, size_t obufmax, const char *ibuf,
                          size_t *len) {
  unsigned char uc, uc2;
  *len = strlen(ibuf);
  /* Must be even number of bytes. */
  if ((*len) & 1) return -1;
  *len /= 2;
  if ((*len) > obufmax) return -1;
  while (*ibuf != 0) {
    if (ascii2uc(*(ibuf++), &uc) != 0) return -1;
    if (ascii2uc(*(ibuf++), &uc2) != 0) return -1;
    *(obuf++) = (uc << 4) | uc2;
  }
  return 0;
}

void mbedtls_test_hexify(unsigned char *obuf, const unsigned char *ibuf,
                         int len) {
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

/**
 * Allocate and zeroize a buffer.
 *
 * If the size if zero, a pointer to a zeroized 1-byte buffer is returned.
 *
 * For convenience, dies if allocation fails.
 */
unsigned char *mbedtls_test_zero_alloc(size_t len) {
  void *p;
  size_t actual_len = (len != 0) ? len : 1;
  p = mbedtls_calloc(1, actual_len);
  TEST_HELPER_ASSERT(p != NULL);
  memset(p, 0x00, actual_len);
  return (p);
}

/**
 * Allocate and fill a buffer from hex data.
 *
 * The buffer is sized exactly as needed. This allows to detect buffer
 * overruns (including overreads) when running the test suite under valgrind.
 *
 * If the size if zero, a pointer to a zeroized 1-byte buffer is returned.
 *
 * For convenience, dies if allocation fails.
 */
unsigned char *mbedtls_test_unhexify_alloc(const char *ibuf, size_t *olen) {
  unsigned char *obuf;
  size_t len;
  *olen = strlen(ibuf) / 2;
  if (*olen == 0) return (mbedtls_test_zero_alloc(*olen));
  obuf = mbedtls_calloc(1, *olen);
  TEST_HELPER_ASSERT(obuf != NULL);
  TEST_HELPER_ASSERT(mbedtls_test_unhexify(obuf, *olen, ibuf, &len) == 0);
  return (obuf);
}

#ifdef MBEDTLS_CHECK_PARAMS
/**
 * \brief   Get the location record of the last call to
 *          mbedtls_test_param_failed().
 *
 * \note    The call expectation is set up and active until the next call to
 *          mbedtls_test_param_failed_check_expected_call() or
 *          mbedtls_param_failed() that cancels it.
 */
void mbedtls_test_param_failed_get_location_record(
    mbedtls_test_param_failed_location_record_t *location_record) {
  *location_record = param_failed_ctx.location_record;
}
#endif

#ifdef MBEDTLS_CHECK_PARAMS
/**
 * \brief   State that a call to mbedtls_param_failed() is expected.
 *
 * \note    The call expectation is set up and active until the next call to
 *          mbedtls_test_param_failed_check_expected_call() or
 *          mbedtls_param_failed that cancel it.
 */
void mbedtls_test_param_failed_expect_call(void) {
  param_failed_ctx.expected_call_happened = 0;
  param_failed_ctx.expected_call = 1;
}
#endif

#ifdef MBEDTLS_CHECK_PARAMS
/**
 * \brief   Check whether mbedtls_param_failed() has been called as expected.
 *
 * \note    Check whether mbedtls_param_failed() has been called between the
 *          last call to mbedtls_test_param_failed_expect_call() and the call
 *          to this function.
 *
 * \return  \c 0 Since the last call to mbedtls_param_failed_expect_call(),
 *               mbedtls_param_failed() has been called.
 *          \c -1 Otherwise.
 */
int mbedtls_test_param_failed_check_expected_call(void) {
  param_failed_ctx.expected_call = 0;
  if (param_failed_ctx.expected_call_happened != 0) return 0;
  return -1;
}
#endif

#ifdef MBEDTLS_CHECK_PARAMS
/**
 * \brief   Get the address of the object of type jmp_buf holding the execution
 *          state information used by mbedtls_param_failed() to do a long jump.
 *
 * \note    If a call to mbedtls_param_failed() is not expected in the sense
 *          that there is no call to mbedtls_test_param_failed_expect_call()
 *          preceding it, then mbedtls_param_failed() will try to restore the
 *          execution to the state stored in the jmp_buf object whose address
 *          is returned by the present function.
 *
 * \note    This function is intended to provide the parameter of the
 *          setjmp() function to set-up where mbedtls_param_failed() should
 *          long-jump if it has to. It is foreseen to be used as:
 *
 *          setjmp( mbedtls_test_param_failed_get_state_buf() ).
 *
 * \note    The type of the returned value is not jmp_buf as jmp_buf is an
 *          an array type (C specification) and a function cannot return an
 *          array type.
 *
 * \note    The type of the returned value is not jmp_buf* as then the return
 *          value couldn't be used by setjmp(), as its parameter's type is
 *          jmp_buf.
 *
 * \return  Address of the object of type jmp_buf holding the execution state
 *          information used by mbedtls_param_failed() to do a long jump.
 */
void *mbedtls_test_param_failed_get_state_buf(void) {
  return &param_failed_ctx.state;
}
#endif

#ifdef MBEDTLS_CHECK_PARAMS
/**
 * \brief   Reset the execution state used by mbedtls_param_failed() to do a
 *          long jump.
 *
 * \note    If a call to mbedtls_param_failed() is not expected in the sense
 *          that there is no call to mbedtls_test_param_failed_expect_call()
 *          preceding it, then mbedtls_param_failed() will try to restore the
 *          execution state that this function reset.
 *
 * \note    It is recommended to reset the execution state when the state
 *          is not relevant anymore. That way an unexpected call to
 *          mbedtls_param_failed() will not trigger a long jump with
 *          undefined behavior but rather a long jump that will rather fault.
 */
void mbedtls_test_param_failed_reset_state(void) {
  memset(param_failed_ctx.state, 0, sizeof(param_failed_ctx.state));
}
#endif

/**
 * This function just returns data from rand().
 * Although predictable and often similar on multiple
 * runs, this does not result in identical random on
 * each run. So do not use this if the results of a
 * test depend on the random data that is generated.
 *
 * rng_state shall be NULL.
 */
int mbedtls_test_rnd_std_rand(void *rng_state, unsigned char *output,
                              size_t len) {
  size_t i;
  if (rng_state != NULL) rng_state = NULL;
  for (i = 0; i < len; ++i) output[i] = rand();
  return 0;
}

/**
 * This function only returns zeros
 *
 * rng_state shall be NULL.
 */
int mbedtls_test_rnd_zero_rand(void *rng_state, unsigned char *output,
                               size_t len) {
  if (rng_state != NULL) rng_state = NULL;
  memset(output, 0, len);
  return 0;
}

/**
 * This function returns random based on a buffer it receives.
 *
 * rng_state shall be a pointer to a rnd_buf_info structure.
 *
 * The number of bytes released from the buffer on each call to
 * the random function is specified by per_call. (Can be between
 * 1 and 4)
 *
 * After the buffer is empty it will return rand();
 */
int mbedtls_test_rnd_buffer_rand(void *rng_state, unsigned char *output,
                                 size_t len) {
  mbedtls_test_rnd_buf_info *info = (mbedtls_test_rnd_buf_info *)rng_state;
  size_t use_len;
  if (rng_state == NULL) return (mbedtls_test_rnd_std_rand(NULL, output, len));
  use_len = len;
  if (len > info->length) use_len = info->length;
  if (use_len) {
    memcpy(output, info->buf, use_len);
    info->buf += use_len;
    info->length -= use_len;
  }
  if (len - use_len > 0)
    return (mbedtls_test_rnd_std_rand(NULL, output + use_len, len - use_len));
  return 0;
}

/**
 * This function returns random based on a pseudo random function.
 * This means the results should be identical on all systems.
 * Pseudo random is based on the XTEA encryption algorithm to
 * generate pseudorandom.
 *
 * rng_state shall be a pointer to a rnd_pseudo_info structure.
 */
int mbedtls_test_rnd_pseudo_rand(void *rng_state, unsigned char *output,
                                 size_t len) {
  mbedtls_test_rnd_pseudo_info *info =
      (mbedtls_test_rnd_pseudo_info *)rng_state;
  uint32_t i, *k, sum, delta = 0x9E3779B9;
  unsigned char result[4], *out = output;
  if (rng_state == NULL) return (mbedtls_test_rnd_std_rand(NULL, output, len));
  k = info->key;
  while (len > 0) {
    size_t use_len = (len > 4) ? 4 : len;
    sum = 0;
    for (i = 0; i < 32; i++) {
      info->v0 +=
          (((info->v1 << 4) ^ (info->v1 >> 5)) + info->v1) ^ (sum + k[sum & 3]);
      sum += delta;
      info->v1 += (((info->v0 << 4) ^ (info->v0 >> 5)) + info->v0) ^
                  (sum + k[(sum >> 11) & 3]);
    }
    PUT_UINT32_BE(info->v0, result, 0);
    memcpy(out, result, use_len);
    len -= use_len;
    out += 4;
  }
  return 0;
}

/**
 * \brief       Verifies that string is in string parameter format i.e. "<str>"
 *              It also strips enclosing '"' from the input string.
 *
 * \param str   String parameter.
 *
 * \return      0 if success else 1
 */
int verify_string(char **str) {
  if ((*str)[0] != '"' || (*str)[strlen(*str) - 1] != '"') {
    WRITE("Expected string (with \"\") for parameter and got: %`'s\n", *str);
    return -1;
  }
  (*str)++;
  (*str)[strlen(*str) - 1] = '\0';
  return 0;
}

/**
 * \brief       Verifies that string is an integer. Also gives the converted
 *              integer value.
 *
 * \param str   Input string.
 * \param value Pointer to int for output value.
 *
 * \return      0 if success else 1
 */
int verify_int(char *str, int *value) {
  size_t i;
  int minus = 0;
  int digits = 1;
  int hex = 0;
  for (i = 0; i < strlen(str); i++) {
    if (i == 0 && str[i] == '-') {
      minus = 1;
      continue;
    }
    if (((minus && i == 2) || (!minus && i == 1)) && str[i - 1] == '0' &&
        (str[i] == 'x' || str[i] == 'X')) {
      hex = 1;
      continue;
    }
    if (!((str[i] >= '0' && str[i] <= '9') ||
          (hex && ((str[i] >= 'a' && str[i] <= 'f') ||
                   (str[i] >= 'A' && str[i] <= 'F'))))) {
      digits = 0;
      break;
    }
  }
  if (digits) {
    if (hex)
      *value = strtol(str, NULL, 16);
    else
      *value = strtol(str, NULL, 10);
    return 0;
  }
  WRITE("Expected integer for parameter and got: %s\n", str);
  return KEY_VALUE_MAPPING_NOT_FOUND;
}

/**
 * \brief       Read a line from the passed file pointer.
 *
 * \param f     FILE pointer
 * \param buf   Pointer to memory to hold read line.
 * \param len   Length of the buf.
 *
 * \return      0 if success else -1
 */
int get_line(FILE *f, char *buf, size_t len) {
  char *ret;
  int i = 0, str_len = 0, has_string = 0;
  /* Read until we get a valid line */
  do {
    ret = fgets(buf, len, f);
    if (ret == NULL) return -1;
    str_len = strlen(buf);
    /* Skip empty line and comment */
    if (str_len == 0 || buf[0] == '#') continue;
    has_string = 0;
    for (i = 0; i < str_len; i++) {
      char c = buf[i];
      if (c != ' ' && c != '\t' && c != '\n' && c != '\v' && c != '\f' &&
          c != '\r') {
        has_string = 1;
        break;
      }
    }
  } while (!has_string);
  /* Strip new line and carriage return */
  ret = buf + strlen(buf);
  if (ret-- > buf && *ret == '\n') *ret = '\0';
  if (ret-- > buf && *ret == '\r') *ret = '\0';
  return 0;
}

/**
 * \brief       Splits string delimited by ':'. Ignores '\:'.
 *
 * \param buf           Input string
 * \param len           Input string length
 * \param params        Out params found
 * \param params_len    Out params array len
 *
 * \return      Count of strings found.
 */
static int parse_arguments(char *buf, size_t len, char **params,
                           size_t params_len) {
  int t = 0;
  size_t cnt = 0, i;
  char *cur = buf;
  char *p = buf, *q;
  params[cnt++] = cur;
  while (*p != '\0' && p < (buf + len)) {
    if (*p == '"') {
      if (t) {
        t = 0;
      } else {
        t = 1;
      }
    }
    if (*p == '\\') {
      p++;
      p++;
      continue;
    }
    if (*p == ':' && !t) {
      if (p + 1 < buf + len) {
        cur = p + 1;
        TEST_HELPER_ASSERT(cnt < params_len);
        params[cnt++] = cur;
      }
      *p = '\0';
    }
    p++;
  }
  /* Replace newlines, question marks and colons in strings */
  for (i = 0; i < cnt; i++) {
    p = params[i];
    q = params[i];
    while (*p != '\0') {
      if (*p == '\\' && *(p + 1) == 'n') {
        p += 2;
        *(q++) = '\n';
      } else if (*p == '\\' && *(p + 1) == ':') {
        p += 2;
        *(q++) = ':';
      } else if (*p == '\\' && *(p + 1) == '?') {
        p += 2;
        *(q++) = '?';
      } else
        *(q++) = *(p++);
    }
    *q = '\0';
  }
  return (cnt);
}

/**
 * \brief       Converts parameters into test function consumable parameters.
 *              Example: Input:  {"int", "0", "char*", "Hello",
 *                                "hex", "abef", "exp", "1"}
 *                      Output:  {
 *                                0,                // Verified int
 *                                "Hello",          // Verified string
 *                                2, { 0xab, 0xef },// Converted len,hex pair
 *                                9600              // Evaluated expression
 *                               }
 *
 *
 * \param cnt               Parameter array count.
 * \param params            Out array of found parameters.
 * \param int_params_store  Memory for storing processed integer parameters.
 *
 * \return      0 for success else 1
 */
static int convert_params(size_t cnt, char **params, int *int_params_store) {
  char **cur = params;
  char **out = params;
  int ret = DISPATCH_TEST_SUCCESS;
  while (cur < params + cnt) {
    char *type = *cur++;
    char *val = *cur++;
    if (strcmp(type, "char*") == 0) {
      if (verify_string(&val) == 0) {
        *out++ = val;
      } else {
        ret = (DISPATCH_INVALID_TEST_DATA);
        break;
      }
    } else if (strcmp(type, "int") == 0) {
      if (verify_int(val, int_params_store) == 0) {
        *out++ = (char *)int_params_store++;
      } else {
        ret = (DISPATCH_INVALID_TEST_DATA);
        break;
      }
    } else if (strcmp(type, "hex") == 0) {
      if (verify_string(&val) == 0) {
        size_t len;
        TEST_HELPER_ASSERT(mbedtls_test_unhexify((unsigned char *)val,
                                                 strlen(val), val, &len) == 0);
        *int_params_store = len;
        *out++ = val;
        *out++ = (char *)(int_params_store++);
      } else {
        ret = (DISPATCH_INVALID_TEST_DATA);
        break;
      }
    } else if (strcmp(type, "exp") == 0) {
      int exp_id = strtol(val, NULL, 10);
      if (get_expression(exp_id, int_params_store) == 0) {
        *out++ = (char *)int_params_store++;
      } else {
        ret = (DISPATCH_INVALID_TEST_DATA);
        break;
      }
    } else {
      ret = (DISPATCH_INVALID_TEST_DATA);
      break;
    }
  }
  return (ret);
}

/**
 * \brief       Tests snprintf implementation with test input.
 *
 * \note
 * At high optimization levels (e.g. gcc -O3), this function may be
 * inlined in run_test_snprintf. This can trigger a spurious warning about
 * potential misuse of snprintf from gcc -Wformat-truncation (observed with
 * gcc 7.2). This warning makes tests in run_test_snprintf redundant on gcc
 * only. They are still valid for other compilers. Avoid this warning by
 * forbidding inlining of this function by gcc.
 *
 * \param n         Buffer test length.
 * \param ref_buf   Expected buffer.
 * \param ref_ret   Expected snprintf return value.
 *
 * \return      0 for success else 1
 */
static dontinline int test_snprintf(size_t n, const char *ref_buf,
                                    int ref_ret) {
  int ret;
  char buf[10] = "xxxxxxxxx";
  const char ref[10] = "xxxxxxxxx";
  if (n >= sizeof(buf)) return -1;
  ret = mbedtls_snprintf(buf, n, "%s", "123");
  if (ret < 0 || (size_t)ret >= n) ret = -1;
  if (strncmp(ref_buf, buf, sizeof(buf)) != 0 || ref_ret != ret ||
      memcmp(buf + n, ref + n, sizeof(buf) - n) != 0) {
    return 1;
  }
  return 0;
}

/**
 * \brief       Tests snprintf implementation.
 *
 * \return      0 for success else 1
 */
static int run_test_snprintf(void) {
  return (test_snprintf(0, "xxxxxxxxx", -1) != 0 ||
          test_snprintf(1, "", -1) != 0 || test_snprintf(2, "1", -1) != 0 ||
          test_snprintf(3, "12", -1) != 0 || test_snprintf(4, "123", 3) != 0 ||
          test_snprintf(5, "123", 3) != 0);
}

/**
 * \brief Write the description of the test case to the outcome CSV file.
 *
 * \param outcome_file  The file to write to.
 *                      If this is \c NULL, this function does nothing.
 * \param argv0         The test suite name.
 * \param test_case     The test case description.
 */
static void write_outcome_entry(FILE *outcome_file, const char *argv0,
                                const char *test_case) {
  /* The non-varying fields are initialized on first use. */
  static const char *platform = NULL;
  static const char *configuration = NULL;
  static const char *test_suite = NULL;
  if (outcome_file == NULL) return;
  if (platform == NULL) {
    platform = getenv("MBEDTLS_TEST_PLATFORM");
    if (platform == NULL) platform = "unknown";
  }
  if (configuration == NULL) {
    configuration = getenv("MBEDTLS_TEST_CONFIGURATION");
    if (configuration == NULL) configuration = "unknown";
  }
  if (test_suite == NULL) {
    test_suite = strrchr(argv0, '/');
    if (test_suite != NULL)
      test_suite += 1;  // skip the '/'
    else
      test_suite = argv0;
  }
  /* Write the beginning of the outcome line.
   * Ignore errors: writing the outcome file is on a best-effort basis. */
  WRITE("%s;%s;%s;%s;", platform, configuration, test_suite, test_case);
}

/**
 * \brief Write the result of the test case to the outcome CSV file.
 *
 * \param outcome_file  The file to write to.
 *                      If this is \c NULL, this function does nothing.
 * \param unmet_dep_count            The number of unmet dependencies.
 * \param unmet_dependencies         The array of unmet dependencies.
 * \param missing_unmet_dependencies Non-zero if there was a problem tracking
 *                                   all unmet dependencies, 0 otherwise.
 * \param ret                        The test dispatch status (DISPATCH_xxx).
 * \param info                       A pointer to the test info structure.
 */
static void write_outcome_result(FILE *outcome_file, size_t unmet_dep_count,
                                 int unmet_dependencies[],
                                 int missing_unmet_dependencies, int ret,
                                 const mbedtls_test_info_t *info) {
  if (outcome_file == NULL) return;
  /* Write the end of the outcome line.
   * Ignore errors: writing the outcome file is on a best-effort basis. */
  switch (ret) {
    case DISPATCH_TEST_SUCCESS:
      if (unmet_dep_count > 0) {
        size_t i;
        WRITE("SKIP");
        for (i = 0; i < unmet_dep_count; i++) {
          WRITE("%c%d", i == 0 ? ';' : ':', unmet_dependencies[i]);
        }
        if (missing_unmet_dependencies) WRITE(":...");
        break;
      }
      switch (info->result) {
        case MBEDTLS_TEST_RESULT_SUCCESS:
          WRITE("PASS;");
          break;
        case MBEDTLS_TEST_RESULT_SKIPPED:
          WRITE("SKIP;Runtime skip");
          break;
        default:
          WRITE("FAIL;%s:%d:%s", info->filename, info->line_no, info->test);
          break;
      }
      break;
    case DISPATCH_TEST_FN_NOT_FOUND:
      WRITE("FAIL;Test function not found");
      break;
    case DISPATCH_INVALID_TEST_DATA:
      WRITE("FAIL;Invalid test data");
      break;
    case DISPATCH_UNSUPPORTED_SUITE:
      WRITE("SKIP;Unsupported suite");
      break;
    default:
      WRITE("FAIL;Unknown cause");
      break;
  }
  WRITE("\n");
  fflush(outcome_file);
}

/**
 * \brief       Desktop implementation of execute_tests().
 *              Parses command line and executes tests from
 *              supplied or default data file.
 *
 * \param argc  Command line argument count.
 * \param argv  Argument array.
 *
 * \return      Program exit status.
 */
int execute_tests(int argc, const char **argv, const char *default_filename) {
  /* Local Configurations and options */
  struct timespec t1, t2;
  const char *test_filename = NULL;
  const char **test_files = NULL;
  size_t testfile_count = 0;
  size_t function_id = 0;
  /* Other Local variables */
  int arg_index = 1;
  const char *next_arg;
  size_t testfile_index, i, cnt;
  int ret;
  unsigned total_errors = 0, total_tests = 0, total_skipped = 0;
  FILE *file;
  char buf[5000];
  char *params[50];
  /* Store for proccessed integer params. */
  int int_params[50];
  void *pointer;
  const char *outcome_file_name = getenv("MBEDTLS_TEST_OUTCOME_FILE");
  FILE *outcome_file = NULL;
  /*
   * The C standard doesn't guarantee that all-bits-0 is the representation
   * of a NULL pointer. We do however use that in our code for initializing
   * structures, which should work on every modern platform. Let's be sure.
   */
  memset(&pointer, 0, sizeof(void *));
  if (pointer != NULL) {
    WRITE("all-bits-zero is not a NULL pointer\n");
    return 1;
  }
  /*
   * Make sure we have a snprintf that correctly zero-terminates
   */
  if (run_test_snprintf() != 0) {
    WRITE("the snprintf implementation is broken\n");
    return 1;
  }
  if (outcome_file_name != NULL && *outcome_file_name != '\0') {
    outcome_file = fopen(outcome_file_name, "a");
    if (outcome_file == NULL) {
      WRITE("Unable to open outcome file. Continuing anyway.\n");
    }
  }
  while (arg_index < argc) {
    next_arg = argv[arg_index];
    if (strcmp(next_arg, "--verbose") == 0 || strcmp(next_arg, "-v") == 0) {
      option_verbose = 1;
    } else if (strcmp(next_arg, "--help") == 0 || strcmp(next_arg, "-h") == 0) {
      WRITE(
          "Usage: %s [OPTIONS] files...\n\n"
          "   Command line arguments:\n"
          "     files...          One or more test data files. If no file is\n"
          "                       specified the following default test case\n"
          "                       file is used:\n"
          "                           %s\n\n"
          "   Options:\n"
          "     -v | --verbose    Display full information about each test\n"
          "     -h | --help       Display this information\n\n",
          argv[0], "TESTCASE_FILENAME");
      exit(EXIT_SUCCESS);
    } else {
      /* Not an option, therefore treat all further arguments as the file
       * list.
       */
      test_files = &argv[arg_index];
      testfile_count = argc - arg_index;
    }
    arg_index++;
  }
  /* If no files were specified, assume a default */
  if (test_files == NULL || testfile_count == 0) {
    test_files = &default_filename;
    testfile_count = 1;
  }
  /* Initialize the struct that holds information about the last test */
  mbedtls_test_info_reset();
  /* Now begin to execute the tests in the testfiles */
  for (testfile_index = 0; testfile_index < testfile_count; testfile_index++) {
    size_t unmet_dep_count = 0;
    int unmet_dependencies[20];
    int missing_unmet_dependencies = 0;
    test_filename = test_files[testfile_index];
    file = fopen(test_filename, "r");
    if (file == NULL) {
      WRITE("%s (%s) failed to open test file: %s %m\n",
            firstnonnull(program_invocation_short_name, "unknown"),
            GetProgramExecutableName(), test_filename);
      if (outcome_file != NULL) fclose(outcome_file);
      return 1;
    }
    while (!feof(file)) {
      if (unmet_dep_count > 0) {
        WRITE("FATAL: Dep count larger than zero at start of loop\n");
        exit(EXIT_FAILURE);
      }
      unmet_dep_count = 0;
      missing_unmet_dependencies = 0;
      if ((ret = get_line(file, buf, sizeof(buf))) != 0) break;
      WRITE("%s%.66s",
            mbedtls_test_info.result == MBEDTLS_TEST_RESULT_FAILED ? "\n" : "",
            buf);
      WRITE(" ");
      for (i = strlen(buf) + 1; i < 67; i++) WRITE(".");
      WRITE(" ");
      fflush(stdout);
      write_outcome_entry(outcome_file, argv[0], buf);
      total_tests++;
      if ((ret = get_line(file, buf, sizeof(buf))) != 0) break;
      cnt = parse_arguments(buf, strlen(buf), params,
                            sizeof(params) / sizeof(params[0]));
      if (strcmp(params[0], "depends_on") == 0) {
        for (i = 1; i < cnt; i++) {
          int dep_id = strtol(params[i], NULL, 10);
          if (dep_check(dep_id) != DEPENDENCY_SUPPORTED) {
            if (unmet_dep_count < ARRAY_LENGTH(unmet_dependencies)) {
              unmet_dependencies[unmet_dep_count] = dep_id;
              unmet_dep_count++;
            } else {
              missing_unmet_dependencies = 1;
            }
          }
        }
        if ((ret = get_line(file, buf, sizeof(buf))) != 0) break;
        cnt = parse_arguments(buf, strlen(buf), params,
                              sizeof(params) / sizeof(params[0]));
      }
      // If there are no unmet dependencies execute the test
      t1 = timespec_real();
      if (unmet_dep_count == 0) {
        mbedtls_test_info_reset();
        function_id = strtoul(params[0], NULL, 10);
        if ((ret = check_test(function_id)) == DISPATCH_TEST_SUCCESS) {
          ret = convert_params(cnt - 1, params + 1, int_params);
          if (DISPATCH_TEST_SUCCESS == ret) {
            ret = dispatch_test(function_id, (void **)(params + 1));
          }
        }
      }
      t2 = timespec_real();
      write_outcome_result(outcome_file, unmet_dep_count, unmet_dependencies,
                           missing_unmet_dependencies, ret, &mbedtls_test_info);
      if (unmet_dep_count > 0 || ret == DISPATCH_UNSUPPORTED_SUITE) {
        total_skipped++;
        WRITE("----");
        if (1 == option_verbose && unmet_dep_count > 0) {
          WRITE(" (unmet dependencies: ");
          for (i = 0; i < unmet_dep_count; i++) {
            if (i) WRITE(",");
            WRITE("%d", unmet_dependencies[i]);
          }
          if (missing_unmet_dependencies) WRITE("...");
        }
        WRITE(")\n");
        fflush(stdout);
        unmet_dep_count = 0;
        missing_unmet_dependencies = 0;
      } else if (ret == DISPATCH_TEST_SUCCESS) {
        if (mbedtls_test_info.result == MBEDTLS_TEST_RESULT_SUCCESS) {
          WRITE("PASS (%,ldus)\n", timespec_tomicros(timespec_sub(t2, t1)));
        } else if (mbedtls_test_info.result == MBEDTLS_TEST_RESULT_SKIPPED) {
          WRITE("----");
          total_skipped++;
        } else {
          total_errors++;
          WRITE("FAILED\n");
          WRITE("  %s\n  at ", mbedtls_test_info.test);
          if (mbedtls_test_info.step != -1) {
            WRITE("step %lu, ", mbedtls_test_info.step);
          }
          WRITE("line %d, %s", mbedtls_test_info.line_no,
                mbedtls_test_info.filename);
        }
        fflush(stdout);
      } else if (ret == DISPATCH_INVALID_TEST_DATA) {
        WRITE("FAILED: FATAL PARSE ERROR\n");
        fclose(file);
        exit(2);
      } else if (ret == DISPATCH_TEST_FN_NOT_FOUND) {
        WRITE("FAILED: FATAL TEST FUNCTION NOT FOUND\n");
        fclose(file);
        exit(2);
      } else
        total_errors++;
    }
    fclose(file);
  }
  if (outcome_file != NULL) fclose(outcome_file);
  WRITE("\n--------------------------------------------------"
        "--------------------------\n\n");
  if (total_errors == 0)
    WRITE("PASSED");
  else
    WRITE("FAILED");
  WRITE(" (%u / %u tests (%u skipped))\n", total_tests - total_errors,
        total_tests, total_skipped);
#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C) && \
    !defined(TEST_SUITE_MEMORY_BUFFER_ALLOC)
#if defined(MBEDTLS_MEMORY_DEBUG)
  mbedtls_memory_buffer_alloc_status();
#endif
  mbedtls_memory_buffer_alloc_free();
#endif
  return total_errors != 0;
}
