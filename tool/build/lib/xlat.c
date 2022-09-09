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
#include "libc/errno.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/ip.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/lock.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/msync.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/consts/rusage.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/consts/tcp.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/consts/w.h"
#include "libc/sysv/errfuns.h"
#include "tool/build/lib/case.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/xlat.h"

#define XLAT(x, y) CASE(x, return y)

int XlatSignal(int x) {
  switch (x) {
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
    XLAT(12, SIGUSR2);
    XLAT(13, SIGPIPE);
    XLAT(14, SIGALRM);
    XLAT(15, SIGTERM);
    XLAT(17, SIGCHLD);
    XLAT(18, SIGCONT);
    XLAT(21, SIGTTIN);
    XLAT(22, SIGTTOU);
    XLAT(24, SIGXCPU);
    XLAT(25, SIGXFSZ);
    XLAT(26, SIGVTALRM);
    XLAT(27, SIGPROF);
    XLAT(28, SIGWINCH);
    XLAT(29, SIGIO);
    XLAT(19, SIGSTOP);
    XLAT(31, SIGSYS);
    XLAT(20, SIGTSTP);
    XLAT(23, SIGURG);
    default:
      return einval();
  }
}

int UnXlatSignal(int x) {
  if (x == SIGHUP) return 1;
  if (x == SIGINT) return 2;
  if (x == SIGQUIT) return 3;
  if (x == SIGILL) return 4;
  if (x == SIGTRAP) return 5;
  if (x == SIGABRT) return 6;
  if (x == SIGBUS) return 7;
  if (x == SIGFPE) return 8;
  if (x == SIGKILL) return 9;
  if (x == SIGUSR1) return 10;
  if (x == SIGSEGV) return 11;
  if (x == SIGUSR2) return 12;
  if (x == SIGPIPE) return 13;
  if (x == SIGALRM) return 14;
  if (x == SIGTERM) return 15;
  if (x == SIGCHLD) return 17;
  if (x == SIGCONT) return 18;
  if (x == SIGTTIN) return 21;
  if (x == SIGTTOU) return 22;
  if (x == SIGXCPU) return 24;
  if (x == SIGXFSZ) return 25;
  if (x == SIGVTALRM) return 26;
  if (x == SIGPROF) return 27;
  if (x == SIGWINCH) return 28;
  if (x == SIGIO) return 29;
  if (x == SIGSTOP) return 19;
  if (x == SIGSYS) return 31;
  if (x == SIGTSTP) return 20;
  if (x == SIGURG) return 23;
  return 15;
}

int UnXlatSicode(int sig, int code) {
  if (code == SI_USER) return 0;
  if (code == SI_QUEUE) return -1;
  if (code == SI_TIMER) return -2;
  if (code == SI_TKILL) return -6;
  if (code == SI_MESGQ) return -3;
  if (code == SI_ASYNCIO) return -4;
  if (code == SI_ASYNCNL) return -60;
  if (code == SI_KERNEL) return 0x80;
  if (sig == SIGCHLD) {
    if (code == CLD_EXITED) return 1;
    if (code == CLD_KILLED) return 2;
    if (code == CLD_DUMPED) return 3;
    if (code == CLD_TRAPPED) return 4;
    if (code == CLD_STOPPED) return 5;
    if (code == CLD_CONTINUED) return 6;
    return -1;
  }
  if (sig == SIGTRAP) {
    if (code == TRAP_BRKPT) return 1;
    if (code == TRAP_TRACE) return 2;
    return -1;
  }
  if (sig == SIGSEGV) {
    if (code == SEGV_MAPERR) return 1;
    if (code == SEGV_ACCERR) return 2;
    return -1;
  }
  if (sig == SIGFPE) {
    if (code == FPE_INTDIV) return 1;
    if (code == FPE_INTOVF) return 2;
    if (code == FPE_FLTDIV) return 3;
    if (code == FPE_FLTOVF) return 4;
    if (code == FPE_FLTUND) return 5;
    if (code == FPE_FLTRES) return 6;
    if (code == FPE_FLTINV) return 7;
    if (code == FPE_FLTSUB) return 8;
    return -1;
  }
  if (sig == SIGILL) {
    if (code == ILL_ILLOPC) return 1;
    if (code == ILL_ILLOPN) return 2;
    if (code == ILL_ILLADR) return 3;
    if (code == ILL_ILLTRP) return 4;
    if (code == ILL_PRVOPC) return 5;
    if (code == ILL_PRVREG) return 6;
    if (code == ILL_COPROC) return 7;
    if (code == ILL_BADSTK) return 8;
    return -1;
  }
  if (sig == SIGBUS) {
    if (code == BUS_ADRALN) return 1;
    if (code == BUS_ADRERR) return 2;
    if (code == BUS_OBJERR) return 3;
    if (code == BUS_MCEERR_AR) return 4;
    if (code == BUS_MCEERR_AO) return 5;
    return -1;
  }
  if (sig == SIGIO) {
    if (code == POLL_IN) return 1;
    if (code == POLL_OUT) return 2;
    if (code == POLL_MSG) return 3;
    if (code == POLL_ERR) return 4;
    if (code == POLL_PRI) return 5;
    if (code == POLL_HUP) return 6;
    return -1;
  }
  return -1;
}

