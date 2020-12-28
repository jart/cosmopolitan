/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/internal.h"
#include "libc/calls/ioctl.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigaction-linux.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/struct/winsize.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/vendor.internal.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/select.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/lock.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/msync.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/rusage.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/consts/tcp.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/consts/w.h"
#include "libc/sysv/errfuns.h"
#include "libc/time/struct/timezone.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "tool/build/lib/case.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/iovs.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/pml4t.h"
#include "tool/build/lib/syscall.h"
#include "tool/build/lib/throw.h"
#include "tool/build/lib/xlaterrno.h"

#define AT_FDCWD_LINUX   -100
#define TIOCGWINSZ_LINUX 0x5413
#define TCGETS_LINUX     0x5401
#define TCSETS_LINUX     0x5402
#define TCSETSW_LINUX    0x5403
#define TCSETSF_LINUX    0x5404
#define ISIG_LINUX       0b0000000000000001
#define ICANON_LINUX     0b0000000000000010
#define ECHO_LINUX       0b0000000000001000
#define OPOST_LINUX      0b0000000000000001

#define POINTER(x)    ((void *)(intptr_t)(x))
#define UNPOINTER(x)  ((int64_t)(intptr_t)(x))
#define SYSCALL(x, y) CASE(x, asm("# " #y); ax = y)
#define XLAT(x, y)    CASE(x, return y)
#define PNN(x)        ResolveAddress(m, x)
#define P(x)          ((x) ? PNN(x) : 0)
#define ASSIGN(D, S)  memcpy(&D, &S, MIN(sizeof(S), sizeof(D)))

const struct MachineFdCb kMachineFdCbHost = {
    .close = close,
    .readv = readv,
    .writev = writev,
    .ioctl = ioctl,
};

static int XlatSignal(int sig) {
  switch (sig) {
    XLAT(1, SIGHUP);
    XLAT(2, SIGINT);
    XLAT(3, SIGQUIT);
    XLAT(4, SIGILL);
    XLAT(5, SIGTRAP);
    XLAT(6, SIGABRT);
    XLAT(7, SIGBUS);
    XLAT(8, SIGFPE);
    XLAT(9, SIGKILL);
    XLAT(10, SIGUSR1);
    XLAT(11, SIGSEGV);
    XLAT(13, SIGPIPE);
    XLAT(14, SIGALRM);
    XLAT(15, SIGTERM);
    XLAT(21, SIGTTIN);
    XLAT(22, SIGTTOU);
    XLAT(24, SIGXCPU);
    XLAT(25, SIGXFSZ);
    XLAT(26, SIGVTALRM);
    XLAT(27, SIGPROF);
    XLAT(28, SIGWINCH);
    XLAT(17, SIGCHLD);
    XLAT(18, SIGCONT);
    XLAT(29, SIGIO);
    XLAT(19, SIGSTOP);
    XLAT(31, SIGSYS);
    XLAT(20, SIGTSTP);
    XLAT(23, SIGURG);
    XLAT(12, SIGUSR2);
    XLAT(0x2000, SIGSTKSZ);
    XLAT(30, SIGPWR);
    XLAT(0x10, SIGSTKFLT);
    default:
      return einval();
  }
}

static int XlatSig(int x) {
  switch (x) {
    XLAT(0, SIG_BLOCK);
    XLAT(1, SIG_UNBLOCK);
    XLAT(2, SIG_SETMASK);
    default:
      return einval();
  }
}

static int XlatSocketFamily(int x) {
  switch (x) {
    XLAT(0, AF_INET);
    XLAT(2, AF_INET);
    default:
      return epfnosupport();
  }
}

static int XlatSocketType(int x) {
  switch (x) {
    XLAT(1, SOCK_STREAM);
    XLAT(2, SOCK_DGRAM);
    default:
      return einval();
  }
}

static int XlatSocketProtocol(int x) {
  switch (x) {
    XLAT(6, IPPROTO_TCP);
    XLAT(17, IPPROTO_UDP);
    default:
      return einval();
  }
}

static unsigned XlatSocketFlags(int flags) {
  unsigned res = 0;
  if (flags & 0x080000) res |= SOCK_CLOEXEC;
  if (flags & 0x000800) res |= SOCK_NONBLOCK;
  return res;
}

static int XlatSocketLevel(int x) {
  switch (x) {
    XLAT(0, SOL_IP);
    XLAT(1, SOL_SOCKET);
    XLAT(6, SOL_TCP);
    XLAT(17, SOL_UDP);
    default:
      return einval();
  }
}

static int XlatSocketOptname(int x) {
  switch (x) {
    XLAT(2, SO_REUSEADDR);
    XLAT(15, SO_REUSEPORT);
    XLAT(9, SO_KEEPALIVE);
    XLAT(5, SO_DONTROUTE);
    XLAT(7, SO_SNDBUF);
    XLAT(8, SO_RCVBUF);
    XLAT(13, SO_LINGER);
    default:
      return einval();
  }
}

static int XlatMapFlags(int x) {
  unsigned res = 0;
  if (x & 1) res |= MAP_SHARED;
  if (x & 2) res |= MAP_PRIVATE;
  if (x & 16) res |= MAP_FIXED;
  if (x & 32) res |= MAP_ANONYMOUS;
  return res;
}

static int XlatAccess(int x) {
  unsigned res = F_OK;
  if (x & 1) res |= X_OK;
  if (x & 2) res |= W_OK;
  if (x & 4) res |= R_OK;
  return res;
}

static int XlatSigaction(int x) {
  unsigned res = 0;
  if (x & 0x00000001) res |= SA_NOCLDSTOP;
  if (x & 0x00000002) res |= SA_NOCLDWAIT;
  if (x & 0x00000004) res |= SA_SIGINFO;
  if (x & 0x04000000) res |= SA_RESTORER;
  if (x & 0x08000000) res |= SA_ONSTACK;
  if (x & 0x10000000) res |= SA_RESTART;
  if (x & 0x40000000) res |= SA_NODEFER;
  if (x & 0x40000000) res |= SA_NOMASK;
  if (x & 0x80000000) res |= SA_RESETHAND;
  if (x & 0x80000000) res |= SA_ONESHOT;
  return res;
}

