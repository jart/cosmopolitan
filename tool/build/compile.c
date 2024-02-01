/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/struct/winsize.h"
#include "libc/calls/termios.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/fmt/libgen.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/limits.h"
#include "libc/log/appendresourcereport.internal.h"
#include "libc/log/color.internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/alg.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/kcpuids.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/nexgen32e/x86info.h"
#include "libc/proc/posix_spawn.h"
#include "libc/runtime/runtime.h"
#include "libc/serialize.h"
#include "libc/stdio/append.h"
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
#include "libc/thread/thread.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "third_party/getopt/getopt.internal.h"

#ifndef NDEBUG
__static_yoink("zipos");
#endif

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
  -P PROCS     set pro limit [default 4096]\n\
  -S BYTES     set stk limit [default 8m]\n\
  -M BYTES     set mem limit [default 2048m]\n\
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
  int n;
  int c;
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
long stkquota;
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
char buf[4096];
sigset_t savemask;
char tmpout[PATH_MAX];
posix_spawnattr_t spawnattr;
posix_spawn_file_actions_t spawnfila;

char *g_tmpout;
const char *g_tmpout_original;

const char *const kSafeEnv[] = {
    "ADDR2LINE",   // needed by GetAddr2linePath
    "HOME",        // needed by ~/.runit.psk
    "HOMEDRIVE",   // needed by ~/.runit.psk
    "HOMEPATH",    // needed by ~/.runit.psk
    "MAKEFLAGS",   // needed by IsRunningUnderMake
    "MODE",        // needed by test scripts
    "PATH",        // needed by clang
    "PWD",         // just seems plain needed
    "STRACE",      // useful for troubleshooting
    "TERM",        // needed to detect colors
    "TMPDIR",      // needed by compiler
    "SYSTEMROOT",  // needed by socket()
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
  appends(&output, "make MODE=");
  appends(&output, mode);
  appends(&output, " -j");
  appendd(&output, buf, FormatUint64(buf, __get_cpu_count()) - buf);
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
    tcgetwinsize(2, &ws);
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
  } else if (tcgetwinsize(2, &ws) != -1) {
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
    m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
    if (IsWindows()) {
      x = strncasecmp(s, kSafeEnv[m], n);
    } else {
      x = strncmp(s, kSafeEnv[m], n);
    }
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

char *Slurp(const char *path) {
  int fd;
  char *res = 0;
  if ((fd = open(path, O_RDONLY)) != -1) {
    ssize_t size;
    if ((size = lseek(fd, 0, SEEK_END)) != -1) {
      char *buf;
      if ((buf = calloc(1, size + 1))) {
        if (pread(fd, buf, size, 0) == size) {
          res = buf;
        } else {
          free(buf);
        }
      }
    }
    close(fd);
  }
  return res;
}

bool HasFlag(const char *flags, const char *s) {
  char buf[256];
  size_t n = strlen(s);
  if (!flags) return false;
  if (n + 2 > sizeof(buf)) return false;
  memcpy(buf, s, n);
  buf[n] = '\n';
  buf[n + 1] = 0;
  return !!strstr(flags, buf);
}

bool IsGccOnlyFlag(const char *s) {
  if (s[0] == '-') {
    if (s[1] == 'f') {
      if (startswith(s, "-ffixed-")) return true;
      if (startswith(s, "-fcall-saved")) return true;
      if (startswith(s, "-fcall-used")) return true;
      if (startswith(s, "-fgcse-")) return true;
      if (startswith(s, "-fvect-cost-model=")) return true;
      if (startswith(s, "-fsimd-cost-model=")) return true;
      if (startswith(s, "-fopt-info")) return true;
    }
    if (startswith(s, "-mstringop-strategy=")) return true;
    if (startswith(s, "-mpreferred-stack-boundary=")) return true;
    if (startswith(s, "-Wframe-larger-than=")) return true;
    if (startswith(s, "-Walloca-larger-than=")) return true;
  }
  static bool once;
  static char *gcc_only_flags;
  if (!once) {
    gcc_only_flags = Slurp("build/bootstrap/gcc-only-flags.txt");
    once = true;
  }
  return HasFlag(gcc_only_flags, s);
}

bool IsClangOnlyFlag(const char *s) {
  static bool once;
  static char *clang_only_flags;
  if (!once) {
    clang_only_flags = Slurp("build/bootstrap/clang-only-flags.txt");
    once = true;
  }
  return HasFlag(clang_only_flags, s);
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
  if (l->n == l->c) {
    if (l->c) {
      l->c += l->c >> 1;
    } else {
      l->c = 16;
    }
    l->p = realloc(l->p, (l->c + 1) * sizeof(*l->p));
  }
  l->p[l->n++] = s;
  l->p[l->n] = 0;
}

void AddEnv(char *s) {
  AddStr(&env, s);
}

char *StripPrefix(char *s, char *p) {
  if (startswith(s, p)) {
    return s + strlen(p);
  } else {
    return s;
  }
}

void AddArg(char *actual) {
  const char *s;
  if (actual == g_tmpout) {
    s = g_tmpout_original;
  } else {
    s = actual;
  }
  if (args.n) {
    appendw(&command, ' ');
  }
  appends(&command, s);
  if (!args.n) {
    appends(&shortened,
            StripPrefix(basename(gc(strdup(s))), "x86_64-linux-musl-"));
  } else if (*s != '-') {
    appendw(&shortened, ' ');
    if ((isar || isbfd || ispkg) &&
        (strcmp(args.p[args.n - 1], "-o") &&
         (endswith(s, ".o") || endswith(s, ".pkg") ||
          (endswith(s, ".a") && !isar)))) {
      appends(&shortened, basename(gc(strdup(s))));
    } else {
      appends(&shortened, s);
    }
  } else if (/*
              * a in ('-', '--') or
              * a.startswith('-o') or
              * c == 'ld' and a == '-T' or
              * c == 'cc' and a.startswith('-O') or
              * c == 'cc' and a.startswith('-x') or
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
  AddStr(&args, actual);
}

static int GetBaseCpuFreqMhz(void) {
  return KCPUIDS(16H, EAX) & 0x7fff;
}

void PlanResource(int resource, struct rlimit rlim) {
  struct rlimit prior;
  if (getrlimit(resource, &prior)) return;
  rlim.rlim_cur = MIN(rlim.rlim_cur, prior.rlim_max);
  rlim.rlim_max = MIN(rlim.rlim_max, prior.rlim_max);
  posix_spawnattr_setrlimit(&spawnattr, resource, &rlim);
}

void SetCpuLimit(int secs) {
  if (secs <= 0) return;
  if (IsWindows()) return;
#ifdef __x86_64__
  int mhz, lim;
  if (!(mhz = GetBaseCpuFreqMhz())) return;
  lim = ceil(3100. / mhz * secs);
  PlanResource(RLIMIT_CPU, (struct rlimit){lim, lim + 1});
#endif
}

void SetFszLimit(long n) {
  if (n <= 0) return;
  if (IsWindows()) return;
  PlanResource(RLIMIT_FSIZE, (struct rlimit){n, n + (n >> 1)});
}

void SetMemLimit(long n) {
  if (n <= 0) return;
  if (IsWindows() || IsXnu()) return;
  PlanResource(RLIMIT_AS, (struct rlimit){n, n});
}

void SetStkLimit(long n) {
  if (IsWindows()) return;
  if (n <= 0) return;
  n = MAX(n, PTHREAD_STACK_MIN * 2);
  PlanResource(RLIMIT_STACK, (struct rlimit){n, n});
}

void SetProLimit(long n) {
  if (n <= 0) return;
  PlanResource(RLIMIT_NPROC, (struct rlimit){n, n});
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
    perror(path);
    exit(1);
  }
}

int Launch(void) {
  size_t got;
  ssize_t rc;
  errno_t err;
  int ws, pid;
  uint64_t us;
  gotchld = 0;

  if (pipe2(pipefds, O_CLOEXEC) == -1) {
    perror("pipe2");
    exit(1);
  }

  posix_spawnattr_init(&spawnattr);
  posix_spawnattr_setsigmask(&spawnattr, &savemask);
  posix_spawnattr_setflags(&spawnattr,
                           POSIX_SPAWN_SETSIGMASK | POSIX_SPAWN_SETRLIMIT);
  SetCpuLimit(cpuquota);
  SetFszLimit(fszquota);
  SetMemLimit(memquota);
  SetStkLimit(stkquota);
  SetProLimit(proquota);

  posix_spawn_file_actions_init(&spawnfila);
  if (stdoutmustclose)
    posix_spawn_file_actions_adddup2(&spawnfila, pipefds[1], 1);
  posix_spawn_file_actions_adddup2(&spawnfila, pipefds[1], 2);

  clock_gettime(CLOCK_MONOTONIC, &start);
  if (timeout > 0) {
    timer.it_value.tv_sec = timeout;
    timer.it_interval.tv_sec = timeout;
    setitimer(ITIMER_REAL, &timer, 0);
  }

  err = posix_spawn(&pid, cmd, &spawnfila, &spawnattr, args.p, env.p);
  if (err) {
    tinyprint(2, program_invocation_short_name, ": failed to spawn ", cmd, ": ",
              strerror(err), " (see --strace for further details)\n", NULL);
    exit(1);
  }

  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  posix_spawn_file_actions_destroy(&spawnfila);
  posix_spawnattr_destroy(&spawnattr);
  close(pipefds[1]);

  for (;;) {
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
        res = false;
        break;
      }
      if (lseek(fdout, 0, SEEK_SET) == -1) {
        res = false;
        break;
      }
      res = copyfd(fdin, fdout, -1) != -1;
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

char *MakeTmpOut(const char *path) {
  int c;
  char *p = tmpout;
  char *e = tmpout + sizeof(tmpout) - 1;
  g_tmpout_original = path;
  p = stpcpy(p, __get_tmpdir());
  while ((c = *path++)) {
    if (c == '/') c = '_';
    if (p == e) {
      tinyprint(2, program_invocation_short_name,
                ": fatal error: MakeTmpOut() generated temporary filename "
                "that's too long: ",
                tmpout, "\n", NULL);
      exit(1);
    }
    *p++ = c;
  }
  *p = 0;
  g_tmpout = tmpout;
  return tmpout;
}

int main(int argc, char *argv[]) {
  uint64_t us;
  bool isineditor;
  size_t i, j, n, m;
  bool isproblematic;
  char *s, *q, **envp;
  int ws, opt, exitcode;

#ifdef MODE_DBG
  ShowCrashReports();
#endif

  mode = firstnonnull(getenv("MODE"), MODE);

  // parse prefix arguments
  verbose = 4;
  timeout = 90;                    // secs
  cpuquota = 32;                   // secs
  proquota = 4096;                 // procs
  stkquota = 8 * 1024 * 1024;      // bytes
  fszquota = 256 * 1000 * 1000;    // bytes
  memquota = 2048L * 1024 * 1024;  // bytes
  if ((s = getenv("V"))) verbose = atoi(s);
  while ((opt = getopt(argc, argv, "hnstvwA:C:F:L:M:O:P:T:V:S:")) != -1) {
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
      case 'S':
        stkquota = sizetol(optarg, 1024);
        break;
      case 'O':
        outquota = sizetol(optarg, 1024);
        break;
      case 'h':
        tinyprint(1, MANUAL, NULL);
        exit(0);
      default:
        tinyprint(2, MANUAL, NULL);
        exit(1);
    }
  }
  if (optind == argc) {
    tinyprint(2, program_invocation_short_name, ": missing arguments\n", NULL);
    exit(1);
  }

  // extend limits for slow UBSAN in particular
  if (!strcmp(mode, "dbg") || !strcmp(mode, "ubsan")) {
    cpuquota *= 2;
    fszquota *= 2;
    stkquota *= 2;
    memquota *= 2;
    timeout *= 2;
  }

  cmd = argv[optind];
  if (!strchr(cmd, '/')) {
    if (!(cmd = commandv(cmd, ccpath, sizeof(ccpath)))) exit(127);
  }

  s = basename(strdup(cmd));
  if (strstr(s, "clang") || strstr(s, "clang++")) {
    iscc = true;
    isclang = true;
  } else if (strstr(s, "gcc") || strstr(s, "g++")) {
    iscc = true;
    isgcc = true;
  } else if (strstr(s, "ld.bfd")) {
    isbfd = true;
  } else if (strstr(s, "ar.com")) {
    isar = true;
  } else if (strstr(s, "package.com")) {
    ispkg = true;
  }

  // ingest arguments
  for (i = optind; i < argc; ++i) {

    // replace output filename argument
    //
    // some commands (e.g. ar) don't use the `-o PATH` notation. in that
    // case we assume the output path was passed to compile.com -TTARGET
    // which means we can replace the appropriate command line argument.
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
    if (startswith(argv[i], "-o")) {
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
    if (isgcc && IsClangOnlyFlag(argv[i])) {
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

#ifdef __x86_64__
    } else if (!strcmp(argv[i], "-march=native")) {
      const struct X86ProcessorModel *model;
      if (X86_HAVE(XOP)) AddArg("-mxop");
      if (X86_HAVE(SSE4A)) AddArg("-msse4a");
      if (X86_HAVE(SSE3)) AddArg("-msse3");
      if (X86_HAVE(SSSE3)) AddArg("-mssse3");
      if (X86_HAVE(SSE4_1)) AddArg("-msse4.1");
      if (X86_HAVE(SSE4_2)) AddArg("-msse4.2");
      if (X86_HAVE(AVX)) AddArg("-mavx");
      if (X86_HAVE(AVX2)) {
        AddArg("-mavx2");
        if (isgcc) {
          AddArg("-msse2avx");
          AddArg("-Wa,-msse2avx");
        }
      }
      if (X86_HAVE(AVX512F)) AddArg("-mavx512f");
      if (X86_HAVE(AVX512PF)) AddArg("-mavx512pf");
      if (X86_HAVE(AVX512ER)) AddArg("-mavx512er");
      if (X86_HAVE(AVX512CD)) AddArg("-mavx512cd");
      if (X86_HAVE(AVX512VL)) AddArg("-mavx512vl");
      if (X86_HAVE(AVX512BW)) AddArg("-mavx512bw");
      if (X86_HAVE(AVX512DQ)) AddArg("-mavx512dq");
      if (X86_HAVE(AVX512IFMA)) AddArg("-mavx512ifma");
      if (X86_HAVE(AVX512VBMI)) AddArg("-mavx512vbmi");
      if (X86_HAVE(SHA)) AddArg("-msha");
      if (X86_HAVE(AES)) AddArg("-maes");
      if (X86_HAVE(VAES)) AddArg("-mvaes");
      if (X86_HAVE(PCLMUL)) AddArg("-mpclmul");
      if (X86_HAVE(FSGSBASE)) AddArg("-mfsgsbase");
      if (X86_HAVE(F16C)) AddArg("-mf16c");
      if (X86_HAVE(FMA)) AddArg("-mfma");
      if (X86_HAVE(POPCNT)) AddArg("-mpopcnt");
      if (X86_HAVE(BMI)) AddArg("-mbmi");
      if (X86_HAVE(BMI2)) AddArg("-mbmi2");
      if (X86_HAVE(ADX)) AddArg("-madx");
      if (X86_HAVE(FXSR)) AddArg("-mfxsr");
      if ((model = getx86processormodel(kX86ProcessorModelKey))) {
        switch (model->march) {
          case X86_MARCH_CORE2:
            AddArg("-march=core2");
            break;
          case X86_MARCH_NEHALEM:
            AddArg("-march=nehalem");
            break;
          case X86_MARCH_WESTMERE:
            AddArg("-march=westmere");
            break;
          case X86_MARCH_SANDYBRIDGE:
            AddArg("-march=sandybridge");
            break;
          case X86_MARCH_IVYBRIDGE:
            AddArg("-march=ivybridge");
            break;
          case X86_MARCH_HASWELL:
            AddArg("-march=haswell");
            break;
          case X86_MARCH_BROADWELL:
            AddArg("-march=broadwell");
            break;
          case X86_MARCH_SKYLAKE:
          case X86_MARCH_KABYLAKE:
            AddArg("-march=skylake");
            break;
          case X86_MARCH_CANNONLAKE:
            AddArg("-march=cannonlake");
            break;
          case X86_MARCH_ICELAKE:
            if (model->grade >= X86_GRADE_SERVER) {
              AddArg("-march=icelake-server");
            } else {
              AddArg("-march=icelake-client");
            }
            break;
          case X86_MARCH_TIGERLAKE:
            AddArg("-march=tigerlake");
            break;
          case X86_MARCH_BONNELL:
          case X86_MARCH_SALTWELL:
            AddArg("-march=bonnell");
            break;
          case X86_MARCH_SILVERMONT:
          case X86_MARCH_AIRMONT:
            AddArg("-march=silvermont");
            break;
          case X86_MARCH_GOLDMONT:
            AddArg("-march=goldmont");
            break;
          case X86_MARCH_GOLDMONTPLUS:
            AddArg("-march=goldmont-plus");
            break;
          case X86_MARCH_TREMONT:
            AddArg("-march=tremont");
            break;
          case X86_MARCH_KNIGHTSLANDING:
            AddArg("-march=knl");
            break;
          case X86_MARCH_KNIGHTSMILL:
            AddArg("-march=knm");
            break;
        }
      }
#endif /* __x86_64__ */

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
    } else if (startswith(argv[i], "-fsanitize=implicit") &&
               strstr(argv[i], "integer")) {
      if (isgcc) AddArg(argv[i]);
    } else if (strstr(argv[i], "stack-protector")) {
      if (isclang || (isgcc && ccversion >= 6)) {
        AddArg(argv[i]);
      }
    } else if (startswith(argv[i], "-fvect-cost") ||
               startswith(argv[i], "-mstringop") ||
               startswith(argv[i], "-gz") || strstr(argv[i], "sanitize") ||
               startswith(argv[i], "-fvect-cost") ||
               startswith(argv[i], "-fvect-cost")) {
      if (isgcc && ccversion >= 6) {
        AddArg(argv[i]);
      }
    } else if (startswith(argv[i], "-fdiagnostic-color=")) {
      colorflag = argv[i];
    } else if (startswith(argv[i], "-R") ||
               !strcmp(argv[i], "-fsave-optimization-record")) {
      if (isclang) AddArg(argv[i]);
    } else if (isclang && startswith(argv[i], "--debug-prefix-map")) {
      /* llvm doesn't provide a gas interface so simulate w/ clang */
      AddArg(xstrcat("-f", argv[i] + 2));
    } else if (isgcc && (!strcmp(argv[i], "-Os") || !strcmp(argv[i], "-O2") ||
                         !strcmp(argv[i], "-O3"))) {
      /* https://gcc.gnu.org/bugzilla/show_bug.cgi?id=97623 */
      AddArg(argv[i]);
      if (!isclang) {
        AddArg("-fno-code-hoisting");
      }
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

  // append special args
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

  // scrub environment for determinism and great justice
  for (envp = environ; *envp; ++envp) {
    if (startswith(*envp, "MODE=")) {
      mode = *envp + 5;
    }
    if (IsSafeEnv(*envp)) {
      AddEnv(*envp);
    }
  }
  AddEnv("LC_ALL=C");
  AddEnv("SOURCE_DATE_EPOCH=0");

  // ensure output directory exists
  if (outpath) {
    outdir = xdirname(outpath);
    if (!isdirectory(outdir)) {
      MakeDirs(outdir, 0755);
    }
  }

  // make sense of standard i/o file descriptors
  // we want to permit pipelines but prevent talking to terminal
  stdoutmustclose = fstat(1, &st) == -1 || S_ISCHR(st.st_mode);
  if (fstat(0, &st) == -1 || S_ISCHR(st.st_mode)) {
    close(0);
    open("/dev/null", O_RDONLY);
  }

  // SIGINT (CTRL-C) and SIGQUIT (CTRL-\) are delivered to the child
  // process, so we should ignore it and wait for the child to die.
  // SIGPIPE shouldn't happen until the very end since we buffer so it
  // is safe to let it kill the prog.
  sigemptyset(&mask);
  sigaddset(&mask, SIGINT);
  sigaddset(&mask, SIGQUIT);
  sigprocmask(SIG_BLOCK, &mask, &savemask);

  // we want SIGCHLD to interrupt the read() operation
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_NOCLDSTOP | SA_SIGINFO;
  sa.sa_sigaction = OnChld;
  sigaction(SIGCHLD, &sa, 0);

  // set a death clock if requested
  if (timeout > 0) {
    sa.sa_flags = 0;
    sa.sa_handler = OnAlrm;
    sigaction(SIGALRM, &sa, 0);
  }

  // run command
  ws = Launch();

  // propagate exit
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

  // describe command that was run
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

  // flush output
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
