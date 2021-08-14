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
#include "libc/assert.h"
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/copyfile.h"
#include "libc/calls/sigbits.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigset.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/log/color.internal.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/kcpuids.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/append.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/consts/sig.h"
#include "libc/x/x.h"
#include "third_party/getopt/getopt.h"

long cpuquota = 8;                 /* secs */
long fszquota = 100 * 1000 * 1000; /* bytes */
long memquota = 256 * 1024 * 1024; /* bytes */

#define MANUAL \
  "\
SYNOPSIS\n\
\n\
  compile.com [FLAGS] COMMAND [ARGS...]\n\
\n\
OVERVIEW\n\
\n\
  Compiler Collection Frontend Frontend\n\
\n\
DESCRIPTION\n\
\n\
  This is a generic command wrapper, e.g.\n\
\n\
    compile.com gcc -o program program.c\n\
\n\
  This wrapper provides the following services:\n\
\n\
    - Ensures the output directory exists\n\
    - Echo the launched subcommand (silent mode supported if V=0)\n\
    - Magic filtering of GCC vs. Clang flag incompatibilities\n\
    - Unzips the vendored GCC toolchain if it hasn't happened yet\n\
    - Making temporary copies of APE executables w/o side-effects\n\
    - Truncating long lines in \"TERM=dumb\" terminals like emacs\n\
\n\
  This wrapper is extremely fast.\n\
\n\
FLAGS\n\
\n\
  -A ACTION    specifies short command name for V=0 logging\n\
  -T TARGET    specifies target name for V=0 logging\n\
  -V NUMBER    specifies compiler version\n\
  -t           touch target on success\n\
  -n           do nothing (used to prime the executable)\n\
  -?           print help\n\
\n"

struct Args {
  size_t n;
  char **p;
};

struct Command {
  size_t n;
  char *p;
};

bool iscc;
bool isclang;
bool isgcc;
bool wantasan;
bool wantfentry;
bool wantframe;
bool wantnop;
bool wantnopg;
bool wantpg;
bool wantrecord;
bool wantubsan;
bool touchtarget;
bool no_sanitize_null;
bool no_sanitize_alignment;
bool no_sanitize_pointer_overflow;

char *cmd;
char *comdbg;
char *cachedcmd;
char *originalcmd;
char *colorflag;
char *outdir;
char *outpath;
char *action;
char *target;
char ccpath[PATH_MAX];
int ccversion;
int columns;

sigset_t mask;
sigset_t savemask;

struct Args env;
struct Args args;
struct Command command;

