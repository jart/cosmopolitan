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
#include "libc/calls/landlock.h"
#include "libc/calls/pledge.h"
#include "libc/calls/pledge.internal.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/sched_param.h"
#include "libc/calls/struct/seccomp.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/sysinfo.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/elf/def.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/promises.internal.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/copyfd.internal.h"
#include "libc/mem/gc.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/kcpuids.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ioprio.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/pr.h"
#include "libc/sysv/consts/prio.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/rlim.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/consts/sched.h"
#include "libc/sysv/errfuns.h"
#include "libc/x/x.h"
#include "third_party/getopt/getopt.h"

// MANUALLY TESTED BY RUNNING
//
//     test/tool/build/pledge_test.sh
//

STATIC_YOINK("strerror_wr");
STATIC_YOINK("zip_uri_support");

#define USAGE \
  "\
usage: pledge.com [-hnN] PROG ARGS...\n\
  -h              show help\n\
  -g GID          call setgid()\n\
  -u UID          call setuid()\n\
  -c PATH         call chroot()\n\
  -v [PERM:]PATH  call unveil(PATH, PERM[rwxc])\n\
  -V              disable unveiling (only pledge)\n\
  -q              disable stderr violation logging\n\
  -k              kill process rather than eperm'ing\n\
  -n              set maximum niceness\n\
  -D              don't drop capabilities\n\
  -N              don't normalize file descriptors\n\
  -C SECS         set cpu limit [default: inherited]\n\
  -M BYTES        set virtual memory limit [default: 4gb]\n\
  -O FILES        set file descriptor limit [default: 64]\n\
  -P PROCS        set process limit [default: preexisting + cpus]\n\
  -F BYTES        set individual file size limit [default: 4gb]\n\
  -T pledge       exits 0 if pledge() is supported by host system\n\
  -T unveil       exits 0 if unveil() is supported by host system\n\
  -p PLEDGE       may contain any of following separated by spaces\n\
     - stdio: allow stdio and benign system calls\n\
     - rpath: read-only path ops\n\
     - wpath: write path ops\n\
     - cpath: create path ops\n\
     - dpath: create special files\n\
     - chown: allows file ownership changes\n\
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
     - tmppath: allows /tmp, $TMPPATH, lstat, unlink\n\
\n\
pledge.com v1.8\n\
copyright 2022 justine alexandra roberts tunney\n\
notice licenses are embedded in the binary\n\
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
int g_kflag;
int g_hflag;
bool g_nice;
bool g_qflag;
bool isdynamic;
bool g_noclose;
long g_cpuquota;
long g_fszquota;
long g_nfdquota;
long g_memquota;
long g_proquota;
long g_dontdrop;
long g_dontunveil;
const char *g_test;
const char *g_chroot;
const char *g_promises;
char dsopath[PATH_MAX];
char tmppath[PATH_MAX];

struct {
  int n;
  char **p;
} unveils;

