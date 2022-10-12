/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/copyfile.h"
#include "libc/calls/ioctl.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/struct/winsize.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/limits.h"
#include "libc/log/appendresourcereport.internal.h"
#include "libc/log/color.internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/alg.h"
#include "libc/mem/copyfd.internal.h"
#include "libc/mem/gc.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/kcpuids.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/append.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/termios.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "third_party/getopt/getopt.h"

#define MANUAL \
  "\
SYNOPSIS\n\
\n\
  compile.com [FLAGS] COMMAND [ARGS...]\n\
\n\
OVERVIEW\n\
\n\
  Build Command Harness\n\
\n\
DESCRIPTION\n\
\n\
  This is a generic command wrapper, e.g.\n\
\n\
    compile.com gcc -o program program.c\n\
\n\
  This wrapper provides the following services:\n\
\n\
    - Logging latency and memory usage\n\
    - Ensures the output directory exists\n\
    - Discarding stderr when commands succeed\n\
    - Imposing cpu / memory / file size quotas\n\
    - Mapping stdin to /dev/null when not a file or fifo\n\
    - Buffering stderr to minimize build log interleaving\n\
    - Schlepping stdout into stderr when not a file or fifo\n\
    - Magic filtering of GCC vs. Clang flag incompatibilities\n\
    - Echo the launched subcommand (silent mode supported if V=0)\n\
    - Unzips the vendored GCC toolchain if it hasn't happened yet\n\
    - Making temporary copies of APE executables w/o side-effects\n\
    - Truncating long lines in \"TERM=dumb\" terminals like emacs\n\
\n\
  Programs running under make that don't wish to have their output\n\
  suppressed (e.g. unit tests with the -b benchmarking flag) shall\n\
  use the exit code `254` which is remapped to `0` with the output\n\
\n\
FLAGS\n\
\n\
  -t           touch target on success\n\
  -T TARGET    specifies target name for V=0 logging\n\
  -A ACTION    specifies short command name for V=0 logging\n\
  -V NUMBER    specifies compiler version\n\
  -C SECS      set cpu limit [default 16]\n\
  -L SECS      set lat limit [default 90]\n\
  -P PROCS     set pro limit [default 2048]\n\
  -M BYTES     set mem limit [default 512m]\n\
  -F BYTES     set fsz limit [default 256m]\n\
  -O BYTES     set out limit [default 1m]\n\
  -s           decrement verbosity [default 4]\n\
  -v           increments verbosity [default 4]\n\
  -n           do nothing (prime ape executable)\n\
  -w           disable landlock tmp workaround\n\
  -h           print help\n\
\n\
ENVIRONMENT\n\
\n\
  V=0          print shortened ephemerally\n\
  V=1          print shortened\n\
  V=2          print command\n\
  V=3          print shortened w/ wall+cpu+mem usage\n\
  V=4          print command w/ wall+cpu+mem usage\n\
  V=5          print output when exitcode is zero\n\
  COLUMNS=INT  explicitly set terminal width for output truncation\n\
  TERM=dumb    disable ansi x3.64 sequences and thousands separators\n\
\n"

struct Strings {
  size_t n;
  char **p;
};

bool isar;
bool iscc;
bool ispkg;
bool isgcc;
bool isbfd;
bool wantpg;
bool wantnop;
bool isclang;
bool wantnopg;
bool wantasan;
bool wantframe;
bool wantubsan;
bool wantfentry;
bool wantrecord;
bool fulloutput;
bool touchtarget;
bool noworkaround;
bool wantnoredzone;
bool stdoutmustclose;
bool no_sanitize_null;
bool no_sanitize_alignment;
bool no_sanitize_pointer_overflow;

int verbose;
int timeout;
int gotalrm;
int gotchld;
int ccversion;
int pipefds[2];

long cpuquota;
long fszquota;
long memquota;
long proquota;
long outquota;

char *cmd;
char *mode;
char *outdir;
char *action;
char *target;
char *output;
char *outpath;
char *command;
char *movepath;
char *shortened;
char *colorflag;
char ccpath[PATH_MAX];

struct stat st;
struct Strings env;
struct Strings args;
struct sigaction sa;
struct rusage usage;
struct timespec start;
struct timespec finish;
struct itimerval timer;
struct timespec signalled;

sigset_t mask;
sigset_t savemask;
char buf[PAGESIZE];
char tmpout[PATH_MAX];

const char *const kSafeEnv[] = {
    "ADDR2LINE",  // needed by GetAddr2linePath
    "HOME",       // needed by ~/.runit.psk
    "HOMEDRIVE",  // needed by ~/.runit.psk
    "HOMEPATH",   // needed by ~/.runit.psk
    "MAKEFLAGS",  // needed by IsRunningUnderMake
    "MODE",       // needed by test scripts
    "PATH",       // needed by clang
    "PWD",        // just seems plain needed
    "STRACE",     // useful for troubleshooting
    "TERM",       // needed to detect colors
    "TMPDIR",     // needed by compiler
};