int XlatSig(int x) {
  switch (x) {
    XLAT(0, SIG_BLOCK);
    XLAT(1, SIG_UNBLOCK);
    XLAT(2, SIG_SETMASK);
    default:
      return einval();
  }
}

int XlatRusage(int x) {
  switch (x) {
    XLAT(0, RUSAGE_SELF);
    XLAT(-1, RUSAGE_CHILDREN);
    XLAT(1, RUSAGE_THREAD);
    default:
      return einval();
  }
}

int XlatSocketFamily(int x) {
  switch (x) {
    XLAT(0, AF_UNSPEC);
    XLAT(1, AF_UNIX);
    XLAT(2, AF_INET);
    default:
      errno = EPFNOSUPPORT;
      return -1;
  }
}

int UnXlatSocketFamily(int x) {
  switch (x) {
    XLAT(AF_UNSPEC, 0);
    XLAT(AF_UNIX, 1);
    XLAT(AF_INET, 2);
    default:
      return x;
  }
}

int XlatSocketType(int x) {
  switch (x) {
    XLAT(1, SOCK_STREAM);
    XLAT(2, SOCK_DGRAM);
    default:
      return einval();
  }
}

int XlatSocketFlags(int flags) {
  unsigned res = 0;
  if (flags & 0x080000) res |= SOCK_CLOEXEC;
  if (flags & 0x000800) res |= SOCK_NONBLOCK;
  return res;
}

int XlatSocketProtocol(int x) {
  switch (x) {
    XLAT(0, 0);
    XLAT(6, IPPROTO_TCP);
    XLAT(17, IPPROTO_UDP);
    default:
      return einval();
  }
}

int XlatSocketLevel(int level) {
  switch (level) {
    XLAT(0, SOL_IP);
    XLAT(1, SOL_SOCKET);
    XLAT(6, SOL_TCP);
    XLAT(17, SOL_UDP);
    default:
      return einval();
  }
}

