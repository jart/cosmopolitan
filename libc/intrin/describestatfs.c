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
#include "libc/calls/struct/statfs.h"
#include "libc/calls/struct/statfs.internal.h"
#include "libc/dce.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/sysv/consts/st.h"

#define N 300

#define append(...) i += ksnprintf(buf + i, N - i, __VA_ARGS__)

const char *(DescribeStatfs)(char buf[N], int rc, const struct statfs *f) {
  int i = 0;
  char ibuf[21];
  int64_t flags;

  if (rc == -1) return "n/a";
  if (!f) return "NULL";
  if ((!IsAsan() && kisdangerous(f)) ||
      (IsAsan() && !__asan_is_valid(f, sizeof(*f)))) {
    ksnprintf(buf, N, "%p", f);
    return buf;
  }

  append("{.f_type=%#lx /* %s */", f->f_type, f->f_fstypename);

  sizefmt(ibuf, f->f_bsize, 1024);
  append(", .f_%s=%sb", "bsize", ibuf);

  sizefmt(ibuf, f->f_blocks * f->f_bsize, 1024);
  append(", .f_%s=%sb", "blocks", ibuf);

  sizefmt(ibuf, f->f_bfree * f->f_bsize, 1024);
  append(", .f_%s=%sb", "bfree", ibuf);

  sizefmt(ibuf, f->f_bavail * f->f_bsize, 1024);
  append(", .f_%s=%sb", "bavail", ibuf);

  append(", .f_%s=%'zu", "files", f->f_files);
  append(", .f_%s=%'zu", "ffree", f->f_ffree);
  append(", .f_fsid=%#lx", f->f_fsid);
  append(", .f_%s=%'zu", "namelen", f->f_namelen);
  append(", .f_%s=%d", "owner", f->f_owner);

  flags = f->f_flags;
  append(", .f_flags=");
  if (ST_RDONLY && (flags & ST_RDONLY)) {
    append("ST_RDONLY|");
    flags &= ~ST_RDONLY;
  }
  if (ST_NOSUID && (flags & ST_NOSUID)) {
    append("ST_NOSUID|");
    flags &= ~ST_NOSUID;
  }
  if (ST_NODEV && (flags & ST_NODEV)) {
    append("ST_NODEV|");
    flags &= ~ST_NODEV;
  }
  if (ST_NOEXEC && (flags & ST_NOEXEC)) {
    append("ST_NOEXEC|");
    flags &= ~ST_NOEXEC;
  }
  if (ST_SYNCHRONOUS && (flags & ST_SYNCHRONOUS)) {
    append("ST_SYNCHRONOUS|");
    flags &= ~ST_SYNCHRONOUS;
  }
  if (ST_MANDLOCK && (flags & ST_MANDLOCK)) {
    append("ST_MANDLOCK|");
    flags &= ~ST_MANDLOCK;
  }
  if (ST_WRITE && (flags & ST_WRITE)) {
    append("ST_WRITE|");
    flags &= ~ST_WRITE;
  }
  if (ST_APPEND && (flags & ST_APPEND)) {
    append("ST_APPEND|");
    flags &= ~ST_APPEND;
  }
  if (ST_IMMUTABLE && (flags & ST_IMMUTABLE)) {
    append("ST_IMMUTABLE|");
    flags &= ~ST_IMMUTABLE;
  }
  if (ST_NOATIME && (flags & ST_NOATIME)) {
    append("ST_NOATIME|");
    flags &= ~ST_NOATIME;
  }
  if (ST_NODIRATIME && (flags & ST_NODIRATIME)) {
    append("ST_NODIRATIME|");
    flags &= ~ST_NODIRATIME;
  }
  if (ST_RELATIME && (flags & ST_RELATIME)) {
    append("ST_RELATIME|");
    flags &= ~ST_RELATIME;
  }

  append("%#lx}", flags);

  return buf;
}
