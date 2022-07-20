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
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/calls/landlock.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/sched_param.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/sysinfo.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/elf/def.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/promises.internal.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/kcpuids.h"
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/strlist.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ioprio.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/prio.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/consts/sched.h"
#include "libc/sysv/errfuns.h"
#include "libc/x/x.h"
#include "third_party/getopt/getopt.h"

STATIC_YOINK("strerror_wr");

#define USAGE \
  "\
usage: pledge.com [-hnN] PROG ARGS...\n\
  -h              show help\n\
  -g GID          call setgid()\n\
  -u UID          call setuid()\n\
  -c PATH         call chroot()\n\
  -v [PERM:]PATH  call unveil(PATH, PERM[rwxc])\n\
  -n              set maximum niceness\n\
  -N              don't normalize file descriptors\n\
  -C SECS         set cpu limit [default: inherited]\n\
  -M BYTES        set virtual memory limit [default: 4gb]\n\
  -P PROCS        set process limit [default: GetCpuCount()*2]\n\
  -F BYTES        set individual file size limit [default: 4gb]\n\
  -p PLEDGE       may contain any of following separated by spaces\n\
     - stdio: allow stdio and benign system calls\n\
     - rpath: read-only path ops\n\
     - wpath: write path ops\n\
     - cpath: create path ops\n\
     - dpath: create special files\n\
     - flock: file locks\n\
     - tty: terminal ioctls\n\
     - recvfd: allow SCM_RIGHTS\n\
     - sendfd: allow SCM_RIGHTS\n\
     - fattr: allow changing some struct stat bits\n\
     - inet: allow IPv4 and IPv6\n\
     - unix: allow local sockets\n\
     - id: allow setuid and friends\n\
     - dns: allow dns and related files\n\
     - proc: allow process and thread creation\n\
     - exec: implied by default\n\
     - prot_exec: allow creating executable memory\n\
     - vminfo: allows /proc/stat, /proc/self/maps, etc.\n\
\n\
pledge.com v1.1\n\
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

int ParsePromises(const char *, unsigned long *);

int g_gflag;
int g_uflag;
int g_hflag;
bool g_nice;
bool g_noclose;
long g_cpuquota;
long g_fszquota;
long g_memquota;
long g_proquota;
const char *g_chroot;
const char *g_promises;

struct {
  int n;
  char **p;
} unveils;

static void GetOpts(int argc, char *argv[]) {
  int opt;
  struct sysinfo si;
  g_promises = 0;
  g_fszquota = 256 * 1000 * 1000;
  g_proquota = GetCpuCount() * 100;
  g_fszquota = 4 * 1000 * 1000 * 1000;
  g_memquota = 4L * 1024 * 1024 * 1024;
  if (!sysinfo(&si)) g_memquota = si.totalram;
  while ((opt = getopt(argc, argv, "hnNp:u:g:c:C:P:M:F:v:")) != -1) {
    switch (opt) {
      case 'n':
        g_nice = true;
        break;
      case 'N':
        g_noclose = true;
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
      case 'C':
        g_cpuquota = atoi(optarg);
        break;
      case 'P':
        g_proquota = atoi(optarg);
        break;
      case 'M':
        g_memquota = sizetol(optarg, 1024);
        break;
      case 'F':
        g_fszquota = sizetol(optarg, 1000);
        break;
      case 'p':
        if (g_promises) {
          g_promises = xstrcat(g_promises, ' ', optarg);
        } else {
          g_promises = optarg;
        }
        break;
      case 'v':
        unveils.p = realloc(unveils.p, ++unveils.n * sizeof(*unveils.p));
        unveils.p[unveils.n - 1] = optarg;
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
  if (!g_promises) {
    g_promises = "stdio rpath";
  }
}

const char *prog;
char pathbuf[PATH_MAX];
struct pollfd pfds[256];

int GetBaseCpuFreqMhz(void) {
  return KCPUIDS(16H, EAX) & 0x7fff;
}

static bool SupportsLandlock(void) {
  int e = errno;
  bool r = landlock_create_ruleset(0, 0, LANDLOCK_CREATE_RULESET_VERSION) >= 0;
  errno = e;
  return r;
}

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
    exit(1);
  }
  for (i = 0; i < 3; ++i) {
    if (pfds[i].revents & POLLNVAL) {
      if ((fd = open("/dev/null", O_RDWR)) == -1) {
        kprintf("error: open(\"/dev/null\") failed: %m\n");
        exit(2);
      }
      if (fd != i) {
        kprintf("error: open() is broken: %d vs. %d\n", fd, i);
        exit(3);
      }
    }
  }
  for (i = 3; i < n; ++i) {
    if (~pfds[i].revents & POLLNVAL) {
      if (close(pfds[i].fd) == -1) {
        kprintf("error: close(%d) failed: %m\n", pfds[i].fd);
        exit(4);
      }
    }
  }
}

