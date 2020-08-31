/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/ioctl.h"
#include "libc/calls/struct/sigaction-linux.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/errno.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/msync.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/tcp.h"
#include "libc/sysv/errfuns.h"
#include "libc/time/struct/timezone.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "tool/build/lib/case.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/pml4t.h"
#include "tool/build/lib/syscall.h"
#include "tool/build/lib/throw.h"
#include "tool/build/lib/xlaterrno.h"

#define AT_FDCWD_LINUX -100

#define POINTER(x)    ((void *)(intptr_t)(x))
#define UNPOINTER(x)  ((int64_t)(intptr_t)(x))
#define SYSCALL(x, y) CASE(x, asm("# " #y); ax = y)
#define XLAT(x, y)    CASE(x, return y)
#define PNN(x)        ResolveAddress(m, x)
#define P(x)          ((x) ? PNN(x) : 0)
#define ASSIGN(D, S)  memcpy(&D, &S, MIN(sizeof(S), sizeof(D)))

const struct MachineFdCb kMachineFdCbHost = {
    .close = close,
    .read = read,
    .write = write,
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
      return sig;
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
  if (x & 0x04000000) res |= SA_RESTORER;
  if (x & 0x08000000) res |= SA_ONSTACK;
  if (x & 0x10000000) res |= SA_RESTART;
  if (x & 1) res |= SA_NOCLDSTOP;
  if (x & 2) res |= SA_NOCLDWAIT;
  if (x & 4) res |= SA_SIGINFO;
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
    XLAT(23, TCP_FASTOPEN);
    XLAT(4, TCP_KEEPIDLE);
    XLAT(5, TCP_KEEPINTVL);
    XLAT(6, TCP_KEEPCNT);
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

static unsigned XlatOpenFlags(unsigned flags) {
  unsigned res = 0;
  if ((flags & 3) == 0) res = O_RDONLY;
  if ((flags & 3) == 1) res = O_WRONLY;
  if ((flags & 3) == 3) res = O_RDWR;
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

static int OpMprotect(struct Machine *m, int64_t addr, uint64_t len, int prot) {
  return 0;
}

static int OpMadvise(struct Machine *m, int64_t addr, size_t length,
                     int advice) {
  return enosys();
}

static int64_t OpMmap(struct Machine *m, int64_t virt, size_t size, int prot,
                      int flags, int fd, int64_t off) {
  void *real;
  flags = XlatMapFlags(flags);
  if (fd != -1 && (fd = XlatFd(m, fd)) == -1) return -1;
  real = mmap(NULL, size, prot, flags & ~MAP_FIXED, fd, off);
  if (real == MAP_FAILED) return -1;
  if (!(flags & MAP_FIXED)) {
    if (0 <= virt && virt < 0x400000) virt = 0x400000;
    if ((virt = FindPml4t(m->cr3, virt, size)) == -1) return -1;
  }
  CHECK_NE(-1, RegisterMemory(m, virt, real, size));
  return virt;
}

static int OpMunmap(struct Machine *m, int64_t addr, uint64_t size) {
  return FreePml4t(m->cr3, addr, size, free, munmap);
}

static int OpMsync(struct Machine *m, int64_t virt, size_t size, int flags) {
  size_t i;
  void *page;
  virt = ROUNDDOWN(virt, 4096);
  flags = XlatMsyncFlags(flags);
  for (i = 0; i < size; i += 4096) {
    if (!(page = FindReal(m, virt + i))) return efault();
    if (msync(page, 4096, flags) == -1) return -1;
  }
  return 0;
}

static void *GetDirectBuf(struct Machine *m, int64_t addr, size_t *size) {
  void *page;
  *size = MIN(*size, 0x1000 - (addr & 0xfff));
  if (!(page = FindReal(m, addr))) return MAP_FAILED;
  return page;
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

static int OpOpenat(struct Machine *m, int dirfd, int64_t path, int flags,
                    int mode) {
  int fd, i;
  flags = XlatOpenFlags(flags);
  if ((dirfd = XlatAfd(m, dirfd)) == -1) return -1;
  if ((i = MachineFdAdd(&m->fds)) == -1) return -1;
  if ((fd = openat(dirfd, LoadStr(m, path), flags, mode)) != -1) {
    m->fds.p[i].cb = &kMachineFdCbHost;
    m->fds.p[i].fd = fd;
    fd = i;
  } else {
    MachineFdRemove(&m->fds, i);
  }
  return fd;
}

static int OpPipe(struct Machine *m, int64_t pipefds_addr) {
  void *p[2];
  uint8_t b[8];
  int rc, i, j, *pipefds;
  if ((i = MachineFdAdd(&m->fds)) == -1) return -1;
  if ((j = MachineFdAdd(&m->fds)) == -1) return -1;
  if ((rc = pipe((pipefds = BeginStoreNp(m, pipefds_addr, 8, p, b)))) != -1) {
    EndStoreNp(m, pipefds_addr, 8, p, b);
    m->fds.p[i].cb = &kMachineFdCbHost;
    m->fds.p[i].fd = pipefds[0];
    m->fds.p[j].cb = &kMachineFdCbHost;
    m->fds.p[j].fd = pipefds[1];
  } else {
    MachineFdRemove(&m->fds, i);
    MachineFdRemove(&m->fds, j);
  }
  return rc;
}

static ssize_t OpRead(struct Machine *m, int fd, int64_t addr, size_t size) {
  void *data;
  ssize_t rc;
  if (!(0 <= fd && fd < m->fds.i) || !m->fds.p[fd].cb) return ebadf();
  if ((data = GetDirectBuf(m, addr, &size)) == MAP_FAILED) return efault();
  if ((rc = m->fds.p[fd].cb->read(m->fds.p[fd].fd, data, size)) != -1) {
    SetWriteAddr(m, addr, rc);
  }
  return rc;
}

static ssize_t OpWrite(struct Machine *m, int fd, int64_t addr, size_t size) {
  void *data;
  ssize_t rc;
  if (!(0 <= fd && fd < m->fds.i) || !m->fds.p[fd].cb) return ebadf();
  if ((data = GetDirectBuf(m, addr, &size)) == MAP_FAILED) return efault();
  if ((rc = m->fds.p[fd].cb->write(m->fds.p[fd].fd, data, size)) != -1) {
    SetReadAddr(m, addr, size);
  }
  return rc;
}

static ssize_t OpPread(struct Machine *m, int fd, int64_t addr, size_t size,
                       int64_t offset) {
  void *data;
  ssize_t rc;
  if ((fd = XlatFd(m, fd)) == -1) return -1;
  if ((data = GetDirectBuf(m, addr, &size)) == MAP_FAILED) return efault();
  if ((rc = pread(fd, data, size, offset)) != -1) SetWriteAddr(m, addr, rc);
  return rc;
}

static ssize_t OpPwrite(struct Machine *m, int fd, int64_t addr, size_t size,
                        int64_t offset) {
  void *data;
  ssize_t rc;
  if ((fd = XlatFd(m, fd)) == -1) return -1;
  if ((data = GetDirectBuf(m, addr, &size)) == MAP_FAILED) return efault();
  if ((rc = pwrite(fd, data, size, offset)) != -1) SetReadAddr(m, addr, size);
  return rc;
}

static int OpFaccessat(struct Machine *m, int dirfd, int64_t path, int mode,
                       int flags) {
  flags = XlatAtf(flags);
  mode = XlatAccess(mode);
  if ((dirfd = XlatAfd(m, dirfd)) == -1) return -1;
  return faccessat(dirfd, LoadStr(m, path), mode, flags);
}

static int OpFstatat(struct Machine *m, int dirfd, int64_t path, int64_t st,
                     int flags) {
  int rc;
  void *stp[2];
  uint8_t *stbuf;
  flags = XlatAtf(flags);
  if ((dirfd = XlatAfd(m, dirfd)) == -1) return -1;
  if (!(stbuf = malloc(sizeof(struct stat)))) return enomem();
  if ((rc = fstatat(dirfd, LoadStr(m, path),
                    BeginStoreNp(m, st, sizeof(stbuf), stp, stbuf), flags)) !=
      -1) {
    EndStoreNp(m, st, sizeof(stbuf), stp, stbuf);
  }
  free(stbuf);
  return rc;
}

static int OpFstat(struct Machine *m, int fd, int64_t st) {
  int rc;
  void *stp[2];
  uint8_t *stbuf;
  if ((fd = XlatFd(m, fd)) == -1) return -1;
  if (!(stbuf = malloc(sizeof(struct stat)))) return enomem();
  if ((rc = fstat(fd, BeginStoreNp(m, st, sizeof(stbuf), stp, stbuf))) != -1) {
    EndStoreNp(m, st, sizeof(stbuf), stp, stbuf);
  }
  free(stbuf);
  return rc;
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

static int64_t OpGetcwd(struct Machine *m, int64_t bufaddr, size_t size) {
  size_t n;
  char *buf;
  int64_t res;
  size = MIN(size, PATH_MAX);
  if (!(buf = malloc(size))) return enomem();
  if ((getcwd)(buf, size)) {
    n = strlen(buf);
    VirtualRecv(m, bufaddr, buf, n);
    SetWriteAddr(m, bufaddr, n);
    res = bufaddr;
  } else {
    res = -1;
  }
  free(buf);
  return res;
}

static int OpSigaction(struct Machine *m, int sig, int64_t act, int64_t old) {
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

void OpSyscall(struct Machine *m) {
  uint64_t i, ax, di, si, dx, r0, r8, r9;
  ax = Read64(m->ax);
  di = Read64(m->di);
  si = Read64(m->si);
  dx = Read64(m->dx);
  r0 = Read32(m->r10);
  r8 = Read32(m->r8);
  r9 = Read32(m->r9);
  switch (ax & 0x1ff) {
    SYSCALL(0x000, OpRead(m, di, si, dx));
    SYSCALL(0x001, OpWrite(m, di, si, dx));
    SYSCALL(0x002, DoOpen(m, di, si, dx));
    SYSCALL(0x003, OpClose(m, di));
    SYSCALL(0x004, DoStat(m, di, si));
    SYSCALL(0x005, OpFstat(m, di, si));
    SYSCALL(0x006, DoLstat(m, di, si));
    SYSCALL(0x007, poll(PNN(di), si, dx));
    SYSCALL(0x008, lseek(di, si, dx));
    SYSCALL(0x009, OpMmap(m, di, si, dx, r0, r8, r9));
    SYSCALL(0x01A, OpMsync(m, di, si, dx));
    SYSCALL(0x00A, OpMprotect(m, di, si, dx));
    SYSCALL(0x00B, OpMunmap(m, di, si));
    SYSCALL(0x00D, OpSigaction(m, di, si, dx));
    SYSCALL(0x00E, sigprocmask(di, P(si), P(dx)));
    SYSCALL(0x010, ioctl(di, si, P(dx)));
    SYSCALL(0x011, OpPread(m, di, si, dx, r0));
    SYSCALL(0x012, OpPwrite(m, di, si, dx, r0));
    SYSCALL(0x013, readv(di, P(si), dx));
    SYSCALL(0x014, writev(di, P(si), dx));
    SYSCALL(0x015, DoAccess(m, di, si));
    SYSCALL(0x016, OpPipe(m, di));
    SYSCALL(0x017, select(di, P(si), P(dx), P(r0), P(r8)));
    SYSCALL(0x018, sched_yield());
    SYSCALL(0x01C, OpMadvise(m, di, si, dx));
    SYSCALL(0x020, dup(di));
    SYSCALL(0x021, dup2(di, si));
    SYSCALL(0x022, pause());
    SYSCALL(0x023, OpNanosleep(m, di, si));
    SYSCALL(0x024, getitimer(di, PNN(si)));
    SYSCALL(0x025, alarm(di));
    SYSCALL(0x026, setitimer(di, PNN(si), P(dx)));
    SYSCALL(0x027, getpid());
    SYSCALL(0x028, sendfile(di, si, P(dx), r0));
    SYSCALL(0x029, socket(di, si, dx));
    SYSCALL(0x02A, connect(di, PNN(si), dx));
    SYSCALL(0x02B, accept(di, PNN(di), PNN(dx)));
    SYSCALL(0x02C, sendto(di, PNN(si), dx, r0, P(r8), r9));
    SYSCALL(0x02D, recvfrom(di, P(si), dx, r0, P(r8), P(r9)));
    SYSCALL(0x030, shutdown(di, si));
    SYSCALL(0x031, bind(di, PNN(si), dx));
    SYSCALL(0x032, listen(di, si));
    SYSCALL(0x033, getsockname(di, PNN(si), PNN(dx)));
    SYSCALL(0x034, getpeername(di, PNN(si), PNN(dx)));
    SYSCALL(0x036, setsockopt(di, si, dx, PNN(r0), r8));
    SYSCALL(0x037, getsockopt(di, si, dx, PNN(r0), PNN(r8)));
    SYSCALL(0x039, fork());
    SYSCALL(0x03B, execve(PNN(r8), PNN(r8), PNN(r8)));
    SYSCALL(0x03D, wait4(di, P(si), dx, P(r0)));
    SYSCALL(0x03E, kill(di, si));
    SYSCALL(0x03F, uname(P(di)));
    SYSCALL(0x048, fcntl(di, si, dx));
    SYSCALL(0x049, flock(di, si));
    SYSCALL(0x04A, fsync(di));
    SYSCALL(0x04B, fdatasync(di));
    SYSCALL(0x04C, OpTruncate(m, di, si));
    SYSCALL(0x04D, ftruncate(di, si));
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
    SYSCALL(0x05B, fchmod(di, si));
    SYSCALL(0x060, OpGettimeofday(m, di, si));
    SYSCALL(0x061, getrlimit(di, P(si)));
    SYSCALL(0x062, getrusage(di, P(si)));
    SYSCALL(0x063, sysinfo(PNN(di)));
    SYSCALL(0x064, times(PNN(di)));
    SYSCALL(0x066, getuid());
    SYSCALL(0x068, getgid());
    SYSCALL(0x06E, getppid());
    SYSCALL(0x075, setresuid(di, si, dx));
    SYSCALL(0x077, setresgid(di, si, dx));
    SYSCALL(0x082, OpSigsuspend(m, di));
    SYSCALL(0x085, OpMknod(m, di, si, dx));
    SYSCALL(0x08C, getpriority(di, si));
    SYSCALL(0x08D, setpriority(di, si, dx));
    SYSCALL(0x0A0, setrlimit(di, P(si)));
    SYSCALL(0x084, utime(PNN(di), PNN(si)));
    SYSCALL(0x0EB, utimes(P(di), P(si)));
    SYSCALL(0x09E, arch_prctl(di, si));
    SYSCALL(0x0BA, gettid());
    SYSCALL(0x0CB, sched_setaffinity(di, si, P(dx)));
    SYSCALL(0x0DD, fadvise(di, si, dx, r0));
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
    SYSCALL(0x177, vmsplice(di, P(si), dx, r0));
    CASE(0xE7, HaltMachine(m, di | 0x100));
    default:
      DEBUGF("missing syscall %03x", ax);
      ax = enosys();
      break;
  }
  Write64(m->ax, ax != -1 ? ax : -(XlatErrno(errno) & 0xfff));
  for (i = 0; i < m->freelist.i; ++i) free(m->freelist.p[i]);
  m->freelist.i = 0;
}
