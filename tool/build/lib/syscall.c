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
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/dirent.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/sysinfo.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/struct/tms.h"
#include "libc/calls/struct/utsname.h"
#include "libc/calls/struct/winsize.h"
#include "libc/calls/weirdtypes.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/vendor.internal.h"
#include "libc/runtime/pc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/select.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
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
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/pr.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/rusage.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/consts/ss.h"
#include "libc/sysv/consts/tcp.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/consts/w.h"
#include "libc/sysv/errfuns.h"
#include "libc/time/struct/timezone.h"
#include "libc/time/struct/utimbuf.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "third_party/mbedtls/endian.h"
#include "tool/build/lib/bits.h"
#include "tool/build/lib/case.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/iovs.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/pml4t.h"
#include "tool/build/lib/signal.h"
#include "tool/build/lib/syscall.h"
#include "tool/build/lib/throw.h"
#include "tool/build/lib/xlat.h"
#include "tool/build/lib/xlaterrno.h"

#define TIOCGWINSZ_LINUX   0x5413
#define TCGETS_LINUX       0x5401
#define TCSETS_LINUX       0x5402
#define TCSETSW_LINUX      0x5403
#define TCSETSF_LINUX      0x5404
#define ARCH_SET_GS_LINUX  0x1001
#define ARCH_SET_FS_LINUX  0x1002
#define ARCH_GET_FS_LINUX  0x1003
#define ARCH_GET_GS_LINUX  0x1004
#define SOCK_CLOEXEC_LINUX 0x080000
#define O_CLOEXEC_LINUX    0x080000
#define SA_RESTORER        0x04000000
#define POLLIN_LINUX       0x01
#define POLLPRI_LINUX      0x02
#define POLLOUT_LINUX      0x04
#define POLLERR_LINUX      0x08
#define POLLHUP_LINUX      0x10
#define POLLNVAL_LINUX     0x20

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
    .poll = poll,
    .writev = writev,
    .ioctl = (void *)ioctl,
};

static int GetFd(struct Machine *m, int fd) {
  if (!(0 <= fd && fd < m->fds.i)) return ebadf();
  if (!m->fds.p[fd].cb) return ebadf();
  return m->fds.p[fd].fd;
}

static int GetAfd(struct Machine *m, int fd) {
  if (fd == -100) return AT_FDCWD;
  return GetFd(m, fd);
}