const char *const kGccOnlyFlags[] = {
    "--nocompress-debug-sections",
    "--noexecstack",
    "-Wa,--nocompress-debug-sections",
    "-Wa,--noexecstack",
    "-Wa,-msse2avx",
    "-Wno-unused-but-set-variable",
    "-Wunsafe-loop-optimizations",
    "-fbranch-target-load-optimize",
    "-fcx-limited-range",
    "-fdelete-dead-exceptions",
    "-femit-struct-debug-baseonly",
    "-fipa-pta",
    "-fivopts",
    "-flimit-function-alignment",
    "-fmerge-constants",
    "-fmodulo-sched",
    "-fmodulo-sched-allow-regmoves",
    "-fno-align-jumps",
    "-fno-align-labels",
    "-fno-align-loops",
    "-fno-fp-int-builtin-inexact",
    "-fno-gnu-unique",
    "-fno-gnu-unique",
    "-fno-instrument-functions",
    "-fno-schedule-insns2",
    "-fno-whole-program",
    "-fopt-info-vec",
    "-fopt-info-vec-missed",
    "-freg-struct-return",
    "-freschedule-modulo-scheduled-loops",
    "-frounding-math",
    "-fsched2-use-superblocks",
    "-fschedule-insns",
    "-fschedule-insns2",
    "-fshrink-wrap",
    "-fshrink-wrap-separate",
    "-fsignaling-nans",
    "-fstack-clash-protection",
    "-ftracer",
    "-ftrapv",
    "-ftree-loop-im",
    "-ftree-loop-vectorize",
    "-funsafe-loop-optimizations",
    "-fversion-loops-for-strides",
    "-fwhole-program",
    "-gdescribe-dies",
    "-gstabs",
    "-mcall-ms2sysv-xlogues",
    "-mdispatch-scheduler",
    "-mfpmath=sse+387",
    "-mmitigate-rop",
    "-mno-fentry",
};

void OnAlrm(int sig) {
  ++gotalrm;
}

void OnChld(int sig, siginfo_t *si, void *ctx) {
  if (!gotchld++) {
    clock_gettime(CLOCK_MONOTONIC, &signalled);
  }
}

void PrintBold(void) {
  if (!__nocolor) {
    appends(&output, "\e[1m");
  }
}

void PrintRed(void) {
  if (!__nocolor) {
    appends(&output, "\e[91;1m");
  }
}

void PrintReset(void) {
  if (!__nocolor) {
    appends(&output, "\e[0m");
  }
}

void PrintMakeCommand(void) {
  const char *s;
  appends(&output, "make MODE=");
  appends(&output, mode);
  appends(&output, " -j");
  appendd(&output, buf, FormatUint64(buf, _getcpucount()) - buf);
  appendw(&output, ' ');
  appends(&output, target);
}

uint64_t GetTimevalMicros(struct timeval tv) {
  return tv.tv_sec * 1000000ull + tv.tv_usec;
}

uint64_t GetTimespecMicros(struct timespec ts) {
  return ts.tv_sec * 1000000ull + ts.tv_nsec / 1000;
}

ssize_t WriteAllUntilSignalledOrError(int fd, const char *p, size_t n) {
  ssize_t rc;
  size_t i, got;
  for (i = 0; i < n; i += got) {
    if ((rc = write(fd, p + i, n - i)) != -1) {
      got = rc;
    } else {
      return -1;
    }
  }
  return i;
}

int GetTerminalWidth(void) {
  char *s;
  struct winsize ws;
  if ((s = getenv("COLUMNS"))) {
    return atoi(s);
  } else {
    ws.ws_col = 0;
    ioctl(2, TIOCGWINSZ, &ws);
    return ws.ws_col;
  }
}

int GetLineWidth(bool *isineditor) {
  char *s;
  struct winsize ws = {0};
  s = getenv("COLUMNS");
  if (isineditor) {
    *isineditor = !!s;
  }
  if (s) {
    return atoi(s);
  } else if (ioctl(2, TIOCGWINSZ, &ws) != -1) {
    if (ws.ws_col && ws.ws_row) {
      return ws.ws_col * ws.ws_row / 3;
    } else {
      return 2048;
    }
  } else {
    return INT_MAX;
  }
}

bool IsSafeEnv(const char *s) {
  const char *p;
  int n, m, l, r, x;
  p = strchr(s, '=');
  n = p ? p - s : -1;
  l = 0;
  r = ARRAYLEN(kSafeEnv) - 1;
  while (l <= r) {
    m = (l + r) >> 1;
    x = strncmp(s, kSafeEnv[m], n);
    if (x < 0) {
      r = m - 1;
    } else if (x > 0) {
      l = m + 1;
    } else {
      return true;
    }
  }
  return false;
}

bool IsGccOnlyFlag(const char *s) {
  int m, l, r, x;
  l = 0;
  r = ARRAYLEN(kGccOnlyFlags) - 1;
  while (l <= r) {
    m = (l + r) >> 1;
    x = strcmp(s, kGccOnlyFlags[m]);
    if (x < 0) {
      r = m - 1;
    } else if (x > 0) {
      l = m + 1;
    } else {
      return true;
    }
  }
  if (_startswith(s, "-ffixed-")) return true;
  if (_startswith(s, "-fcall-saved")) return true;
  if (_startswith(s, "-fcall-used")) return true;
  if (_startswith(s, "-fgcse-")) return true;
  if (_startswith(s, "-fvect-cost-model=")) return true;
  if (_startswith(s, "-fsimd-cost-model=")) return true;
  if (_startswith(s, "-fopt-info")) return true;
  if (_startswith(s, "-mstringop-strategy=")) return true;
  if (_startswith(s, "-mpreferred-stack-boundary=")) return true;
  if (_startswith(s, "-Wframe-larger-than=")) return true;
  return false;
}