void SetCpuLimit(int secs) {
  int mhz, lim;
  struct rlimit rlim;
  if (secs <= 0) return;
  if (!(mhz = GetBaseCpuFreqMhz())) return;
  lim = ceil(3100. / mhz * secs);
  rlim.rlim_cur = lim;
  rlim.rlim_max = lim + 1;
  if (setrlimit(RLIMIT_CPU, &rlim) != -1) {
    return;
  } else if (getrlimit(RLIMIT_CPU, &rlim) != -1) {
    if (lim < rlim.rlim_cur) {
      rlim.rlim_cur = lim;
      if (setrlimit(RLIMIT_CPU, &rlim) != -1) {
        return;
      }
    }
  }
  kprintf("error: setrlimit(RLIMIT_CPU) failed: %m\n");
  exit(20);
}

void SetFszLimit(long n) {
  struct rlimit rlim;
  if (n <= 0) return;
  rlim.rlim_cur = n;
  rlim.rlim_max = n << 1;
  if (setrlimit(RLIMIT_FSIZE, &rlim) != -1) {
    return;
  } else if (getrlimit(RLIMIT_FSIZE, &rlim) != -1) {
    rlim.rlim_cur = n;
    if (setrlimit(RLIMIT_FSIZE, &rlim) != -1) {
      return;
    }
  }
  kprintf("error: setrlimit(RLIMIT_FSIZE) failed: %m\n");
  exit(21);
}

void SetMemLimit(long n) {
  struct rlimit rlim = {n, n};
  if (n <= 0) return;
  if (setrlimit(RLIMIT_AS, &rlim) == -1) {
    kprintf("error: setrlimit(RLIMIT_AS) failed: %m\n");
    exit(22);
  }
}

void SetProLimit(long n) {
  struct rlimit rlim = {n, n};
  if (n <= 0) return;
  if (setrlimit(RLIMIT_NPROC, &rlim) == -1) {
    kprintf("error: setrlimit(RLIMIT_NPROC) failed: %m\n");
    exit(22);
  }
}

bool PathExists(const char *path) {
  int err;
  struct stat st;
  if (path) {
    err = errno;
    if (!stat(path, &st)) {
      return true;
    } else {
      errno = err;
      return false;
    }
  } else {
    return false;
  }
}

bool IsDynamicExecutable(const char *prog) {
  int fd;
  Elf64_Ehdr e;
  struct stat st;
  if ((fd = open(prog, O_RDONLY)) == -1) {
    kprintf("open(%#s, O_RDONLY) failed: %m\n", prog);
    exit(13);
  }
  if (read(fd, &e, sizeof(e)) != sizeof(e)) {
    kprintf("%s: read(64) failed: %m\n", prog);
    exit(16);
  }
  close(fd);
  return e.e_type == ET_DYN &&  //
         READ32LE(e.e_ident) == READ32LE(ELFMAG);
}

void Unveil(const char *path, const char *perm) {
  if (unveil(path, perm) == -1) {
    kprintf("error: unveil(%#s, %#s) failed: %m\n", path, perm);
    _Exit(20);
  }
}

void UnveilIfExists(const char *path, const char *perm) {
  int err;
  if (path) {
    err = errno;
    if (unveil(path, perm) == -1) {
      if (errno == ENOENT) {
        errno = err;
      } else {
        kprintf("error: unveil(%#s, %#s) failed: %m\n", path, perm);
        _Exit(20);
      }
    }
  }
}

void MakeProcessNice(void) {
  if (!g_nice) return;
  if (setpriority(PRIO_PROCESS, 0, 19) == -1) {
    kprintf("error: setpriority(PRIO_PROCESS, 0, 19) failed: %m\n");
    exit(23);
  }
  if (ioprio_set(IOPRIO_WHO_PROCESS, 0,
                 IOPRIO_PRIO_VALUE(IOPRIO_CLASS_IDLE, 0)) == -1) {
    kprintf("error: ioprio_set() failed: %m\n");
    exit(23);
  }
  struct sched_param p = {sched_get_priority_min(SCHED_IDLE)};
  if (sched_setscheduler(0, SCHED_IDLE, &p) == -1) {
    kprintf("error: sched_setscheduler(SCHED_IDLE) failed: %m\n");
    exit(23);
  }
}