int XlatSocketOptname(int level, int optname) {
  if (level == SOL_SOCKET) {
    switch (optname) {
      XLAT(1, SO_DEBUG);
      XLAT(2, SO_REUSEADDR);
      XLAT(3, SO_TYPE);
      XLAT(4, SO_ERROR);
      XLAT(5, SO_DONTROUTE);
      XLAT(6, SO_BROADCAST);
      XLAT(7, SO_SNDBUF);
      XLAT(8, SO_RCVBUF);
      XLAT(9, SO_KEEPALIVE);
      XLAT(13, SO_LINGER);
      XLAT(15, SO_REUSEPORT);
      XLAT(18, SO_RCVLOWAT);
      XLAT(19, SO_SNDLOWAT);
      XLAT(30, SO_ACCEPTCONN);
      default:
        return einval();
    }
  }
  if (level == SOL_TCP) {
    switch (optname) {
      XLAT(1, TCP_NODELAY);
      XLAT(2, TCP_MAXSEG);
#if defined(TCP_CORK)
      XLAT(3, TCP_CORK);
#elif defined(TCP_NOPUSH)
      XLAT(3, TCP_NOPUSH);
#endif
#ifdef TCP_KEEPIDLE
      XLAT(4, TCP_KEEPIDLE);
#endif
#ifdef TCP_KEEPINTVL
      XLAT(5, TCP_KEEPINTVL);
#endif
#ifdef TCP_KEEPCNT
      XLAT(6, TCP_KEEPCNT);
#endif
#ifdef TCP_SYNCNT
      XLAT(7, TCP_SYNCNT);
#endif
#ifdef TCP_WINDOW_CLAMP
      XLAT(10, TCP_WINDOW_CLAMP);
#endif
#ifdef TCP_FASTOPEN
      XLAT(23, TCP_FASTOPEN);
#endif
#ifdef TCP_QUICKACK
      XLAT(12, TCP_QUICKACK);
#endif
#ifdef TCP_NOTSENT_LOWAT
      XLAT(25, TCP_NOTSENT_LOWAT);
#endif
#ifdef TCP_SAVE_SYN
      XLAT(27, TCP_SAVE_SYN);
#endif
#ifdef TCP_FASTOPEN_CONNECT
      XLAT(30, TCP_FASTOPEN_CONNECT);
#endif
      default:
        return einval();
    }
  }
  if (level == SOL_IP) {
    switch (optname) {
      XLAT(1, IP_TOS);
      XLAT(2, IP_TTL);
      XLAT(3, IP_HDRINCL);
      XLAT(14, IP_MTU);
      default:
        return einval();
    }
  }
  return einval();
}

int XlatAccess(int x) {
  int r = F_OK;
  if (x & 1) r |= X_OK;
  if (x & 2) r |= W_OK;
  if (x & 4) r |= R_OK;
  return r;
}

int XlatLock(int x) {
  int r = 0;
  if (x & 1) r |= LOCK_SH;
  if (x & 2) r |= LOCK_EX;
  if (x & 4) r |= LOCK_NB;
  if (x & 8) r |= LOCK_UN;
  return r;
}

int XlatWait(int x) {
  int r = 0;
  if (x & 1) r |= WNOHANG;
  if (x & 2) r |= WUNTRACED;
  if (x & 8) r |= WCONTINUED;
  return r;
}

int XlatMapFlags(int x) {
  int r = 0;
  if (x & 1) r |= MAP_SHARED;
  if (x & 2) r |= MAP_PRIVATE;
  if (x & 16) r |= MAP_FIXED;
  if (x & 32) r |= MAP_ANONYMOUS;
  return r;
}

int XlatMsyncFlags(int x) {
  unsigned res = 0;
  if (x & 1) res |= MS_ASYNC;
  if (x & 2) res |= MS_INVALIDATE;
  if (x & 4) res |= MS_SYNC;
  return res;
}

int XlatClock(int x) {
  switch (x) {
    XLAT(0, CLOCK_REALTIME);
    XLAT(1, CLOCK_MONOTONIC);
    XLAT(2, CLOCK_PROCESS_CPUTIME_ID);
#ifdef CLOCK_MONOTONIC_RAW
    XLAT(4, CLOCK_MONOTONIC_RAW);
#endif
    default:
      return x;
  }
}

int XlatAtf(int x) {
  int res = 0;
  if (x & 0x0100) res |= AT_SYMLINK_NOFOLLOW;
  if (x & 0x0200) res |= AT_REMOVEDIR;
  if (x & 0x0400) res |= AT_SYMLINK_FOLLOW;
  if (x & 0x1000) res |= AT_EMPTY_PATH;
  return res;
}

int XlatOpenMode(int flags) {
  switch (flags & 3) {
    case 0:
      return O_RDONLY;
    case 1:
      return O_WRONLY;
    case 2:
      return O_RDWR;
    default:
      for (;;) (void)0;
  }
}