bool FileExistsAndIsNewerThan(const char *filepath, const char *thanpath) {
  struct stat st1, st2;
  if (stat(filepath, &st1) == -1) return false;
  if (stat(thanpath, &st2) == -1) return false;
  if (st1.st_mtim.tv_sec < st2.st_mtim.tv_sec) return false;
  if (st1.st_mtim.tv_sec > st2.st_mtim.tv_sec) return true;
  return st1.st_mtim.tv_nsec >= st2.st_mtim.tv_nsec;
}

static size_t TallyArgs(char **p) {
  size_t n;
  for (n = 0; *p; ++p) {
    n += sizeof(*p);
    n += strlen(*p) + 1;
  }
  return n;
}

void AddStr(struct Strings *l, char *s) {
  l->p = realloc(l->p, (++l->n + 1) * sizeof(*l->p));
  l->p[l->n - 1] = s;
  l->p[l->n - 0] = 0;
}

void AddEnv(char *s) {
  AddStr(&env, s);
}

char *StripPrefix(char *s, char *p) {
  if (_startswith(s, p)) {
    return s + strlen(p);
  } else {
    return s;
  }
}

void AddArg(char *s) {
  if (args.n) {
    appendw(&command, ' ');
  }
  appends(&command, s);
  if (!args.n) {
    appends(&shortened, StripPrefix(basename(s), "x86_64-linux-musl-"));
  } else if (*s != '-') {
    appendw(&shortened, ' ');
    if ((isar || isbfd || ispkg) &&
        (strcmp(args.p[args.n - 1], "-o") &&
         (_endswith(s, ".o") || _endswith(s, ".pkg") ||
          (_endswith(s, ".a") && !isar)))) {
      appends(&shortened, basename(s));
    } else {
      appends(&shortened, s);
    }
  } else if (/*
              * a in ('-', '--') or
              * a._startswith('-o') or
              * c == 'ld' and a == '-T' or
              * c == 'cc' and a._startswith('-O') or
              * c == 'cc' and a._startswith('-x') or
              * c == 'cc' and a in ('-c', '-E', '-S')
              */
             s[0] == '-' && (!s[1] || s[1] == 'o' || (s[1] == '-' && !s[2]) ||
                             (isbfd && (s[1] == 'T' && !s[2])) ||
                             (iscc && (s[1] == 'O' || s[1] == 'x' ||
                                       (!s[2] && (s[1] == 'c' || s[1] == 'E' ||
                                                  s[1] == 'S')))))) {
    appendw(&shortened, ' ');
    appends(&shortened, s);
  }
  AddStr(&args, s);
}

int GetBaseCpuFreqMhz(void) {
  return KCPUIDS(16H, EAX) & 0x7fff;
}

void SetCpuLimit(int secs) {
  int mhz, lim;
  struct rlimit rlim;
  if (secs <= 0) return;
  if (IsWindows()) return;
  if (!(mhz = GetBaseCpuFreqMhz())) return;
  lim = ceil(3100. / mhz * secs);
  rlim.rlim_cur = lim;
  rlim.rlim_max = lim + 1;
  if (setrlimit(RLIMIT_CPU, &rlim) == -1) {
    if (getrlimit(RLIMIT_CPU, &rlim) == -1) return;
    if (lim < rlim.rlim_cur) {
      rlim.rlim_cur = lim;
      setrlimit(RLIMIT_CPU, &rlim);
    }
  }
}

void SetFszLimit(long n) {
  struct rlimit rlim;
  if (n <= 0) return;
  if (IsWindows()) return;
  rlim.rlim_cur = n;
  rlim.rlim_max = n + (n >> 1);
  if (setrlimit(RLIMIT_FSIZE, &rlim) == -1) {
    if (getrlimit(RLIMIT_FSIZE, &rlim) == -1) return;
    rlim.rlim_cur = n;
    setrlimit(RLIMIT_FSIZE, &rlim);
  }
}

void SetMemLimit(long n) {
  struct rlimit rlim = {n, n};
  if (n <= 0) return;
  if (IsWindows() || IsXnu()) return;
  setrlimit(RLIMIT_AS, &rlim);
}

void SetProLimit(long n) {
  struct rlimit rlim = {n, n};
  if (n <= 0) return;
  setrlimit(RLIMIT_NPROC, &rlim);
}

bool ArgNeedsShellQuotes(const char *s) {
  if (*s) {
    for (;;) {
      switch (*s++ & 255) {
        case 0:
          return false;
        case '-':
        case '.':
        case '/':
        case '_':
        case '=':
        case ':':
        case '0' ... '9':
        case 'A' ... 'Z':
        case 'a' ... 'z':
          break;
        default:
          return true;
      }
    }
  } else {
    return true;
  }
}

char *AddShellQuotes(const char *s) {
  char *p, *q;
  size_t i, j, n;
  n = strlen(s);
  p = malloc(1 + n * 5 + 1 + 1);
  j = 0;
  p[j++] = '\'';
  for (i = 0; i < n; ++i) {
    if (s[i] != '\'') {
      p[j++] = s[i];
    } else {
      p[j + 0] = '\'';
      p[j + 1] = '"';
      p[j + 2] = '\'';
      p[j + 3] = '"';
      p[j + 4] = '\'';
      j += 5;
    }
  }
  p[j++] = '\'';
  p[j] = 0;
  if ((q = realloc(p, j + 1))) p = q;
  return p;
}

void MakeDirs(const char *path, int mode) {
  if (makedirs(path, mode)) {
    kprintf("error: makedirs(%#s) failed\n", path);
    exit(1);
  }
}

