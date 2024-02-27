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
#include "libc/calls/cp.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/stat.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/paths.h"
#include "libc/proc/execve.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/mfd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/consts/shm.h"
#include "libc/sysv/errfuns.h"
#include "libc/zip.internal.h"

static bool IsAPEFd(const int fd) {
  char buf[8];
  return (sys_pread(fd, buf, 8, 0, 0) == 8) && IsApeMagic(buf);
}

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

#define defer(fn) __attribute__((cleanup(fn)))

void cleanup_close(int *pFD) {
  STRACE("time to close");
  if (*pFD != -1) {
    close(*pFD);
  }
}
#define defer_close defer(cleanup_close)

void cleanup_unlink(const char **path) {
  STRACE("time to unlink");
  if (*path != NULL) {
    sys_unlink(*path);
  }
}
#define defer_unlink defer(cleanup_unlink)

#undef defer_unlink
#undef defer_close
#undef defer

static inline int isZipFile(const void *data, size_t data_size) {
  if (!_weaken(GetZipEocd)) {
    return enosys();
  }
  int ziperror;
  return _weaken(GetZipEocd)(data, data_size, &ziperror) != NULL;
}

static int isFdAZipFile(const int fd) {
  if (!_weaken(mmap) || !_weaken(munmap) || !_weaken(GetZipEocd) || __vforked) {
    return enosys();
  }

  struct stat st;
  if (fstat(fd, &st) == -1) {
    return -1;
  }
  void *space = _weaken(mmap)(0, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
  if (space == MAP_FAILED) {
    return -1;
  }
  int rc = isZipFile(space, st.st_size);
  if(_weaken(munmap)(space, st.st_size) == -1) {
    return -1;
  }
  return rc;
}

typedef enum {
  FEXEF_ZIP = 1 << 0,
  FEXEF_APE = 1 << 1
} FEXEF;

static inline int getFexeFlags(const void *data, size_t data_size) {
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

/**
 * Creates a memfd and copies fd to it.
 *
 * This does an inplace conversion of APE to ELF when detected!!!!
 */
static int fd_to_mem_fd(const int infd, FEXEF *flags) {
  if ((!IsLinux() && !IsFreebsd()) || !_weaken(mmap) || !_weaken(munmap)) {
    return enosys();
  }

  struct stat st;
  if (fstat(infd, &st) == -1) {
    return -1;
  }
  int fd;
  if (IsLinux()) {
    fd = sys_memfd_create(__func__, MFD_CLOEXEC);
  } else if (IsFreebsd()) {
    fd = sys_shm_open(SHM_ANON, O_CREAT | O_RDWR, 0);
  } else {
    return enosys();
  }
  if (fd == -1) {
    return -1;
  }
  void *space;
  if ((sys_ftruncate(fd, st.st_size, st.st_size) != -1) &&
      ((space = _weaken(mmap)(0, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED,
                              fd, 0)) != MAP_FAILED)) {
    ssize_t readRc;
    readRc = pread(infd, space, st.st_size, 0);
    bool success = readRc != -1;
    if (success) {
      int fexe_flags = getFexeFlags(space, st.st_size);
      success = fexe_flags != -1;
      *flags = fexe_flags;
    }
    const int e = errno;
    if ((_weaken(munmap)(space, st.st_size) != -1) && success) {
      unassert(readRc == st.st_size);
      return fd;
    } else if (!success) {
      errno = e;
    }
  }
  const int e = errno;
  close(fd);
  errno = e;
  return -1;
}

/**
 * Executes binary executable at file descriptor.
 *
 * This is only supported on Linux and FreeBSD. APE binaries are
 * supported. Zipos is supported. Zipos fds are copied to a new memfd. Zip files
 * and APE files are F_DUPFD to a high number with FD_CLOEXEC turned off. APE
 * files are ran with execve.
 *
 * @param fd is opened executable and current file position is ignored
 * @return doesn't return on success, otherwise -1 w/ errno
 * @raise ENOEXEC if file at `fd` isn't an assimilated ELF executable
 * @raise ENOSYS on Windows, XNU, OpenBSD, NetBSD, and Metal
 */
int fexecve(int fd, char *const argv[], char *const envp[]) {
  int rc = 0;
  if (!argv || !envp ||
      (IsAsan() &&
       (!__asan_is_valid_strlist(argv) || !__asan_is_valid_strlist(envp)))) {
    rc = efault();
  } else {
    STRACE("fexecve(%d, %s, %s) → ...", fd, DescribeStringList(argv),
           DescribeStringList(envp));
    int newfd = fd;
    do {
      if (!IsLinux() && !IsFreebsd()) {
        rc = enosys();
        break;
      }
      FEXEF fflags;
      if (__isfdkind(fd, kFdZip)) {
        BLOCK_SIGNALS;
        BLOCK_CANCELATION;
        strace_enabled(-1);
        newfd = fd_to_mem_fd(fd, &fflags);
        strace_enabled(+1);
        ALLOW_CANCELATION;
        ALLOW_SIGNALS;
        if (newfd == -1) {
          break;
        }
      } else {
        if (!__vforked) {
          int isFdAZipFileRc;
          BLOCK_SIGNALS;
          BLOCK_CANCELATION;
          strace_enabled(-1);
          isFdAZipFileRc = isFdAZipFile(newfd);
          strace_enabled(+1);
          ALLOW_CANCELATION;
          ALLOW_SIGNALS;
          if (isFdAZipFileRc == -1) {
            break;
          }
          fflags = isFdAZipFileRc << 0;
        }
        bool isAPE;
        BLOCK_SIGNALS;
        BLOCK_CANCELATION;
        isAPE = IsAPEFd(newfd);
        ALLOW_CANCELATION;
        ALLOW_SIGNALS;
        fflags |= (int)isAPE << 1;
      }
      if (fflags) {
        int flags;
        BLOCK_SIGNALS;
        BLOCK_CANCELATION;
        flags = fcntl(newfd, F_GETFD);
        if (flags != -1) {
          flags = fcntl(newfd, F_SETFD, flags & (~FD_CLOEXEC));
        }
        ALLOW_CANCELATION;
        ALLOW_SIGNALS;
        if (flags == -1) {
          break;
        }
        BLOCK_SIGNALS;
        BLOCK_CANCELATION;
        const int highfd = fcntl(newfd, F_DUPFD, 9001);
        if (highfd != -1) {
          close(newfd);
          newfd = highfd;
        }
        ALLOW_CANCELATION;
        ALLOW_SIGNALS;
      }
      if (fflags & FEXEF_ZIP) {
        char *path = alloca(PATH_MAX);
        FormatInt32(stpcpy(path, "COSMOPOLITAN_INIT_ZIPOS="), newfd);
        size_t numenvs;
        for (numenvs = 0; envp[numenvs];) ++numenvs;
        static _Thread_local char *envs[500];
        memcpy(envs, envp, numenvs * sizeof(char *));
        envs[numenvs] = path;
        envs[numenvs + 1] = NULL;
        envp = envs;
      }
      if (fflags & FEXEF_APE) {
        char path[14 + 12];
        FormatInt32(stpcpy(path, "/dev/fd/"), newfd);
        sys_execve(path, argv, envp);
        break;
      }
      fexecve_impl(newfd, argv, envp);
    } while (0);
    if (newfd != fd) {
      int keepErrno = errno;
      BLOCK_SIGNALS;
      BLOCK_CANCELATION;
      strace_enabled(-1);
      close(newfd);
      strace_enabled(+1);
      ALLOW_CANCELATION;
      ALLOW_SIGNALS;
      errno = keepErrno;
    }
    rc = -1;
  }
  STRACE("fexecve(%d) failed %d% m", fd, rc);
  return rc;
}
