/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

#define T(NAME) NAME
#define S(S)    S
#define C(C)    C
#include "test/libc/str/strrchr_test.inc"
#undef C
#undef S
#undef T

#define T(NAME)       NAME##16
#define S(S)          u##S
#define C(C)          u##C
#define strrchr(x, y) strrchr16(x, y)
#define strchr(x, y)  strchr16(x, y)
#include "test/libc/str/strrchr_test.inc"
#undef strchr
#undef strrchr
#undef C
#undef S
#undef T

#define T(NAME)       NAME##32
#define S(S)          L##S
#define C(C)          L##C
#define strchr(x, y)  wcschr(x, y)
#define strrchr(x, y) wcsrchr(x, y)
#include "test/libc/str/strrchr_test.inc"
#undef strchr
#undef strrchr
#undef C
#undef S
#undef T
