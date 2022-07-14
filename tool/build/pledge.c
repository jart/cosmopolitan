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
#include "libc/calls/calls.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/rlimit.h"
#include "third_party/getopt/getopt.h"

STATIC_YOINK("strerror_wr");

#define GETOPTS "hFp:u:g:c:"

#define USAGE \
  "\
usage: pledge.com [-h] PROG ARGS...\n\
  -h         show help\n\
  -g GID     call setgid()\n\
  -u UID     call setuid()\n\
  -c PATH    call chroot()\n\
  -F         don't normalize file descriptors\n\
  -p PLEDGE  may contain any of following separated by spaces\n\
     - stdio: allow stdio and benign system calls\n\
     - rpath: read-only path ops\n\
     - wpath: write path ops\n\
     - cpath: create path ops\n\
     - dpath: create special files\n\
     - flock: file locks\n\
     - tty: terminal ioctls\n\
     - recvfd: allow SCM_RIGHTS\n\
     - fattr: allow changing some struct stat bits\n\
     - inet: allow IPv4 and IPv6\n\
     - unix: allow local sockets\n\
     - dns: allow dns\n\
     - proc: allow fork, clone and friends\n\
     - thread: allow clone\n\
     - id: allow setuid and friends\n\
     - exec: allow executing ape binaries\n\
\n\
pledge.com v1.o\n\
copyright 2022 justine alexandra roberts tunney\n\
https://twitter.com/justinetunney\n\
https://linkedin.com/in/jtunney\n\
https://justine.lol/pledge/\n\
https://github.com/jart\n\
\n\
this program lets you launch linux commands in a sandbox that's\n\
inspired by the design of openbsd's pledge() system call. Visit\n\
the https://justine.lol/pledge/ page for online documentation.\n\
\n\
"

int g_gflag;
int g_uflag;
int g_hflag;
bool g_noclose;
const char *g_pflag;
const char *g_chroot;

static void GetOpts(int argc, char *argv[]) {
  int opt;
  g_pflag = "";
  while ((opt = getopt(argc, argv, GETOPTS)) != -1) {
    switch (opt) {
      case 'p':
        g_pflag = optarg;
        break;
      case 'c':
        g_chroot = optarg;
        break;
      case 'g':
        g_gflag = atoi(optarg);
        break;
      case 'u':
        g_uflag = atoi(optarg);
        break;
      case 'F':
        g_noclose = true;
        break;
      case 'h':
      case '?':
        write(1, USAGE, sizeof(USAGE) - 1);
        exit(0);
      default:
        write(2, USAGE, sizeof(USAGE) - 1);
        exit(64);
    }
  }
}

const char *prog;
char pledges[1024];
char pathbuf[PATH_MAX];
struct pollfd pfds[256];

int GetPollMaxFds(void) {
  int n;
  struct rlimit rl;
  if (getrlimit(RLIMIT_NOFILE, &rl) != -1) {
    n = rl.rlim_cur;
  } else {
    n = 64;
  }
  return MIN(ARRAYLEN(pfds), MAX(3, n));
}

void NormalizeFileDescriptors(void) {
  int i, n, fd;
  n = GetPollMaxFds();
  for (i = 0; i < n; ++i) {
    pfds[i].fd = i;
    pfds[i].events = POLLIN;
  }
  if (poll(pfds, n, 0) == -1) {
    kprintf("error: poll() failed: %m\n");
    exit(__COUNTER__);
  }
  for (i = 0; i < 3; ++i) {
    if (pfds[i].revents & POLLNVAL) {
      if ((fd = open("/dev/null", O_RDWR)) == -1) {
        kprintf("error: open(\"/dev/null\") failed: %m\n");
        exit(__COUNTER__);
      }
      if (fd != i) {
        kprintf("error: open() is broken: %d vs. %d\n", fd, i);
        exit(__COUNTER__);
      }
    }
  }
  for (i = 3; i < n; ++i) {
    if (~pfds[i].revents & POLLNVAL) {
      if (close(pfds[i].fd) == -1) {
        kprintf("error: close(%d) failed: %m\n", pfds[i].fd);
        exit(__COUNTER__);
      }
    }
  }
}