static int XlatSo(int x) {
  switch (x) {
    XLAT(-1, SO_EXCLUSIVEADDRUSE);
    XLAT(1, SO_DEBUG);
    XLAT(2, SO_REUSEADDR);
    XLAT(3, SO_TYPE);
    XLAT(4, SO_ERROR);
    XLAT(5, SO_DONTROUTE);
    XLAT(6, SO_BROADCAST);
    XLAT(7, SO_SNDBUF);
    XLAT(8, SO_RCVBUF);
    XLAT(9, SO_KEEPALIVE);
    XLAT(10, SO_OOBINLINE);
    XLAT(13, SO_LINGER);
    XLAT(15, SO_REUSEPORT);
    XLAT(17, SO_PEERCRED);
    XLAT(18, SO_RCVLOWAT);
    XLAT(19, SO_SNDLOWAT);
    XLAT(20, SO_RCVTIMEO);
    XLAT(21, SO_SNDTIMEO);
    XLAT(29, SO_TIMESTAMP);
    XLAT(30, SO_ACCEPTCONN);
    XLAT(38, SO_PROTOCOL);
    XLAT(39, SO_DOMAIN);
    XLAT(47, SO_MAX_PACING_RATE);
    default:
      return x;
  }
}

static int XlatClock(int x) {
  switch (x) {
    XLAT(0, CLOCK_REALTIME);
    XLAT(4, CLOCK_MONOTONIC);
    default:
      return x;
  }
}

static int XlatTcp(int x) {
  switch (x) {
    XLAT(1, TCP_NODELAY);
    XLAT(2, TCP_MAXSEG);
    XLAT(4, TCP_KEEPIDLE);
    XLAT(5, TCP_KEEPINTVL);
    XLAT(6, TCP_KEEPCNT);
    XLAT(23, TCP_FASTOPEN);
    default:
      return x;
  }
}

static int XlatFd(struct Machine *m, int fd) {
  if (!(0 <= fd && fd < m->fds.i)) return ebadf();
  if (!m->fds.p[fd].cb) return ebadf();
  return m->fds.p[fd].fd;
}

static int XlatAfd(struct Machine *m, int fd) {
  if (fd == AT_FDCWD_LINUX) return AT_FDCWD;
  return XlatFd(m, fd);
}

static int XlatAtf(int x) {
  unsigned res = 0;
  if (x & 0x0100) res |= AT_SYMLINK_NOFOLLOW;
  if (x & 0x0200) res |= AT_REMOVEDIR;
  if (x & 0x0400) res |= AT_SYMLINK_FOLLOW;
  if (x & 0x1000) res |= AT_EMPTY_PATH;
  return res;
}

static int XlatMsyncFlags(int x) {
  unsigned res = 0;
  if (x & 1) res |= MS_ASYNC;
  if (x & 2) res |= MS_INVALIDATE;
  if (x & 4) res |= MS_SYNC;
  return res;
}

static unsigned XlatOpenMode(unsigned flags) {
  switch (flags & 3) {
    case 0:
      return O_RDONLY;
    case 1:
      return O_WRONLY;
    case 2:
      return O_RDWR;
    default:
      unreachable;
  }
}

static unsigned XlatOpenFlags(unsigned flags) {
  unsigned res = 0;
  res = XlatOpenMode(flags);
  if (flags & 0x80000) res |= O_CLOEXEC;
  if (flags & 0x400) res |= O_APPEND;
  if (flags & 0x40) res |= O_CREAT;
  if (flags & 0x80) res |= O_EXCL;
  if (flags & 0x200) res |= O_TRUNC;
  if (flags & 0x0800) res |= O_NDELAY;
  if (flags & 0x4000) res |= O_DIRECT;
  if (flags & 0x0800) res |= O_NONBLOCK;
  if (flags & 0x1000) res |= O_DSYNC;
  if (flags & 0x101000) res |= O_RSYNC;
  if (flags & 0x040000) res |= O_NOATIME;
  return res;
}

static int XlatFcntlCmd(int x) {
  switch (x) {
    XLAT(1, F_GETFD);
    XLAT(2, F_SETFD);
    XLAT(3, F_GETFL);
    XLAT(4, F_SETFL);
    default:
      return einval();
  }
}

static int XlatFcntlArg(int x) {
  switch (x) {
    XLAT(0, 0);
    XLAT(1, FD_CLOEXEC);
    XLAT(0x0800, O_NONBLOCK);
    default:
      return einval();
  }
}

static int XlatAdvice(int x) {
  switch (x) {
    XLAT(0, MADV_NORMAL);
    XLAT(1, MADV_RANDOM);
    XLAT(2, MADV_SEQUENTIAL);
    XLAT(3, MADV_WILLNEED);
    XLAT(4, MADV_DONTNEED);
    XLAT(8, MADV_FREE);
    XLAT(12, MADV_MERGEABLE);
    default:
      return einval();
  }
}

static int XlatLock(int x) {
  unsigned res = 0;
  if (x & 1) res |= LOCK_SH;
  if (x & 2) res |= LOCK_EX;
  if (x & 4) res |= LOCK_NB;
  if (x & 8) res |= LOCK_UN;
  return res;
}

static int XlatWait(int x) {
  unsigned res = 0;
  if (x & 1) res |= WNOHANG;
  if (x & 2) res |= WUNTRACED;
  if (x & 8) res |= WCONTINUED;
  return res;
}

static int XlatRusage(int x) {
  switch (x) {
    XLAT(0, RUSAGE_SELF);
    XLAT(-1, RUSAGE_CHILDREN);
    XLAT(1, RUSAGE_THREAD);
    default:
      return einval();
  }
}

static const char *GetSimulated(void) {
  if (IsGenuineCosmo()) {
    return " SIMULATED";
  } else {
    return "";
  }
}

static int AppendIovsReal(struct Machine *m, struct Iovs *ib, int64_t addr,
                          size_t size) {
  void *real;
  size_t have;
  unsigned got;
  while (size) {
    if (!(real = FindReal(m, addr))) return efault();
    have = 0x1000 - (addr & 0xfff);
    got = MIN(size, have);
    if (AppendIovs(ib, real, got) == -1) return -1;
    addr += got;
    size -= got;
  }
  return 0;
}

static int AppendIovsGuest(struct Machine *m, struct Iovs *iv, int64_t iovaddr,
                           long iovlen) {
  int rc;
  size_t i, iovsize;
  struct iovec *guestiovs;
  if (!__builtin_mul_overflow(iovlen, sizeof(struct iovec), &iovsize) &&
      (0 <= iovsize && iovsize <= 0x7ffff000)) {
    if ((guestiovs = malloc(iovsize))) {
      VirtualSendRead(m, guestiovs, iovaddr, iovsize);
      for (rc = i = 0; i < iovlen; ++i) {
        if (AppendIovsReal(m, iv, (intptr_t)guestiovs[i].iov_base,
                           guestiovs[i].iov_len) == -1) {
          rc = -1;
          break;
        }
      }
      free(guestiovs);
    } else {
      rc = enomem();
    }
  } else {
    rc = eoverflow();
  }
  return rc;
}

