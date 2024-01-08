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
#include "libc/proc/execve.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/mfd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/shm.h"
#include "libc/sysv/errfuns.h"

static bool IsAPEFd(const int fd) {
  char buf[8];
  return (sys_pread(fd, buf, 8, 0, 0) == 8) && IsApeLoadable(buf);
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

typedef enum {
  PTF_NUM = 1 << 0,
  PTF_NUM2 = 1 << 1,
  PTF_NUM3 = 1 << 2,
  PTF_ANY = 1 << 3
} PTF_PARSE;

static bool ape_to_elf(void *ape, const size_t apesize) {
  static const char printftok[] = "printf '";
  static const size_t printftoklen = sizeof(printftok) - 1;
  const char *tok = memmem(ape, apesize, printftok, printftoklen);
  if (tok) {
    tok += printftoklen;
    uint8_t *dest = ape;
    PTF_PARSE state = PTF_ANY;
    uint8_t value = 0;
    for (; tok < (const char *)(dest + apesize); tok++) {
      if ((state & (PTF_NUM | PTF_NUM2 | PTF_NUM3)) &&
          (*tok >= '0' && *tok <= '7')) {
        value = (value << 3) | (*tok - '0');
        state <<= 1;
        if (state & PTF_ANY) {
          *dest++ = value;
        }
      } else if (state & PTF_NUM) {
        break;
      } else {
        if (state & (PTF_NUM2 | PTF_NUM3)) {
          *dest++ = value;
        }
        if (*tok == '\\') {
          state = PTF_NUM;
          value = 0;
        } else if (*tok == '\'') {
          return true;
        } else {
          *dest++ = *tok;
          state = PTF_ANY;
        }
      }
    }
  }
  errno = ENOEXEC;
  return false;
}

/**
 * Creates a memfd and copies fd to it.
 *
 * This does an inplace conversion of APE to ELF when detected!!!!
 */
static int fd_to_mem_fd(const int infd, char *path) {
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
    if (success && (st.st_size > 8) && IsApeLoadable(space)) {
      int flags = fcntl(fd, F_GETFD);
      if ((success = (flags != -1) &&
                     (fcntl(fd, F_SETFD, flags & (~FD_CLOEXEC)) != -1) &&
                     ape_to_elf(space, st.st_size))) {
        const int newfd = fcntl(fd, F_DUPFD, 9001);
        if (newfd != -1) {
          close(fd);
          fd = newfd;
        }
      }
    }
    const int e = errno;
    if ((_weaken(munmap)(space, st.st_size) != -1) && success) {
      if (path) {
        FormatInt32(stpcpy(path, "COSMOPOLITAN_INIT_ZIPOS="), fd);
      }
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
 * supported. Zipos is supported. Zipos fds or FD_CLOEXEC APE fds or
 * fds that fail fexecve with ENOEXEC are copied to a new memfd (with
 * in-memory APE to ELF conversion) and fexecve is (re)attempted.
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
    int savedErr = 0;
    do {
      if (!IsLinux() && !IsFreebsd()) {
        rc = enosys();
        break;
      }
      if (!__isfdkind(fd, kFdZip)) {
        bool memfdReq;
        BLOCK_SIGNALS;
        BLOCK_CANCELATION;
        strace_enabled(-1);
        memfdReq = ((rc = fcntl(fd, F_GETFD)) != -1) && (rc & FD_CLOEXEC) &&
                   IsAPEFd(fd);
        strace_enabled(+1);
        ALLOW_CANCELATION;
        ALLOW_SIGNALS;
        if (rc == -1) {
          break;
        } else if (!memfdReq) {
          fexecve_impl(fd, argv, envp);
          if (errno != ENOEXEC) {
            break;
          }
          savedErr = ENOEXEC;
        }
      }
      int newfd;
      char *path = alloca(PATH_MAX);
      BLOCK_SIGNALS;
      BLOCK_CANCELATION;
      strace_enabled(-1);
      newfd = fd_to_mem_fd(fd, path);
      strace_enabled(+1);
      ALLOW_CANCELATION;
      ALLOW_SIGNALS;
      if (newfd == -1) {
        break;
      }
      size_t numenvs;
      for (numenvs = 0; envp[numenvs];) ++numenvs;
      // const size_t desenvs = min(500, max(numenvs + 1, 2));
      static _Thread_local char *envs[500];
      memcpy(envs, envp, numenvs * sizeof(char *));
      envs[numenvs] = path;
      envs[numenvs + 1] = NULL;
      fexecve_impl(newfd, argv, envs);
      if (!savedErr) {
        savedErr = errno;
      }
      BLOCK_SIGNALS;
      BLOCK_CANCELATION;
      strace_enabled(-1);
      close(newfd);
      strace_enabled(+1);
      ALLOW_CANCELATION;
      ALLOW_SIGNALS;
    } while (0);
    if (savedErr) {
      errno = savedErr;
    }
    rc = -1;
  }
  STRACE("fexecve(%d) failed %d% m", fd, rc);
  return rc;
}
