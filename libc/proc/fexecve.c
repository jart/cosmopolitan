/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2026 Gavin Arthur Hayes                                            │
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
#include "libc/calls/cp.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/stat.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/safemacros.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/paths.h"
#include "libc/proc/execve.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/mfd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/consts/shm.h"
#include "libc/sysv/errfuns.h"
#include "libc/zip.h"

__notice(fexecve_notice, "\
Cosmopolitan fexecve (ISC)\n\
Copyright (c) 2026 Gavin Arthur Hayes <gavin@computoid.com>");

static int fexecve_impl(const int fd, char *const argv[], char *const envp[]) {
  int rc;
  if (IsLinux()) {
    char path[14 + 12];
    FormatInt32(stpcpy(path, "/proc/self/fd/"), fd);
    rc = __sys_execve(path, argv, envp);
  } else if (IsFreebsd()) {
    rc = sys_fexecve(fd, argv, envp);
  } else {
    rc = enosys();
  }
  return rc;
}

static int isZipFile(const void *data, size_t data_size) {
  if (!_weaken(GetZipEocd)) {
    return enosys();
  }
  int ziperror;
  return _weaken(GetZipEocd)(data, data_size, &ziperror) != NULL;
}

typedef enum { FEXEF_ZIP = 1 << 0, FEXEF_APE = 1 << 1 } FEXEF;

static int getFexeFlags(const void *data, size_t data_size) {
  if (!_weaken(GetZipEocd)) {
    return enosys();
  }
  int rc = isZipFile(data, data_size);
  if (rc == -1) {
    return -1;
  }
  int flags = rc << 0;
  if (data_size >= 8) {
    flags |= (int)IsApeMagic(data) << 1;
  }
  return flags;
}

//  __sys_mmap used instead of mmap to be vfork safer
static int __getFdFexeFlags(const int fd) {
  if (!_weaken(GetZipEocd)) {
    return enosys();
  }
  struct stat st;
  if (fstat(fd, &st) == -1) {
    return -1;
  }
  void *space = __sys_mmap(0, st.st_size, PROT_READ, MAP_SHARED, fd, 0, 0);
  if (space == MAP_FAILED) {
    return -1;
  }
  int flags = getFexeFlags(space, st.st_size);
  if (__sys_munmap(space, st.st_size) == -1) {
    return -1;
  }
  return flags;
}

/**
 * Creates a memfd and copies fd to it.
 *
 * If file is a zip file or ape file, FD_CLOEXEC is NOT set, however the file
 * descriptor number will be over 9000 to keep it out of the way of the new
 * process. If the file is not a zip file and not a ape file, FD_CLOEXEC will be
 * set unless executing under aarch64 QEMU user. These transformations are
 * applied to make executing from zipos work as expected, avoid leaking file
 * descriptors when possible, but when not possible, avoid conflicts with
 * programs that assume file descriptor numbers are available.
 *
 * FD_CLOEXEC is always set on zipos file descriptors, however, FD_CLOEXEC
 * makes the program inaccessible to the APE loader as when the APE loader
 * starts, the fd descriptor is closed. Additionally, closing the file
 * descriptor prevents it from being usable in COSMOPOLITAN_INIT_ZIPOS=,
 * preventing working zipos in the newly executed. Therefore, FD_CLOEXEC cannot
 * be set on APEs or zip files. Likewise with the APE loader, FD_CLOEXEC
 * prevents the qemu aarch64 user interpreter from accessing the ELF, so we
 * don't set it then either.
 *
 * @param outfd should have a pthread_cleanup handler registered such as
 * close_memfd so the created memfd isn't leaked in the event of pthread
 * cancellation.
 */
