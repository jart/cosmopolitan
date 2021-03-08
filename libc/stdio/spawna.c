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
#include "libc/calls/calls.h"
#include "libc/calls/scheduler.h"
#include "libc/calls/sigbits.h"
#include "libc/stdio/spawn.h"
#include "libc/stdio/spawna.internal.h"

/**
 * Initialize object with default values.
 */
int posix_spawnattr_init(posix_spawnattr_t *attr) {
  attr->posix_attr_flags = 0;
  attr->posix_attr_pgroup = 0;
  sigprocmask(0, NULL, &attr->posix_attr_sigmask);
  sigemptyset(&attr->posix_attr_sigdefault);
  attr->posix_attr_schedpolicy = sched_getscheduler(0);
  sched_getparam(0, &attr->posix_attr_schedparam);
  return 0;
}

int posix_spawnattr_destroy(posix_spawnattr_t *attr) {
  return 0;
}

int posix_spawnattr_getflags(const posix_spawnattr_t *attr, short *flags) {
  *flags = attr->posix_attr_flags;
  return 0;
}

int posix_spawnattr_setflags(posix_spawnattr_t *attr, short flags) {
  attr->posix_attr_flags = flags;
  return 0;
}

int posix_spawnattr_getpgroup(const posix_spawnattr_t *attr, int *pgroup) {
  *pgroup = attr->posix_attr_pgroup;
  return 0;
}

int posix_spawnattr_setpgroup(posix_spawnattr_t *attr, int pgroup) {
  attr->posix_attr_pgroup = pgroup;
  return 0;
}

int posix_spawnattr_getschedpolicy(const posix_spawnattr_t *attr,
                                   int *schedpolicy) {
  *schedpolicy = attr->posix_attr_schedpolicy;
  return 0;
}

int posix_spawnattr_setschedpolicy(posix_spawnattr_t *attr, int schedpolicy) {
  attr->posix_attr_schedpolicy = schedpolicy;
  return 0;
}

int posix_spawnattr_getschedparam(const posix_spawnattr_t *attr,
                                  struct sched_param *schedparam) {
  *schedparam = attr->posix_attr_schedparam;
  return 0;
}

int posix_spawnattr_setschedparam(posix_spawnattr_t *attr,
                                  const struct sched_param *schedparam) {
  attr->posix_attr_schedparam = *schedparam;
  return 0;
}

int posix_spawnattr_getsigmask(const posix_spawnattr_t *attr,
                               sigset_t *sigmask) {
  *sigmask = attr->posix_attr_sigmask;
  return 0;
}

int posix_spawnattr_setsigmask(posix_spawnattr_t *attr,
                               const sigset_t *sigmask) {
  attr->posix_attr_sigmask = *sigmask;
  return 0;
}

int posix_spawnattr_getsigdefault(const posix_spawnattr_t *attr,
                                  sigset_t *sigdefault) {
  *sigdefault = attr->posix_attr_sigdefault;
  return 0;
}

int posix_spawnattr_setsigdefault(posix_spawnattr_t *attr,
                                  const sigset_t *sigdefault) {
  attr->posix_attr_sigdefault = *sigdefault;
  return 0;
}