static struct sigaction *CoerceSigactionToCosmo(
    struct sigaction *dst, const struct sigaction$linux *src) {
  if (!src) return NULL;
  memset(dst, 0, sizeof(*dst));
  ASSIGN(dst->sa_handler, src->sa_handler);
  ASSIGN(dst->sa_restorer, src->sa_restorer);
  ASSIGN(dst->sa_flags, src->sa_flags);
  ASSIGN(dst->sa_mask, src->sa_mask);
  return dst;
}

static struct sigaction$linux *CoerceSigactionToLinux(
    struct sigaction$linux *dst, const struct sigaction *src) {
  if (!dst) return NULL;
  memset(dst, 0, sizeof(*dst));
  ASSIGN(dst->sa_handler, src->sa_handler);
  ASSIGN(dst->sa_restorer, src->sa_restorer);
  ASSIGN(dst->sa_flags, src->sa_flags);
  ASSIGN(dst->sa_mask, src->sa_mask);
  return dst;
}

static int OpArchPrctl(struct Machine *m, int code, int64_t addr) {
  switch (code) {
    case ARCH_SET_GS:
      Write64(m->gs, addr);
      return 0;
    case ARCH_SET_FS:
      Write64(m->fs, addr);
      return 0;
    case ARCH_GET_GS:
      VirtualRecvWrite(m, addr, m->gs, 8);
      return 0;
    case ARCH_GET_FS:
      VirtualRecvWrite(m, addr, m->fs, 8);
      return 0;
    default:
      return einval();
  }
}

static int OpMprotect(struct Machine *m, int64_t addr, uint64_t len, int prot) {
  return 0;
}

static int OpMadvise(struct Machine *m, int64_t addr, size_t length,
                     int advice) {
  return enosys();
}

static int64_t OpBrk(struct Machine *m, int64_t addr) {
  addr = ROUNDUP(addr, PAGESIZE);
  if (addr > m->brk) {
    if (ReserveVirtual(m, m->brk, addr - m->brk, 0x0207) != -1) {
      m->brk = addr;
    }
  } else if (addr < m->brk) {
    if (FreeVirtual(m, addr, m->brk - addr) != -1) {
      m->brk = addr;
    }
  }
  return m->brk;
}

static int OpMunmap(struct Machine *m, int64_t virt, uint64_t size) {
  VERBOSEF("MUNMAP%s %p %,ld", GetSimulated(), virt, size);
  return FreeVirtual(m, virt, size);
}

static int64_t OpMmap(struct Machine *m, int64_t virt, size_t size, int prot,
                      int flags, int fd, int64_t offset) {
  void *tmp;
  uint64_t key;
  VERBOSEF("MMAP%s %p %,ld %#x %#x %d %#lx", GetSimulated(), virt, size, prot,
           flags, fd, offset);
  if (prot & PROT_READ) {
    key = 0x0205;
    if (prot & PROT_WRITE) key |= 2;
    if (!(prot & PROT_EXEC)) key |= 0x8000000000000000;
    flags = XlatMapFlags(flags);
    if (fd != -1 && (fd = XlatFd(m, fd)) == -1) return -1;
    if (!(flags & MAP_FIXED)) {
      if (!virt) {
        if ((virt = FindVirtual(m, m->brk, size)) == -1) return -1;
        m->brk = virt + size;
      } else {
        if ((virt = FindVirtual(m, virt, size)) == -1) return -1;
      }
    }
    if (ReserveVirtual(m, virt, size, key) != -1) {
      if (fd != -1 && !(flags & MAP_ANONYMOUS)) {
        /* TODO: lazy file mappings */
        CHECK_NOTNULL((tmp = malloc(size)));
        CHECK_EQ(size, pread(fd, tmp, size, offset));
        VirtualRecvWrite(m, virt, tmp, size);
        free(tmp);
      }
    } else {
      FreeVirtual(m, virt, size);
      return -1;
    }
    return virt;
  } else {
    return FreeVirtual(m, virt, size);
  }
}

static int OpMsync(struct Machine *m, int64_t virt, size_t size, int flags) {
  return enosys();
#if 0
  size_t i;
  void *page;
  virt = ROUNDDOWN(virt, 4096);
  flags = XlatMsyncFlags(flags);
  for (i = 0; i < size; i += 4096) {
    if (!(page = FindReal(m, virt + i))) return efault();
    if (msync(page, 4096, flags) == -1) return -1;
  }
  return 0;
#endif
}

static int OpClose(struct Machine *m, int fd) {
  int rc;
  struct FdClosed *closed;
  if (!(0 <= fd && fd < m->fds.i)) return ebadf();
  if (!m->fds.p[fd].cb) return ebadf();
  rc = m->fds.p[fd].cb->close(m->fds.p[fd].fd);
  MachineFdRemove(&m->fds, fd);
  return rc;
}

static int OpOpenat(struct Machine *m, int dirfd, int64_t pathaddr, int flags,
                    int mode) {
  int fd, i;
  const char *path;
  flags = XlatOpenFlags(flags);
  if ((dirfd = XlatAfd(m, dirfd)) == -1) return -1;
  if ((i = MachineFdAdd(&m->fds)) == -1) return -1;
  path = LoadStr(m, pathaddr);
  if ((fd = openat(dirfd, path, flags, mode)) != -1) {
    m->fds.p[i].cb = &kMachineFdCbHost;
    m->fds.p[i].fd = fd;
    VERBOSEF("openat(%#x, %`'s, %#x, %#x) → %d [%d]", dirfd, path, flags, mode,
             i, fd);
    fd = i;
  } else {
    MachineFdRemove(&m->fds, i);
    VERBOSEF("openat(%#x, %`'s, %#x, %#x) failed", dirfd, path, flags, mode);
  }
  return fd;
}

static int OpPipe(struct Machine *m, int64_t pipefds_addr) {
  int i, j, pipefds[2];
  if ((i = MachineFdAdd(&m->fds)) != -1) {
    if ((j = MachineFdAdd(&m->fds)) != -1) {
      if (pipe(pipefds) != -1) {
        m->fds.p[i].cb = &kMachineFdCbHost;
        m->fds.p[i].fd = pipefds[0];
        m->fds.p[j].cb = &kMachineFdCbHost;
        m->fds.p[j].fd = pipefds[1];
        pipefds[0] = i;
        pipefds[1] = j;
        VirtualRecvWrite(m, pipefds_addr, pipefds, sizeof(pipefds));
        return 0;
      }
      MachineFdRemove(&m->fds, j);
    }
    MachineFdRemove(&m->fds, i);
  }
  return -1;
}