static bool fd_to_mem_fd(const int infd, FEXEF *flags, int *outfd) {
  struct stat st;
  void *space;
  int fexe_flags, fd;
  int savedErrno = 0;
  bool success = false;

  if (!IsLinux() && !IsFreebsd()) {
    enosys();
    return false;
  } else if (__vforked) {
    enotsup();
    return false;
  }

  BLOCK_SIGNALS;
  BLOCK_CANCELATION;
  strace_enabled(-1);
  if (fstat(infd, &st) == -1) {
    goto fd_to_mem_fd_ALLOW;
  }
  if (IsLinux()) {
    fd = sys_memfd_create(__func__, 0);
  } else if (IsFreebsd()) {
    fd = sys_shm_open(SHM_ANON, O_CREAT | O_RDWR, 0);
  } else {
    fd = enosys();
  }
  if (fd == -1) {
    goto fd_to_mem_fd_ALLOW;
  }
  if ((sys_ftruncate(fd, st.st_size, st.st_size) == -1) ||
      ((space = mmap(0, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                     0)) == MAP_FAILED)) {
    goto fd_to_mem_fd_CLOSE;
  }
  success = (pread(infd, space, st.st_size, 0) == st.st_size) &&
            ((fexe_flags = getFexeFlags(space, st.st_size)) != -1);
  if (!success) {
    savedErrno = errno;
  }
  if ((success = ((munmap(space, st.st_size) == 0) && success))) {
    if (((fexe_flags & (FEXEF_ZIP | FEXEF_APE)) == 0) &&
        (!IsAarch64() || !IsQemuUser())) {
      // setting cloexec isn't strictly required, don't fail if it does
      int flags = fcntl(fd, F_GETFD);
      if (flags != -1) {
        fcntl(fd, F_SETFD, flags | FD_CLOEXEC);
      }
    } else {
      // The dup isn't strictly required, don't fail if it does
      const int highfd = fcntl(fd, F_DUPFD, 9001);
      if (highfd != -1) {
        close(fd);
        fd = highfd;
      }
    }
    *flags = fexe_flags;
    *outfd = fd;
  } else {
    if (savedErrno != 0) {
      errno = savedErrno;
    }
  fd_to_mem_fd_CLOSE:
    savedErrno = errno;
    close(fd);
    errno = savedErrno;
  }
fd_to_mem_fd_ALLOW:
  strace_enabled(+1);
  ALLOW_CANCELATION;
  ALLOW_SIGNALS;
  return success;
}

/**
 * Determines if a file is a zip and/or APE file.
 *
 * On Linux if O_PATH is set, no determination is made as the file is not
 * readable.
 */
static int getFdFexeFlags(const int fd) {
  char buf[8];
  ssize_t rcRead;
  int fl_flags;
  int fd_flags = 0;
  FEXEF fflags = -1;
  BLOCK_SIGNALS;
  BLOCK_CANCELATION;
  strace_enabled(-1);
  if ((fl_flags = fcntl(fd, F_GETFL)) == -1) {
    fflags = -1;
  } else if (IsLinux() && fl_flags & _O_PATH) {
    fflags = 0;
  } else if ((fd_flags = fcntl(fd, F_GETFD)) == -1) {
    fflags = -1;
  } else if ((fd_flags & FD_CLOEXEC) == 0) {
    fflags = __getFdFexeFlags(fd);
  } else if ((rcRead = sys_pread(fd, buf, 8, 0, 0)) == -1) {
    fflags = -1;
  } else {
    int isAPE = (rcRead == 8) && IsApeMagic(buf);
    fflags = isAPE << 1;
  }
  strace_enabled(+1);
  ALLOW_CANCELATION;
  ALLOW_SIGNALS;
  if ((fflags != -1) && (fd_flags & FD_CLOEXEC)) {
    if (fflags & FEXEF_APE) {
      STRACE("warning: APE fd (%d) has FD_CLOEXEC set, APE loading likely not "
             "possible",
             fd);
    } else if (IsAarch64() && IsQemuUser()) {
      STRACE("warning: fd (%d) has FD_CLOEXEC set, qemu user loading likely "
             "not possible",
             fd);
    }
  }
  return fflags;
}

