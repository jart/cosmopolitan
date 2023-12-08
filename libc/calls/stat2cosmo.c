/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/metastat.internal.h"
#include "libc/dce.h"

void __stat2cosmo(struct stat *restrict st, const union metastat *ms) {
  if (st) {
    if (IsLinux()) {
      st->st_dev = ms->linux.st_dev;
      st->st_ino = ms->linux.st_ino;
      st->st_nlink = ms->linux.st_nlink;
      st->st_mode = ms->linux.st_mode;
      st->st_uid = ms->linux.st_uid;
      st->st_gid = ms->linux.st_gid;
      st->st_flags = 0;
      st->st_gen = 0;
      st->st_rdev = ms->linux.st_rdev;
      st->st_size = ms->linux.st_size;
      st->st_blksize = ms->linux.st_blksize;
      st->st_blocks = ms->linux.st_blocks;
      st->st_atim = ms->linux.st_atim;
      st->st_mtim = ms->linux.st_mtim;
      st->st_ctim = ms->linux.st_ctim;
      st->st_birthtim = st->st_ctim;
      if (st->st_atim.tv_sec < st->st_ctim.tv_sec)
        st->st_birthtim = st->st_atim;
      if (st->st_mtim.tv_sec < st->st_ctim.tv_sec)
        st->st_birthtim = st->st_mtim;
    } else if (IsXnu()) {
      st->st_dev = ms->xnu.st_dev;
      st->st_ino = ms->xnu.st_ino;
      st->st_nlink = ms->xnu.st_nlink;
      st->st_mode = ms->xnu.st_mode;
      st->st_uid = ms->xnu.st_uid;
      st->st_gid = ms->xnu.st_gid;
      st->st_flags = ms->xnu.st_flags;
      st->st_rdev = ms->xnu.st_rdev;
      st->st_size = ms->xnu.st_size;
      st->st_blksize = ms->xnu.st_blksize;
      st->st_blocks = ms->xnu.st_blocks;
      st->st_gen = ms->xnu.st_gen;
      st->st_atim = ms->xnu.st_atim;
      st->st_mtim = ms->xnu.st_mtim;
      st->st_ctim = ms->xnu.st_ctim;
      st->st_birthtim = ms->xnu.st_birthtim;
    } else if (IsFreebsd()) {
      st->st_dev = ms->freebsd.st_dev;
      st->st_ino = ms->freebsd.st_ino;
      st->st_nlink = ms->freebsd.st_nlink;
      st->st_mode = ms->freebsd.st_mode;
      st->st_uid = ms->freebsd.st_uid;
      st->st_gid = ms->freebsd.st_gid;
      st->st_flags = ms->freebsd.st_flags;
      st->st_rdev = ms->freebsd.st_rdev;
      st->st_size = ms->freebsd.st_size;
      st->st_blksize = ms->freebsd.st_blksize;
      st->st_blocks = ms->freebsd.st_blocks;
      st->st_gen = ms->freebsd.st_gen;
      st->st_atim = ms->freebsd.st_atim;
      st->st_mtim = ms->freebsd.st_mtim;
      st->st_ctim = ms->freebsd.st_ctim;
      st->st_birthtim = ms->freebsd.st_birthtim;
    } else if (IsOpenbsd()) {
      st->st_dev = ms->openbsd.st_dev;
      st->st_ino = ms->openbsd.st_ino;
      st->st_nlink = ms->openbsd.st_nlink;
      st->st_mode = ms->openbsd.st_mode;
      st->st_uid = ms->openbsd.st_uid;
      st->st_gid = ms->openbsd.st_gid;
      st->st_flags = ms->openbsd.st_flags;
      st->st_rdev = ms->openbsd.st_rdev;
      st->st_size = ms->openbsd.st_size;
      st->st_blksize = ms->openbsd.st_blksize;
      st->st_blocks = ms->openbsd.st_blocks;
      st->st_gen = ms->openbsd.st_gen;
      st->st_atim = ms->openbsd.st_atim;
      st->st_mtim = ms->openbsd.st_mtim;
      st->st_ctim = ms->openbsd.st_ctim;
      st->st_birthtim = ms->openbsd.st_ctim;
    } else if (IsNetbsd()) {
      st->st_dev = ms->netbsd.st_dev;
      st->st_ino = ms->netbsd.st_ino;
      st->st_nlink = ms->netbsd.st_nlink;
      st->st_mode = ms->netbsd.st_mode;
      st->st_uid = ms->netbsd.st_uid;
      st->st_gid = ms->netbsd.st_gid;
      st->st_flags = ms->netbsd.st_flags;
      st->st_rdev = ms->netbsd.st_rdev;
      st->st_size = ms->netbsd.st_size;
      st->st_blksize = ms->netbsd.st_blksize;
      st->st_blocks = ms->netbsd.st_blocks;
      st->st_gen = ms->netbsd.st_gen;
      st->st_atim = ms->netbsd.st_atim;
      st->st_mtim = ms->netbsd.st_mtim;
      st->st_ctim = ms->netbsd.st_ctim;
      st->st_birthtim = ms->netbsd.st_birthtim;
    }
  }
}
