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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/macros.internal.h"
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
  int rc, e = errno;
  struct _posix_spawna *a;
  if ((a = calloc(1, sizeof(struct _posix_spawna)))) {
    *attr = a;
    rc = 0;
  } else {
    rc = errno;
    errno = e;
  }
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
 * @param attr was initialized by posix_spawnattr_init()
 * @param flags may have any of the following
 *     - `POSIX_SPAWN_RESETIDS`
 *     - `POSIX_SPAWN_SETPGROUP`
 *     - `POSIX_SPAWN_SETSIGDEF`
 *     - `POSIX_SPAWN_SETSIGMASK`
 *     - `POSIX_SPAWN_SETSCHEDPARAM`
 *     - `POSIX_SPAWN_SETSCHEDULER`
 *     - `POSIX_SPAWN_SETSID`
 * @return 0 on success, or errno on error
 * @raise EINVAL if `flags` has invalid bits
 */
int posix_spawnattr_setflags(posix_spawnattr_t *attr, short flags) {
  if (flags &
      ~(POSIX_SPAWN_RESETIDS | POSIX_SPAWN_SETPGROUP | POSIX_SPAWN_SETSIGDEF |
        POSIX_SPAWN_SETSIGMASK | POSIX_SPAWN_SETSCHEDPARAM |
        POSIX_SPAWN_SETSCHEDULER | POSIX_SPAWN_SETSID)) {
    return EINVAL;
  }
  (*attr)->flags = flags;
  return 0;
}

/**
 * Gets process group id associated with attributes.
 *
 * @param attr was initialized by posix_spawnattr_init()
 * @param pgroup receives the result on success
 * @return 0 on success, or errno on error
 */
int posix_spawnattr_getpgroup(const posix_spawnattr_t *attr, int *pgroup) {
  *pgroup = (*attr)->pgroup;
  return 0;
}

/**
 * Specifies process group into which child process is placed.
 *
 * Setting `pgroup` to zero will ensure newly created processes are
 * placed within their own brand new process group.
 *
 * This setter also sets the `POSIX_SPAWN_SETPGROUP` flag.
 *
 * @param attr was initialized by posix_spawnattr_init()
 * @param pgroup is the process group id, or 0 for self
 * @return 0 on success, or errno on error
 */
int posix_spawnattr_setpgroup(posix_spawnattr_t *attr, int pgroup) {
  (*attr)->flags |= POSIX_SPAWN_SETPGROUP;
  (*attr)->pgroup = pgroup;
  return 0;
}

/**
 * Gets signal mask for sigprocmask() in child process.
 *
 * The signal mask is applied to the child process in such a way that
 * signal handlers from the parent process can't get triggered in the
 * child process.
 *
 * @return 0 on success, or errno on error
 */
int posix_spawnattr_getsigmask(const posix_spawnattr_t *attr,
                               sigset_t *sigmask) {
  *sigmask = (*attr)->sigmask;
  return 0;
}

/**
 * Specifies signal mask for sigprocmask() in child process.
 *
 * This setter also sets the `POSIX_SPAWN_SETSIGMASK` flag.
 *
 * @return 0 on success, or errno on error
 */
int posix_spawnattr_setsigmask(posix_spawnattr_t *attr,
                               const sigset_t *sigmask) {
  (*attr)->flags |= POSIX_SPAWN_SETSIGMASK;
  (*attr)->sigmask = *sigmask;
  return 0;
}

/**
 * Retrieves which signals will be restored to `SIG_DFL`.
 *
 * @return 0 on success, or errno on error
 */
int posix_spawnattr_getsigdefault(const posix_spawnattr_t *attr,
                                  sigset_t *sigdefault) {
  *sigdefault = (*attr)->sigdefault;
  return 0;
}