void ApplyFilesystemPolicy(unsigned long ipromises) {

  if (!SupportsLandlock()) {
    if (unveils.n) {
      kprintf("error: the unveil() -v flag needs Linux 5.13+\n");
      _Exit(20);
    }
  }

  Unveil(prog, "rx");

  if (IsDynamicExecutable(prog)) {
    UnveilIfExists("/lib", "rx");
    UnveilIfExists("/lib64", "rx");
    UnveilIfExists("/usr/lib", "rx");
    UnveilIfExists("/usr/lib64", "rx");
    UnveilIfExists("/usr/local/lib", "rx");
    UnveilIfExists("/usr/local/lib64", "rx");
    UnveilIfExists("/etc/ld-musl-x86_64.path", "r");
    UnveilIfExists("/etc/ld.so.conf", "r");
    UnveilIfExists("/etc/ld.so.cache", "r");
    UnveilIfExists("/etc/ld.so.conf.d", "r");
    UnveilIfExists("/etc/ld.so.preload", "r");
  }

  if (~ipromises & (1ul << PROMISE_STDIO)) {
    UnveilIfExists("/dev/fd", "r");
    UnveilIfExists("/dev/log", "w");
    UnveilIfExists("/dev/zero", "r");
    UnveilIfExists("/dev/null", "rw");
    UnveilIfExists("/dev/full", "rw");
    UnveilIfExists("/dev/stdin", "rw");
    UnveilIfExists("/dev/stdout", "rw");
    UnveilIfExists("/dev/stderr", "rw");
    UnveilIfExists("/dev/urandom", "r");
    UnveilIfExists("/dev/localtime", "r");
    UnveilIfExists("/proc/self/fd", "rw");
    UnveilIfExists("/proc/self/stat", "r");
    UnveilIfExists("/proc/self/status", "r");
    UnveilIfExists("/usr/share/locale", "r");
    UnveilIfExists("/proc/self/cmdline", "r");
    UnveilIfExists("/usr/share/zoneinfo", "r");
    UnveilIfExists("/proc/sys/kernel/version", "r");
    UnveilIfExists("/usr/share/common-licenses", "r");
    UnveilIfExists("/proc/sys/kernel/ngroups_max", "r");
    UnveilIfExists("/proc/sys/kernel/cap_last_cap", "r");
    UnveilIfExists("/proc/sys/vm/overcommit_memory", "r");
  }

  if (~ipromises & (1ul << PROMISE_INET)) {
    UnveilIfExists("/etc/ssl/certs/ca-certificates.crt", "r");
  }

  if (~ipromises & (1ul << PROMISE_RPATH)) {
    UnveilIfExists("/proc/filesystems", "r");
  }

  if (~ipromises & (1ul << PROMISE_DNS)) {
    UnveilIfExists("/etc/hosts", "r");
    UnveilIfExists("/etc/hostname", "r");
    UnveilIfExists("/etc/services", "r");
    UnveilIfExists("/etc/protocols", "r");
    UnveilIfExists("/etc/resolv.conf", "r");
  }

  if (~ipromises & (1ul << PROMISE_TTY)) {
    UnveilIfExists(ttyname(0), "rw");  // 1-up apparmor
    UnveilIfExists("/etc/tty", "rw");
    UnveilIfExists("/etc/console", "rw");
    UnveilIfExists("/usr/share/terminfo", "r");
  }

  if (~ipromises & (1ul << PROMISE_PROT_EXEC)) {
    UnveilIfExists("/usr/bin/ape", "rx");
  }

  if (~ipromises & (1ul << PROMISE_VMINFO)) {
    UnveilIfExists("/proc/stat", "r");
    UnveilIfExists("/proc/meminfo", "r");
    UnveilIfExists("/proc/cpuinfo", "r");
    UnveilIfExists("/proc/diskstats", "r");
    UnveilIfExists("/proc/self/maps", "r");
    UnveilIfExists("/sys/devices/system/cpu", "r");
  }

  for (int i = 0; i < unveils.n; ++i) {
    char *s, *t;
    const char *path;
    const char *perm;
    s = unveils.p[i];
    if ((t = strchr(s, ':'))) {
      *t = 0;
      perm = s;
      path = t + 1;
    } else {
      perm = "r";
      path = s;
    }
    Unveil(path, perm);
  }

  if (unveil(0, 0) == -1) {
    kprintf("error: unveil(0, 0) failed: %m\n");
    _Exit(20);
  }
}