static int OpDup(struct Machine *m, int fd) {
  int i;
  if ((fd = XlatFd(m, fd)) != -1) {
    if ((i = MachineFdAdd(&m->fds)) != -1) {
      if ((fd = dup(fd)) != -1) {
        m->fds.p[i].cb = &kMachineFdCbHost;
        m->fds.p[i].fd = fd;
        return i;
      }
      MachineFdRemove(&m->fds, i);
    }
  }
  return -1;
}

static int OpDup2(struct Machine *m, int fd, int newfd) {
  int i, rc;
  if ((fd = XlatFd(m, fd)) == -1) return -1;
  if ((0 <= newfd && newfd < m->fds.i)) {
    if ((rc = dup2(fd, m->fds.p[newfd].fd)) != -1) {
      m->fds.p[newfd].cb = &kMachineFdCbHost;
      m->fds.p[newfd].fd = rc;
      rc = newfd;
    }
  } else if ((i = MachineFdAdd(&m->fds)) != -1) {
    if ((rc = dup(fd)) != -1) {
      m->fds.p[i].cb = &kMachineFdCbHost;
      m->fds.p[i].fd = rc;
      rc = i;
    }
  } else {
    rc = -1;
  }
  return rc;
}

static int OpSocket(struct Machine *m, int family, int type, int protocol) {
  int i, fd;
  if ((family = XlatSocketFamily(family)) == -1) return -1;
  if ((type = XlatSocketType(type)) == -1) return -1;
  if ((protocol = XlatSocketProtocol(protocol)) == -1) return -1;
  if ((i = MachineFdAdd(&m->fds)) == -1) return -1;
  if ((fd = socket(family, type, protocol)) != -1) {
    m->fds.p[i].cb = &kMachineFdCbHost;
    m->fds.p[i].fd = fd;
    fd = i;
  } else {
    MachineFdRemove(&m->fds, i);
  }
  return fd;
}

static int OpAccept4(struct Machine *m, int fd, int64_t addraddr,
                     int64_t addrsizeaddr, int flags) {
  int i, rc;
  void *addr;
  uint8_t b[4];
  uint32_t addrsize;
  if ((fd = XlatFd(m, fd)) == -1) return -1;
  VirtualSendRead(m, b, addrsizeaddr, 4);
  addrsize = Read32(b);
  if (!(addr = malloc(addrsize))) return -1;
  if ((i = rc = MachineFdAdd(&m->fds)) != -1) {
    if ((rc = accept4(fd, addr, &addrsize, XlatSocketFlags(flags))) != -1) {
      Write32(b, addrsize);
      VirtualRecv(m, addrsizeaddr, b, 4);
      VirtualRecvWrite(m, addraddr, addr, addrsize);
      m->fds.p[i].cb = &kMachineFdCbHost;
      m->fds.p[i].fd = rc;
      rc = i;
    } else {
      MachineFdRemove(&m->fds, i);
    }
  }
  free(addr);
  return rc;
}

static int OpConnectBind(struct Machine *m, int fd, intptr_t addraddr,
                         uint32_t addrsize,
                         int impl(int, const void *, uint32_t)) {
  int rc;
  void *addr;
  if ((fd = XlatFd(m, fd)) == -1) return -1;
  if (!(addr = malloc(addrsize))) return -1;
  VirtualSendRead(m, addr, addraddr, addrsize);
  rc = impl(fd, addr, addrsize);
  free(addr);
  return rc;
}

static int OpBind(struct Machine *m, int fd, intptr_t addraddr,
                  uint32_t addrsize) {
  return OpConnectBind(m, fd, addraddr, addrsize, bind);
}

static int OpConnect(struct Machine *m, int fd, int64_t addraddr,
                     uint32_t addrsize) {
  return OpConnectBind(m, fd, addraddr, addrsize, connect);
}

static int OpSetsockopt(struct Machine *m, int fd, int level, int optname,
                        int64_t optvaladdr, uint32_t optvalsize) {
  int rc;
  void *optval;
  if ((level = XlatSocketLevel(level)) == -1) return -1;
  if ((optname = XlatSocketOptname(optname)) == -1) return -1;
  if ((fd = XlatFd(m, fd)) == -1) return -1;
  if (!(optval = malloc(optvalsize))) return -1;
  VirtualSendRead(m, optval, optvaladdr, optvalsize);
  rc = setsockopt(fd, level, optname, optval, optvalsize);
  free(optval);
  return rc;
}

static ssize_t OpRead(struct Machine *m, int fd, int64_t addr, size_t size) {
  ssize_t rc;
  struct Iovs iv;
  InitIovs(&iv);
  if ((0 <= fd && fd < m->fds.i) && m->fds.p[fd].cb) {
    if ((rc = AppendIovsReal(m, &iv, addr, size)) != -1) {
      if ((rc = m->fds.p[fd].cb->readv(m->fds.p[fd].fd, iv.p, iv.i)) != -1) {
        SetWriteAddr(m, addr, rc);
      }
    }
  } else {
    rc = ebadf();
  }
  FreeIovs(&iv);
  return rc;
}

static ssize_t OpPread(struct Machine *m, int fd, int64_t addr, size_t size,
                       int64_t offset) {
  ssize_t rc;
  struct Iovs iv;
  InitIovs(&iv);
  if ((rc = XlatFd(m, fd)) != -1) {
    fd = rc;
    if ((rc = AppendIovsReal(m, &iv, addr, size)) != -1) {
      if ((rc = preadv(fd, iv.p, iv.i, offset)) != -1) {
        SetWriteAddr(m, addr, rc);
      }
    }
  }
  FreeIovs(&iv);
  return rc;
}

static ssize_t OpWrite(struct Machine *m, int fd, int64_t addr, size_t size) {
  ssize_t rc;
  struct Iovs iv;
  InitIovs(&iv);
  if ((0 <= fd && fd < m->fds.i) && m->fds.p[fd].cb) {
    if ((rc = AppendIovsReal(m, &iv, addr, size)) != -1) {
      if ((rc = m->fds.p[fd].cb->writev(m->fds.p[fd].fd, iv.p, iv.i)) != -1) {
        SetReadAddr(m, addr, rc);
      } else {
        VERBOSEF("write(%d [%d], %p, %zu) failed: %s", fd, m->fds.p[fd].fd,
                 addr, size, strerror(errno));
      }
    }
  } else {
    VERBOSEF("write(%d, %p, %zu) bad fd", fd, addr, size);
    rc = ebadf();
  }
  FreeIovs(&iv);
  return rc;
}