int XlatOpenFlags(int flags) {
  int res;
  res = XlatOpenMode(flags);
  if (flags & 0x00400) res |= O_APPEND;
  if (flags & 0x00040) res |= O_CREAT;
  if (flags & 0x00080) res |= O_EXCL;
  if (flags & 0x00200) res |= O_TRUNC;
  if (flags & 0x00800) res |= O_NDELAY;
  if (flags & 0x04000) res |= O_DIRECT;
  if (flags & 0x10000) res |= O_DIRECTORY;
  if (flags & 0x20000) res |= O_NOFOLLOW;
  if (flags & 0x80000) res |= O_CLOEXEC;
  if (flags & 0x00100) res |= O_NOCTTY;
#ifdef O_ASYNC
  if (flags & 0x02000) res |= O_ASYNC;
#endif
#ifdef O_NOATIME
  if (flags & 0x40000) res |= O_NOATIME;
#endif
#ifdef O_DSYNC
  if (flags & 0x000001000) res |= O_DSYNC;
#endif
#ifdef O_SYNC
  if ((flags & 0x00101000) == 0x00101000) res |= O_SYNC;
#endif
  return res;
}

int XlatFcntlCmd(int x) {
  switch (x) {
    XLAT(1, F_GETFD);
    XLAT(2, F_SETFD);
    XLAT(3, F_GETFL);
    XLAT(4, F_SETFL);
    default:
      return einval();
  }
}

int XlatFcntlArg(int x) {
  switch (x) {
    XLAT(0, 0);
    XLAT(1, FD_CLOEXEC);
    XLAT(0x0800, O_NONBLOCK);
    default:
      return einval();
  }
}

