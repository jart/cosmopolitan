/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/describentoverlapped.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"

const char *(DescribeNtOverlapped)(char b[128], const struct NtOverlapped *o) {
  int i = 0, n = 128;
  bool gotsome = false;
  if (!o) return "NULL";
  i += ksnprintf(b + i, MAX(0, n - i), "{");

  if (o->hEvent) {
    if (gotsome) {
      i += ksnprintf(b + i, MAX(0, n - i), ", ");
    } else {
      gotsome = true;
    }
    i += ksnprintf(b + i, MAX(0, n - i), ".hEvent = %ld", o->hEvent);
  }

  if (o->Pointer) {
    if (gotsome) {
      i += ksnprintf(b + i, MAX(0, n - i), ", ");
    } else {
      gotsome = true;
    }
    i += ksnprintf(b + i, MAX(0, n - i), ".Pointer = (void *)%p", o->Pointer);
  }

  i += ksnprintf(b + i, MAX(0, n - i), "}");
  return b;
}