int Launch(void) {
  size_t got;
  ssize_t rc;
  int ws, pid;
  uint64_t us;
  gotchld = 0;

  if (pipe2(pipefds, O_CLOEXEC) == -1) {
    kprintf("pipe2 failed: %s\n", _strerrno(errno));
    exit(1);
  }

  clock_gettime(CLOCK_MONOTONIC, &start);
  if (timeout > 0) {
    timer.it_value.tv_sec = timeout;
    timer.it_interval.tv_sec = timeout;
    setitimer(ITIMER_REAL, &timer, 0);
  }

  pid = vfork();
  if (pid == -1) {
    kprintf("vfork failed: %s\n", _strerrno(errno));
    exit(1);
  }

#if 0
  int fd;
  size_t n;
  char b[1024], *p;
  size_t t = strlen(cmd) + 1 + TallyArgs(args.p) + 9 + TallyArgs(env.p) + 9;
  n = ksnprintf(b, sizeof(b), "%ld %s %s\n", t, cmd, outpath);
  fd = open("o/argmax.txt", O_APPEND | O_CREAT | O_WRONLY, 0644);
  write(fd, b, n);
  close(fd);
#endif

  if (!pid) {
    SetCpuLimit(cpuquota);
    SetFszLimit(fszquota);
    SetMemLimit(memquota);
    SetProLimit(proquota);
    if (stdoutmustclose) dup2(pipefds[1], 1);
    dup2(pipefds[1], 2);
    sigprocmask(SIG_SETMASK, &savemask, 0);
    execve(cmd, args.p, env.p);
    kprintf("execve(%#s) failed: %s\n", cmd, _strerrno(errno));
    _Exit(127);
  }
  close(pipefds[1]);

  for (;;) {
    if (gotchld) {
      rc = 0;
      break;
    }
    if (gotalrm) {
      PrintRed();
      appends(&output, "\n\n`");
      PrintMakeCommand();
      appends(&output, "` timed out after ");
      appendd(&output, buf, FormatInt64(buf, timeout) - buf);
      appends(&output, " seconds! ");
      PrintReset();
      kill(pid, SIGXCPU);
      rc = -1;
      break;
    }
    if ((rc = read(pipefds[0], buf, sizeof(buf))) != -1) {
      if (!(got = rc)) break;
      appendd(&output, buf, got);
      if (outquota > 0 && appendz(output).i > outquota) {
        kill(pid, SIGXFSZ);
        PrintRed();
        appendw(&output, '`');
        PrintMakeCommand();
        appends(&output, "` printed ");
        appendd(&output, buf,
                FormatUint64Thousands(buf, appendz(output).i) - buf);
        appends(&output, " bytes of output which exceeds the limit ");
        appendd(&output, buf, FormatUint64Thousands(buf, outquota) - buf);
        appendw(&output, READ16LE("! "));
        PrintReset();
        rc = -1;
        break;
      }
    } else if (errno == EINTR) {
      continue;
    } else {
      /* this should never happen */
      PrintRed();
      appends(&output, "error: compile.com read() failed w/ ");
      appendd(&output, buf, FormatInt64(buf, errno) - buf);
      PrintReset();
      appendw(&output, READ16LE(": "));
      kill(pid, SIGTERM);
      break;
    }
  }
  close(pipefds[0]);
  while (wait4(pid, &ws, 0, &usage) == -1) {
    if (errno == EINTR) {
      if (gotalrm > 1) {
        PrintRed();
        appends(&output, "and it willfully ignored our SIGXCPU signal! ");
        PrintReset();
        kill(pid, SIGKILL);
        gotalrm = 1;
      }
    } else if (errno == ECHILD) {
      break;
    } else {
      /* this should never happen */
      PrintRed();
      appends(&output, "error: compile.com wait4() failed w/ ");
      appendd(&output, buf, FormatInt64(buf, errno) - buf);
      PrintReset();
      appendw(&output, READ16LE(": "));
      ws = -1;
      break;
    }
  }
  clock_gettime(CLOCK_MONOTONIC, &finish);
  if (gotchld) {
    us = GetTimespecMicros(finish) - GetTimespecMicros(signalled);
    if (us > 1000000) {
      appends(&output, "wut: compile.com needed ");
      appendd(&output, buf, FormatUint64Thousands(buf, us) - buf);
      appends(&output, "µs to wait() for zombie ");
      rc = -1;
    }
    if (gotchld > 1) {
      appends(&output, "wut: compile.com got multiple sigchld?! ");
      rc = -1;
    }
  }
  return ws | rc;
}

void ReportResources(void) {
  uint64_t us;
  appendw(&output, '\n');
  if ((us = GetTimespecMicros(finish) - GetTimespecMicros(start))) {
    appends(&output, "consumed ");
    appendd(&output, buf, FormatUint64Thousands(buf, us) - buf);
    appends(&output, "µs wall time\n");
  }
  AppendResourceReport(&output, &usage, "\n");
  appendw(&output, '\n');
}

bool MovePreservingDestinationInode(const char *from, const char *to) {
  bool res;
  ssize_t rc;
  size_t remain;
  struct stat st;
  int fdin, fdout;
  if ((fdin = open(from, O_RDONLY)) == -1) {
    return false;
  }
  fstat(fdin, &st);
  if ((fdout = creat(to, st.st_mode)) == -1) {
    close(fdin);
    return false;
  }
  fadvise(fdin, 0, st.st_size, MADV_SEQUENTIAL);
  ftruncate(fdout, st.st_size);
  for (res = true, remain = st.st_size; remain;) {
    rc = copy_file_range(fdin, 0, fdout, 0, remain, 0);
    if (rc != -1) {
      remain -= rc;
    } else if (errno == EXDEV || errno == ENOSYS) {
      if (lseek(fdin, 0, SEEK_SET) == -1) {
        kprintf("%s: failed to lseek\n", from);
        res = false;
        break;
      }
      if (lseek(fdout, 0, SEEK_SET) == -1) {
        kprintf("%s: failed to lseek\n", to);
        res = false;
        break;
      }
      res = _copyfd(fdin, fdout, -1) != -1;
      break;
    } else {
      res = false;
      break;
    }
  }
  close(fdin);
  close(fdout);
  return res;
}