static const char *GetSimulated(void) {
  if (IsGenuineBlink()) {
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

static int OpPrctl(struct Machine *m, int op, int64_t a, int64_t b, int64_t c,
                   int64_t d) {
  return einval();
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
    if (ReserveVirtual(m, m->brk, addr - m->brk,
                       PAGE_V | PAGE_RW | PAGE_U | PAGE_RSRV) != -1) {
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
  VERBOSEF("MUNMAP%s %012lx %,ld", GetSimulated(), virt, size);
  return FreeVirtual(m, virt, size);
}

static int64_t OpMmap(struct Machine *m, int64_t virt, size_t size, int prot,
                      int flags, int fd, int64_t offset) {
  int e;
  void *tmp;
  ssize_t rc;
  uint64_t key;
  bool recoverable;
  VERBOSEF("MMAP%s %012lx %,ld %#x %#x %d %#lx", GetSimulated(), virt, size,
           prot, flags, fd, offset);
  if (prot & PROT_READ) {
    key = PAGE_RSRV | PAGE_U | PAGE_V;
    if (prot & PROT_WRITE) key |= PAGE_RW;
    if (!(prot & PROT_EXEC)) key |= PAGE_XD;
    if (flags & 256 /* MAP_GROWSDOWN */) key |= PAGE_GROD;
    flags = XlatMapFlags(flags);
    if (fd != -1 && (fd = GetFd(m, fd)) == -1) return -1;
    if (flags & MAP_FIXED) {
      recoverable = false;
    } else {
      recoverable = true;
      if (!virt) {
        if ((virt = FindVirtual(m, m->brk, size)) == -1) return -1;
        m->brk = virt + size;
      } else {
        if ((virt = FindVirtual(m, virt, size)) == -1) return -1;
      }
    }
    if (ReserveVirtual(m, virt, size, key) != -1) {
      if (fd != -1 && !(flags & MAP_ANONYMOUS)) {
        // TODO: lazy file mappings
        CHECK_NOTNULL((tmp = malloc(size)));
        for (e = errno;;) {
          rc = pread(fd, tmp, size, offset);
          if (rc != -1) break;
          CHECK_EQ(EINTR, errno);
          errno = e;
        }
        CHECK_EQ(size, rc);
        VirtualRecvWrite(m, virt, tmp, size);
        free(tmp);
      }
      return virt;
    } else if (recoverable) {
      FreeVirtual(m, virt, size);
      return -1;
    } else {
      FATALF("unrecoverable oom with mmap(MAP_FIXED)");
    }
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
  if ((dirfd = GetAfd(m, dirfd)) == -1) return -1;
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
  if ((fd = GetFd(m, fd)) != -1) {
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
  if ((fd = GetFd(m, fd)) == -1) return -1;
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

static int OpDup3(struct Machine *m, int fd, int newfd, int flags) {
  int i, rc;
  if (!(flags & ~O_CLOEXEC_LINUX)) {
    if ((fd = GetFd(m, fd)) == -1) return -1;
    if ((0 <= newfd && newfd < m->fds.i)) {
      if ((rc = dup2(fd, m->fds.p[newfd].fd)) != -1) {
        if (flags & O_CLOEXEC_LINUX) {
          fcntl(rc, F_SETFD, FD_CLOEXEC);
        }
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
  } else {
    return einval();
  }
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

static int OpAccept4(struct Machine *m, int fd, int64_t aa, int64_t asa,
                     int flags) {
  int i, rc;
  uint32_t addrsize;
  uint8_t gaddrsize[4];
  struct sockaddr_in addr;
  struct sockaddr_in_bits gaddr;
  if (!(flags & ~SOCK_CLOEXEC_LINUX)) {
    if ((fd = GetFd(m, fd)) == -1) return -1;
    if (aa) {
      VirtualSendRead(m, gaddrsize, asa, sizeof(gaddrsize));
      if (Read32(gaddrsize) < sizeof(gaddr)) return einval();
    }
    if ((i = rc = MachineFdAdd(&m->fds)) != -1) {
      addrsize = sizeof(addr);
      if ((rc = accept(fd, (struct sockaddr *)&addr, &addrsize)) != -1) {
        if (flags & SOCK_CLOEXEC_LINUX) {
          fcntl(fd, F_SETFD, FD_CLOEXEC);
        }
        if (aa) {
          Write32(gaddrsize, sizeof(gaddr));
          XlatSockaddrToLinux(&gaddr, &addr);
          VirtualRecv(m, asa, gaddrsize, sizeof(gaddrsize));
          VirtualRecvWrite(m, aa, &gaddr, sizeof(gaddr));
        }
        m->fds.p[i].cb = &kMachineFdCbHost;
        m->fds.p[i].fd = rc;
        rc = i;
      } else {
        MachineFdRemove(&m->fds, i);
      }
    }
    return rc;
  } else {
    return einval();
  }
}

static int OpConnectBind(struct Machine *m, int fd, int64_t aa, uint32_t as,
                         int impl(int, const struct sockaddr *, uint32_t)) {
  struct sockaddr_in addr;
  struct sockaddr_in_bits gaddr;
  if (as != sizeof(gaddr)) return einval();
  if ((fd = GetFd(m, fd)) == -1) return -1;
  VirtualSendRead(m, &gaddr, aa, sizeof(gaddr));
  XlatSockaddrToHost(&addr, &gaddr);
  return impl(fd, (const struct sockaddr *)&addr, sizeof(addr));
}

static int OpBind(struct Machine *m, int fd, int64_t aa, uint32_t as) {
  return OpConnectBind(m, fd, aa, as, bind);
}

static int OpConnect(struct Machine *m, int fd, int64_t aa, uint32_t as) {
  return OpConnectBind(m, fd, aa, as, connect);
}

static int OpSetsockopt(struct Machine *m, int fd, int level, int optname,
                        int64_t optvaladdr, uint32_t optvalsize) {
  int rc;
  void *optval;
  if ((level = XlatSocketLevel(level)) == -1) return -1;
  if ((optname = XlatSocketOptname(level, optname)) == -1) return -1;
  if ((fd = GetFd(m, fd)) == -1) return -1;
  if (!(optval = malloc(optvalsize))) return -1;
  VirtualSendRead(m, optval, optvaladdr, optvalsize);
  rc = setsockopt(fd, level, optname, optval, optvalsize);
  free(optval);
  return rc;
}

static int OpGetsockopt(struct Machine *m, int fd, int level, int optname,
                        int64_t optvaladdr, int64_t optsizeaddr) {
  int rc;
  void *optval;
  uint32_t optsize;
  if ((level = XlatSocketLevel(level)) == -1) return -1;
  if ((optname = XlatSocketOptname(level, optname)) == -1) return -1;
  if ((fd = GetFd(m, fd)) == -1) return -1;
  if (!optvaladdr) {
    rc = getsockopt(fd, level, optname, 0, 0);
  } else {
    VirtualSendRead(m, &optsize, optsizeaddr, sizeof(optsize));
    if (!(optval = malloc(optsize))) return -1;
    if ((rc = getsockopt(fd, level, optname, optval, &optsize)) != -1) {
      VirtualRecvWrite(m, optvaladdr, optval, optsize);
      VirtualRecvWrite(m, optsizeaddr, &optsize, sizeof(optsize));
    }
    free(optval);
  }
  return rc;
}

static int OpSocketName(struct Machine *m, int fd, int64_t aa, int64_t asa,
                        int SocketName(int, struct sockaddr *, socklen_t *)) {
  int rc;
  uint32_t addrsize;
  uint8_t gaddrsize[4];
  struct sockaddr_in addr;
  struct sockaddr_in_bits gaddr;
  if ((fd = GetFd(m, fd)) == -1) return -1;
  VirtualSendRead(m, gaddrsize, asa, sizeof(gaddrsize));
  if (Read32(gaddrsize) < sizeof(gaddr)) return einval();
  addrsize = sizeof(addr);
  if ((rc = SocketName(fd, (struct sockaddr *)&addr, &addrsize)) != -1) {
    Write32(gaddrsize, sizeof(gaddr));
    XlatSockaddrToLinux(&gaddr, &addr);
    VirtualRecv(m, asa, gaddrsize, sizeof(gaddrsize));
    VirtualRecvWrite(m, aa, &gaddr, sizeof(gaddr));
  }
  return rc;
}

static int OpGetsockname(struct Machine *m, int fd, int64_t aa, int64_t asa) {
  return OpSocketName(m, fd, aa, asa, getsockname);
}

static int OpGetpeername(struct Machine *m, int fd, int64_t aa, int64_t asa) {
  return OpSocketName(m, fd, aa, asa, getpeername);
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

static int OpGetdents(struct Machine *m, int dirfd, int64_t addr,
                      uint32_t size) {
  int rc;
  DIR *dir;
  struct dirent *ent;
  if (size < sizeof(struct dirent)) return einval();
  if (0 <= dirfd && dirfd < m->fds.i) {
    if ((dir = fdopendir(m->fds.p[dirfd].fd))) {
      rc = 0;
      while (rc + sizeof(struct dirent) <= size) {
        if (!(ent = readdir(dir))) break;
        VirtualRecvWrite(m, addr + rc, ent, ent->d_reclen);
        rc += ent->d_reclen;
      }
      free(dir);
    } else {
      rc = -1;
    }
  } else {
    rc = ebadf();
  }
  return rc;
}

static ssize_t OpPread(struct Machine *m, int fd, int64_t addr, size_t size,
                       int64_t offset) {
  ssize_t rc;
  struct Iovs iv;
  InitIovs(&iv);
  if ((rc = GetFd(m, fd)) != -1) {
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
        VERBOSEF("write(%d [%d], %012lx, %zu) failed: %s", fd, m->fds.p[fd].fd,
                 addr, size, strerror(errno));
      }
    }
  } else {
    VERBOSEF("write(%d, %012lx, %zu) bad fd", fd, addr, size);
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
  if ((rc = GetFd(m, fd)) != -1) {
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
                           int fn(int, int, ...)) {
  int rc;
  struct winsize ws;
  struct winsize_bits gws;
  if ((rc = fn(fd, TIOCGWINSZ, &ws)) != -1) {
    XlatWinsizeToLinux(&gws, &ws);
    VirtualRecvWrite(m, addr, &gws, sizeof(gws));
  }
  return rc;
}

static int IoctlTcgets(struct Machine *m, int fd, int64_t addr,
                       int fn(int, int, ...)) {
  int rc;
  struct termios tio;
  struct termios_bits gtio;
  if ((rc = fn(fd, TCGETS, &tio)) != -1) {
    XlatTermiosToLinux(&gtio, &tio);
    VirtualRecvWrite(m, addr, &gtio, sizeof(gtio));
  }
  return rc;
}

static int IoctlTcsets(struct Machine *m, int fd, int64_t request, int64_t addr,
                       int (*fn)(int, int, ...)) {
  struct termios tio;
  struct termios_bits gtio;
  VirtualSendRead(m, &gtio, addr, sizeof(gtio));
  XlatLinuxToTermios(&tio, &gtio);
  return fn(fd, request, &tio);
}

static int OpIoctl(struct Machine *m, int fd, uint64_t request, int64_t addr) {
  int (*fn)(int, int, ...);
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
  if ((fd = GetFd(m, fd)) == -1) return -1;
  return lseek(fd, offset, whence);
}

static ssize_t OpFtruncate(struct Machine *m, int fd, int64_t size) {
  if ((fd = GetFd(m, fd)) == -1) return -1;
  return ftruncate(fd, size);
}

static int OpFaccessat(struct Machine *m, int dirfd, int64_t path, int mode,
                       int flags) {
  flags = XlatAtf(flags);
  mode = XlatAccess(mode);
  if ((dirfd = GetAfd(m, dirfd)) == -1) return -1;
  return faccessat(dirfd, LoadStr(m, path), mode, flags);
}

static int OpFstatat(struct Machine *m, int dirfd, int64_t path, int64_t staddr,
                     int flags) {
  int rc;
  struct stat st;
  struct stat_bits gst;
  flags = XlatAtf(flags);
  if ((dirfd = GetAfd(m, dirfd)) == -1) return -1;
  if ((rc = fstatat(dirfd, LoadStr(m, path), &st, flags)) != -1) {
    XlatStatToLinux(&gst, &st);
    VirtualRecvWrite(m, staddr, &gst, sizeof(gst));
  }
  return rc;
}

static int OpFstat(struct Machine *m, int fd, int64_t staddr) {
  int rc;
  struct stat st;
  struct stat_bits gst;
  if ((fd = GetFd(m, fd)) == -1) return -1;
  if ((rc = fstat(fd, &st)) != -1) {
    XlatStatToLinux(&gst, &st);
    VirtualRecvWrite(m, staddr, &gst, sizeof(gst));
  }
  return rc;
}

static int OpListen(struct Machine *m, int fd, int backlog) {
  if ((fd = GetFd(m, fd)) == -1) return -1;
  return listen(fd, backlog);
}

static int OpShutdown(struct Machine *m, int fd, int how) {
  if ((fd = GetFd(m, fd)) == -1) return -1;
  return shutdown(fd, how);
}

static int OpFsync(struct Machine *m, int fd) {
  if ((fd = GetFd(m, fd)) == -1) return -1;
  return fsync(fd);
}

static int OpFdatasync(struct Machine *m, int fd) {
  if ((fd = GetFd(m, fd)) == -1) return -1;
  return fdatasync(fd);
}

static int OpFchmod(struct Machine *m, int fd, uint32_t mode) {
  if ((fd = GetFd(m, fd)) == -1) return -1;
  return fchmod(fd, mode);
}

static int OpFcntl(struct Machine *m, int fd, int cmd, int arg) {
  if ((cmd = XlatFcntlCmd(cmd)) == -1) return -1;
  if ((arg = XlatFcntlArg(arg)) == -1) return -1;
  if ((fd = GetFd(m, fd)) == -1) return -1;
  return fcntl(fd, cmd, arg);
}

static int OpFadvise(struct Machine *m, int fd, uint64_t offset, uint64_t len,
                     int advice) {
  if ((fd = GetFd(m, fd)) == -1) return -1;
  if ((advice = XlatAdvice(advice)) == -1) return -1;
  return fadvise(fd, offset, len, advice);
}

static int OpFlock(struct Machine *m, int fd, int lock) {
  if ((fd = GetFd(m, fd)) == -1) return -1;
  if ((lock = XlatLock(lock)) == -1) return -1;
  return flock(fd, lock);
}

static int OpChdir(struct Machine *m, int64_t path) {
  return chdir(LoadStr(m, path));
}

static int OpMkdir(struct Machine *m, int64_t path, int mode) {
  return mkdir(LoadStr(m, path), mode);
}

static int OpMkdirat(struct Machine *m, int dirfd, int64_t path, int mode) {
  return mkdirat(GetAfd(m, dirfd), LoadStr(m, path), mode);
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

static int OpUnlinkat(struct Machine *m, int dirfd, int64_t path, int flags) {
  return unlinkat(GetAfd(m, dirfd), LoadStr(m, path), XlatAtf(flags));
}

static int OpRename(struct Machine *m, int64_t src, int64_t dst) {
  return rename(LoadStr(m, src), LoadStr(m, dst));
}

static int OpRenameat(struct Machine *m, int srcdirfd, int64_t src,
                      int dstdirfd, int64_t dst) {
  return renameat(GetAfd(m, srcdirfd), LoadStr(m, src), GetAfd(m, dstdirfd),
                  LoadStr(m, dst));
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
  int pid;
  pid = fork();
  if (!pid) m->isfork = true;
  return pid;
}

static int OpExecve(struct Machine *m, int64_t programaddr, int64_t argvaddr,
                    int64_t envpaddr) {
  return enosys();
}

wontreturn static void OpExit(struct Machine *m, int rc) {
  if (m->isfork) {
    _Exit(rc);
  } else {
    HaltMachine(m, rc | 0x100);
  }
}

_Noreturn static void OpExitGroup(struct Machine *m, int rc) {
  OpExit(m, rc);
}

static int OpWait4(struct Machine *m, int pid, int64_t opt_out_wstatus_addr,
                   int options, int64_t opt_out_rusage_addr) {
  int rc;
  int32_t wstatus;
  struct rusage hrusage;
  struct rusage_bits grusage;
  if ((options = XlatWait(options)) == -1) return -1;
  if ((rc = wait4(pid, &wstatus, options, &hrusage)) != -1) {
    if (opt_out_wstatus_addr) {
      VirtualRecvWrite(m, opt_out_wstatus_addr, &wstatus, sizeof(wstatus));
    }
    if (opt_out_rusage_addr) {
      XlatRusageToLinux(&grusage, &hrusage);
      VirtualRecvWrite(m, opt_out_rusage_addr, &grusage, sizeof(grusage));
    }
  }
  return rc;
}

static int OpGetrusage(struct Machine *m, int resource, int64_t rusageaddr) {
  int rc;
  struct rusage hrusage;
  struct rusage_bits grusage;
  if ((resource = XlatRusage(resource)) == -1) return -1;
  if ((rc = getrusage(resource, &hrusage)) != -1) {
    XlatRusageToLinux(&grusage, &hrusage);
    VirtualRecvWrite(m, rusageaddr, &grusage, sizeof(grusage));
  }
  return rc;
}

static int OpGetrlimit(struct Machine *m, int resource, int64_t rlimitaddr) {
  return enosys();
}

static ssize_t OpReadlinkat(struct Machine *m, int dirfd, int64_t pathaddr,
                            int64_t bufaddr, size_t size) {
  char *buf;
  ssize_t rc;
  const char *path;
  if ((dirfd = GetAfd(m, dirfd)) == -1) return -1;
  path = LoadStr(m, pathaddr);
  if (!(buf = malloc(size))) return enomem();
  if ((rc = readlinkat(dirfd, path, buf, size)) != -1) {
    VirtualRecvWrite(m, bufaddr, buf, rc);
  }
  free(buf);
  return rc;
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

static int OpSigaction(struct Machine *m, int sig, int64_t act, int64_t old,
                       uint64_t sigsetsize) {
  if ((sig = XlatSignal(sig) - 1) != -1 &&
      (0 <= sig && sig < ARRAYLEN(m->sighand)) && sigsetsize == 8) {
    if (old) VirtualRecvWrite(m, old, &m->sighand[sig], sizeof(m->sighand[0]));
    if (act) VirtualSendRead(m, &m->sighand[sig], act, sizeof(m->sighand[0]));
    return 0;
  } else {
    return einval();
  }
}

static int OpSigprocmask(struct Machine *m, int how, int64_t setaddr,
                         int64_t oldsetaddr, uint64_t sigsetsize) {
  uint8_t set[8];
  if ((how = XlatSig(how)) != -1 && sigsetsize == sizeof(set)) {
    if (oldsetaddr) {
      VirtualRecvWrite(m, oldsetaddr, m->sigmask, sizeof(set));
    }
    if (setaddr) {
      VirtualSendRead(m, set, setaddr, sizeof(set));
      if (how == SIG_BLOCK) {
        Write64(m->sigmask, Read64(m->sigmask) | Read64(set));
      } else if (how == SIG_UNBLOCK) {
        Write64(m->sigmask, Read64(m->sigmask) & ~Read64(set));
      } else {
        Write64(m->sigmask, Read64(set));
      }
    }
    return 0;
  } else {
    return einval();
  }
}

static int OpGetitimer(struct Machine *m, int which, int64_t curvaladdr) {
  int rc;
  struct itimerval it;
  struct itimerval_bits git;
  if ((rc = getitimer(which, &it)) != -1) {
    XlatItimervalToLinux(&git, &it);
    VirtualRecvWrite(m, curvaladdr, &git, sizeof(git));
  }
  return rc;
}

static int OpSetitimer(struct Machine *m, int which, int64_t neuaddr,
                       int64_t oldaddr) {
  int rc;
  struct itimerval neu, old;
  struct itimerval_bits git;
  VirtualSendRead(m, &git, neuaddr, sizeof(git));
  XlatLinuxToItimerval(&neu, &git);
  if ((rc = setitimer(which, &neu, &old)) != -1) {
    if (oldaddr) {
      XlatItimervalToLinux(&git, &old);
      VirtualRecvWrite(m, oldaddr, &git, sizeof(git));
    }
  }
  return rc;
}

static int OpNanosleep(struct Machine *m, int64_t req, int64_t rem) {
  int rc;
  struct timespec hreq, hrem;
  struct timespec_bits gtimespec;
  if (req) {
    VirtualSendRead(m, &gtimespec, req, sizeof(gtimespec));
    hreq.tv_sec = Read64(gtimespec.tv_sec);
    hreq.tv_nsec = Read64(gtimespec.tv_nsec);
  }
  if ((rc = nanosleep(req ? &hreq : 0, rem ? &hrem : 0)) != -1) {
    if (rem) {
      Write64(gtimespec.tv_sec, hrem.tv_sec);
      Write64(gtimespec.tv_nsec, hrem.tv_nsec);
      VirtualRecvWrite(m, rem, &gtimespec, sizeof(gtimespec));
    }
  }
  return rc;
}

static int OpSigsuspend(struct Machine *m, int64_t maskaddr) {
  sigset_t mask;
  uint8_t gmask[8];
  VirtualSendRead(m, &gmask, maskaddr, 8);
  XlatLinuxToSigset(&mask, gmask);
  return sigsuspend(&mask);
}

static int OpClockGettime(struct Machine *m, int clockid, int64_t ts) {
  int rc;
  struct timespec htimespec;
  struct timespec_bits gtimespec;
  if ((rc = clock_gettime(XlatClock(clockid), ts ? &htimespec : 0)) != -1) {
    if (ts) {
      Write64(gtimespec.tv_sec, htimespec.tv_sec);
      Write64(gtimespec.tv_nsec, htimespec.tv_nsec);
      VirtualRecvWrite(m, ts, &gtimespec, sizeof(gtimespec));
    }
  }
  return rc;
}

static int OpGettimeofday(struct Machine *m, int64_t tv, int64_t tz) {
  int rc;
  struct timeval htimeval;
  struct timezone htimezone;
  struct timeval_bits gtimeval;
  struct timezone_bits gtimezone;
  if ((rc = gettimeofday(&htimeval, tz ? &htimezone : 0)) != -1) {
    Write64(gtimeval.tv_sec, htimeval.tv_sec);
    Write64(gtimeval.tv_usec, htimeval.tv_usec);
    VirtualRecvWrite(m, tv, &gtimeval, sizeof(gtimeval));
    if (tz) {
      Write32(gtimezone.tz_minuteswest, htimezone.tz_minuteswest);
      Write32(gtimezone.tz_dsttime, htimezone.tz_dsttime);
      VirtualRecvWrite(m, tz, &gtimezone, sizeof(gtimezone));
    }
  }
  return rc;
}

static int OpUtimes(struct Machine *m, int64_t pathaddr, int64_t tvsaddr) {
  const char *path;
  struct timeval tvs[2];
  struct timeval_bits gtvs[2];
  path = LoadStr(m, pathaddr);
  if (tvsaddr) {
    VirtualSendRead(m, gtvs, tvsaddr, sizeof(gtvs));
    tvs[0].tv_sec = Read64(gtvs[0].tv_sec);
    tvs[0].tv_usec = Read64(gtvs[0].tv_usec);
    tvs[1].tv_sec = Read64(gtvs[1].tv_sec);
    tvs[1].tv_usec = Read64(gtvs[1].tv_usec);
    return utimes(path, tvs);
  } else {
    return utimes(path, 0);
  }
}

static int OpPoll(struct Machine *m, int64_t fdsaddr, uint64_t nfds,
                  int32_t timeout_ms) {
  uint64_t gfdssize;
  struct pollfd hfds[1];
  int i, fd, rc, ev, count;
  struct timespec ts1, ts2;
  struct pollfd_bits *gfds;
  int64_t wait, elapsed, timeout;
  timeout = timeout_ms * 1000L;
  if (!__builtin_mul_overflow(nfds, sizeof(struct pollfd_bits), &gfdssize) &&
      gfdssize <= 0x7ffff000) {
    if ((gfds = malloc(gfdssize))) {
      rc = 0;
      VirtualSendRead(m, gfds, fdsaddr, gfdssize);
      ts1 = timespec_mono();
      for (;;) {
        for (i = 0; i < nfds; ++i) {
          fd = Read32(gfds[i].fd);
          if ((0 <= fd && fd < m->fds.i) && m->fds.p[fd].cb) {
            hfds[0].fd = m->fds.p[fd].fd;
            ev = Read16(gfds[i].events);
            hfds[0].events = (((ev & POLLIN_LINUX) ? POLLIN : 0) |
                              ((ev & POLLOUT_LINUX) ? POLLOUT : 0) |
                              ((ev & POLLPRI_LINUX) ? POLLPRI : 0));
            switch (m->fds.p[fd].cb->poll(hfds, 1, 0)) {
              case 0:
                Write16(gfds[i].revents, 0);
                break;
              case 1:
                ++rc;
                ev = 0;
                if (hfds[0].revents & POLLIN) ev |= POLLIN_LINUX;
                if (hfds[0].revents & POLLPRI) ev |= POLLPRI_LINUX;
                if (hfds[0].revents & POLLOUT) ev |= POLLOUT_LINUX;
                if (hfds[0].revents & POLLERR) ev |= POLLERR_LINUX;
                if (hfds[0].revents & POLLHUP) ev |= POLLHUP_LINUX;
                if (hfds[0].revents & POLLNVAL) ev |= POLLERR_LINUX;
                if (!ev) ev |= POLLERR_LINUX;
                Write16(gfds[i].revents, ev);
                break;
              case -1:
                ++rc;
                Write16(gfds[i].revents, POLLERR_LINUX);
                break;
              default:
                break;
            }
          } else {
            Write16(gfds[i].revents, POLLNVAL_LINUX);
          }
        }
        if (rc || !timeout) break;
        wait = __SIG_POLLING_INTERVAL_MS * 1000;
        if (timeout < 0) {
          if (usleep(wait)) {
            errno = EINTR;
            rc = -1;
            goto Finished;
          }
        } else {
          ts2 = timespec_mono();
          elapsed = timespec_tomicros(timespec_sub(ts2, ts1));
          if (elapsed >= timeout) {
            break;
          }
          if (timeout - elapsed < wait) {
            wait = timeout - elapsed;
          }
          if (usleep(wait)) {
            errno = EINTR;
            rc = -1;
            goto Finished;
          }
        }
      }
      VirtualRecvWrite(m, fdsaddr, gfds, nfds * sizeof(*gfds));
    } else {
      errno = ENOMEM;
      rc = -1;
    }
  Finished:
    free(gfds);
    return rc;
  } else {
    return einval();
  }
}

static int OpGetPid(struct Machine *m) {
  return getpid();
}

static int OpGetPpid(struct Machine *m) {
  return getppid();
}

static void OpKill(struct Machine *m, int pid, int sig) {
  if (!pid || pid == getpid()) {
    DeliverSignal(m, sig, SI_USER);
  } else {
    Write64(m->ax, -3);  // ESRCH
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
  return OpOpenat(m, -100, path, flags, mode);
}

static int DoCreat(struct Machine *m, int64_t file, int mode) {
  return DoOpen(m, file, 0x241, mode);
}

static int DoAccess(struct Machine *m, int64_t path, int mode) {
  return OpFaccessat(m, -100, path, mode, 0);
}

static int DoStat(struct Machine *m, int64_t path, int64_t st) {
  return OpFstatat(m, -100, path, st, 0);
}

static int DoLstat(struct Machine *m, int64_t path, int64_t st) {
  return OpFstatat(m, -100, path, st, 0x0400);
}

static int OpAccept(struct Machine *m, int fd, int64_t sa, int64_t sas) {
  return OpAccept4(m, fd, sa, sas, 0);
}

void OpSyscall(struct Machine *m, uint32_t rde) {
  uint64_t i, ax, di, si, dx, r0, r8, r9;
  if (m->redraw) {
    m->redraw();
  }
  ax = Read64(m->ax);
  if (m->ismetal) {
    WARNF("metal syscall 0x%03x", ax);
  }
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
    SYSCALL(0x00D, OpSigaction(m, di, si, dx, r0));
    SYSCALL(0x00E, OpSigprocmask(m, di, si, dx, r0));
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
    SYSCALL(0x124, OpDup3(m, di, si, dx));
    SYSCALL(0x022, OpPause(m));
    SYSCALL(0x023, OpNanosleep(m, di, si));
    SYSCALL(0x024, OpGetitimer(m, di, si));
    SYSCALL(0x025, OpAlarm(m, di));
    SYSCALL(0x026, OpSetitimer(m, di, si, dx));
    SYSCALL(0x027, OpGetPid(m));
    SYSCALL(0x028, sendfile(di, si, P(dx), r0));
    SYSCALL(0x029, OpSocket(m, di, si, dx));
    SYSCALL(0x02A, OpConnect(m, di, si, dx));
    SYSCALL(0x02B, OpAccept(m, di, di, dx));
    SYSCALL(0x02C, sendto(di, PNN(si), dx, r0, P(r8), r9));
    SYSCALL(0x02D, recvfrom(di, P(si), dx, r0, P(r8), P(r9)));
    SYSCALL(0x030, OpShutdown(m, di, si));
    SYSCALL(0x031, OpBind(m, di, si, dx));
    SYSCALL(0x032, OpListen(m, di, si));
    SYSCALL(0x033, OpGetsockname(m, di, si, dx));
    SYSCALL(0x034, OpGetpeername(m, di, si, dx));
    SYSCALL(0x036, OpSetsockopt(m, di, si, dx, r0, r8));
    SYSCALL(0x037, OpGetsockopt(m, di, si, dx, r0, r8));
    SYSCALL(0x039, OpFork(m));
    SYSCALL(0x03B, OpExecve(m, di, si, dx));
    SYSCALL(0x03D, OpWait4(m, di, si, dx, r0));
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
    SYSCALL(0x0EB, OpUtimes(m, di, si));
    SYSCALL(0x09D, OpPrctl(m, di, si, dx, r0, r8));
    SYSCALL(0x09E, OpArchPrctl(m, di, si));
    SYSCALL(0x0BA, OpGetTid(m));
    SYSCALL(0x0D9, OpGetdents(m, di, si, dx));
    SYSCALL(0x0DD, OpFadvise(m, di, si, dx, r0));
    SYSCALL(0x0E4, OpClockGettime(m, di, si));
    SYSCALL(0x101, OpOpenat(m, di, si, dx, r0));
    SYSCALL(0x102, OpMkdirat(m, di, si, dx));
    SYSCALL(0x104, fchownat(GetAfd(m, di), P(si), dx, r0, XlatAtf(r8)));
    SYSCALL(0x105, futimesat(GetAfd(m, di), P(si), P(dx)));
    SYSCALL(0x106, OpFstatat(m, di, si, dx, r0));
    SYSCALL(0x107, OpUnlinkat(m, di, si, dx));
    SYSCALL(0x108, OpRenameat(m, di, si, dx, r0));
    SYSCALL(0x10B, OpReadlinkat(m, di, si, dx, r0));
    SYSCALL(0x10D, OpFaccessat(m, di, si, dx, r0));
    SYSCALL(0x113, splice(di, P(si), dx, P(r0), r8, XlatAtf(r9)));
    SYSCALL(0x115, sync_file_range(di, si, dx, XlatAtf(r0)));
    SYSCALL(0x118, utimensat(GetAfd(m, di), P(si), P(dx), XlatAtf(r0)));
    SYSCALL(0x120, OpAccept4(m, di, si, dx, r0));
    SYSCALL(0x177, vmsplice(di, P(si), dx, r0));
    case 0x3C:
      OpExit(m, di);
    case 0xE7:
      OpExitGroup(m, di);
    case 0x00F:
      OpRestore(m);
      return;
    case 0x03E:
      OpKill(m, di, si);
      return;
    default:
      WARNF("missing syscall 0x%03x", ax);
      ax = enosys();
      break;
  }
  Write64(m->ax, ax != -1 ? ax : -(XlatErrno(errno) & 0xfff));
  for (i = 0; i < m->freelist.i; ++i) free(m->freelist.p[i]);
  m->freelist.i = 0;
}