int XlatAdvice(int x) {
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

void XlatSockaddrToHost(struct sockaddr_in *dst,
                        const struct sockaddr_in_bits *src) {
  memset(dst, 0, sizeof(*dst));
  dst->sin_family = XlatSocketFamily(Read16(src->sin_family));
  dst->sin_port = src->sin_port;
  dst->sin_addr.s_addr = src->sin_addr;
}

void XlatSockaddrToLinux(struct sockaddr_in_bits *dst,
                         const struct sockaddr_in *src) {
  memset(dst, 0, sizeof(*dst));
  Write16(dst->sin_family, UnXlatSocketFamily(src->sin_family));
  dst->sin_port = src->sin_port;
  dst->sin_addr = src->sin_addr.s_addr;
}

void XlatStatToLinux(struct stat_bits *dst, const struct stat *src) {
  Write64(dst->st_dev, src->st_dev);
  Write64(dst->st_ino, src->st_ino);
  Write64(dst->st_nlink, src->st_nlink);
  Write32(dst->st_mode, src->st_mode);
  Write32(dst->st_uid, src->st_uid);
  Write32(dst->st_gid, src->st_gid);
  Write32(dst->__pad, 0);
  Write64(dst->st_rdev, src->st_rdev);
  Write64(dst->st_size, src->st_size);
  Write64(dst->st_blksize, src->st_blksize);
  Write64(dst->st_blocks, src->st_blocks);
  Write64(dst->st_dev, src->st_dev);
  Write64(dst->st_atim.tv_sec, src->st_atim.tv_sec);
  Write64(dst->st_atim.tv_nsec, src->st_atim.tv_nsec);
  Write64(dst->st_mtim.tv_sec, src->st_mtim.tv_sec);
  Write64(dst->st_mtim.tv_nsec, src->st_mtim.tv_nsec);
  Write64(dst->st_ctim.tv_sec, src->st_ctim.tv_sec);
  Write64(dst->st_ctim.tv_nsec, src->st_ctim.tv_nsec);
}

void XlatRusageToLinux(struct rusage_bits *dst, const struct rusage *src) {
  Write64(dst->ru_utime.tv_sec, src->ru_utime.tv_sec);
  Write64(dst->ru_utime.tv_usec, src->ru_utime.tv_usec);
  Write64(dst->ru_stime.tv_sec, src->ru_stime.tv_sec);
  Write64(dst->ru_stime.tv_usec, src->ru_stime.tv_usec);
  Write64(dst->ru_maxrss, src->ru_maxrss);
  Write64(dst->ru_ixrss, src->ru_ixrss);
  Write64(dst->ru_idrss, src->ru_idrss);
  Write64(dst->ru_isrss, src->ru_isrss);
  Write64(dst->ru_minflt, src->ru_minflt);
  Write64(dst->ru_majflt, src->ru_majflt);
  Write64(dst->ru_nswap, src->ru_nswap);
  Write64(dst->ru_inblock, src->ru_inblock);
  Write64(dst->ru_oublock, src->ru_oublock);
  Write64(dst->ru_msgsnd, src->ru_msgsnd);
  Write64(dst->ru_msgrcv, src->ru_msgrcv);
  Write64(dst->ru_nsignals, src->ru_nsignals);
  Write64(dst->ru_nvcsw, src->ru_nvcsw);
  Write64(dst->ru_nivcsw, src->ru_nivcsw);
}

void XlatItimervalToLinux(struct itimerval_bits *dst,
                          const struct itimerval *src) {
  Write64(dst->it_interval.tv_sec, src->it_interval.tv_sec);
  Write64(dst->it_interval.tv_usec, src->it_interval.tv_usec);
  Write64(dst->it_value.tv_sec, src->it_value.tv_sec);
  Write64(dst->it_value.tv_usec, src->it_value.tv_usec);
}

void XlatLinuxToItimerval(struct itimerval *dst,
                          const struct itimerval_bits *src) {
  dst->it_interval.tv_sec = Read64(src->it_interval.tv_sec);
  dst->it_interval.tv_usec = Read64(src->it_interval.tv_usec);
  dst->it_value.tv_sec = Read64(src->it_value.tv_sec);
  dst->it_value.tv_usec = Read64(src->it_value.tv_usec);
}

void XlatWinsizeToLinux(struct winsize_bits *dst, const struct winsize *src) {
  memset(dst, 0, sizeof(*dst));
  Write16(dst->ws_row, src->ws_row);
  Write16(dst->ws_col, src->ws_col);
}

void XlatSigsetToLinux(uint8_t dst[8], const sigset_t *src) {
  int i;
  uint64_t x;
  for (x = i = 0; i < 64; ++i) {
    if (sigismember(src, i + 1)) {
      x |= 1ull << i;
    }
  }
  Write64(dst, x);
}

void XlatLinuxToSigset(sigset_t *dst, const uint8_t src[8]) {
  int i;
  uint64_t x;
  x = Read64(src);
  sigemptyset(dst);
  for (i = 0; i < 64; ++i) {
    if ((1ull << i) & x) {
      sigaddset(dst, i + 1);
    }
  }
}

static int XlatTermiosCflag(int x) {
  int r = 0;
  if (x & 0x0001) r |= ISIG;
  if (x & 0x0040) r |= CSTOPB;
  if (x & 0x0080) r |= CREAD;
  if (x & 0x0100) r |= PARENB;
  if (x & 0x0200) r |= PARODD;
  if (x & 0x0400) r |= HUPCL;
  if (x & 0x0800) r |= CLOCAL;
  if ((x & 0x0030) == 0x0010) {
    r |= CS6;
  } else if ((x & 0x0030) == 0x0020) {
    r |= CS7;
  } else if ((x & 0x0030) == 0x0030) {
    r |= CS8;
  }
  return r;
}

static int UnXlatTermiosCflag(int x) {
  int r = 0;
  if (x & ISIG) r |= 0x0001;
  if (x & CSTOPB) r |= 0x0040;
  if (x & CREAD) r |= 0x0080;
  if (x & PARENB) r |= 0x0100;
  if (x & PARODD) r |= 0x0200;
  if (x & HUPCL) r |= 0x0400;
  if (x & CLOCAL) r |= 0x0800;
  if ((x & CSIZE) == CS5) {
    r |= 0x0000;
  } else if ((x & CSIZE) == CS6) {
    r |= 0x0010;
  } else if ((x & CSIZE) == CS7) {
    r |= 0x0020;
  } else if ((x & CSIZE) == CS8) {
    r |= 0x0030;
  }
  return r;
}

static int XlatTermiosLflag(int x) {
  int r = 0;
  if (x & 0x0001) r |= ISIG;
  if (x & 0x0002) r |= ICANON;
  if (x & 0x0008) r |= ECHO;
  if (x & 0x0010) r |= ECHOE;
  if (x & 0x0020) r |= ECHOK;
  if (x & 0x0040) r |= ECHONL;
  if (x & 0x0080) r |= NOFLSH;
  if (x & 0x0100) r |= TOSTOP;
  if (x & 0x8000) r |= IEXTEN;
#ifdef ECHOCTL
  if (x & 0x0200) r |= ECHOCTL;
#endif
#ifdef ECHOPRT
  if (x & 0x0400) r |= ECHOPRT;
#endif
#ifdef ECHOKE
  if (x & 0x0800) r |= ECHOKE;
#endif
#ifdef FLUSHO
  if (x & 0x1000) r |= FLUSHO;
#endif
#ifdef PENDIN
  if (x & 0x4000) r |= PENDIN;
#endif
#ifdef XCASE
  if (x & 0x0004) r |= XCASE;
#endif
  return r;
}

static int UnXlatTermiosLflag(int x) {
  int r = 0;
  if (x & ISIG) r |= 0x0001;
  if (x & ICANON) r |= 0x0002;
  if (x & ECHO) r |= 0x0008;
  if (x & ECHOE) r |= 0x0010;
  if (x & ECHOK) r |= 0x0020;
  if (x & ECHONL) r |= 0x0040;
  if (x & NOFLSH) r |= 0x0080;
  if (x & TOSTOP) r |= 0x0100;
  if (x & IEXTEN) r |= 0x8000;
#ifdef ECHOCTL
  if (x & ECHOCTL) r |= 0x0200;
#endif
#ifdef ECHOPRT
  if (x & ECHOPRT) r |= 0x0400;
#endif
#ifdef ECHOKE
  if (x & ECHOKE) r |= 0x0800;
#endif
#ifdef FLUSHO
  if (x & FLUSHO) r |= 0x1000;
#endif
#ifdef PENDIN
  if (x & PENDIN) r |= 0x4000;
#endif
#ifdef XCASE
  if (x & XCASE) r |= 0x0004;
#endif
  return r;
}

static int XlatTermiosIflag(int x) {
  int r = 0;
  if (x & 0x0001) r |= IGNBRK;
  if (x & 0x0002) r |= BRKINT;
  if (x & 0x0004) r |= IGNPAR;
  if (x & 0x0008) r |= PARMRK;
  if (x & 0x0010) r |= INPCK;
  if (x & 0x0020) r |= ISTRIP;
  if (x & 0x0040) r |= INLCR;
  if (x & 0x0080) r |= IGNCR;
  if (x & 0x0100) r |= ICRNL;
  if (x & 0x0400) r |= IXON;
  if (x & 0x0800) r |= IXANY;
  if (x & 0x1000) r |= IXOFF;
#ifdef IMAXBEL
  if (x & 0x2000) r |= IMAXBEL;
#endif
#ifdef IUTF8
  if (x & 0x4000) r |= IUTF8;
#endif
#ifdef IUCLC
  if (x & 0x0200) r |= IUCLC;
#endif
  return r;
}

static int UnXlatTermiosIflag(int x) {
  int r = 0;
  if (x & IGNBRK) r |= 0x0001;
  if (x & BRKINT) r |= 0x0002;
  if (x & IGNPAR) r |= 0x0004;
  if (x & PARMRK) r |= 0x0008;
  if (x & INPCK) r |= 0x0010;
  if (x & ISTRIP) r |= 0x0020;
  if (x & INLCR) r |= 0x0040;
  if (x & IGNCR) r |= 0x0080;
  if (x & ICRNL) r |= 0x0100;
  if (x & IXON) r |= 0x0400;
  if (x & IXANY) r |= 0x0800;
  if (x & IXOFF) r |= 0x1000;
#ifdef IMAXBEL
  if (x & IMAXBEL) r |= 0x2000;
#endif
#ifdef IUTF8
  if (x & IUTF8) r |= 0x4000;
#endif
#ifdef IUCLC
  if (x & IUCLC) r |= 0x0200;
#endif
  return r;
}

static int XlatTermiosOflag(int x) {
  int r = 0;
  if (x & 0x0001) r |= OPOST;
#ifdef ONLCR
  if (x & 0x0004) r |= ONLCR;
#endif
#ifdef OCRNL
  if (x & 0x0008) r |= OCRNL;
#endif
#ifdef ONOCR
  if (x & 0x0010) r |= ONOCR;
#endif
#ifdef ONLRET
  if (x & 0x0020) r |= ONLRET;
#endif
#ifdef OFILL
  if (x & 0x0040) r |= OFILL;
#endif
#ifdef OFDEL
  if (x & 0x0080) r |= OFDEL;
#endif
#ifdef NLDLY
  if ((x & 0x0100) == 0x0000) {
    r |= NL0;
  } else if ((x & 0x0100) == 0x0100) {
    r |= NL1;
#ifdef NL2
  } else if ((x & 0x0100) == 0x0000) {
    r |= NL2;
#endif
#ifdef NL3
  } else if ((x & 0x0100) == 0x0000) {
    r |= NL3;
#endif
  }
#endif
#ifdef CRDLY
  if ((x & 0x0600) == 0x0000) {
    r |= CR0;
  } else if ((x & 0x0600) == 0x0200) {
    r |= CR1;
  } else if ((x & 0x0600) == 0x0400) {
    r |= CR2;
  } else if ((x & 0x0600) == 0x0600) {
    r |= CR3;
  }
#endif
#ifdef TABDLY
  if ((x & 0x1800) == 0x0000) {
    r |= TAB0;
#ifdef TAB1
  } else if ((x & 0x1800) == 0x0800) {
    r |= TAB1;
#endif
#ifdef TAB1
  } else if ((x & 0x1800) == 0x1000) {
    r |= TAB2;
#endif
  } else if ((x & 0x1800) == 0x1800) {
    r |= TAB3;
  }
#endif
#ifdef BSDLY
  if ((x & 0x2000) == 0x0000) {
    r |= BS0;
  } else if ((x & 0x2000) == 0x2000) {
    r |= BS1;
  }
#endif
#ifdef VTBDLY
  if ((x & 0x4000) == 0x0000) {
    r |= VT0;
  } else if ((x & 0x4000) == 0x4000) {
    r |= VT1;
  }
#endif
#ifdef FFBDLY
  if ((x & 0x8000) == 0x0000) {
    r |= FF0;
  } else if ((x & 0x8000) == 0x8000) {
    r |= FF1;
  }
#endif
#ifdef OLCUC
  if (x & 0x0002) r |= OLCUC;
#endif
  return r;
}

static int UnXlatTermiosOflag(int x) {
  int r = 0;
  if (x & OPOST) r |= 0x0001;
#ifdef ONLCR
  if (x & ONLCR) r |= 0x0004;
#endif
#ifdef OCRNL
  if (x & OCRNL) r |= 0x0008;
#endif
#ifdef ONOCR
  if (x & ONOCR) r |= 0x0010;
#endif
#ifdef ONLRET
  if (x & ONLRET) r |= 0x0020;
#endif
#ifdef OFILL
  if (x & OFILL) r |= 0x0040;
#endif
#ifdef OFDEL
  if (x & OFDEL) r |= 0x0080;
#endif
#ifdef NLDLY
  if ((x & NLDLY) == NL0) {
    r |= 0x0000;
  } else if ((x & NLDLY) == NL1) {
    r |= 0x0100;
#ifdef NL2
  } else if ((x & NLDLY) == NL2) {
    r |= 0x0000;
#endif
#ifdef NL3
  } else if ((x & NLDLY) == NL3) {
    r |= 0x0000;
#endif
  }
#endif
#ifdef CRDLY
  if ((x & CRDLY) == CR0) {
    r |= 0x0000;
  } else if ((x & CRDLY) == CR1) {
    r |= 0x0200;
  } else if ((x & CRDLY) == CR2) {
    r |= 0x0400;
  } else if ((x & CRDLY) == CR3) {
    r |= 0x0600;
  }
#endif
#ifdef TABDLY
  if ((x & TABDLY) == TAB0) {
    r |= 0x0000;
#ifdef TAB1
  } else if ((x & TABDLY) == TAB1) {
    r |= 0x0800;
#endif
#ifdef TAB2
  } else if ((x & TABDLY) == TAB2) {
    r |= 0x1000;
#endif
  } else if ((x & TABDLY) == TAB3) {
    r |= 0x1800;
  }
#endif
#ifdef BSDLY
  if ((x & BSDLY) == BS0) {
    r |= 0x0000;
  } else if ((x & BSDLY) == BS1) {
    r |= 0x2000;
  }
#endif
#ifdef VTDLY
  if ((x & VTDLY) == VT0) {
    r |= 0x0000;
  } else if ((x & VTDLY) == VT1) {
    r |= 0x4000;
  }
#endif
#ifdef FFDLY
  if ((x & FFDLY) == FF0) {
    r |= 0x0000;
  } else if ((x & FFDLY) == FF1) {
    r |= 0x8000;
  }
#endif
#ifdef OLCUC
  if (x & OLCUC) r |= 0x0002;
#endif
  return r;
}

static void XlatTermiosCc(struct termios *dst, const struct termios_bits *src) {
  dst->c_cc[VINTR] = src->c_cc[0];
  dst->c_cc[VQUIT] = src->c_cc[1];
  dst->c_cc[VERASE] = src->c_cc[2];
  dst->c_cc[VKILL] = src->c_cc[3];
  dst->c_cc[VEOF] = src->c_cc[4];
  dst->c_cc[VTIME] = src->c_cc[5];
  dst->c_cc[VMIN] = src->c_cc[6];
  dst->c_cc[VSTART] = src->c_cc[8];
  dst->c_cc[VSTOP] = src->c_cc[9];
  dst->c_cc[VSUSP] = src->c_cc[10];
  dst->c_cc[VEOL] = src->c_cc[11];
#ifdef VSWTC
  dst->c_cc[VSWTC] = src->c_cc[7];
#endif
#ifdef VREPRINT
  dst->c_cc[VREPRINT] = src->c_cc[12];
#endif
#ifdef VDISCARD
  dst->c_cc[VDISCARD] = src->c_cc[13];
#endif
#ifdef VWERASE
  dst->c_cc[VWERASE] = src->c_cc[14];
#endif
#ifdef VLNEXT
  dst->c_cc[VLNEXT] = src->c_cc[15];
#endif
#ifdef VEOL2
  dst->c_cc[VEOL2] = src->c_cc[16];
#endif
}

static void UnXlatTermiosCc(struct termios_bits *dst,
                            const struct termios *src) {
  dst->c_cc[0] = src->c_cc[VINTR];
  dst->c_cc[1] = src->c_cc[VQUIT];
  dst->c_cc[2] = src->c_cc[VERASE];
  dst->c_cc[3] = src->c_cc[VKILL];
  dst->c_cc[4] = src->c_cc[VEOF];
  dst->c_cc[5] = src->c_cc[VTIME];
  dst->c_cc[6] = src->c_cc[VMIN];
  dst->c_cc[8] = src->c_cc[VSTART];
  dst->c_cc[9] = src->c_cc[VSTOP];
  dst->c_cc[10] = src->c_cc[VSUSP];
  dst->c_cc[11] = src->c_cc[VEOL];
#ifdef VSWTC
  dst->c_cc[7] = src->c_cc[VSWTC];
#endif
#ifdef VREPRINT
  dst->c_cc[12] = src->c_cc[VREPRINT];
#endif
#ifdef VDISCARD
  dst->c_cc[13] = src->c_cc[VDISCARD];
#endif
#ifdef VWERASE
  dst->c_cc[14] = src->c_cc[VWERASE];
#endif
#ifdef VLNEXT
  dst->c_cc[15] = src->c_cc[VLNEXT];
#endif
#ifdef VEOL2
  dst->c_cc[16] = src->c_cc[VEOL2];
#endif
}

void XlatLinuxToTermios(struct termios *dst, const struct termios_bits *src) {
  memset(dst, 0, sizeof(*dst));
  dst->c_iflag = XlatTermiosIflag(Read32(src->c_iflag));
  dst->c_oflag = XlatTermiosOflag(Read32(src->c_oflag));
  dst->c_cflag = XlatTermiosCflag(Read32(src->c_cflag));
  dst->c_lflag = XlatTermiosLflag(Read32(src->c_lflag));
  XlatTermiosCc(dst, src);
}

void XlatTermiosToLinux(struct termios_bits *dst, const struct termios *src) {
  memset(dst, 0, sizeof(*dst));
  Write32(dst->c_iflag, UnXlatTermiosIflag(src->c_iflag));
  Write32(dst->c_oflag, UnXlatTermiosOflag(src->c_oflag));
  Write32(dst->c_cflag, UnXlatTermiosCflag(src->c_cflag));
  Write32(dst->c_lflag, UnXlatTermiosLflag(src->c_lflag));
  UnXlatTermiosCc(dst, src);
}
