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
#include "libc/calls/pledge.h"
#include "ape/ape.h"
#include "libc/calls/calls.h"
#include "libc/calls/landlock.h"
#include "libc/calls/pledge.internal.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/sched_param.h"
#include "libc/calls/struct/seccomp.internal.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/sysinfo.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/elf/def.h"
#include "libc/elf/elf.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/elf/struct/phdr.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/serialize.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/promises.internal.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/alloca.h"
#include "libc/nexgen32e/kcpuids.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/sysparam.h"
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
#include "third_party/getopt/getopt.internal.h"

// MANUALLY TESTED BY RUNNING
//
//     test/tool/build/pledge_test.sh
//

__static_yoink("zipos");

#define USAGE \
  "\
usage: pledge [-hnN] PROG ARGS...\n\
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
Cosompolitan Pledge v1.9\n\
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

#ifdef __x86_64__
#define ARCH_NAME "x86_64"
#elif defined(__aarch64__)
#define ARCH_NAME "aarch64"
#else
#error "unsupported architecture"
#endif

enum Strategy {
  kStrategyNull,
  kStrategyStatic,
  kStrategyDynamic,
  kStrategyApe,
};

int g_gflag;
int g_uflag;
int g_kflag;
int g_hflag;
bool g_nice;
bool g_qflag;
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
char pledgevar[64];
char g_promises[256];
char dsopath[PATH_MAX];
char tmppath[PATH_MAX];
char preloadvar[PATH_MAX];

struct {
  int n;
  char *p[10000];
} unveils;

unsigned long HasPromise(unsigned long ipromises, int promise) {
  return ~ipromises & (1ul << promise);
}

long Atoi(const char *s) {
  long i;
  char *ep;
  errno = 0;
  i = strtol(s, &ep, 0);
  if (*ep || errno) {
    tinyprint(2, program_invocation_name, ": invalid integer: ", s, "\n", NULL);
    exit(1);
  }
  return i;
}

long ParseSiSize(const char *s, long b) {
  long i;
  errno = 0;
  i = sizetol(s, b);
  if (errno) {
    tinyprint(2, program_invocation_name, ": invalid size: ", s, "\n", NULL);
    exit(1);
  }
  return i;
}

void AddPromise(const char *s) {
  while (isspace(*s)) ++s;
  if (!*s) return;
  if (*g_promises) {
    strlcat(g_promises, " ", sizeof(g_promises));
  }
  if (strlcat(g_promises, s, sizeof(g_promises)) >= sizeof(g_promises)) {
    tinyprint(2, program_invocation_name, ": too many promises\n", NULL);
    exit(1);
  }
}