static ssize_t OpPwrite(struct Machine *m, int fd, int64_t addr, size_t size,
                        int64_t offset) {
  ssize_t rc;
  struct Iovs iv;
  InitIovs(&iv);
  if ((rc = XlatFd(m, fd)) != -1) {
    fd = rc;
    if ((rc = AppendIovsReal(m, &iv, addr, size)) != -1) {
      if ((rc = pwritev(fd, iv.p, iv.i, offset)) != -1) {
        SetReadAddr(m, addr, rc);
      }
    }
  }
  FreeIovs(&iv);
  return rc;
}

static int IoctlTiocgwinsz(struct Machine *m, int fd, int64_t addr,
                           int (*fn)(int, uint64_t, void *)) {
  int rc;
  struct winsize ws;
  if ((rc = fn(fd, TIOCGWINSZ, &ws)) != -1) {
    VirtualRecvWrite(m, addr, &ws, sizeof(ws));
  }
  return rc;
}

static int IoctlTcgets(struct Machine *m, int fd, int64_t addr,
                       int (*fn)(int, uint64_t, void *)) {
  int rc;
  struct termios tio, tio2;
  if ((rc = fn(fd, TCGETS, &tio)) != -1) {
    memcpy(&tio2, &tio, sizeof(tio));
    tio2.c_iflag = 0;
    if (tio.c_lflag & ISIG) tio2.c_lflag |= ISIG_LINUX;
    if (tio.c_lflag & ICANON) tio2.c_lflag |= ICANON_LINUX;
    if (tio.c_lflag & ECHO) tio2.c_lflag |= ECHO_LINUX;
    tio2.c_oflag = 0;
    if (tio.c_oflag & OPOST) tio2.c_oflag |= OPOST_LINUX;
    VirtualRecvWrite(m, addr, &tio2, sizeof(tio2));
  }
  return rc;
}

static int IoctlTcsets(struct Machine *m, int fd, int64_t request, int64_t addr,
                       int (*fn)(int, uint64_t, void *)) {
  struct termios tio, tio2;
  VirtualSendRead(m, &tio, addr, sizeof(tio));
  memcpy(&tio2, &tio, sizeof(tio));
  tio2.c_iflag = 0;
  if (tio.c_lflag & ISIG_LINUX) tio2.c_lflag |= ISIG;
  if (tio.c_lflag & ICANON_LINUX) tio2.c_lflag |= ICANON;
  if (tio.c_lflag & ECHO_LINUX) tio2.c_lflag |= ECHO;
  tio2.c_oflag = 0;
  if (tio.c_oflag & OPOST_LINUX) tio2.c_oflag |= OPOST;
  return fn(fd, request, &tio2);
}

static int OpIoctl(struct Machine *m, int fd, uint64_t request, int64_t addr) {
  int (*fn)(int, uint64_t, void *);
  if (!(0 <= fd && fd < m->fds.i) || !m->fds.p[fd].cb) return ebadf();
  fn = m->fds.p[fd].cb->ioctl;
  fd = m->fds.p[fd].fd;
  switch (request) {
    case TIOCGWINSZ_LINUX:
      return IoctlTiocgwinsz(m, fd, addr, fn);
    case TCGETS_LINUX:
      return IoctlTcgets(m, fd, addr, fn);
    case TCSETS_LINUX:
      return IoctlTcsets(m, fd, TCSETS, addr, fn);
    case TCSETSW_LINUX:
      return IoctlTcsets(m, fd, TCSETSW, addr, fn);
    case TCSETSF_LINUX:
      return IoctlTcsets(m, fd, TCSETSF, addr, fn);
    default:
      return einval();
  }
}

static ssize_t OpReadv(struct Machine *m, int fd, int64_t iovaddr, int iovlen) {
  ssize_t rc;
  struct Iovs iv;
  InitIovs(&iv);
  if ((0 <= fd && fd < m->fds.i) && m->fds.p[fd].cb) {
    if ((rc = AppendIovsGuest(m, &iv, iovaddr, iovlen)) != -1) {
      rc = m->fds.p[fd].cb->readv(m->fds.p[fd].fd, iv.p, iv.i);
    }
  } else {
    rc = ebadf();
  }
  FreeIovs(&iv);
  return rc;
}

static ssize_t OpWritev(struct Machine *m, int fd, int64_t iovaddr,
                        int iovlen) {
  ssize_t rc;
  struct Iovs iv;
  InitIovs(&iv);
  if ((0 <= fd && fd < m->fds.i) && m->fds.p[fd].cb) {
    if ((rc = AppendIovsGuest(m, &iv, iovaddr, iovlen)) != -1) {
      rc = m->fds.p[fd].cb->writev(m->fds.p[fd].fd, iv.p, iv.i);
    }
  } else {
    rc = ebadf();
  }
  FreeIovs(&iv);
  return rc;
}

static int64_t OpLseek(struct Machine *m, int fd, int64_t offset, int whence) {
  if ((fd = XlatFd(m, fd)) == -1) return -1;
  return lseek(fd, offset, whence);
}

static ssize_t OpFtruncate(struct Machine *m, int fd, int64_t size) {
  if ((fd = XlatFd(m, fd)) == -1) return -1;
  return ftruncate(fd, size);
}

static int OpFaccessat(struct Machine *m, int dirfd, int64_t path, int mode,
                       int flags) {
  flags = XlatAtf(flags);
  mode = XlatAccess(mode);
  if ((dirfd = XlatAfd(m, dirfd)) == -1) return -1;
  return faccessat(dirfd, LoadStr(m, path), mode, flags);
}

static int OpFstatat(struct Machine *m, int dirfd, int64_t path, int64_t staddr,
                     int flags) {
  int rc;
  struct stat st;
  flags = XlatAtf(flags);
  if ((dirfd = XlatAfd(m, dirfd)) == -1) return -1;
  if ((rc = fstatat(dirfd, LoadStr(m, path), &st, flags)) != -1) {
    VirtualRecvWrite(m, staddr, &st, sizeof(struct stat));
  }
  return rc;
}

static int OpFstat(struct Machine *m, int fd, int64_t staddr) {
  int rc;
  struct stat st;
  if ((fd = XlatFd(m, fd)) == -1) return -1;
  if ((rc = fstat(fd, &st)) != -1) {
    VirtualRecvWrite(m, staddr, &st, sizeof(struct stat));
  }
  return rc;
}

