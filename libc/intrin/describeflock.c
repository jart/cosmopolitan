/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/flock.h"
#include "libc/calls/struct/flock.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/sysv/consts/f.h"

#define N 300

#define append(...) o += ksnprintf(buf + o, N - o, __VA_ARGS__)

const char *(DescribeFlock)(char buf[N], int cmd, const struct flock *l) {
  int o = 0;

  if (!l) return "NULL";
  if ((!IsAsan() && kisdangerous(l)) ||
      (IsAsan() && !__asan_is_valid(l, sizeof(*l)))) {
    ksnprintf(buf, N, "%p", l);
    return buf;
  }

  append("{.l_type=%s", DescribeFlockType(l->l_type));

  if (l->l_whence) {
    append(", .l_whence=%s", DescribeWhence(l->l_whence));
  }

  if (l->l_start) {
    append(", .l_start=%#lx", l->l_start);
  }

  if (l->l_len) {
    append(", .l_len=%'ld", l->l_len);
  }

  if (l->l_pid && (cmd == F_GETLK || cmd == F_OFD_GETLK)) {
    append(", .l_pid=%d", l->l_pid);
  }

  if (IsFreebsd() && l->l_sysid) {
    append(", .l_sysid=%d", l->l_sysid);
  }

  append("}");

  return buf;
}