static void GetOpts(int argc, char *argv[]) {
  int opt;
  struct sysinfo si;
  g_promises = 0;
  g_nfdquota = 64;
  g_fszquota = 256 * 1000 * 1000;
  if (!sysinfo(&si)) {
    g_memquota = si.totalram;
    g_proquota = _getcpucount() + si.procs;
  } else {
    g_proquota = _getcpucount() * 100;
    g_memquota = 4L * 1024 * 1024 * 1024;
  }
  while ((opt = getopt(argc, argv, "hnqkNVT:p:u:g:c:C:D:P:M:F:O:v:")) != -1) {
    switch (opt) {
      case 'n':
        g_nice = true;
        break;
      case 'q':
        g_qflag = true;
        break;
      case 'k':
        g_kflag = true;
        break;
      case 'N':
        g_noclose = true;
        break;
      case 'D':
        g_dontdrop = true;
        break;
      case 'V':
        g_dontunveil = true;
        break;
      case 'T':
        g_test = optarg;
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
      case 'O':
        g_nfdquota = atoi(optarg);
        break;
      case 'F':
        errno = 0;
        g_fszquota = sizetol(optarg, 1000);
        if (errno) {
          kprintf("error: invalid size: -F %s\n", optarg);
          exit(1);
        }
        break;
      case 'M':
        errno = 0;
        g_memquota = sizetol(optarg, 1024);
        if (errno) {
          kprintf("error: invalid size: -F %s\n", optarg);
          exit(1);
        }
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
  int e, i, n, fd;
  n = GetPollMaxFds();
  e = errno;
  closefrom(3);  // more secure if linux 5.9+
  errno = e;
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

int SetLimit(int r, long lo, long hi) {
  struct rlimit old;
  struct rlimit lim = {lo, hi};
  if (r < 0 || r >= RLIM_NLIMITS) return 0;
  if (!setrlimit(r, &lim)) return 0;
  if (getrlimit(r, &old)) return -1;
  lim.rlim_cur = MIN(lim.rlim_cur, old.rlim_max);
  lim.rlim_max = MIN(lim.rlim_max, old.rlim_max);
  return setrlimit(r, &lim);
}

int GetBaseCpuFreqMhz(void) {
  return KCPUIDS(16H, EAX) & 0x7fff;
}

int SetCpuLimit(int secs) {
  int mhz, lim;
  if (secs <= 0) return 0;
  if (!(mhz = GetBaseCpuFreqMhz())) return eopnotsupp();
  lim = ceil(3100. / mhz * secs);
  return SetLimit(RLIMIT_CPU, lim, lim);
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

void Unveil(const char *path, const char *perm) {
  if (unveil(path, perm) == -1) {
    kprintf("error: unveil(%#s, %#s) failed: %m\n", path, perm);
    _Exit(20);
  }
}

int UnveilIfExists(const char *path, const char *perm) {
  int err;
  if (path) {
    err = errno;
    if (unveil(path, perm) != -1) {
      return 0;
    } else if (errno == ENOENT) {
      errno = err;
    } else {
      kprintf("error: unveil(%#s, %#s) failed: %m\n", path, perm);
      _Exit(20);
    }
  }
  return -1;
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
  const char *p;

  if (g_dontunveil) return;
  if (!SupportsLandlock()) return;

  Unveil(prog, "rx");

  if (isdynamic) {
    Unveil(dsopath, "rx");
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
    UnveilIfExists("/etc/localtime", "r");
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
    UnveilIfExists(ttyname(0), "rw");
    UnveilIfExists("/dev/tty", "rw");
    UnveilIfExists("/dev/console", "rw");
    UnveilIfExists("/etc/terminfo", "r");
    UnveilIfExists("/usr/lib/terminfo", "r");
    UnveilIfExists("/usr/share/terminfo", "r");
  }

  if (~ipromises & (1ul << PROMISE_PROT_EXEC)) {
    if (UnveilIfExists("/usr/bin/ape", "rx") == -1) {
      if ((p = getenv("TMPDIR"))) {
        UnveilIfExists(xjoinpaths(p, ".ape"), "rx");
      }
      if ((p = getenv("HOME"))) {
        UnveilIfExists(xjoinpaths(p, ".ape"), "rx");
      }
    }
  }

  if (~ipromises & (1ul << PROMISE_VMINFO)) {
    UnveilIfExists("/proc/stat", "r");
    UnveilIfExists("/proc/meminfo", "r");
    UnveilIfExists("/proc/cpuinfo", "r");
    UnveilIfExists("/proc/diskstats", "r");
    UnveilIfExists("/proc/self/maps", "r");
    UnveilIfExists("/sys/devices/system/cpu", "r");
  }

  if (~ipromises & (1ul << PROMISE_TMPPATH)) {
    UnveilIfExists("/tmp", "rwc");
    UnveilIfExists(getenv("TMPPATH"), "rwc");
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
    UnveilIfExists(path, perm);
  }

  if (unveil(0, 0) == -1) {
    kprintf("error: unveil(0, 0) failed: %m\n");
    _Exit(20);
  }
}

void DropCapabilities(void) {
  int e, i;
  for (e = errno, i = 0;; ++i) {
    if (prctl(PR_CAPBSET_DROP, i) == -1) {
      if (errno == EINVAL || errno == EPERM) {
        errno = e;
        break;
      } else {
        kprintf("error: prctl(PR_CAPBSET_DROP, %d) failed: %m\n", i);
        _Exit(25);
      }
    }
  }
}

bool FileExistsAndIsNewerThan(const char *filepath, const char *thanpath) {
  struct stat st1, st2;
  if (stat(filepath, &st1) == -1) return false;
  if (stat(thanpath, &st2) == -1) return false;
  if (st1.st_mtim.tv_sec < st2.st_mtim.tv_sec) return false;
  if (st1.st_mtim.tv_sec > st2.st_mtim.tv_sec) return true;
  return st1.st_mtim.tv_nsec >= st2.st_mtim.tv_nsec;
}

int Extract(const char *from, const char *to, int mode) {
  int fdin, fdout;
  if ((fdin = open(from, O_RDONLY)) == -1) return -1;
  if ((fdout = creat(to, mode)) == -1) {
    close(fdin);
    return -1;
  }
  if (_copyfd(fdin, fdout, -1) == -1) {
    close(fdout);
    close(fdin);
    return -1;
  }
  return close(fdout) | close(fdin);
}

int main(int argc, char *argv[]) {
  const char *s;
  bool hasfunbits;
  int fdin, fdout;
  char buf[PATH_MAX];
  int e, zipfd, memfd;
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
  if (g_test) {
    if (!strcmp(g_test, "pledge")) {
      if (IsOpenbsd() || (IsLinux() && __is_linux_2_6_23())) {
        exit(0);
      } else {
        exit(1);
      }
    }
    if (!strcmp(g_test, "unveil")) {
      if (IsOpenbsd() || (IsLinux() && SupportsLandlock())) {
        exit(0);
      } else {
        exit(1);
      }
    }
    kprintf("error: unknown test: %s\n", g_test);
    exit(2);
  }
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

  if (SetCpuLimit(g_cpuquota) == -1) {
    kprintf("error: setrlimit(%s) failed: %m\n", "RLIMIT_CPU");
    exit(1);
  }

  if (SetLimit(RLIMIT_FSIZE, g_fszquota, g_fszquota * 1.5) == -1) {
    kprintf("error: setrlimit(%s) failed: %m\n", "RLIMIT_FSIZE");
    exit(1);
  }

  if (SetLimit(RLIMIT_AS, g_memquota, g_memquota) == -1) {
    kprintf("error: setrlimit(%s) failed: %m\n", "RLIMIT_AS");
    exit(1);
  }

  if (SetLimit(RLIMIT_NPROC, g_proquota, g_proquota) == -1) {
    kprintf("error: setrlimit(%s) failed: %m\n", "RLIMIT_NPROC");
    exit(1);
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
      _Exit(6);
    }
  }

  // check if user has permission to chroot directory
  if (hasfunbits && g_chroot) {
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

  // figure out where we want the dso
  if (_IsDynamicExecutable(prog)) {
    isdynamic = true;
    if ((s = getenv("TMPDIR")) ||  //
        (s = getenv("HOME")) ||    //
        (s = ".")) {
      ksnprintf(dsopath, sizeof(dsopath), "%s/sandbox.so", s);
      if (!FileExistsAndIsNewerThan(dsopath, GetProgramExecutableName())) {
        ksnprintf(tmppath, sizeof(tmppath), "%s/sandbox.so.%d", s, getpid());
        if (Extract("/zip/sandbox.so", tmppath, 0755) == -1) {
          kprintf("error: extract dso failed: %m\n");
          exit(1);
        }
        if (rename(tmppath, dsopath) == -1) {
          kprintf("error: rename dso failed: %m\n");
          exit(1);
        }
      }
      ksnprintf(buf, sizeof(buf), "LD_PRELOAD=%s", dsopath);
      putenv(buf);
    }
  }

  if (g_dontdrop) {
    if (hasfunbits) {
      kprintf("error: -D flag forbidden on setuid binaries\n");
      _Exit(6);
    }
  } else {
    DropCapabilities();
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

  // pledge.com uses the return eperm instead of killing the process
  // model. we do this becasue it's only possible to have sigsys print
  // crash messages if we're not pledging exec, which is what this tool
  // always has to do currently.
  if (g_kflag) {
    __pledge_mode = PLEDGE_PENALTY_KILL_PROCESS;
  } else {
    __pledge_mode = PLEDGE_PENALTY_RETURN_EPERM;
  }

  // we need to be able to call execv and mmap the dso
  // it'll be pledged away once/if the dso gets loaded
  if (!(~ipromises & (1ul << PROMISE_EXEC))) {
    g_promises = xstrcat(g_promises, ' ', "exec");
    if (!g_qflag) {
      __pledge_mode |= PLEDGE_STDERR_LOGGING;
    }
  }
  if (isdynamic) {
    g_promises = xstrcat(g_promises, ' ', "prot_exec");
  }

  // pass arguments to pledge() inside the dso
  if (isdynamic) {
    ksnprintf(buf, sizeof(buf), "_PLEDGE=%ld,%ld", ~ipromises, __pledge_mode);
    putenv(buf);
  }

  if (SetLimit(RLIMIT_NOFILE, g_nfdquota, g_nfdquota) == -1) {
    kprintf("error: setrlimit(%s) failed: %m\n", "RLIMIT_NOFILE");
    exit(1);
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
