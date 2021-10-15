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
#include "libc/calls/ntspawn.h"
#include "libc/runtime/gc.internal.h"
#include "libc/testlib/testlib.h"

char16_t envvars[ARG_MAX];

TEST(mkntenvblock, emptyList_onlyOutputsDoubleNulStringTerminator) {
  char *envp[] = {NULL};
  ASSERT_NE(-1, mkntenvblock(envvars, envp, NULL));
  ASSERT_BINEQ(u"  ", envvars);
}

TEST(mkntenvblock, envp_becomesSortedDoubleNulTerminatedUtf16String) {
  char *envp[] = {"u=b", "c=d", "韩=非", "uh=d", "hduc=d", NULL};
  ASSERT_NE(-1, mkntenvblock(envvars, envp, NULL));
  ASSERT_BINEQ(u"C = d   "
               u"H D U C = d   "
               u"U = b   "
               u"U H = d   "
               u"Θù= ^ù  "
               u"  ",
               envvars);
}

TEST(mkntenvblock, extraVar_getsAdded) {
  char *envp[] = {"u=b", "c=d", "韩=非", "uh=d", "hduc=d", NULL};
  ASSERT_NE(-1, mkntenvblock(envvars, envp, "a=a"));
  ASSERT_BINEQ(u"A = a   "
               u"C = d   "
               u"H D U C = d   "
               u"U = b   "
               u"U H = d   "
               u"Θù= ^ù  "
               u"  ",
               envvars);
}
