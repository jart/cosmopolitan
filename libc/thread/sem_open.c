/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/calls.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/thread/semaphore.h"
#include "libc/thread/semaphore.internal.h"

/**
 * Initializes and opens named semaphore.
 *
 * @param name can be absolute path or should be component w/o slashes
 * @param oflga can have `O_CREAT` and/or `O_EXCL`
 * @return semaphore object which needs sem_close(), or SEM_FAILED w/ errno
 * @raise ENOTDIR if a directory component in `name` exists as non-directory
 * @raise ENAMETOOLONG if symlink-resolved `name` length exceeds `PATH_MAX`
 * @raise ENAMETOOLONG if component in `name` exists longer than `NAME_MAX`
 * @raise ELOOP if `flags` had `O_NOFOLLOW` and `name` is a symbolic link
 * @raise ENOSPC if file system is full when `name` would be `O_CREAT`ed
 * @raise ELOOP if a loop was detected resolving components of `name`
 * @raise EEXIST if `O_CREAT|O_EXCL` is used and semaphore exists
 * @raise EACCES if we didn't have permission to create semaphore
 * @raise EMFILE if process `RLIMIT_NOFILE` has been reached
 * @raise ENFILE if system-wide file limit has been reached
 * @raise EINTR if signal was delivered instead
 */
sem_t *sem_open(const char *name, int oflag, ...) {
  int fd;
  sem_t *sem;
  va_list va;
  unsigned mode;
  char path[PATH_MAX];

  va_start(va, oflag);
  mode = va_arg(va, unsigned);
  va_end(va);

  oflag |= O_RDWR | O_CLOEXEC;
  if ((fd = openat(AT_FDCWD, __sem_name(name, path), oflag, mode)) == -1) {
    return SEM_FAILED;
  }

  if (ftruncate(fd, sizeof(sem_t)) == -1) {
    _npassert(!close(fd));
    return SEM_FAILED;
  }

  sem = mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (sem != MAP_FAILED) sem->sem_pshared = true;
  _npassert(!close(fd));
  return sem;
}
