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
#include "libc/fmt/itoa.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

static uintptr_t lastfree_;

void *AddressBirthAction(void *addr) {
  char buf[64], *p;
  p = buf;
  p = stpcpy(p, __FUNCTION__);
  p = stpcpy(p, ": 0x");
  p += uint64toarray_radix16((uintptr_t)addr, p);
  *p++ = '\n';
  __print(buf, p - buf);
  if (lastfree_ == (uintptr_t)addr) {
    lastfree_ = 0;
  }
  return addr;
}

void *AddressDeathAction(void *addr) {
  char buf[64], *p;
  p = buf;
  p = stpcpy(p, __FUNCTION__);
  p = stpcpy(p, ": 0x");
  p += uint64toarray_radix16((uintptr_t)addr, p);
  if (lastfree_ != (uintptr_t)addr) {
    lastfree_ = (uintptr_t)addr;
  } else {
    p = stpcpy(p, " [OBVIOUS DOUBLE FREE]");
  }
  *p++ = '\n';
  __print(buf, p - buf);
  return addr;
}