static int OpListen(struct Machine *m, int fd, int backlog) {
  if ((fd = XlatFd(m, fd)) == -1) return -1;
  return listen(fd, backlog);
}

static int OpShutdown(struct Machine *m, int fd, int how) {
  if ((fd = XlatFd(m, fd)) == -1) return -1;
  return shutdown(fd, how);
}

static int OpFsync(struct Machine *m, int fd) {
  if ((fd = XlatFd(m, fd)) == -1) return -1;
  return fsync(fd);
}

static int OpFdatasync(struct Machine *m, int fd) {
  if ((fd = XlatFd(m, fd)) == -1) return -1;
  return fdatasync(fd);
}

static int OpFchmod(struct Machine *m, int fd, uint32_t mode) {
  if ((fd = XlatFd(m, fd)) == -1) return -1;
  return fchmod(fd, mode);
}

static int OpFcntl(struct Machine *m, int fd, int cmd, int arg) {
  if ((cmd = XlatFcntlCmd(cmd)) == -1) return -1;
  if ((arg = XlatFcntlArg(arg)) == -1) return -1;
  if ((fd = XlatFd(m, fd)) == -1) return -1;
  return fcntl(fd, cmd, arg);
}

static int OpFadvise(struct Machine *m, int fd, uint64_t offset, uint64_t len,
                     int advice) {
  if ((fd = XlatFd(m, fd)) == -1) return -1;
  if ((advice = XlatAdvice(advice)) == -1) return -1;
  return fadvise(fd, offset, len, advice);
}

static int OpFlock(struct Machine *m, int fd, int lock) {
  if ((fd = XlatFd(m, fd)) == -1) return -1;
  if ((lock = XlatLock(lock)) == -1) return -1;
  return flock(fd, lock);
}

static int OpChdir(struct Machine *m, int64_t path) {
  return chdir(LoadStr(m, path));
}

static int OpMkdir(struct Machine *m, int64_t path, int mode) {
  return mkdir(LoadStr(m, path), mode);
}

static int OpMknod(struct Machine *m, int64_t path, uint32_t mode,
                   uint64_t dev) {
  return mknod(LoadStr(m, path), mode, dev);
}

static int OpRmdir(struct Machine *m, int64_t path) {
  return rmdir(LoadStr(m, path));
}

static int OpUnlink(struct Machine *m, int64_t path) {
  return unlink(LoadStr(m, path));
}

static int OpRename(struct Machine *m, int64_t src, int64_t dst) {
  return rename(LoadStr(m, src), LoadStr(m, dst));
}

static int OpTruncate(struct Machine *m, int64_t path, uint64_t length) {
  return truncate(LoadStr(m, path), length);
}

static int OpLink(struct Machine *m, int64_t existingpath, int64_t newpath) {
  return link(LoadStr(m, existingpath), LoadStr(m, newpath));
}

static int OpSymlink(struct Machine *m, int64_t target, int64_t linkpath) {
  return symlink(LoadStr(m, target), LoadStr(m, linkpath));
}

static int OpChmod(struct Machine *m, int64_t path, uint32_t mode) {
  return chmod(LoadStr(m, path), mode);
}

static int OpFork(struct Machine *m) {
  return enosys();
}

static int OpExecve(struct Machine *m, int64_t programaddr, int64_t argvaddr,
                    int64_t envpaddr) {
  return enosys();
}

static int OpWait4(struct Machine *m, int pid, int64_t opt_out_wstatus_addr,
                   int options, int64_t opt_out_rusage_addr) {
  int rc;
  int32_t wstatus;
  struct rusage rusage;
  if ((options = XlatWait(options)) == -1) return -1;
  if ((rc = wait4(pid, &wstatus, options, &rusage)) != -1) {
    if (opt_out_wstatus_addr) {
      VirtualRecvWrite(m, opt_out_wstatus_addr, &wstatus, sizeof(wstatus));
    }
    if (opt_out_rusage_addr) {
      VirtualRecvWrite(m, opt_out_rusage_addr, &rusage, sizeof(rusage));
    }
  }
  return rc;
}

static int OpGetrusage(struct Machine *m, int resource, int64_t rusageaddr) {
  int rc;
  struct rusage rusage;
  if ((resource = XlatRusage(resource)) == -1) return -1;
  if ((rc = getrusage(resource, &rusage)) != -1) {
    VirtualRecvWrite(m, rusageaddr, &rusage, sizeof(rusage));
  }
  return rc;
}

static int OpGetrlimit(struct Machine *m, int resource, int64_t rlimitaddr) {
  return enosys();
}

static int64_t OpGetcwd(struct Machine *m, int64_t bufaddr, size_t size) {
  size_t n;
  char *buf;
  int64_t res;
  size = MIN(size, PATH_MAX);
  if (!(buf = malloc(size))) return enomem();
  if ((getcwd)(buf, size)) {
    n = strlen(buf);
    VirtualRecvWrite(m, bufaddr, buf, n);
    res = bufaddr;
  } else {
    res = -1;
  }
  free(buf);
  return res;
}

static int OpSigaction(struct Machine *m, int sig, int64_t act, int64_t old) {
  return 0;
  int rc;
  struct OpSigactionMemory {
    struct sigaction act, old;
    uint8_t b[sizeof(struct sigaction$linux)];
    void *p[2];
  } * mem;
  if (!(mem = malloc(sizeof(*mem)))) return enomem();
  if ((rc = sigaction(
           XlatSignal(sig),
           CoerceSigactionToCosmo(
               &mem->act, LoadBuf(m, act, sizeof(struct sigaction$linux))),
           &mem->old)) != -1) {
    CoerceSigactionToLinux(BeginStoreNp(m, old, sizeof(mem->b), mem->p, mem->b),
                           &mem->old);
    EndStoreNp(m, old, sizeof(mem->b), mem->p, mem->b);
  }
  free(mem);
  return rc;
}

static int OpNanosleep(struct Machine *m, int64_t req, int64_t rem) {
  int rc;
  void *p[2];
  uint8_t b[sizeof(struct timespec)];
  if ((rc = nanosleep(LoadBuf(m, req, sizeof(b)),
                      BeginStoreNp(m, rem, sizeof(b), p, b))) != -1) {
    EndStoreNp(m, rem, sizeof(b), p, b);
  }
  return rc;
}

static int OpSigsuspend(struct Machine *m, int64_t maskaddr) {
  void *p;
  sigset_t mask;
  if (!(p = LoadBuf(m, maskaddr, 8))) return efault();
  memset(&mask, 0, sizeof(mask));
  memcpy(&mask, p, 8);
  return sigsuspend(&mask);
}

