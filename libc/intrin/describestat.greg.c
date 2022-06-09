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
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/kprintf.h"

const char *DescribeStat(int rc, const struct stat *st) {
  _Alignas(char) static char buf[300];
  int i, n;

  if (rc == -1) return "n/a";
  if (!st) return "NULL";
  if ((!IsAsan() && kisdangerous(st)) ||
      (IsAsan() && !__asan_is_valid(st, sizeof(*st)))) {
    ksnprintf(buf, sizeof(buf), "%p", st);
    return buf;
  }

  i = 0;
  n = sizeof(buf);

  i += ksnprintf(buf + i, n - i, "{.st_%s=%'ld", "size", st->st_size);

  if (st->st_blocks) {
    i +=
        ksnprintf(buf + i, n - i, ", .st_blocks=%'lu/512", st->st_blocks * 512);
  }

  if (st->st_mode) {
    i += ksnprintf(buf + i, n - i, ", .st_%s=%#o", "mode", st->st_mode);
  }

  if (st->st_nlink != 1) {
    i += ksnprintf(buf + i, n - i, ", .st_%s=%'lu", "nlink", st->st_nlink);
  }

  if (st->st_uid) {
    i += ksnprintf(buf + i, n - i, ", .st_%s=%lu", "uid", st->st_uid);
  }

  if (st->st_gid) {
    i += ksnprintf(buf + i, n - i, ", .st_%s=%lu", "gid", st->st_gid);
  }

  if (st->st_ino) {
    i += ksnprintf(buf + i, n - i, ", .st_%s=%lu", "ino", st->st_ino);
  }

  if (st->st_gen) {
    i += ksnprintf(buf + i, n - i, ", .st_%s=%'lu", "gen", st->st_gen);
  }

  if (st->st_flags) {
    i += ksnprintf(buf + i, n - i, ", .st_%s=%lx", "flags", st->st_flags);
  }

  if (st->st_rdev) {
    i += ksnprintf(buf + i, n - i, ", .st_%s=%'lu", "rdev", st->st_rdev);
  }

  if (st->st_blksize != PAGESIZE) {
    i += ksnprintf(buf + i, n - i, ", .st_%s=%'lu", "blksize", st->st_blksize);
  }

  if (n - i >= 2) {
    buf[i + 0] = '}';
    buf[i + 1] = 0;
  }

  return buf;
}
