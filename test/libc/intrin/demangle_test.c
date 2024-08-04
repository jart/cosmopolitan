/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "demangle_cases.inc"
#include "libc/cosmo.h"
#include "libc/macros.h"
#include "libc/stdio/internal.h"
#include "libc/str/str.h"

char got[19500];

static void regenerate_cases(void) {
  FILE *f = fopen("demangle_cases.inc", "w");
  fprintf(f, "const char* demangle_cases[][2] = {\n");
  for (int i = 0; i < ARRAYLEN(demangle_cases); ++i) {
    const char *input = demangle_cases[i][0];
    const char *want = demangle_cases[i][1];
    if (__demangle(got, input, sizeof(got)) == -1) {
      fprintf(f, "\n    // {%`'s,\n    //  %`'s},\n    // got error\n\n", input,
              want);
    } else if (!strcmp(want, got)) {
      fprintf(f, "    {%`'s, %`'s},\n", input, got);
    } else {
      fprintf(f,
              "\n    // {%`'s,\n    //  %`'s},\n    //  %`'s was returned\n\n",
              input, want, got);
    }
    fflush(f);
  }
  fprintf(f, "};\n");
  fclose(f);
}

static int test_cases_success(void) {
  for (int i = 0; i < ARRAYLEN(demangle_cases); ++i) {
    const char *input = demangle_cases[i][0];
    const char *want = demangle_cases[i][1];
    if (__demangle(got, input, sizeof(got)) == -1) {
      fprintf(stderr, "%s:%d: error: demangle failed\n", __FILE__, __LINE__);
      fprintf(stderr, "\tinput %`'s\n", input);
      fprintf(stderr, "\t want %`'s\n", want);
      return 1;
    }
    if (strcmp(want, got)) {
      fprintf(stderr, "%s:%d: error: demangle incorrect\n", __FILE__, __LINE__);
      fprintf(stderr, "\tinput %`'s\n", input);
      fprintf(stderr, "\t want %`'s\n", want);
      fprintf(stderr, "\t  got %`'s\n", got);
      return 2;
    }
  }
  return 0;
}

static int test_is_mangled(void) {
  if (__is_mangled(0))
    return 3;
  if (__is_mangled(""))
    return 4;
  if (__is_mangled("__dlmalloc"))
    return 5;
  if (!__is_mangled("_ZN4dyld18getCoalescedImagesEPP11ImageLoader"))
    return 6;
  if (!__is_mangled("_GLOBAL__I_lol"))
    return 7;
  return 0;
}

static int test_oom(void) {
  char got[2048];
  const char *sym = "_ZN10__cxxabiv112_GLOBAL__N_119InitByteGlobalMutexINS0_"
                    "11LibcppMutexENS0_13LibcppCondVarEL_ZNS0_"
                    "12GlobalStaticIS2_E8instanceEEL_ZNS4_IS3_E8instanceEEXadL_"
                    "ZNS0_16PlatformThreadIDEvEEE9LockGuardC2EPKc";
  if (__demangle(got, sym, sizeof(got)) != -1)
    return 8;
  return 0;
}

static int test_compiler_suffixes(void) {
  char got[1400];
  const char *sym = "_ZN12_GLOBAL__N_18tinyBLASILi6ELi8EDv8_fS1_"
                    "fffE4gemmILi1ELi1ELi0EEEvllll.constprop.0";
  if (__demangle(got, sym, sizeof(got)) == -1)
    return 9;
  return 0;
}

static int test_weird_destructors(void) {
  char got[216];
  const char *sym = "_ZN13AutoEncoderKLD5Ev";
  if (__demangle(got, sym, sizeof(got)) == -1)
    return 10;
  if (strcmp(got, "AutoEncoderKL::~AutoEncoderKL()"))
    return 11;
  return 0;
}

static int test_guard_variable(void) {
  char got[250];
  const char *sym = "_ZGVZ23llama_print_system_infoE1s";
  if (__demangle(got, sym, sizeof(got)) == -1)
    return 10;
  if (strcmp(got, "guard variable for llama_print_system_info::s"))
    return 11;
  return 0;
}

int main() {
  int rc;
  if ((rc = test_guard_variable()))
    return rc;
  if ((rc = test_weird_destructors()))
    return rc;
  if ((rc = test_compiler_suffixes()))
    return rc;
  if ((rc = test_cases_success()))
    return rc;
  if ((rc = test_is_mangled()))
    return rc;
  if ((rc = test_oom()))
    return rc;
}