static int OpClockGettime(struct Machine *m, int clockid, int64_t ts) {
  int rc;
  void *tsp[2];
  uint8_t tsb[sizeof(struct timespec)];
  if ((rc = clock_gettime(XlatClock(clockid),
                          BeginStoreNp(m, ts, sizeof(tsb), tsp, tsb))) != -1) {
    EndStoreNp(m, ts, sizeof(tsb), tsp, tsb);
  }
  return rc;
}

static int OpGettimeofday(struct Machine *m, int64_t tv, int64_t tz) {
  int rc;
  void *tvp[2], *tzp[2];
  uint8_t tvb[sizeof(struct timeval)];
  uint8_t tzb[sizeof(struct timezone)];
  if ((rc = gettimeofday(BeginStoreNp(m, tv, sizeof(tvb), tvp, tvb),
                         BeginStoreNp(m, tz, sizeof(tzb), tzp, tzb))) != -1) {
    EndStoreNp(m, tv, sizeof(tvb), tvp, tvb);
    EndStoreNp(m, tz, sizeof(tzb), tzp, tzb);
  }
  return rc;
}

static int OpPoll(struct Machine *m, int64_t fdsaddr, uint64_t nfds,
                  int32_t timeout_ms) {
  int count, i;
  uint64_t fdssize;
  struct pollfd *hostfds, *guestfds;
  if (!__builtin_mul_overflow(nfds, sizeof(struct pollfd), &fdssize) &&
      fdssize <= 0x7ffff000) {
    hostfds = malloc(fdssize);
    guestfds = malloc(fdssize);
    if (hostfds && guestfds) {
      VirtualSendRead(m, guestfds, fdsaddr, fdssize);
      memcpy(hostfds, guestfds, fdssize);
      for (i = 0; i < nfds; ++i) {
        hostfds[i].fd = XlatFd(m, hostfds[i].fd);
      }
      if ((count = poll(hostfds, nfds, timeout_ms)) != -1) {
        for (i = 0; i < count; ++i) {
          hostfds[i].fd = guestfds[i].fd;
        }
        VirtualRecvWrite(m, fdsaddr, hostfds, count * sizeof(struct pollfd));
      }
    } else {
      count = enomem();
    }
    free(guestfds);
    free(hostfds);
  } else {
    count = einval();
  }
  return count;
}

static int OpSigprocmask(struct Machine *m, int how, int64_t setaddr,
                         int64_t oldsetaddr) {
  int rc;
  sigset_t *set, oldset, ss;
  if (setaddr) {
    set = &ss;
    memset(set, 0, sizeof(ss));
    VirtualSendRead(m, set, setaddr, 8);
  } else {
    set = NULL;
  }
  if ((rc = sigprocmask(XlatSig(how), set, &oldset)) != -1) {
    if (setaddr) VirtualRecvWrite(m, setaddr, set, 8);
    if (oldsetaddr) VirtualRecvWrite(m, oldsetaddr, &oldset, 8);
  }
  return rc;
}

static int OpGetPid(struct Machine *m) {
  return getpid();
}

static int OpGetPpid(struct Machine *m) {
  return getppid();
}

static int OpKill(struct Machine *m, int pid, int sig) {
  if (pid == getpid()) {
    ThrowProtectionFault(m);
  } else {
    return kill(pid, sig);
  }
}

static int OpGetUid(struct Machine *m) {
  return getuid();
}

static int OpGetGid(struct Machine *m) {
  return getgid();
}

static int OpGetTid(struct Machine *m) {
  return gettid();
}

static int OpSchedYield(struct Machine *m) {
  return sched_yield();
}

static int OpAlarm(struct Machine *m, unsigned seconds) {
  return alarm(seconds);
}

static int OpPause(struct Machine *m) {
  return pause();
}

static int DoOpen(struct Machine *m, int64_t path, int flags, int mode) {
  return OpOpenat(m, AT_FDCWD_LINUX, path, flags, mode);
}

static int DoCreat(struct Machine *m, int64_t file, int mode) {
  return DoOpen(m, file, 0x241, mode);
}

static int DoAccess(struct Machine *m, int64_t path, int mode) {
  return OpFaccessat(m, AT_FDCWD_LINUX, path, mode, 0);
}

static int DoStat(struct Machine *m, int64_t path, int64_t st) {
  return OpFstatat(m, AT_FDCWD_LINUX, path, st, 0);
}

static int DoLstat(struct Machine *m, int64_t path, int64_t st) {
  return OpFstatat(m, AT_FDCWD_LINUX, path, st, 0x0400);
}

static int DoAccept(struct Machine *m, int fd, int64_t addraddr,
                    int64_t addrsizeaddr) {
  return OpAccept4(m, fd, addraddr, addrsizeaddr, 0);
}