/**
 * Specifies which signals should be restored to `SIG_DFL`.
 *
 * This routine isn't necessary in most cases, since posix_spawn() by
 * default will try to avoid vfork() race conditions by tracking what
 * signals have a handler function and then resets them automatically
 * within the child process, before applying the child's signal mask.
 * This function may be used to ensure the `SIG_IGN` disposition will
 * not propagate across execve in cases where this process explicitly
 * set the signals to `SIG_IGN` earlier (since posix_spawn() will not
 * issue O(128) system calls just to be totally pedantic about that).
 *
 * Using this setter automatically sets `POSIX_SPAWN_SETSIGDEF`.
 *
 * @return 0 on success, or errno on error
 */
int posix_spawnattr_setsigdefault(posix_spawnattr_t *attr,
                                  const sigset_t *sigdefault) {
  (*attr)->flags |= POSIX_SPAWN_SETSIGDEF;
  (*attr)->sigdefault = *sigdefault;
  return 0;
}

/**
 * Gets resource limit for spawned process.
 *
 * @return 0 on success, or errno on error
 * @raise EINVAL if `resource` is invalid
 * @raise ENOENT if `resource` is absent
 */
int posix_spawnattr_getrlimit(const posix_spawnattr_t *attr, int resource,
                              struct rlimit *rlim) {
  if ((0 <= resource && resource < ARRAYLEN((*attr)->rlim))) {
    if (((*attr)->rlimset & (1u << resource))) {
      *rlim = (*attr)->rlim[resource];
      return 0;
    } else {
      return ENOENT;
    }
  } else {
    return EINVAL;
  }
}

/**
 * Sets resource limit on spawned process.
 *
 * Using this setter automatically sets `POSIX_SPAWN_SETRLIMIT`.
 *
 * @return 0 on success, or errno on error
 * @raise EINVAL if resource is invalid
 */
int posix_spawnattr_setrlimit(posix_spawnattr_t *attr, int resource,
                              const struct rlimit *rlim) {
  if (0 <= resource && resource < ARRAYLEN((*attr)->rlim)) {
    (*attr)->flags |= POSIX_SPAWN_SETRLIMIT;
    (*attr)->rlimset |= 1u << resource;
    (*attr)->rlim[resource] = *rlim;
    return 0;
  } else {
    return EINVAL;
  }
}

/**
 * Gets scheduler policy that'll be used for spawned process.
 *
 * @param attr was initialized by posix_spawnattr_init()
 * @param schedpolicy receives the result
 * @return 0 on success, or errno on error
 */
int posix_spawnattr_getschedpolicy(const posix_spawnattr_t *attr,
                                   int *schedpolicy) {
  *schedpolicy = (*attr)->schedpolicy;
  return 0;
}

/**
 * Specifies scheduler policy override for spawned process.
 *
 * Using this setter automatically sets `POSIX_SPAWN_SETSCHEDULER`.
 *
 * @param attr was initialized by posix_spawnattr_init()
 * @return 0 on success, or errno on error
 */
int posix_spawnattr_setschedpolicy(posix_spawnattr_t *attr, int schedpolicy) {
  (*attr)->flags |= POSIX_SPAWN_SETSCHEDULER;
  (*attr)->schedpolicy = schedpolicy;
  return 0;
}

/**
 * Gets scheduler parameter.
 *
 * @param attr was initialized by posix_spawnattr_init()
 * @param schedparam receives the result
 * @return 0 on success, or errno on error
 */
int posix_spawnattr_getschedparam(const posix_spawnattr_t *attr,
                                  struct sched_param *schedparam) {
  *schedparam = (*attr)->schedparam;
  return 0;
}

/**
 * Specifies scheduler parameter override for spawned process.
 *
 * Using this setter automatically sets `POSIX_SPAWN_SETSCHEDPARAM`.
 *
 * @param attr was initialized by posix_spawnattr_init()
 * @param schedparam receives the result
 * @return 0 on success, or errno on error
 */
int posix_spawnattr_setschedparam(posix_spawnattr_t *attr,
                                  const struct sched_param *schedparam) {
  (*attr)->flags |= POSIX_SPAWN_SETSCHEDPARAM;
  (*attr)->schedparam = *schedparam;
  return 0;
}