int main(int argc, char *argv[]) {
  bool hasfunbits;
  int useruid, usergid;
  int owneruid, ownergid;
  int oldfsuid, oldfsgid;
  unsigned long ipromises;

  if (!IsLinux()) {
    kprintf("error: this program is only intended for linux\n");
    exit(5);
  }

  // parse flags
  GetOpts(argc, argv);
  if (optind == argc) {
    kprintf("error: too few args\n");
    write(2, USAGE, sizeof(USAGE) - 1);
    exit(64);
  }

  if (!g_noclose) {
    NormalizeFileDescriptors();
  }

  // set resource limits
  MakeProcessNice();
  SetCpuLimit(g_cpuquota);
  SetFszLimit(g_fszquota);
  SetMemLimit(g_memquota);
  SetProLimit(g_proquota);

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
      _Exit(6);
    }
  }

  // check if user has permission to chroot directory
  if (hasfunbits) {
    oldfsuid = setfsuid(useruid);
    oldfsgid = setfsgid(usergid);
    if (access(g_chroot, R_OK) == -1) {
      kprintf("error: access(%#s) failed: %m\n", g_chroot);
      _Exit(7);
    }
    setfsuid(oldfsuid);
    setfsgid(oldfsgid);
  }

  // change root fs path
  if (g_chroot) {
    if (chdir(g_chroot) == -1) {
      kprintf("error: chdir(%#s) failed: %m\n", g_chroot);
      _Exit(8);
    }
    if (chroot(g_chroot) == -1) {
      kprintf("error: chroot(%#s) failed: %m\n", g_chroot);
      _Exit(9);
    }
  }

  // find program
  if (hasfunbits) {
    oldfsuid = setfsuid(useruid);
    oldfsgid = setfsgid(usergid);
  }
  if (!(prog = commandv(argv[optind], pathbuf, sizeof(pathbuf)))) {
    kprintf("error: command not found: %m\n", argv[optind]);
    _Exit(10);
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
      _Exit(11);
    }
    if (getgid() != usergid || getegid() != usergid) {
      kprintf("error: setgid() broken\n");
      _Exit(12);
    }
  } else if (g_gflag) {
    // otherwise we trust the gid flag
    if (setgid(g_gflag) == -1) {
      kprintf("error: setgid(%d) failed: %m\n", g_gflag);
      _Exit(13);
    }
    if (getgid() != g_gflag || getegid() != g_gflag) {
      kprintf("error: setgid() broken\n");
      _Exit(14);
    }
  }

  // set user id
  if (useruid != owneruid) {
    // setuid binaries must use the uid of the user that ran it
    if (setuid(useruid) == -1) {
      kprintf("error: setuid(%d) failed: %m\n", useruid);
      _Exit(15);
    }
    if (getuid() != useruid || geteuid() != useruid) {
      kprintf("error: setuid() broken\n");
      _Exit(16);
    }
  } else if (g_uflag) {
    // otherwise we trust the uid flag
    if (setuid(g_uflag) == -1) {
      kprintf("error: setuid(%d) failed: %m\n", g_uflag);
      _Exit(17);
    }
    if (getuid() != g_uflag || geteuid() != g_uflag) {
      kprintf("error: setuid() broken\n");
      _Exit(18);
    }
  }

  if (ParsePromises(g_promises, &ipromises) == -1) {
    kprintf("error: bad promises list: %s\n", g_promises);
    _Exit(21);
  }

  ApplyFilesystemPolicy(ipromises);

  // we always need exec which is a weakness of this model
  if (!(~ipromises & (1ul << PROMISE_EXEC))) {
    g_promises = xstrcat(g_promises, ' ', "exec");
  }

  // apply sandbox
  if (pledge(g_promises, g_promises) == -1) {
    kprintf("error: pledge(%#s) failed: %m\n", g_promises);
    _Exit(19);
  }

  // launch program
  sys_execve(prog, argv + optind, environ);
  kprintf("%s: execve failed: %m\n", prog);
  return 127;
}
