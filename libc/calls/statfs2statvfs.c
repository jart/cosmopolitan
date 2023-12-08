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
#include "libc/calls/struct/statfs.h"
#include "libc/calls/struct/statvfs.h"
#include "libc/str/str.h"

void statfs2statvfs(struct statvfs *sv, const struct statfs *sf) {
  bzero(sv, sizeof(*sv));
  sv->f_bsize = sf->f_bsize;
  sv->f_frsize = sf->f_frsize ? sf->f_frsize : sf->f_bsize;
  sv->f_blocks = sf->f_blocks;
  sv->f_bfree = sf->f_bfree;
  sv->f_bavail = sf->f_bavail;
  sv->f_files = sf->f_files;
  sv->f_ffree = sf->f_ffree;
  sv->f_favail = sf->f_ffree;
  sv->f_fsid = sf->f_fsid.__val[0];
  sv->f_flag = sf->f_flags;
  sv->f_namemax = sf->f_namelen;
}