int main(int argc, char *argv[]) {
  bool hasfunbits;
  int useruid, usergid;
  int owneruid, ownergid;
  int oldfsuid, oldfsgid;

  if (!IsLinux()) {
    kprintf("error: this program is only intended for linux\n");
    exit(__COUNTER__);
  }

  // parse flags
  GetOpts(argc, argv);
  if (optind == argc) {
    kprintf("error: too few args\n", g_pflag);
    write(2, USAGE, sizeof(USAGE) - 1);
    exit(64);
  }

  if (!g_noclose) {
    NormalizeFileDescriptors();
  }

  // test for weird chmod bits
  usergid = getgid();
  ownergid = getegid();
  useruid = getuid();
  owneruid = geteuid();
  hasfunbits = usergid != ownergid || useruid != owneruid;

  if (hasfunbits) {
    setuid(owneruid);
    setgid(ownergid);
  }

  // some flags can't be allowed if binary has setuid bits
  if (hasfunbits) {
    if (g_uflag || g_gflag) {
      kprintf("error: setuid flags forbidden on setuid binaries\n");
      _Exit(__COUNTER__);
    }
  }

  // check if user has permission to chroot directory
  if (hasfunbits) {
    oldfsuid = setfsuid(useruid);
    oldfsgid = setfsgid(usergid);
    if (access(g_chroot, R_OK) == -1) {
      kprintf("error: access(%#s) failed: %m\n", g_chroot);
      _Exit(__COUNTER__);
    }
    setfsuid(oldfsuid);
    setfsgid(oldfsgid);
  }

  // change root fs path
  // all the documentation on the subject is unprofessional and crazy
  // the linux devs willfully deprive linux users of security tools
  // linux appears to not even forbid chroot on setuid binaries
  // yes i've considered fchdir() and i don't really care
  // ohh it's sooo insecure they say, and they solve it
  // by imposing a requirement that we must only do
  // the "insecure" thing as the root user lool
  if (g_chroot) {
    if (chdir(g_chroot) == -1) {
      kprintf("error: chdir(%#s) failed: %m\n", g_chroot);
      _Exit(__COUNTER__);
    }
    if (chroot(g_chroot) == -1) {
      kprintf("error: chroot(%#s) failed: %m\n", g_chroot);
      _Exit(__COUNTER__);
    }
  }

  // find program
  if (hasfunbits) {
    oldfsuid = setfsuid(useruid);
    oldfsgid = setfsgid(usergid);
  }
  if (!(prog = commandv(argv[optind], pathbuf, sizeof(pathbuf)))) {
    kprintf("error: command not found: %m\n", argv[optind]);
    _Exit(__COUNTER__);
  }
  if (hasfunbits) {
    setfsuid(oldfsuid);
    setfsgid(oldfsgid);
  }

  // set group id
  if (usergid != ownergid) {
    // setgid binaries must use the gid of the user that ran it
    if (setgid(usergid) == -1) {
      kprintf("error: setgid(%d) failed: %m\n", usergid);
      _Exit(__COUNTER__);
    }
    if (getgid() != usergid || getegid() != usergid) {
      kprintf("error: setgid() broken\n");
      _Exit(__COUNTER__);
    }
  } else if (g_gflag) {
    // otherwise we trust the gid flag
    if (setgid(g_gflag) == -1) {
      kprintf("error: setgid(%d) failed: %m\n", g_gflag);
      _Exit(__COUNTER__);
    }
    if (getgid() != g_gflag || getegid() != g_gflag) {
      kprintf("error: setgid() broken\n");
      _Exit(__COUNTER__);
    }
  }

  // set user id
  if (useruid != owneruid) {
    // setuid binaries must use the uid of the user that ran it
    if (setuid(useruid) == -1) {
      kprintf("error: setuid(%d) failed: %m\n", useruid);
      _Exit(__COUNTER__);
    }
    if (getuid() != useruid || geteuid() != useruid) {
      kprintf("error: setuid() broken\n");
      _Exit(__COUNTER__);
    }
  } else if (g_uflag) {
    // otherwise we trust the uid flag
    if (setuid(g_uflag) == -1) {
      kprintf("error: setuid(%d) failed: %m\n", g_uflag);
      _Exit(__COUNTER__);
    }
    if (getuid() != g_uflag || geteuid() != g_uflag) {
      kprintf("error: setuid() broken\n");
      _Exit(__COUNTER__);
    }
  }

  // apply sandbox
  ksnprintf(pledges, sizeof(pledges), "%s execnative", g_pflag);
  if (pledge(pledges, 0) == -1) {
    kprintf("error: pledge(%#s) failed: %m\n", pledges);
    _Exit(__COUNTER__);
  }

  // launch program
  __sys_execve(prog, argv + optind, environ);
  kprintf("%s: execve failed: %m\n", prog);
  return 127;
}
