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
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/runtime/pathconf.h"
#include "libc/sysv/consts/_posix.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/errfuns.h"

long fpathconf(int fd, int name) {
  if (name == _PC_LINK_MAX) return _POSIX_LINK_MAX;
  if (name == _PC_MAX_CANON) return _POSIX_MAX_CANON;
  if (name == _PC_MAX_INPUT) return _POSIX_MAX_INPUT;
  if (name == _PC_NAME_MAX) return NAME_MAX;
  if (name == _PC_PATH_MAX) return PATH_MAX;
  if (name == _PC_PIPE_BUF) return PIPE_BUF;
  if (name == _PC_CHOWN_RESTRICTED) return 1;
  if (name == _PC_NO_TRUNC) return 1;
  if (name == _PC_VDISABLE) return 0;
  if (name == _PC_SYNC_IO) return 1;
  if (name == _PC_ASYNC_IO) return -1;
  if (name == _PC_PRIO_IO) return -1;
  if (name == _PC_SOCK_MAXBUF) return -1;
  if (name == _PC_FILESIZEBITS) return FILESIZEBITS;
  if (name == _PC_REC_INCR_XFER_SIZE) return 4096;
  if (name == _PC_REC_MAX_XFER_SIZE) return 4096;
  if (name == _PC_REC_MIN_XFER_SIZE) return 4096;
  if (name == _PC_REC_XFER_ALIGN) return 4096;
  if (name == _PC_ALLOC_SIZE_MIN) return 4096;
  if (name == _PC_SYMLINK_MAX) return -1;
  if (name == _PC_2_SYMLINKS) return 1;
  return einval();
}

long pathconf(const char *path, int name) {
  return fpathconf(-1, name);
}