const char *const kSafeEnv[] = {
    "ADDR2LINE",  // needed by GetAddr2linePath
    "HOME",       // needed by ~/.runit.psk
    "HOMEDRIVE",  // needed by ~/.runit.psk
    "HOMEPATH",   // needed by ~/.runit.psk
    "MAKEFLAGS",  // needed by IsRunningUnderMake
    "MODE",       // needed by test scripts
    "PATH",       // needed by clang
    "PWD",        // just seems plain needed
    "TERM",       // needed by IsTerminalInarticulate
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

char *DescribeCommand(void) {
  if (iscc) {
    if (isgcc) {
      return xasprintf("%s %d", "gcc", ccversion);
    } else if (isclang) {
      return xasprintf("%s %d", "clang", ccversion);
    }
  }
  return basename(cmd);
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
  if (startswith(s, "-ffixed-")) return true;
  if (startswith(s, "-fcall-saved")) return true;
  if (startswith(s, "-fcall-used")) return true;
  if (startswith(s, "-fgcse-")) return true;
  if (startswith(s, "-fvect-cost-model=")) return true;
  if (startswith(s, "-fsimd-cost-model=")) return true;
  if (startswith(s, "-fopt-info")) return true;
  if (startswith(s, "-mstringop-strategy=")) return true;
  if (startswith(s, "-mpreferred-stack-boundary=")) return true;
  if (startswith(s, "-Wframe-larger-than=")) return true;
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

void AddEnv(char *s) {
  env.p = realloc(env.p, ++env.n * sizeof(*env.p));
  env.p[env.n - 1] = s;
}

void AddArg(char *s) {
  size_t n;
  args.p = realloc(args.p, ++args.n * sizeof(*args.p));
  args.p[args.n - 1] = s;
  if (s) {
    n = strlen(s);
    if (command.n) {
      command.p = realloc(command.p, command.n + 1 + n);
      command.p[command.n] = ' ';
      memcpy(command.p + command.n + 1, s, n);
      command.n += 1 + n;
    } else {
      command.p = realloc(command.p, command.n + n);
      memcpy(command.p + command.n, s, n);
      command.n += n;
    }
  } else {
    command.p = realloc(command.p, command.n + 1);
    command.p[command.n++] = '\n';
  }
}

int GetBaseCpuFreqMhz(void) {
  return KCPUIDS(16H, EAX) & 0x7fff;
}

void SetCpuLimit(int secs) {
  int mhz;
  struct rlimit rlim;
  if (secs < 0) return;
  if (IsWindows()) return;
  if (!(mhz = GetBaseCpuFreqMhz())) return;
  if (getrlimit(RLIMIT_CPU, &rlim) == -1) return;
  rlim.rlim_cur = ceil(3100. / mhz * secs);
  setrlimit(RLIMIT_CPU, &rlim);
}

void SetFszLimit(long n) {
  struct rlimit rlim;
  if (n < 0) return;
  if (IsWindows()) return;
  if (getrlimit(RLIMIT_FSIZE, &rlim) == -1) return;
  rlim.rlim_cur = n;
  setrlimit(RLIMIT_FSIZE, &rlim);
}

void SetMemLimit(long n) {
  struct rlimit rlim = {n, n};
  if (n < 0) return;
  if (IsWindows() || IsXnu()) return;
  setrlimit(!IsOpenbsd() ? RLIMIT_AS : RLIMIT_DATA, &rlim);
}

int Launch(struct rusage *ru) {
  int ws, pid;
  if ((pid = vfork()) == -1) exit(errno);
  if (!pid) {
    SetCpuLimit(cpuquota);
    SetFszLimit(fszquota);
    SetMemLimit(memquota);
    sigprocmask(SIG_SETMASK, &savemask, NULL);
    execve(cmd, args.p, env.p);
    _exit(127);
  }
  while (wait4(pid, &ws, 0, ru) == -1) {
    if (errno != EINTR) exit(errno);
  }
  return ws;
}

char *GetResourceReport(struct rusage *ru) {
  char *report = 0;
  appendf(&report, "\n");
  AppendResourceReport(&report, ru, "\n");
  return report;
}

int main(int argc, char *argv[]) {
  size_t n;
  char *p, **envp;
  struct rusage ru;
  int i, ws, rc, opt;

  /*
   * parse prefix arguments
   */
  while ((opt = getopt(argc, argv, "hntC:M:F:A:T:V:")) != -1) {
    switch (opt) {
      case 'n':
        exit(0);
      case 't':
        touchtarget = true;
        break;
      case 'A':
        action = optarg;
        break;
      case 'T':
        target = optarg;
        break;
      case 'V':
        ccversion = atoi(optarg);
        break;
      case 'C':
        cpuquota = atoi(optarg);
        break;
      case 'M':
        memquota = sizetol(optarg, 1024);
        break;
      case 'F':
        fszquota = sizetol(optarg, 1000);
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

  cmd = argv[optind];
  if (!strchr(cmd, '/')) {
    if (!(cmd = commandv(cmd, ccpath))) exit(127);
  }

  isgcc = !!strstr(basename(cmd), "gcc");
  isclang = !!strstr(basename(cmd), "clang");
  iscc = isgcc | isclang;

  /*
   * ingest arguments
   */
  for (i = optind; i < argc; ++i) {
    if (argv[i][0] != '-') {
      AddArg(argv[i]);
      continue;
    }
    if (!strcmp(argv[i], "-o")) {
      AddArg(argv[i]);
      AddArg((outpath = argv[++i]));
      continue;
    }
    if (!iscc) {
      AddArg(argv[i]);
      continue;
    }
    if (isclang && IsGccOnlyFlag(argv[i])) {
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
    } else if (startswith(argv[i], "-fsanitize=implicit") &&
               strstr(argv[i], "integer")) {
      if (isgcc) AddArg(argv[i]);
    } else if (startswith(argv[i], "-fvect-cost") ||
               startswith(argv[i], "-mstringop") ||
               startswith(argv[i], "-gz") ||
               strstr(argv[i], "stack-protector") ||
               strstr(argv[i], "sanitize") ||
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
      AddArg(xasprintf("%s%s", "-f", argv[i] + 2));
    } else if (isgcc && (!strcmp(argv[i], "-Os") || !strcmp(argv[i], "-O2") ||
                         !strcmp(argv[i], "-O3"))) {
      /* https://gcc.gnu.org/bugzilla/show_bug.cgi?id=97623 */
      AddArg(argv[i]);
      AddArg("-fno-code-hoisting");
    } else {
      AddArg(argv[i]);
    }
  }
  if (!outpath) {
    outpath = target;
  }

  /*
   * append special args
   */
  if (iscc) {
    if (isclang) {
      AddArg("-Wno-unused-command-line-argument");
      AddArg("-Wno-incompatible-pointer-types-discards-qualifiers");
    }
    AddArg("-no-canonical-prefixes");
    if (!IsTerminalInarticulate()) {
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
      AddArg("-D__FSANITIZE_ADDRESS__");
    }
    if (wantubsan) {
      AddArg("-fsanitize=undefined");
      AddArg("-fno-data-sections");
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
    if (wantframe) {
      AddArg("-fno-omit-frame-pointer");
    }
  }

  /*
   * terminate args
   */
  AddArg(NULL);

  /*
   * scrub environment for determinism and great justice
   */
  for (envp = environ; *envp; ++envp) {
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
      makedirs(outdir, 0755);
    }
  }

  /*
   * log command being run
   */
  if (!strcmp(nulltoempty(getenv("V")), "0") && !IsTerminalInarticulate()) {
    p = (xasprintf)("\r\e[K%-15s%s\r", firstnonnull(action, "BUILD"),
                    firstnonnull(target, nulltoempty(outpath)));
    n = strlen(p);
  } else {
    if (IsTerminalInarticulate() &&
        (columns = atoi(nulltoempty(getenv("COLUMNS")))) > 25 &&
        command.n > columns + 2) {
      /* emacs command window is very slow so truncate lines */
      command.n = columns + 2;
      command.p[command.n - 4] = '.';
      command.p[command.n - 3] = '.';
      command.p[command.n - 2] = '.';
      command.p[command.n - 1] = '\n';
    }
    p = command.p;
    n = command.n;
  }
  write(2, p, n);

  /*
   * create temporary copy when launching APE binaries
   * and we help FindDebugBinary to find debug symbols
   */
  if (!IsWindows() && endswith(cmd, ".com")) {
    comdbg = xasprintf("%s%s", cmd, ".dbg");
    cachedcmd = xasprintf("o/%s", cmd);
    if (fileexists(comdbg)) {
      AddEnv(xasprintf("COMDBG=%s", comdbg));
    }
    if (FileExistsAndIsNewerThan(cachedcmd, cmd)) {
      cmd = cachedcmd;
    } else {
      if (startswith(cmd, "o/")) {
        cachedcmd = NULL;
      }
      originalcmd = cmd;
      cmd = xasprintf("%s.tmp.%d", originalcmd, getpid());
      copyfile(originalcmd, cmd, 0);
    }
  }

  /*
   * terminate environment
   */
  AddEnv(NULL);

  /*
   * launch command
   */
  sigfillset(&mask);
  sigprocmask(SIG_BLOCK, &mask, &savemask);
  ws = Launch(&ru);

  /*
   * if execve() failed unzip gcc and try again
   */
  if (WIFEXITED(ws) && WEXITSTATUS(ws) == 127 &&
      startswith(cmd, "o/third_party/gcc") &&
      fileexists("third_party/gcc/unbundle.sh")) {
    system("third_party/gcc/unbundle.sh");
    ws = Launch(&ru);
  }

  /*
   * cleanup temporary copy of ape executable
   */
  if (originalcmd) {
    if (cachedcmd && WIFEXITED(ws) && !WEXITSTATUS(ws)) {
      makedirs(xdirname(cachedcmd), 0755);
      rename(cmd, cachedcmd);
    } else {
      unlink(cmd);
    }
  }

  /*
   * propagate exit
   */
  if (WIFEXITED(ws)) {
    if (!WEXITSTATUS(ws)) {
      if (touchtarget && target) {
        makedirs(xdirname(target), 0755);
        touch(target, 0644);
      }
      return 0;
    } else {
      p = xasprintf("%s%s %s %d%s: %.*s%s\n", RED2, DescribeCommand(),
                    "exited with", WEXITSTATUS(ws), RESET, command.n, command.p,
                    GetResourceReport(&ru));
      rc = WEXITSTATUS(ws);
    }
  } else {
    p = xasprintf("%s%s %s %s%s: %.*s%s\n", RED2, DescribeCommand(),
                  "terminated by", strsignal(WTERMSIG(ws)), RESET, command.n,
                  command.p, GetResourceReport(&ru));
    rc = 128 + WTERMSIG(ws);
  }

  /*
   * print full command in the event of error
   */
  write(2, p, strlen(p));
  return rc;
}
