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
#include "libc/dce.h"

union metaflock {
  struct flock cosmo;
  struct flock_linux {
    int16_t l_type;
    int16_t l_whence;
    int64_t l_start;
    int64_t l_len;
    int32_t l_pid;
  } linux;
  struct flock_xnu {
    int64_t l_start;
    int64_t l_len;
    int32_t l_pid;
    int16_t l_type;
    int16_t l_whence;
  } xnu;
  struct flock_freebsd {
    int64_t l_start;
    int64_t l_len;
    int32_t l_pid;
    int16_t l_type;
    int16_t l_whence;
    int32_t l_sysid;
  } freebsd;
  struct flock_openbsd {
    int64_t l_start;
    int64_t l_len;
    int32_t l_pid;
    int16_t l_type;
    int16_t l_whence;
  } openbsd;
  struct flock_netbsd {
    int64_t l_start;
    int64_t l_len;
    int32_t l_pid;
    int16_t l_type;
    int16_t l_whence;
  } netbsd;
};

void flock2cosmo(uintptr_t memory) {
  int64_t l_start;
  int64_t l_len;
  int32_t l_pid;
  int16_t l_type;
  int16_t l_whence;
  int32_t l_sysid;
  union metaflock *u;
  u = (union metaflock *)memory;
  if (IsLinux()) {
    l_start = u->linux.l_start;
    l_len = u->linux.l_len;
    l_pid = u->linux.l_pid;
    l_type = u->linux.l_type;
    l_whence = u->linux.l_whence;
  } else if (IsXnu()) {
    l_start = u->xnu.l_start;
    l_len = u->xnu.l_len;
    l_pid = u->xnu.l_pid;
    l_type = u->xnu.l_type;
    l_whence = u->xnu.l_whence;
  } else if (IsFreebsd()) {
    l_start = u->freebsd.l_start;
    l_len = u->freebsd.l_len;
    l_pid = u->freebsd.l_pid;
    l_type = u->freebsd.l_type;
    l_whence = u->freebsd.l_whence;
    l_sysid = u->freebsd.l_sysid;
    u->cosmo.l_sysid = l_sysid;
  } else if (IsOpenbsd()) {
    l_start = u->openbsd.l_start;
    l_len = u->openbsd.l_len;
    l_pid = u->openbsd.l_pid;
    l_type = u->openbsd.l_type;
    l_whence = u->openbsd.l_whence;
  } else if (IsNetbsd()) {
    l_start = u->netbsd.l_start;
    l_len = u->netbsd.l_len;
    l_pid = u->netbsd.l_pid;
    l_type = u->netbsd.l_type;
    l_whence = u->netbsd.l_whence;
  } else {
    return;
  }
  u->cosmo.l_start = l_start;
  u->cosmo.l_len = l_len;
  u->cosmo.l_pid = l_pid;
  u->cosmo.l_type = l_type;
  u->cosmo.l_whence = l_whence;
}

void cosmo2flock(uintptr_t memory) {
  int64_t l_start;
  int64_t l_len;
  int32_t l_pid;
  int16_t l_type;
  int16_t l_whence;
  int32_t l_sysid;
  union metaflock *u;
  u = (union metaflock *)memory;
  l_start = u->cosmo.l_start;
  l_len = u->cosmo.l_len;
  l_pid = u->cosmo.l_pid;
  l_type = u->cosmo.l_type;
  l_whence = u->cosmo.l_whence;
  l_sysid = u->cosmo.l_sysid;
  if (IsLinux()) {
    u->linux.l_start = l_start;
    u->linux.l_len = l_len;
    u->linux.l_pid = l_pid;
    u->linux.l_type = l_type;
    u->linux.l_whence = l_whence;
  } else if (IsXnu()) {
    u->xnu.l_start = l_start;
    u->xnu.l_len = l_len;
    u->xnu.l_pid = l_pid;
    u->xnu.l_type = l_type;
    u->xnu.l_whence = l_whence;
  } else if (IsFreebsd()) {
    u->freebsd.l_start = l_start;
    u->freebsd.l_len = l_len;
    u->freebsd.l_pid = l_pid;
    u->freebsd.l_type = l_type;
    u->freebsd.l_whence = l_whence;
    u->freebsd.l_sysid = l_sysid;
  } else if (IsOpenbsd()) {
    u->openbsd.l_start = l_start;
    u->openbsd.l_len = l_len;
    u->openbsd.l_pid = l_pid;
    u->openbsd.l_type = l_type;
    u->openbsd.l_whence = l_whence;
  } else if (IsNetbsd()) {
    u->netbsd.l_start = l_start;
    u->netbsd.l_len = l_len;
    u->netbsd.l_pid = l_pid;
    u->netbsd.l_type = l_type;
    u->netbsd.l_whence = l_whence;
  }
}
