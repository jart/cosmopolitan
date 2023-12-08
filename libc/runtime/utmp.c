/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/runtime/utmp.h"
#include "libc/errno.h"
#include "libc/runtime/utmpx.h"

void endutxent(void) {
}

void updwtmpx(const char *x, const struct utmpx *y) {
}

struct utmpx *pututxline(const struct utmpx *p) {
  return 0;
}

void setutxent(void) {
}

struct utmpx *getutxent(void) {
  return 0;
}

struct utmpx *getutxid(const struct utmpx *x) {
  return 0;
}

struct utmpx *getutxline(const struct utmpx *x) {
  return 0;
}

int __utmpxname() {
  errno = ENOTSUP;
  return -1;
}

__weak_reference(endutxent, endutent);
__weak_reference(setutxent, setutent);
__weak_reference(getutxent, getutent);
__weak_reference(getutxid, getutid);
__weak_reference(getutxline, getutline);
__weak_reference(pututxline, pututline);
__weak_reference(updwtmpx, updwtmp);
__weak_reference(__utmpxname, utmpname);
__weak_reference(__utmpxname, utmpxname);
