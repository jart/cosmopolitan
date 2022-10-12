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
#include "libc/calls/struct/sigset.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/stdio/posix_spawn.h"
#include "libc/stdio/posix_spawn.internal.h"
#include "libc/sysv/consts/sig.h"

/**
 * Initialize posix_spawn() attributes object with default values.
 *
 * @param attr needs to be passed to posix_spawnattr_destroy() later
 * @return 0 on success, or errno on error
 * @raise ENOMEM if we require more vespene gas
 */
int posix_spawnattr_init(posix_spawnattr_t *attr) {
  int e, rc;
  struct _posix_spawna *a;
  e = errno;
  errno = 0;
  if ((a = calloc(1, sizeof(*a)))) {
    a->flags = 0;
    a->pgroup = 0;
    sigemptyset(&a->sigdefault);
    a->schedpolicy = sched_getscheduler(0);
    sched_getparam(0, &a->schedparam);
  }
  rc = errno;
  errno = e;
  return rc;
}

/**
 * Destroys posix_spawn() attributes object.
 *
 * This function is safe to call multiple times.
 *
 * @param attr was initialized by posix_spawnattr_init()
 * @return 0 on success, or errno on error
 */
int posix_spawnattr_destroy(posix_spawnattr_t *attr) {
  if (*attr) {
    free(*attr);
    *attr = 0;
  }
  return 0;
}

/**
 * Gets posix_spawn() flags.
 *
 * @param attr was initialized by posix_spawnattr_init()
 * @return 0 on success, or errno on error
 */
int posix_spawnattr_getflags(const posix_spawnattr_t *attr, short *flags) {
  *flags = (*attr)->flags;
  return 0;
}

/**
 * Sets posix_spawn() flags.
 *
 * Setting these flags is needed in order for the other setters in this
 * function to take effect.
 *
 * @param attr was initialized by posix_spawnattr_init()
 * @param flags may have any of the following
 *     - `POSIX_SPAWN_RESETIDS`
 *     - `POSIX_SPAWN_SETPGROUP`
 *     - `POSIX_SPAWN_SETSIGDEF`
 *     - `POSIX_SPAWN_SETSIGMASK`
 *     - `POSIX_SPAWN_SETSCHEDPARAM`
 *     - `POSIX_SPAWN_SETSCHEDULER`
 * @return 0 on success, or errno on error
 * @raise EINVAL if `flags` has invalid bits
 */
int posix_spawnattr_setflags(posix_spawnattr_t *attr, short flags) {
  if (flags & ~(POSIX_SPAWN_RESETIDS | POSIX_SPAWN_SETPGROUP |
                POSIX_SPAWN_SETSIGDEF | POSIX_SPAWN_SETSIGMASK |
                POSIX_SPAWN_SETSCHEDPARAM | POSIX_SPAWN_SETSCHEDULER)) {
    return EINVAL;
  }
  (*attr)->flags = flags;
  return 0;
}

int posix_spawnattr_getpgroup(const posix_spawnattr_t *attr, int *pgroup) {
  *pgroup = (*attr)->pgroup;
  return 0;
}

int posix_spawnattr_setpgroup(posix_spawnattr_t *attr, int pgroup) {
  (*attr)->pgroup = pgroup;
  return 0;
}

/**
 * Gets scheduler policy that'll be used for spawned process.
 *
 * If the setter wasn't called then this function will return the
 * scheduling policy of the current process.
 *
 * @param attr was initialized by posix_spawnattr_init()
 * @param schedpolicy receives the result
 * @return 0 on success, or errno on error
 */
int posix_spawnattr_getschedpolicy(const posix_spawnattr_t *attr,
                                   int *schedpolicy) {
  if (!(*attr)->schedpolicy_isset) {
    (*attr)->schedpolicy = sched_getscheduler(0);
    (*attr)->schedpolicy_isset = true;
  }
  *schedpolicy = (*attr)->schedpolicy;
  return 0;
}

/**
 * Specifies scheduler policy override for spawned process.
 *
 * Scheduling policies are inherited by default. Use this to change it.
 *
 * @param attr was initialized by posix_spawnattr_init()
 * @param schedpolicy receives the result
 * @return 0 on success, or errno on error
 */
int posix_spawnattr_setschedpolicy(posix_spawnattr_t *attr, int schedpolicy) {
  (*attr)->schedpolicy = schedpolicy;
  (*attr)->schedpolicy_isset = true;
  return 0;
}

/**
 * Gets scheduler parameter that'll be used for spawned process.
 *
 * If the setter wasn't called then this function will return the
 * scheduling parameter of the current process.
 *
 * @param attr was initialized by posix_spawnattr_init()
 * @param schedparam receives the result
 * @return 0 on success, or errno on error
 */
int posix_spawnattr_getschedparam(const posix_spawnattr_t *attr,
                                  struct sched_param *schedparam) {
  if (!(*attr)->schedparam_isset) {
    sched_getparam(0, &(*attr)->schedparam);
    (*attr)->schedparam_isset = true;
  }
  *schedparam = (*attr)->schedparam;
  return 0;
}

/**
 * Specifies scheduler parameter override for spawned process.
 *
 * Scheduling parameters are inherited by default. Use this to change it.
 *
 * @param attr was initialized by posix_spawnattr_init()
 * @param schedparam receives the result
 * @return 0 on success, or errno on error
 */
int posix_spawnattr_setschedparam(posix_spawnattr_t *attr,
                                  const struct sched_param *schedparam) {
  (*attr)->schedparam = *schedparam;
  (*attr)->schedparam_isset = true;
  return 0;
}

int posix_spawnattr_getsigmask(const posix_spawnattr_t *attr,
                               sigset_t *sigmask) {
  if (!(*attr)->sigmask_isset) {
    sigprocmask(SIG_SETMASK, 0, &(*attr)->sigmask);
    (*attr)->sigmask_isset = true;
  }
  *sigmask = (*attr)->sigmask;
  return 0;
}

int posix_spawnattr_setsigmask(posix_spawnattr_t *attr,
                               const sigset_t *sigmask) {
  (*attr)->sigmask = *sigmask;
  (*attr)->sigmask_isset = true;
  return 0;
}

int posix_spawnattr_getsigdefault(const posix_spawnattr_t *attr,
                                  sigset_t *sigdefault) {
  *sigdefault = (*attr)->sigdefault;
  return 0;
}

int posix_spawnattr_setsigdefault(posix_spawnattr_t *attr,
                                  const sigset_t *sigdefault) {
  (*attr)->sigdefault = *sigdefault;
  return 0;
}