void OpSyscall(struct Machine *m, uint32_t rde) {
  uint64_t i, ax, di, si, dx, r0, r8, r9;
  ax = Read64(m->ax);
  di = Read64(m->di);
  si = Read64(m->si);
  dx = Read64(m->dx);
  r0 = Read64(m->r10);
  r8 = Read64(m->r8);
  r9 = Read64(m->r9);
  switch (ax & 0x1ff) {
    SYSCALL(0x000, OpRead(m, di, si, dx));
    SYSCALL(0x001, OpWrite(m, di, si, dx));
    SYSCALL(0x002, DoOpen(m, di, si, dx));
    SYSCALL(0x003, OpClose(m, di));
    SYSCALL(0x004, DoStat(m, di, si));
    SYSCALL(0x005, OpFstat(m, di, si));
    SYSCALL(0x006, DoLstat(m, di, si));
    SYSCALL(0x007, OpPoll(m, di, si, dx));
    SYSCALL(0x008, OpLseek(m, di, si, dx));
    SYSCALL(0x009, OpMmap(m, di, si, dx, r0, r8, r9));
    SYSCALL(0x01A, OpMsync(m, di, si, dx));
    SYSCALL(0x00A, OpMprotect(m, di, si, dx));
    SYSCALL(0x00B, OpMunmap(m, di, si));
    SYSCALL(0x00C, OpBrk(m, di));
    SYSCALL(0x00D, OpSigaction(m, di, si, dx));
    SYSCALL(0x00E, OpSigprocmask(m, di, si, dx));
    SYSCALL(0x010, OpIoctl(m, di, si, dx));
    SYSCALL(0x011, OpPread(m, di, si, dx, r0));
    SYSCALL(0x012, OpPwrite(m, di, si, dx, r0));
    SYSCALL(0x013, OpReadv(m, di, si, dx));
    SYSCALL(0x014, OpWritev(m, di, si, dx));
    SYSCALL(0x015, DoAccess(m, di, si));
    SYSCALL(0x016, OpPipe(m, di));
    SYSCALL(0x017, select(di, P(si), P(dx), P(r0), P(r8)));
    SYSCALL(0x018, OpSchedYield(m));
    SYSCALL(0x01C, OpMadvise(m, di, si, dx));
    SYSCALL(0x020, OpDup(m, di));
    SYSCALL(0x021, OpDup2(m, di, si));
    SYSCALL(0x022, OpPause(m));
    SYSCALL(0x023, OpNanosleep(m, di, si));
    SYSCALL(0x024, getitimer(di, PNN(si)));
    SYSCALL(0x025, OpAlarm(m, di));
    SYSCALL(0x026, setitimer(di, PNN(si), P(dx)));
    SYSCALL(0x027, OpGetPid(m));
    SYSCALL(0x028, sendfile(di, si, P(dx), r0));
    SYSCALL(0x029, OpSocket(m, di, si, dx));
    SYSCALL(0x02A, OpConnect(m, di, si, dx));
    SYSCALL(0x02B, DoAccept(m, di, di, dx));
    SYSCALL(0x02C, sendto(di, PNN(si), dx, r0, P(r8), r9));
    SYSCALL(0x02D, recvfrom(di, P(si), dx, r0, P(r8), P(r9)));
    SYSCALL(0x030, OpShutdown(m, di, si));
    SYSCALL(0x031, OpBind(m, di, si, dx));
    SYSCALL(0x032, OpListen(m, di, si));
    SYSCALL(0x033, getsockname(di, PNN(si), PNN(dx)));
    SYSCALL(0x034, getpeername(di, PNN(si), PNN(dx)));
    SYSCALL(0x036, OpSetsockopt(m, di, si, dx, r0, r8));
    SYSCALL(0x037, getsockopt(di, si, dx, PNN(r0), PNN(r8)));
    SYSCALL(0x039, OpFork(m));
    SYSCALL(0x03B, OpExecve(m, di, si, dx));
    SYSCALL(0x03D, OpWait4(m, di, si, dx, r0));
    SYSCALL(0x03E, OpKill(m, di, si));
    SYSCALL(0x03F, uname(P(di)));
    SYSCALL(0x048, OpFcntl(m, di, si, dx));
    SYSCALL(0x049, OpFlock(m, di, si));
    SYSCALL(0x04A, OpFsync(m, di));
    SYSCALL(0x04B, OpFdatasync(m, di));
    SYSCALL(0x04C, OpTruncate(m, di, si));
    SYSCALL(0x04D, OpFtruncate(m, di, si));
    SYSCALL(0x04F, OpGetcwd(m, di, si));
    SYSCALL(0x050, OpChdir(m, di));
    SYSCALL(0x052, OpRename(m, di, si));
    SYSCALL(0x053, OpMkdir(m, di, si));
    SYSCALL(0x054, OpRmdir(m, di));
    SYSCALL(0x055, DoCreat(m, di, si));
    SYSCALL(0x056, OpLink(m, di, si));
    SYSCALL(0x057, OpUnlink(m, di));
    SYSCALL(0x058, OpSymlink(m, di, si));
    SYSCALL(0x05A, OpChmod(m, di, si));
    SYSCALL(0x05B, OpFchmod(m, di, si));
    SYSCALL(0x060, OpGettimeofday(m, di, si));
    SYSCALL(0x061, OpGetrlimit(m, di, si));
    SYSCALL(0x062, OpGetrusage(m, di, si));
    SYSCALL(0x063, sysinfo(PNN(di)));
    SYSCALL(0x064, times(PNN(di)));
    SYSCALL(0x066, OpGetUid(m));
    SYSCALL(0x068, OpGetGid(m));
    SYSCALL(0x06E, OpGetPpid(m));
    SYSCALL(0x075, setresuid(di, si, dx));
    SYSCALL(0x077, setresgid(di, si, dx));
    SYSCALL(0x082, OpSigsuspend(m, di));
    SYSCALL(0x085, OpMknod(m, di, si, dx));
    SYSCALL(0x08C, getpriority(di, si));
    SYSCALL(0x08D, setpriority(di, si, dx));
    SYSCALL(0x0A0, setrlimit(di, P(si)));
    SYSCALL(0x084, utime(PNN(di), PNN(si)));
    SYSCALL(0x0EB, utimes(P(di), P(si)));
    SYSCALL(0x09E, OpArchPrctl(m, di, si));
    SYSCALL(0x0BA, OpGetTid(m));
    SYSCALL(0x0CB, sched_setaffinity(di, si, P(dx)));
    SYSCALL(0x0DD, OpFadvise(m, di, si, dx, r0));
    SYSCALL(0x0E4, OpClockGettime(m, di, si));
    SYSCALL(0x101, OpOpenat(m, di, si, dx, r0));
    SYSCALL(0x102, mkdirat(XlatAfd(m, di), P(si), dx));
    SYSCALL(0x104, fchownat(XlatAfd(m, di), P(si), dx, r0, XlatAtf(r8)));
    SYSCALL(0x105, futimesat(XlatAfd(m, di), P(si), P(dx)));
    SYSCALL(0x106, OpFstatat(m, di, si, dx, r0));
    SYSCALL(0x107, unlinkat(XlatAfd(m, di), P(si), XlatAtf(dx)));
    SYSCALL(0x108, renameat(XlatAfd(m, di), P(si), XlatAfd(m, dx), P(r0)));
    SYSCALL(0x10D, OpFaccessat(m, di, si, dx, r0));
    SYSCALL(0x113, splice(di, P(si), dx, P(r0), r8, XlatAtf(r9)));
    SYSCALL(0x115, sync_file_range(di, si, dx, XlatAtf(r0)));
    SYSCALL(0x118, utimensat(XlatAfd(m, di), P(si), P(dx), XlatAtf(r0)));
    SYSCALL(0x120, OpAccept4(m, di, si, dx, r0));
    SYSCALL(0x177, vmsplice(di, P(si), dx, r0));
    CASE(0xE7, HaltMachine(m, di | 0x100));
    default:
      VERBOSEF("missing syscall 0x%03x", ax);
      ax = enosys();
      break;
  }
  Write64(m->ax, ax != -1 ? ax : -(XlatErrno(errno) & 0xfff));
  for (i = 0; i < m->freelist.i; ++i) free(m->freelist.p[i]);
  m->freelist.i = 0;
}