void GetOpts(int argc, char *argv[]) {
  int opt;
  struct sysinfo si;
  bool got_promise_flag = false;
  g_nfdquota = 64;
  g_fszquota = 256 * 1000 * 1000;
  if (!sysinfo(&si)) {
    g_memquota = si.totalram;
    g_proquota = __get_cpu_count() + si.procs;
  } else {
    g_proquota = __get_cpu_count() * 100;
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
        g_gflag = Atoi(optarg);
        break;
      case 'u':
        g_uflag = Atoi(optarg);
        break;
      case 'C':
        g_cpuquota = Atoi(optarg);
        break;
      case 'P':
        g_proquota = Atoi(optarg);
        break;
      case 'O':
        g_nfdquota = Atoi(optarg);
        break;
      case 'F':
        g_fszquota = ParseSiSize(optarg, 1024);
        break;
      case 'M':
        g_memquota = ParseSiSize(optarg, 1024);
        break;
      case 'p':
        AddPromise(optarg);
        got_promise_flag = true;
        break;
      case 'v':
        if (unveils.n == ARRAYLEN(unveils.p)) {
          tinyprint(2, program_invocation_name, ": too many unveils\n", NULL);
          exit(1);
        }
        unveils.p[unveils.n++] = optarg;
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
  if (!got_promise_flag) {
    stpcpy(g_promises, "stdio rpath");
  }
}

const char *g_prog;
char pathbuf[PATH_MAX];
struct pollfd pfds[256];

bool SupportsLandlock(void) {
  int e = errno;
  bool r = landlock_create_ruleset(0, 0, LANDLOCK_CREATE_RULESET_VERSION) >= 0;
  errno = e;
  return r;
}

int GetPollMaxFds(void) {
  int n;
  struct rlimit rl;
  if (!getrlimit(RLIMIT_NOFILE, &rl)) {
    n = rl.rlim_cur;
  } else {
    n = 64;
  }
  return MIN(ARRAYLEN(pfds), MAX(3, n));
}

void NormalizeFileDescriptors(void) {
  int i, n, fd;
  closefrom(3);  // faster and more secure if linux 5.9+ or bsd
  n = GetPollMaxFds();
  for (i = 0; i < n; ++i) {
    pfds[i].fd = i;
    pfds[i].events = POLLIN;
  }
  if (poll(pfds, n, 0) == -1) {
    perror("poll");
    exit(1);
  }
  for (i = 0; i < 3; ++i) {
    if (pfds[i].revents & POLLNVAL) {
      if ((fd = open("/dev/null", O_RDWR)) == -1) {
        perror("/dev/null");
        exit(2);
      }
      if (fd != i) {
        tinyprint(2, program_invocation_name, ": poll() or open() is broken\n",
                  NULL);
        exit(3);
      }
    }
  }
  for (i = 3; i < n; ++i) {
    if (~pfds[i].revents & POLLNVAL) {
      if (close(pfds[i].fd) == -1) {
        perror("close");
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

static int GetBaseCpuFreqMhz(void) {
  return KCPUIDS(16H, EAX) & 0x7fff;
}

int SetCpuLimit(int secs) {
#ifdef __x86_64__
  int mhz, lim;
  if (secs <= 0) return 0;
  if (!(mhz = GetBaseCpuFreqMhz())) return eopnotsupp();
  lim = ceil(3100. / mhz * secs);
  return SetLimit(RLIMIT_CPU, lim, lim);
#else
  return 0;
#endif
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
    perror(path);
    exit(20);
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
      perror(path);
      exit(20);
    }
  }
  return -1;
}

enum Strategy GetStrategy(void) {
  static enum Strategy strategy;
  if (strategy == kStrategyNull) {
    strategy = kStrategyStatic;
    int fd;
    if ((fd = open(g_prog, O_RDONLY)) != -1) {
      union {
        char magic[8];
        Elf64_Ehdr ehdr;
      } hdr = {0};
      if (pread(fd, &hdr, sizeof(hdr), 0) == sizeof(hdr)) {
        if (READ64LE(hdr.magic) == READ64LE("MZqFpD='") ||
            READ64LE(hdr.magic) == READ64LE("jartsr='") ||
            READ64LE(hdr.magic) == READ64LE("APEDBG='")) {
          strategy = kStrategyApe;
        } else if ((IsLinux() || IsFreebsd() || IsNetbsd() || IsOpenbsd()) &&
                   IsElf64Binary(&hdr.ehdr, sizeof(hdr))) {
          if (hdr.ehdr.e_type == ET_DYN) {
            strategy = kStrategyDynamic;
          } else {
            Elf64_Phdr phdrs[16];
            int count = MIN(hdr.ehdr.e_phnum, ARRAYLEN(phdrs));
            int bytes = count * sizeof(Elf64_Phdr);
            if (pread(fd, phdrs, bytes, hdr.ehdr.e_phoff) == bytes) {
              for (int i = 0; i < count; ++i) {
                if (phdrs[i].p_type == PT_INTERP ||
                    phdrs[i].p_type == PT_DYNAMIC) {
                  strategy = kStrategyDynamic;
                }
              }
            }
          }
        }
      }
      close(fd);
    }
  }
  return strategy;
}

void ApplyFilesystemPolicy(unsigned long ipromises) {
  const char *p;

  if (g_dontunveil) return;
  if (!SupportsLandlock()) return;

  Unveil(g_prog, "rx");

  if (GetStrategy() == kStrategyDynamic) {
    Unveil(dsopath, "rx");
    UnveilIfExists("/lib", "rx");
    UnveilIfExists("/lib64", "rx");
    UnveilIfExists("/usr/lib", "rx");
    UnveilIfExists("/usr/lib64", "rx");
    UnveilIfExists("/usr/local/lib", "rx");
    UnveilIfExists("/usr/local/lib64", "rx");
    UnveilIfExists("/etc/ld-musl-" ARCH_NAME ".path", "r");
    UnveilIfExists("/etc/ld.so.conf", "r");
    UnveilIfExists("/etc/ld.so.cache", "r");
    UnveilIfExists("/etc/ld.so.conf.d", "r");
    UnveilIfExists("/etc/ld.so.preload", "r");
    // in case musl is symlinked somewhere else
    UnveilIfExists("/lib/ld-musl-" ARCH_NAME ".so.1", "rx");
  }

  if (HasPromise(ipromises, PROMISE_STDIO)) {
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

  if (HasPromise(ipromises, PROMISE_INET)) {
    UnveilIfExists("/etc/ssl/certs/ca-certificates.crt", "r");
  }

  if (HasPromise(ipromises, PROMISE_RPATH)) {
    UnveilIfExists("/proc/filesystems", "r");
  }

  if (HasPromise(ipromises, PROMISE_DNS)) {
    UnveilIfExists("/etc/hosts", "r");
    UnveilIfExists("/etc/hostname", "r");
    UnveilIfExists("/etc/services", "r");
    UnveilIfExists("/etc/protocols", "r");
    UnveilIfExists("/etc/resolv.conf", "r");
  }

  if (HasPromise(ipromises, PROMISE_TTY)) {
    UnveilIfExists(ttyname(0), "rw");
    UnveilIfExists("/dev/tty", "rw");
    UnveilIfExists("/dev/console", "rw");
    UnveilIfExists("/etc/terminfo", "r");
    UnveilIfExists("/usr/lib/terminfo", "r");
    UnveilIfExists("/usr/share/terminfo", "r");
  }

  if (GetStrategy() == kStrategyApe) {
    if (UnveilIfExists("/usr/bin/ape", "rx") == -1) {
      char buf[PATH_MAX];
      if ((p = getenv("TMPDIR"))) {
        UnveilIfExists(
            __join_paths(buf, sizeof(buf), p, ".ape-" APE_VERSION_STR), "rx");
      }
      if ((p = getenv("HOME"))) {
        UnveilIfExists(
            __join_paths(buf, sizeof(buf), p, ".ape-" APE_VERSION_STR), "rx");
      }
    }
  }

  if (HasPromise(ipromises, PROMISE_VMINFO)) {
    UnveilIfExists("/proc/stat", "r");
    UnveilIfExists("/proc/meminfo", "r");
    UnveilIfExists("/proc/cpuinfo", "r");
    UnveilIfExists("/proc/diskstats", "r");
    UnveilIfExists("/proc/self/maps", "r");
    UnveilIfExists("/sys/devices/system/cpu", "r");
  }

  if (HasPromise(ipromises, PROMISE_TMPPATH)) {
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
    perror("unveil");
    exit(20);
  }
}

void DropCapabilities(void) {
  int e, i;
  if (!IsLinux()) return;
  for (e = errno, i = 0;; ++i) {
    if (prctl(PR_CAPBSET_DROP, i) == -1) {
      if (errno == EINVAL || errno == EPERM) {
        errno = e;
        break;
      } else {
        perror("prctl(PR_CAPBSET_DROP)");
        exit(25);
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
  if (copyfd(fdin, fdout, -1) == -1) {
    close(fdout);
    close(fdin);
    return -1;
  }
  return close(fdout) | close(fdin);
}

int CountEnviron(char **ep) {
  int res = 0;
  while (*ep++) ++res;
  return res;
}

int main(int argc, char *argv[]) {
  const char *s;
  bool hasfunbits;
  int useruid, usergid;
  int owneruid, ownergid;
  int oldfsuid, oldfsgid;
  unsigned long ipromises;

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
    tinyprint(2, g_test, ": unknown test\n", NULL);
    exit(2);
  }
  if (optind == argc) {
    tinyprint(2, "error: missing command\n", NULL);
    write(2, USAGE, sizeof(USAGE) - 1);
    exit(64);
  }

  // perform process setup
  if (!g_noclose) {
    NormalizeFileDescriptors();
  }
  if (g_nice) {
    verynice();
  }
  if (SetCpuLimit(g_cpuquota) == -1) {
    perror("setrlimit(RLIMIT_CPU)");
  }
  if (SetLimit(RLIMIT_FSIZE, g_fszquota, g_fszquota * 1.5) == -1) {
    perror("setrlimit(RLIMIT_FSIZE)");
  }
  if (SetLimit(RLIMIT_AS, g_memquota, g_memquota) == -1) {
    perror("setrlimit(RLIMIT_AS)");
  }
  if (SetLimit(RLIMIT_NPROC, g_proquota, g_proquota) == -1) {
    perror("setrlimit(RLIMIT_NPROC)");
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
      tinyprint(2, program_invocation_name,
                ": setuid flags forbidden on setuid binaries\n", NULL);
      exit(6);
    }
  }

  // check if user has permission to chroot directory
  if (hasfunbits && g_chroot) {
    oldfsuid = setfsuid(useruid);
    oldfsgid = setfsgid(usergid);
    if (access(g_chroot, R_OK) == -1) {
      perror(g_chroot);
      exit(7);
    }
    setfsuid(oldfsuid);
    setfsgid(oldfsgid);
  }

  // change root fs path
  if (g_chroot) {
    if (chdir(g_chroot) == -1) {
      perror(g_chroot);
      exit(8);
    }
    if (chroot(g_chroot) == -1) {
      perror(g_chroot);
      exit(9);
    }
  }

  // find program
  if (hasfunbits) {
    oldfsuid = setfsuid(useruid);
    oldfsgid = setfsgid(usergid);
  }
  if (!(g_prog = commandv(argv[optind], pathbuf, sizeof(pathbuf)))) {
    perror(argv[optind]);
    exit(10);
  }
  if (hasfunbits) {
    setfsuid(oldfsuid);
    setfsgid(oldfsgid);
  }

  // copy environment
  // to setup child environment
  // and remove variables we might create
  unsetenv("_PLEDGE");
  unsetenv("LD_PRELOAD");
  int child_environ_count = CountEnviron(environ);
  int child_environ_capacity = child_environ_count + 2 + 1;
  int child_environ_bytes = child_environ_capacity * sizeof(char *);
  char **child_environ = alloca(child_environ_bytes);
  CheckLargeStackAllocation(child_environ, child_environ_bytes);
  memcpy(child_environ, environ, child_environ_count * sizeof(char *));
  bzero(child_environ + child_environ_count,
        (child_environ_capacity - child_environ_count) * sizeof(char *));

  // figure out where we want the dso
  if (GetStrategy() == kStrategyDynamic) {
    if ((s = getenv("TMPDIR")) ||  //
        (s = getenv("HOME")) ||    //
        (s = ".")) {
      strlcpy(dsopath, s, sizeof(dsopath));
      strlcat(dsopath, "/.pledge-sandbox.so", sizeof(dsopath));
      if (!FileExistsAndIsNewerThan(dsopath, GetProgramExecutableName())) {
        errno = 0;
        char pidstr[21];
        FormatInt64(pidstr, getpid());
        strlcpy(tmppath, s, sizeof(tmppath));
        strlcat(tmppath, "/.pledge-sandbox.so.", sizeof(tmppath));
        strlcat(tmppath, pidstr, sizeof(tmppath));
        if (Extract("/zip/sandbox-" ARCH_NAME ".so", tmppath, 0755) == -1) {
          perror(tmppath);
          exit(1);
        }
        if (rename(tmppath, dsopath) == -1) {
          perror(dsopath);
          exit(1);
        }
      }
      stpcpy(preloadvar, "LD_PRELOAD=");
      strlcat(preloadvar, dsopath, sizeof(preloadvar));
      child_environ[child_environ_count++] = preloadvar;
    }
  }

  if (g_dontdrop) {
    if (hasfunbits) {
      tinyprint(2, program_invocation_name,
                ": -D flag forbidden on setuid binaries\n", NULL);
      exit(6);
    }
  } else {
    DropCapabilities();
  }

  // set group id
  if (usergid != ownergid) {
    // setgid binaries must use the gid of the user that ran it
    if (setgid(usergid) == -1) {
      perror("setgid");
      exit(11);
    }
    if (getgid() != usergid || getegid() != usergid) {
      tinyprint(2, "error: setgid() broken\n", NULL);
      exit(12);
    }
  } else if (g_gflag) {
    // otherwise we trust the gid flag
    if (setgid(g_gflag) == -1) {
      perror("setgid");
      exit(13);
    }
    if (getgid() != g_gflag || getegid() != g_gflag) {
      tinyprint(2, "error: setgid() broken\n", NULL);
      exit(14);
    }
  }

  // set user id
  if (useruid != owneruid) {
    // setuid binaries must use the uid of the user that ran it
    if (setuid(useruid) == -1) {
      perror("setuid");
      exit(15);
    }
    if (getuid() != useruid || geteuid() != useruid) {
      tinyprint(2, "error: setuid() broken\n", NULL);
      exit(16);
    }
  } else if (g_uflag) {
    // otherwise we trust the uid flag
    if (setuid(g_uflag) == -1) {
      perror("setuid");
      exit(17);
    }
    if (getuid() != g_uflag || geteuid() != g_uflag) {
      tinyprint(2, "error: setuid() broken\n", NULL);
      exit(18);
    }
  }

  // parse requested promises
  // further changes to g_promises will be *effective* transient promises
  if (ParsePromises(g_promises, &ipromises, 0) == -1) {
    tinyprint(2, program_invocation_name, ": bad promises list: ", g_promises,
              "\n", NULL);
    exit(21);
  }

  // perform unveiling
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

  // weaken system call policy to allow execution
  //
  // 1. we always need to pledge("exec") in order to use execve(). this
  //    is the primary disadvantage to using the `pledge` command to
  //    bolt security onto unsecured programs (as opposed to using
  //    pledge as it was intended, by having the program authors update
  //    their code to invoke the pledge() system call from within their
  //    programs at the appropriate moments)
  //
  // 2. we usually need to force pledge("rpath prot_exec") too; dynamic
  //    executables need it mmap() shared objects during initialization
  //    and actually portable executables need it so /usr/bin/ape can
  //    mmap() the ELF program headers. the only time we don't require
  //    `prot_exec` is when launching a native ELF PT_EXEC binaries,
  //    e.g. assimilated actually portable executables.
  //
  // 3. in some cases we can remove the `exec` and `prot_exec` promises
  //    later on in the loading process. on musl and glibc systems, we
  //    do that by injecting an LD_PRELOAD DSO which calls pledge()
  //    again with the requested promises.
  if (!HasPromise(ipromises, PROMISE_EXEC)) {
    AddPromise("exec");
    if (!g_qflag) {
      // TODO(jart): Fix me.
      // __pledge_mode |= PLEDGE_STDERR_LOGGING;
    }
  }
  if (GetStrategy() != kStrategyStatic) {
    if (!HasPromise(ipromises, PROMISE_RPATH)) {
      AddPromise("rpath");
    }
    if (!HasPromise(ipromises, PROMISE_PROT_EXEC)) {
      AddPromise("prot_exec");
    }
  }

  // pass parameters to injected dso
  if (GetStrategy() == kStrategyDynamic) {
    char *p = pledgevar;
    p = stpcpy(p, "_PLEDGE=");
    p = FormatInt64(p, ~ipromises);
    p = stpcpy(p, ",");
    p = FormatInt64(p, __pledge_mode);
    child_environ[child_environ_count++] = pledgevar;
  }

  // this limit needs to come last since unveil() opens fds
  if (SetLimit(RLIMIT_NOFILE, g_nfdquota, g_nfdquota) == -1) {
    perror("setrlimit(RLIMIT_NOFILE)");
    exit(1);
  }

  // apply sandbox
  if (pledge(g_promises, g_promises) == -1) {
    perror("pledge");
    exit(19);
  }

  // launch program
  if (!IsWindows()) {
    sys_execve(g_prog, argv + optind, child_environ);
  } else {
    sys_execve_nt(g_prog, argv + optind, child_environ);
  }
  perror(g_prog);
  return 127;
}
