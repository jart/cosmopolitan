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
#include "libc/fmt/itoa.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sock.h"

const char *(DescribeSocketType)(char buf[64], int type) {
  int x;
  char *p;
  p = buf;
  x = type & ~(SOCK_CLOEXEC | SOCK_NONBLOCK);
  if (x == SOCK_STREAM) {
    p = stpcpy(p, "SOCK_STREAM");
  } else if (x == SOCK_DGRAM) {
    p = stpcpy(p, "SOCK_DGRAM");
  } else if (x == SOCK_RAW) {
    p = stpcpy(p, "SOCK_RAW");
  } else if (x == SOCK_RDM) {
    p = stpcpy(p, "SOCK_RDM");
  } else if (x == SOCK_SEQPACKET) {
    p = stpcpy(p, "SOCK_SEQPACKET");
  } else {
    p = FormatInt32(p, x);
  }
  if (type & SOCK_CLOEXEC) p = stpcpy(p, "|SOCK_CLOEXEC");
  if (type & SOCK_NONBLOCK) p = stpcpy(p, "|SOCK_NONBLOCK");
  return buf;
}
