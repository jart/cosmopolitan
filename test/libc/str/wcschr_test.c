/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bsdstdlib.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

TEST(wcschr, blank) {
  const wchar_t *const blank = L"";
  EXPECT_EQ(NULL, wcschr(blank, L'#'));
  EXPECT_EQ(blank, wcschr(blank, L'\0'));
}

TEST(wcschr, text) {
  wchar_t buf[] = L"hellothere";
  EXPECT_STREQ(L"there", wcschr(buf, L't'));
}

TEST(wcschr, testsse) {
  wchar_t buf[] = L"hellohellohellohellohellohellohellohello"
                  L"theretheretheretheretheretheretherethere";
  EXPECT_WCSEQ(L"theretheretheretheretheretheretherethere", wcschr(buf, L't'));
}

TEST(wcschrnul, text) {
  wchar_t buf[] = L"hellothere";
  EXPECT_WCSEQ(L"there", wcschrnul(buf, L't'));
}

TEST(wcschr, nulTerminator) {
  wchar_t buf[] = L"hellothere";
  EXPECT_WCSEQ(L"", wcschr(buf, L'\0'));
}

TEST(wcschrnul, nulTerminator) {
  wchar_t buf[] = L"hellothere";
  EXPECT_WCSEQ(L"", wcschrnul(buf, L'\0'));
}

TEST(wcschr, notFound_returnsNul) {
  wchar_t buf[] = L"hellothere";
  EXPECT_EQ(NULL, wcschr(buf, L'z'));
}

TEST(wcschrnul, notFound_returnsPointerToNulByte) {
  wchar_t buf[] = L"hi";
  EXPECT_WCSEQ(L"", wcschrnul(buf, L'z'));
  EXPECT_EQ(&buf[2], wcschrnul(buf, L'z'));
}

wchar_t *wcschr_pure(const wchar_t *s, wchar_t c) {
  for (;; ++s) {
    if (*s == c)
      return (wchar_t *)s;
    if (!*s)
      return NULL;
  }
}

TEST(wcschr, fuzz) {
  wchar_t *p;
  int i, j;
  p = calloc(sizeof(wchar_t), 64);
  for (i = -2; i < 257; ++i) {
    for (j = 0; j < 17; ++j) {
      arc4random_buf(p, 63);
      ASSERT_EQ(wcschr_pure(p + j, i), wcschr(p + j, i));
    }
  }
  free(p);
}

BENCH(wcschr, bench) {
  EZBENCH2("wcschr 0", donothing, __expropriate(wcschr(__veil("r", L""), 0)));
  EZBENCH2("wcschr 5", donothing,
           __expropriate(wcschr(__veil("r", L"hello"), L'o')));
  EZBENCH2("wcschr 8", donothing,
           __expropriate(wcschr(__veil("r", L"hellzzzo"), L'o')));
  EZBENCH2("wcschr 17", donothing,
           __expropriate(wcschr(__veil("r", L"hellzzzhellzzzeeo"), L'o')));
  EZBENCH2("wcschr 34", donothing,
           __expropriate(wcschr(
               __veil("r", L"hellzzzhellzzzeeAhellzzzhellzzzeeo"), L'o')));
}

wchar_t *wcschrnul_pure(const wchar_t *s, wchar_t c) {
  for (;; ++s) {
    if (*s == c)
      return (wchar_t *)s;
    if (!*s)
      return (void *)s;
  }
}

TEST(wcschrnul, fuzz) {
  int i, j;
  wchar_t *p;
  p = calloc(1, 64);
  for (i = -2; i < 257; ++i) {
    for (j = 0; j < 17; ++j) {
      arc4random_buf(p, 63);
      ASSERT_EQ(wcschrnul(p + j, i), wcschrnul_pure(p + j, i));
    }
  }
  free(p);
}

BENCH(wcschr, bench2) {
  size_t hyperionSize;
  wchar_t *hyperion = gc(utf8to32(kHyperion, -1, &hyperionSize));
  wchar_t *wcschr_(const wchar_t *, wchar_t) asm("wcschr");
  wchar_t *wcschrnul_(const wchar_t *, wchar_t) asm("wcschrnul");
  wchar_t *wcslen_(const wchar_t *) asm("wcslen");
  EZBENCH2("wcschr z", donothing, wcschr_(hyperion, L'z'));
  EZBENCH2("wcschr Z", donothing, wcschr_(hyperion, L'Z'));
  EZBENCH2("wcslen", donothing, wcslen_(hyperion));
  EZBENCH2("wcschrnul z", donothing, wcschrnul_(hyperion, L'z'));
  EZBENCH2("wcschrnul Z", donothing, wcschrnul_(hyperion, L'Z'));
}
