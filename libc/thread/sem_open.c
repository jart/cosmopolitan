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
#include "libc/assert.h"
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/strace.internal.h"
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/semaphore.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

static struct Semaphores {
  pthread_once_t once;
  pthread_mutex_t lock;
  struct Semaphore {
    struct Semaphore *next;
    sem_t *sem;
    char *path;
    bool dead;
    int refs;
  } * list;
} g_semaphores;

static void sem_open_lock(void) {
  pthread_mutex_lock(&g_semaphores.lock);
}

static void sem_open_unlock(void) {
  pthread_mutex_unlock(&g_semaphores.lock);
}

static void sem_open_wipe(void) {
  pthread_mutex_init(&g_semaphores.lock, 0);
}

static void sem_open_setup(void) {
  sem_open_wipe();
  pthread_atfork(sem_open_lock, sem_open_unlock, sem_open_wipe);
}

static void sem_open_init(void) {
  pthread_once(&g_semaphores.once, sem_open_setup);
}

static sem_t *sem_open_impl(const char *path, int oflag, unsigned mode,
                            unsigned value) {
  int fd;
  sem_t *sem;
  struct stat st;
  oflag |= O_RDWR | O_CLOEXEC;
  if ((fd = openat(AT_FDCWD, path, oflag, mode)) == -1) {
    return SEM_FAILED;
  }
  npassert(!fstat(fd, &st));
  if (st.st_size < 4096 && ftruncate(fd, 4096) == -1) {
    npassert(!close(fd));
    return SEM_FAILED;
  }
  sem = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (sem != MAP_FAILED) {
    atomic_store_explicit(&sem->sem_value, value, memory_order_relaxed);
    sem->sem_magic = SEM_MAGIC_NAMED;
    sem->sem_dev = st.st_dev;
    sem->sem_ino = st.st_ino;
    sem->sem_pshared = true;
  } else {
    sem = SEM_FAILED;
  }
  npassert(!close(fd));
  return sem;
}

static struct Semaphore *sem_open_find(const char *path) {
  struct Semaphore *s;
  for (s = g_semaphores.list; s; s = s->next) {
    if (!strcmp(path, s->path)) {
      return s;
    }
  }
  return 0;
}

static struct Semaphore *sem_open_reopen(const char *path) {
  int e = errno;
  struct stat st;
  struct Semaphore *s;
  for (s = g_semaphores.list; s; s = s->next) {
    if (!s->dead &&  //
        !strcmp(path, s->path)) {
      if (!fstatat(AT_FDCWD, path, &st, 0) &&  //
          st.st_dev == s->sem->sem_dev &&      //
          st.st_ino == s->sem->sem_ino) {
        return s;
      } else {
        errno = e;
        s->dead = true;
      }
    }
  }
  return 0;
}

static struct Semaphore *sem_open_get(const sem_t *sem,
                                      struct Semaphore ***out_prev) {
  struct Semaphore *s, **p;
  for (p = &g_semaphores.list, s = *p; s; p = &s->next, s = s->next) {
    if (s && sem == s->sem) {
      *out_prev = p;
      return s;
    }
  }
  return 0;
}

/**
 * Initializes and opens named semaphore.
 *
 * This function tracks open semaphore objects within a process. When a
 * process calls sem_open() multiple times with the same name, then the
 * same shared memory address will be returned, unless it was unlinked.
 *
 * @param name is arbitrary string that begins with a slash character
 * @param oflag can have any of:
 *     - `O_CREAT` to create the named semaphore if it doesn't exist,
 *       in which case two additional arguments must be supplied
 *     - `O_EXCL` to raise `EEXIST` if semaphore already exists
 * @param mode is octal mode bits, required if `oflag & O_CREAT`
 * @param value is initial of semaphore, required if `oflag & O_CREAT`
 * @return semaphore object which needs sem_close(), or SEM_FAILED w/ errno
 * @raise ENOSPC if file system is full when `name` would be `O_CREAT`ed
 * @raise EINVAL if `oflag` has bits other than `O_CREAT | O_EXCL`
 * @raise EINVAL if `value` is negative or exceeds `SEM_VALUE_MAX`
 * @raise EEXIST if `O_CREAT|O_EXCL` is used and semaphore exists
 * @raise EACCES if we didn't have permission to create semaphore
 * @raise EACCES if recreating open semaphore pending an unlink
 * @raise EMFILE if process `RLIMIT_NOFILE` has been reached
 * @raise ENFILE if system-wide file limit has been reached
 * @raise ENOMEM if we require more vespene gas
 * @raise EINTR if signal handler was called
 */