void close_memfd(void *pFd) {
  int fd = *(int *)pFd;
  if (fd == -1) {
    return;
  }
  int keepErrno = errno;
  BLOCK_SIGNALS;
  BLOCK_CANCELATION;
  strace_enabled(-1);
  close(fd);
  strace_enabled(+1);
  ALLOW_CANCELATION;
  ALLOW_SIGNALS;
  errno = keepErrno;
}

static void fexecve_with_zipos(int fd, char *const argv[], char *const envp[],
                               FEXEF fflags) {
  if (fflags & FEXEF_ZIP) {
    char *path = alloca(PATH_MAX);
    FormatInt32(stpcpy(path, "COSMOPOLITAN_INIT_ZIPOS="), fd);
    size_t numenvs;
    for (numenvs = 0; envp[numenvs];)
      ++numenvs;
    static _Thread_local char *envs[500];
    memcpy(envs, envp, numenvs * sizeof(char *));
    envs[numenvs] = path;
    envs[numenvs + 1] = NULL;
    envp = envs;
  }
  fexecve_impl(fd, argv, envp);
  char path[14 + 12];
  FormatInt32(stpcpy(path, "/dev/fd/"), fd);
  STRACE("execve(%#s, %s) due to %s", path, DescribeStringList(argv),
         _strerrno(errno));
  sys_execve(path, argv, envp);
}

/**
 * Executes binary executable at file descriptor.
 *
 * Linux is fully supported on x86_64 and aarch64. FreeBSD is supported, but no
 * testing has been done to confirm, in particular, running APE binaries,
 * running from a zipos fd and passing zipos fd via COSMOPOLITAN_INIT_ZIPOS= may
 * not work on FreeBSD. No support is provided for other systems.
 *
 * Interpreted binaries / scripts such as APEs or executables loaded with
 * qemu user aarch64 will not load if FD_CLOEXEC is set. Zipos may fail to
 * initialize if FD_CLOEXEC is set.
 *
 * When a zipos fd is passed, its FD_CLOEXEC setting is ignored and it is copied
 * to a memfd to make it "real". If it's not an APE file or a zip file and we're
 * not running on qemu user aarch64, FD_CLOEXEC is set on the memfd. If we do
 * not set FD_CLOEXEC, the fd is dup'd to over 9000 to prevent interference with
 * programs that assume fd numbering.
 *
 * @param fd is opened executable and current file position is ignored
 * @return doesn't return on success, otherwise -1 w/ errno
 * @raise ENOEXEC if file at `fd` fails to execute
 * @raise ENOSYS on Windows, XNU, OpenBSD, NetBSD, and Metal
 * @raise ENOTSUP if a zipos file is passed when vforked
 * @asyncsignalsafe
 * @vforksafe
 */
int fexecve(int fd, char *const argv[], char *const envp[]) {
  int rc = 0;
  STRACE("fexecve(%d, %s, %s) → ...", fd, DescribeStringList(argv),
         DescribeStringList(envp));
  do {
    if (!argv || !envp) {
      rc = efault();
      break;
    }
    if (!IsLinux() && !IsFreebsd()) {
      rc = enosys();
      break;
    }
    FEXEF fflags = 0;
    if (__isfdkind(fd, kFdZip)) {
      if (__vforked) {
        rc = enotsup();
        break;
      }
      int memfd = -1;
      pthread_cleanup_push(&close_memfd, &memfd);
      if (fd_to_mem_fd(fd, &fflags, &memfd)) {
        fexecve_with_zipos(memfd, argv, envp, fflags);
      }
      pthread_cleanup_pop(1);
    } else {
      if ((fflags = getFdFexeFlags(fd)) == -1) {
        break;
      }
      fexecve_with_zipos(fd, argv, envp, fflags);
    }
  } while (0);
  rc = -1;
  STRACE("fexecve(%d) failed %d% m", fd, rc);
  return rc;
}
