/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/strace.internal.h"
#include "libc/fmt/itoa.h"
#include "libc/sysv/consts/rlimit.h"

const char *(DescribeRlimitName)(char buf[12], int resource) {
  if (resource == 127) return "n/a";
  if (resource == RLIMIT_AS) return "RLIMIT_AS";
  if (resource == RLIMIT_CPU) return "RLIMIT_CPU";
  if (resource == RLIMIT_FSIZE) return "RLIMIT_FSIZE";
  if (resource == RLIMIT_NPROC) return "RLIMIT_NPROC";
  if (resource == RLIMIT_NOFILE) return "RLIMIT_NOFILE";
  if (resource == RLIMIT_RSS) return "RLIMIT_RSS";
  if (resource == RLIMIT_DATA) return "RLIMIT_DATA";
  if (resource == RLIMIT_CORE) return "RLIMIT_CORE";
  if (resource == RLIMIT_STACK) return "RLIMIT_STACK";
  if (resource == RLIMIT_SIGPENDING) return "RLIMIT_SIGPENDING";
  if (resource == RLIMIT_MEMLOCK) return "RLIMIT_MEMLOCK";
  if (resource == RLIMIT_LOCKS) return "RLIMIT_LOCKS";
  if (resource == RLIMIT_MSGQUEUE) return "RLIMIT_MSGQUEUE";
  if (resource == RLIMIT_NICE) return "RLIMIT_NICE";
  if (resource == RLIMIT_RTPRIO) return "RLIMIT_RTPRIO";
  if (resource == RLIMIT_RTTIME) return "RLIMIT_RTTIME";
  if (resource == RLIMIT_SWAP) return "RLIMIT_SWAP";
  if (resource == RLIMIT_SBSIZE) return "RLIMIT_SBSIZE";
  if (resource == RLIMIT_NPTS) return "RLIMIT_NPTS";
  FormatInt32(buf, resource);
  return buf;
}