sem_t *sem_open(const char *name, int oflag, ...) {
  sem_t *sem;
  va_list va;
  char path[78];
  struct Semaphore *s;
  unsigned mode = 0, value = 0;

  va_start(va, oflag);
  mode = va_arg(va, unsigned);
  value = va_arg(va, unsigned);
  va_end(va);

#if 0
  if (IsXnuSilicon()) {
    long kernel;
    if (!(sem = calloc(1, sizeof(sem_t)))) return SEM_FAILED;
    sem->sem_magic = SEM_MAGIC_KERNEL;
    kernel = _sysret(__syslib->__sem_open(name, oflag, mode, value));
    if (kernel == -1) {
      free(sem);
      return SEM_FAILED;
    }
    sem->sem_magic = SEM_MAGIC_KERNEL;
    sem->sem_kernel = (int *)kernel;
  }
#endif

  if (oflag & ~(O_CREAT | O_EXCL)) {
    einval();
    return SEM_FAILED;
  }
  if (oflag & O_CREAT) {
    if (value > SEM_VALUE_MAX) {
      einval();
      return SEM_FAILED;
    }
  }
  shm_path_np(name, path);
  BLOCK_CANCELATION;
  sem_open_init();
  sem_open_lock();
  if ((s = sem_open_reopen(path))) {
    if (s->sem->sem_lazydelete) {
      if (oflag & O_CREAT) {
        eacces();
      } else {
        enoent();
      }
      sem = SEM_FAILED;
    } else if (~oflag & O_EXCL) {
      sem = s->sem;
      atomic_fetch_add_explicit(&sem->sem_prefs, 1, memory_order_acq_rel);
      ++s->refs;
    } else {
      eexist();
      sem = SEM_FAILED;
    }
  } else if ((s = calloc(1, sizeof(struct Semaphore)))) {
    if ((s->path = strdup(path))) {
      if ((sem = sem_open_impl(path, oflag, mode, value)) != SEM_FAILED) {
        atomic_fetch_add_explicit(&sem->sem_prefs, 1, memory_order_acq_rel);
        s->next = g_semaphores.list;
        s->sem = sem;
        s->refs = 1;
        g_semaphores.list = s;
      } else {
        free(s->path);
        free(s);
      }
    } else {
      free(s);
      sem = SEM_FAILED;
    }
  } else {
    sem = SEM_FAILED;
  }
  sem_open_unlock();
  ALLOW_CANCELATION;
  return sem;
}

/**
 * Closes named semaphore.
 *
 * Calling sem_close() on a semaphore not created by sem_open() has
 * undefined behavior. Using `sem` after calling sem_close() from either
 * the current process or forked processes sharing the same address is
 * also undefined behavior. If any threads in this process or forked
 * children are currently blocked on `sem` then calling sem_close() has
 * undefined behavior.
 *
 * @param sem was created with sem_open()
 * @return 0 on success, or -1 w/ errno
 */
int sem_close(sem_t *sem) {
  int prefs;
  bool unmap, delete;
  struct Semaphore *s, **p;

#if 0
  if (IsXnuSilicon()) {
    npassert(sem->sem_magic == SEM_MAGIC_KERNEL);
    return _sysret(__syslib->__sem_close(sem->sem_kernel));
  }
#endif

  npassert(sem->sem_magic == SEM_MAGIC_NAMED);
  sem_open_init();
  sem_open_lock();
  npassert((s = sem_open_get(sem, &p)));
  prefs = atomic_fetch_add_explicit(&sem->sem_prefs, -1, memory_order_acq_rel);
  npassert(s->refs > 0);
  if ((unmap = !--s->refs)) {
    npassert(prefs > 0);
    delete = sem->sem_lazydelete && prefs == 1;
    *p = s->next;
  } else {
    npassert(prefs > 1);
    delete = false;
  }
  sem_open_unlock();
  if (unmap) {
    npassert(!munmap(sem, 4096));
  }
  if (delete) {
    unlink(s->path);
  }
  if (unmap) {
    free(s->path);
    free(s);
  }
  return 0;
}

/**
 * Removes named semaphore.
 *
 * This causes the file resource to be deleted. If processes have this
 * semaphore currently opened, then on platforms like Windows deletion
 * may be postponed until the last process calls sem_close().
 *
 * @param name can be absolute path or should be component w/o slashes
 * @return 0 on success, or -1 w/ errno
 * @raise ACCESS if Windows is being fussy about deleting open files
 * @raise EPERM if pledge() is in play w/o `cpath` promise
 * @raise ENOENT if named semaphore doesn't exist
 * @raise EACCES if permission is denied
 * @raise ENAMETOOLONG if too long
 */
int sem_unlink(const char *name) {
  char path[78];
  int rc, e = errno;
  struct Semaphore *s;

#if 0
  if (IsXnuSilicon()) {
    return _sysret(__syslib->__sem_unlink(name));
  }
#endif

  shm_path_np(name, path);
  if ((rc = unlink(path)) == -1 && IsWindows() && errno == EACCES) {
    sem_open_init();
    sem_open_lock();
    if ((s = sem_open_find(path))) {
      s->sem->sem_lazydelete = true;
      errno = e;
      rc = 0;
    }
    sem_open_unlock();
  }
  return rc;
}