bool IsNativeExecutable(const char *path) {
  bool res;
  char buf[4];
  int got, fd;
  res = false;
  if ((fd = open(path, O_RDONLY)) != -1) {
    if ((got = read(fd, buf, 4)) == 4) {
      if (IsWindows()) {
        res = READ16LE(buf) == READ16LE("MZ");
      } else if (IsXnu()) {
        res = READ32LE(buf) == 0xFEEDFACEu + 1;
      } else {
        res = READ32LE(buf) == READ32LE("\177ELF");
      }
    }
    close(fd);
  }
  return res;
}

char *MakeTmpOut(const char *path) {
  int c;
  char *p = tmpout;
  char *e = tmpout + sizeof(tmpout) - 1;
  p = stpcpy(p, kTmpPath);
  while ((c = *path++)) {
    if (c == '/') c = '_';
    if (p == e) {
      kprintf("MakeTmpOut path too long: %s\n", tmpout);
      exit(1);
    }
    *p++ = c;
  }
  *p = 0;
  return tmpout;
}

int main(int argc, char *argv[]) {
  int columns;
  uint64_t us;
  bool isineditor;
  size_t i, j, n, m;
  bool isproblematic;
  int ws, opt, exitcode;
  char *s, *p, *q, **envp;

  mode = firstnonnull(getenv("MODE"), MODE);

  /*
   * parse prefix arguments
   */
  verbose = 4;
  timeout = 90;                 /* secs */
  cpuquota = 16;                /* secs */
  proquota = 2048;              /* procs */
  fszquota = 256 * 1000 * 1000; /* bytes */
  memquota = 512 * 1024 * 1024; /* bytes */
  if ((s = getenv("V"))) verbose = atoi(s);
  while ((opt = getopt(argc, argv, "hnstvwA:C:F:L:M:O:P:T:V:")) != -1) {
    switch (opt) {
      case 'n':
        exit(0);
      case 's':
        --verbose;
        break;
      case 'v':
        ++verbose;
        break;
      case 'A':
        action = optarg;
        break;
      case 'T':
        target = optarg;
        break;
      case 't':
        touchtarget = true;
        break;
      case 'w':
        noworkaround = true;
        break;
      case 'L':
        timeout = atoi(optarg);
        break;
      case 'C':
        cpuquota = atoi(optarg);
        break;
      case 'V':
        ccversion = atoi(optarg);
        break;
      case 'P':
        proquota = atoi(optarg);
        break;
      case 'F':
        fszquota = sizetol(optarg, 1000);
        break;
      case 'M':
        memquota = sizetol(optarg, 1024);
        break;
      case 'O':
        outquota = sizetol(optarg, 1024);
        break;
      case 'h':
        fputs(MANUAL, stdout);
        exit(0);
      default:
        fputs(MANUAL, stderr);
        exit(1);
    }
  }
  if (optind == argc) {
    fputs("error: missing arguments\n", stderr);
    exit(1);
  }

  /*
   * extend limits for slow UBSAN in particular
   */
  if (!strcmp(mode, "dbg") || !strcmp(mode, "ubsan")) {
    cpuquota *= 2;
    fszquota *= 2;
    memquota *= 2;
    timeout *= 2;
  }

  cmd = argv[optind];
  if (!strchr(cmd, '/')) {
    if (!(cmd = commandv(cmd, ccpath, sizeof(ccpath)))) exit(127);
  }

  s = basename(strdup(cmd));
  if (strstr(s, "gcc")) {
    iscc = true;
    isgcc = true;
  } else if (strstr(s, "clang")) {
    iscc = true;
    isclang = true;
  } else if (strstr(s, "ld.bfd")) {
    isbfd = true;
  } else if (strstr(s, "ar.com")) {
    isar = true;
  } else if (strstr(s, "package.com")) {
    ispkg = true;
  }

  /*
   * ingest arguments
   */
  for (i = optind; i < argc; ++i) {

    /*
     * replace output filename argument
     *
     * some commands (e.g. ar) don't use the `-o PATH` notation. in that
     * case we assume the output path was passed to compile.com -TTARGET
     * which means we can replace the appropriate command line argument.
     */
    if (!noworkaround &&  //
        !movepath &&      //
        !outpath &&       //
        target &&         //
        !strcmp(target, argv[i])) {
      AddArg(MakeTmpOut(argv[i]));
      outpath = target;
      movepath = target;
      MovePreservingDestinationInode(target, tmpout);
      continue;
    }

    /*
     * capture arguments
     */
    if (argv[i][0] != '-') {
      AddArg(argv[i]);
      continue;
    }

    /*
     * capture flags
     */
    if (_startswith(argv[i], "-o")) {
      if (!strcmp(argv[i], "-o")) {
        outpath = argv[++i];
      } else {
        outpath = argv[i] + 2;
      }
      AddArg("-o");
      if (noworkaround) {
        AddArg(outpath);
      } else {
        movepath = outpath;
        AddArg(MakeTmpOut(outpath));
      }
      continue;
    }
    if (!iscc) {
      AddArg(argv[i]);
      continue;
    }
    if (isclang && IsGccOnlyFlag(argv[i])) {
      continue;
    }
    if (!X86_HAVE(AVX) &&
        (!strcmp(argv[i], "-msse2avx") || !strcmp(argv[i], "-Wa,-msse2avx"))) {
      // Avoid any chance of people using Intel's older or low power
      // CPUs encountering a SIGILL error due to these awesome flags
      continue;
    }
    if (!strcmp(argv[i], "-w")) {
      AddArg(argv[i]);
      AddArg("-D__W__");
    } else if (!strcmp(argv[i], "-Oz")) {
      if (isclang) {
        AddArg(argv[i]);
      } else {
        AddArg("-Os");
      }
    } else if (!strcmp(argv[i], "-pg")) {
      wantpg = true;
    } else if (!strcmp(argv[i], "-x-no-pg")) {
      wantnopg = true;
    } else if (!strcmp(argv[i], "-mfentry")) {
      wantfentry = true;
    } else if (!strcmp(argv[i], "-mnop-mcount")) {
      wantnop = true;
    } else if (!strcmp(argv[i], "-mrecord-mcount")) {
      wantrecord = true;
    } else if (!strcmp(argv[i], "-fno-omit-frame-pointer")) {
      wantframe = true;
    } else if (!strcmp(argv[i], "-fomit-frame-pointer")) {
      wantframe = false;
    } else if (!strcmp(argv[i], "-mno-red-zone")) {
      wantnoredzone = true;
    } else if (!strcmp(argv[i], "-mred-zone")) {
      wantnoredzone = false;
    } else if (!strcmp(argv[i], "-mno-vzeroupper")) {
      if (isgcc) {
        AddArg("-Wa,-msse2avx");
        AddArg("-D__MNO_VZEROUPPER__");
      } else if (isclang) {
        AddArg("-mllvm");
        AddArg("-x86-use-vzeroupper=0");
      }
    } else if (!strcmp(argv[i], "-msse2avx")) {
      if (isgcc) {
        AddArg(argv[i]);
      }
    } else if (!strcmp(argv[i], "-fsanitize=address")) {
      if (isgcc && ccversion >= 6) wantasan = true;
    } else if (!strcmp(argv[i], "-fsanitize=undefined")) {
      if (isgcc && ccversion >= 6) wantubsan = true;
    } else if (!strcmp(argv[i], "-fno-sanitize=address")) {
      wantasan = false;
    } else if (!strcmp(argv[i], "-fno-sanitize=undefined")) {
      wantubsan = false;
    } else if (!strcmp(argv[i], "-fno-sanitize=all")) {
      wantasan = false;
      wantubsan = false;
    } else if (!strcmp(argv[i], "-fno-sanitize=null")) {
      if (isgcc && ccversion >= 6) no_sanitize_null = true;
    } else if (!strcmp(argv[i], "-fno-sanitize=alignment")) {
      if (isgcc && ccversion >= 6) no_sanitize_alignment = true;
    } else if (!strcmp(argv[i], "-fno-sanitize=pointer-overflow")) {
      if (isgcc && ccversion >= 6) no_sanitize_pointer_overflow = true;
    } else if (_startswith(argv[i], "-fsanitize=implicit") &&
               strstr(argv[i], "integer")) {
      if (isgcc) AddArg(argv[i]);
    } else if (_startswith(argv[i], "-fvect-cost") ||
               _startswith(argv[i], "-mstringop") ||
               _startswith(argv[i], "-gz") ||
               strstr(argv[i], "stack-protector") ||
               strstr(argv[i], "sanitize") ||
               _startswith(argv[i], "-fvect-cost") ||
               _startswith(argv[i], "-fvect-cost")) {
      if (isgcc && ccversion >= 6) {
        AddArg(argv[i]);
      }
    } else if (_startswith(argv[i], "-fdiagnostic-color=")) {
      colorflag = argv[i];
    } else if (_startswith(argv[i], "-R") ||
               !strcmp(argv[i], "-fsave-optimization-record")) {
      if (isclang) AddArg(argv[i]);
    } else if (isclang && _startswith(argv[i], "--debug-prefix-map")) {
      /* llvm doesn't provide a gas interface so simulate w/ clang */
      AddArg(xstrcat("-f", argv[i] + 2));
    } else if (isgcc && (!strcmp(argv[i], "-Os") || !strcmp(argv[i], "-O2") ||
                         !strcmp(argv[i], "-O3"))) {
      /* https://gcc.gnu.org/bugzilla/show_bug.cgi?id=97623 */
      AddArg(argv[i]);
      AddArg("-fno-code-hoisting");
    } else {
      AddArg(argv[i]);
    }
  }
  if (outpath) {
    if (!target) {
      target = outpath;
    }
  } else if (target) {
    outpath = target;
  } else {
    fputs("error: compile.com needs -TTARGET or -oOUTPATH\n", stderr);
    exit(7);
  }

  /*
   * append special args
   */
  if (iscc) {
    if (isclang) {
      AddArg("-Wno-unused-command-line-argument");
      AddArg("-Wno-incompatible-pointer-types-discards-qualifiers");
    }
    if (!__nocolor) {
      AddArg(firstnonnull(colorflag, "-fdiagnostics-color=always"));
    }
    if (wantpg && !wantnopg) {
      AddArg("-pg");
      AddArg("-D__PG__");
      if (wantnop && !isclang) {
        AddArg("-mnop-mcount");
        AddArg("-D__MNOP_MCOUNT__");
      }
      if (wantrecord) {
        AddArg("-mrecord-mcount");
        AddArg("-D__MRECORD_MCOUNT__");
      }
      if (wantfentry) {
        AddArg("-mfentry");
        AddArg("-D__MFENTRY__");
      }
    }
    if (wantasan) {
      AddArg("-fsanitize=address");
      /* compiler adds this by default */
      /* AddArg("-D__SANITIZE_ADDRESS__"); */
    }
    if (wantubsan) {
      AddArg("-fsanitize=undefined");
      AddArg("-fno-data-sections");
      AddArg("-D__SANITIZE_UNDEFINED__");
    }
    if (no_sanitize_null) {
      AddArg("-fno-sanitize=null");
    }
    if (no_sanitize_alignment) {
      AddArg("-fno-sanitize=alignment");
    }
    if (no_sanitize_pointer_overflow) {
      AddArg("-fno-sanitize=pointer-overflow");
    }
    if (wantnoredzone) {
      AddArg("-mno-red-zone");
      AddArg("-D__MNO_RED_ZONE__");
    }
    if (wantframe) {
      AddArg("-fno-omit-frame-pointer");
      AddArg("-D__FNO_OMIT_FRAME_POINTER__");
    } else {
      AddArg("-fomit-frame-pointer");
    }
  }

  /*
   * scrub environment for determinism and great justice
   */
  for (envp = environ; *envp; ++envp) {
    if (_startswith(*envp, "MODE=")) {
      mode = *envp + 5;
    }
    if (IsSafeEnv(*envp)) {
      AddEnv(*envp);
    }
  }
  AddEnv("LC_ALL=C");
  AddEnv("SOURCE_DATE_EPOCH=0");

  /*
   * ensure output directory exists
   */
  if (outpath) {
    outdir = xdirname(outpath);
    if (!isdirectory(outdir)) {
      MakeDirs(outdir, 0755);
    }
  }

  /*
   * make sense of standard i/o file descriptors
   * we want to permit pipelines but prevent talking to terminal
   */
  stdoutmustclose = fstat(1, &st) == -1 || S_ISCHR(st.st_mode);
  if (fstat(0, &st) == -1 || S_ISCHR(st.st_mode)) {
    close(0);
    open("/dev/null", O_RDONLY);
  }

  /*
   * SIGINT (CTRL-C) and SIGQUIT (CTRL-\) are delivered to process group
   * so the correct thing to do is to do nothing, and wait for the child
   * to die as a result of those signals. SIGPIPE shouldn't happen until
   * the very end since we buffer so it is safe to let it kill the prog.
   * Most importantly we need SIGCHLD to interrupt the read() operation!
   */
  sigfillset(&mask);
  sigdelset(&mask, SIGILL);
  sigdelset(&mask, SIGBUS);
  sigdelset(&mask, SIGPIPE);
  sigdelset(&mask, SIGALRM);
  sigdelset(&mask, SIGSEGV);
  sigdelset(&mask, SIGCHLD);
  sigprocmask(SIG_BLOCK, &mask, &savemask);
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_NOCLDSTOP | SA_SIGINFO;
  sa.sa_sigaction = OnChld;
  if (sigaction(SIGCHLD, &sa, 0) == -1) exit(83);
  if (timeout > 0) {
    sa.sa_flags = 0;
    sa.sa_handler = OnAlrm;
    sigaction(SIGALRM, &sa, 0);
  }

  /*
   * run command
   */
  ws = Launch();

  /*
   * propagate exit
   */
  if (ws != -1) {
    if (WIFEXITED(ws)) {
      if (!(exitcode = WEXITSTATUS(ws)) || exitcode == 254) {
        if (touchtarget && target) {
          MakeDirs(xdirname(target), 0755);
          if (touch(target, 0644)) {
            exitcode = 90;
            appends(&output, "\nfailed to touch output file\n");
          }
        }
        if (movepath) {
          if (!MovePreservingDestinationInode(tmpout, movepath)) {
            unlink(tmpout);
            exitcode = 90;
            appends(&output, "\nfailed to move output file\n");
            appends(&output, tmpout);
            appends(&output, "\n");
            appends(&output, movepath);
            appends(&output, "\n");
          } else {
            unlink(tmpout);
          }
        }
      } else {
        appendw(&output, '\n');
        PrintRed();
        appendw(&output, '`');
        PrintMakeCommand();
        appends(&output, "` exited with ");
        appendd(&output, buf, FormatUint64(buf, exitcode) - buf);
        PrintReset();
        appendw(&output, READ16LE(":\n"));
      }
    } else if (WTERMSIG(ws) == SIGINT) {
      appendr(&output, 0);
      appends(&output, "\rinterrupted: ");
      PrintMakeCommand();
      appendw(&output, '\n');
      WriteAllUntilSignalledOrError(2, output, appendz(output).i);
      return 128 + SIGINT;
    } else {
      exitcode = 128 + WTERMSIG(ws);
      appendw(&output, '\n');
      PrintRed();
      appendw(&output, '`');
      PrintMakeCommand();
      appends(&output, "` terminated by ");
      appends(&output, strsignal(WTERMSIG(ws)));
      PrintReset();
      appendw(&output, READ16LE(":\n"));
      appends(&output, "env -i ");
      for (i = 0; i < env.n; ++i) {
        if (ArgNeedsShellQuotes(env.p[i])) {
          q = AddShellQuotes(env.p[i]);
          appends(&output, q);
          free(q);
        } else {
          appends(&output, env.p[i]);
        }
        appendw(&output, ' ');
      }
    }
  } else {
    exitcode = 89;
  }

  /*
   * describe command that was run
   */
  if (!exitcode || exitcode == 254) {
    if (exitcode == 254) {
      exitcode = 0;
      fulloutput = true;
    } else if (verbose < 5) {
      appendr(&output, 0);
      fulloutput = false;
    } else {
      fulloutput = !!appendz(output).i;
    }
    if (fulloutput) {
      ReportResources();
    }
    if (!__nocolor && ischardev(2)) {
      /* clear line forward */
      appendw(&output, READ32LE("\e[K"));
    }
    if (verbose < 1) {
      /* make silent mode, i.e. `V=0 make o//target` */
      appendr(&command, 0);
      if (!action) action = "BUILD";
      if (!outpath) outpath = shortened;
      n = strlen(action);
      appends(&command, action);
      do appendw(&command, ' '), ++n;
      while (n < 15);
      appends(&command, outpath);
      n += strlen(outpath);
      m = GetTerminalWidth();
      if (m > 3 && n > m) {
        appendd(&output, command, m - 3);
        appendw(&output, READ32LE("..."));
      } else {
        if (n < m && (__nocolor || !ischardev(2))) {
          while (n < m) appendw(&command, ' '), ++n;
        }
        appendd(&output, command, n);
      }
      appendw(&output, m > 0 ? '\r' : '\n');
    } else {
      n = 0;
      if (verbose >= 3) {
        /* make bonus mode (shows resource usage) */
        if (timeout > 0) {
          us = GetTimespecMicros(finish) - GetTimespecMicros(start);
          i = FormatUint64Thousands(buf, us) - buf;
          j = ceil(log10(timeout));
          j += (j - 1) / 3;
          j += 1 + 3;
          j += 1 + 3;
          while (i < j) appendw(&output, ' '), ++i;
          if (us > timeout * 1000000ull / 2) {
            if (us > timeout * 1000000ull) {
              PrintRed();
            } else {
              PrintBold();
            }
            appends(&output, buf);
            PrintReset();
          } else {
            appends(&output, buf);
          }
          appendw(&output, READ32LE("⏰ "));
          n += i + 2 + 1;
        }
        if (cpuquota > 0) {
          if (!(us = GetTimevalMicros(usage.ru_utime) +
                     GetTimevalMicros(usage.ru_stime))) {
            us = GetTimespecMicros(finish) - GetTimespecMicros(start);
          }
          i = FormatUint64Thousands(buf, us) - buf;
          j = ceil(log10(cpuquota));
          j += (j - 1) / 3;
          j += 1 + 3;
          j += 1 + 3;
          while (i < j) appendw(&output, ' '), ++i;
          if ((isproblematic = us > cpuquota * 1000000ull / 2)) {
            if (us > cpuquota * 1000000ull - (cpuquota * 1000000ull) / 5) {
              PrintRed();
            } else {
              PrintBold();
            }
          }
          appends(&output, buf);
          appendw(&output, READ32LE("⏳ "));
          if (isproblematic) {
            PrintReset();
          }
          n += i + 2 + 1;
        }
        if (memquota > 0) {
          i = FormatUint64Thousands(buf, usage.ru_maxrss) - buf;
          j = ceil(log10(memquota / 1024));
          j += (j - 1) / 3;
          while (i < j) appendw(&output, ' '), ++i;
          if ((isproblematic = usage.ru_maxrss * 1024 > memquota / 2)) {
            if (usage.ru_maxrss * 1024 > memquota - memquota / 5) {
              PrintRed();
            } else {
              PrintBold();
            }
          }
          appends(&output, buf);
          appendw(&output, READ16LE("k "));
          if (isproblematic) {
            PrintReset();
          }
          n += i + 1 + 1;
        }
        if (fszquota > 0) {
          us = usage.ru_inblock + usage.ru_oublock;
          i = FormatUint64Thousands(buf, us) - buf;
          while (i < 7) appendw(&output, ' '), ++i;
          appends(&output, buf);
          appendw(&output, READ32LE("iop "));
          n += i + 4;
        }
      }
      /* make normal mode (echos run commands) */
      if (verbose < 2 || verbose == 3) {
        command = shortened;
      }
      m = GetLineWidth(&isineditor);
      if (m > n + 3 && appendz(command).i > m - n) {
        if (isineditor) {
          if (m > n + 3 && appendz(shortened).i > m - n) {
            appendd(&output, shortened, m - n - 3);
            appendw(&output, READ32LE("..."));
          } else {
            appendd(&output, shortened, appendz(shortened).i);
          }
        } else {
          appendd(&output, command, m - n - 3);
          appendw(&output, READ32LE("..."));
        }
      } else {
        appendd(&output, command, appendz(command).i);
      }
      appendw(&output, '\n');
    }
  } else {
    n = appendz(command).i;
    if (n > 2048) {
      appendr(&command, (n = 2048));
      appendw(&command, READ32LE("..."));
    }
    appendd(&output, command, n);
    ReportResources();
  }

  /*
   * flush output
   */
  if (WriteAllUntilSignalledOrError(2, output, appendz(output).i) == -1) {
    if (errno == EINTR) {
      s = "notice: compile.com output truncated\n";
    } else {
      if (!exitcode) exitcode = 55;
      s = "error: compile.com failed to write result\n";
    }
    write(2, s, strlen(s));
  }

  /*
   * all done!
   */
  return exitcode;
}
