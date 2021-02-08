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
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/sigbits.h"
#include "libc/calls/struct/sigset.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"
#include "libc/x/x.h"

#define MANUAL \
  "\
OVERVIEW\n\
\n\
  GNU/LLVM Compiler Collection Frontend Frontend\n\
\n\
DESCRIPTION\n\
\n\
  This launches gcc or clang after scrubbing flags.\n\
\n\
EXAMPLE\n\
\n\
  compile.com gcc -o program program.c\n\
\n"

struct Flags {
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

char *cc;
char *colorflag;
char *outdir;
char *outpath;
char ccpath[PATH_MAX];
int ccversion;

struct Flags flags;
struct Command command;

const char *const kGccOnlyFlags[] = {
    "--nocompress-debug-sections",
    "--noexecstack",
    "-Wa,--nocompress-debug-sections",
    "-Wa,--noexecstack",
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

void AddFlag(char *s) {
  size_t n;
  flags.p = realloc(flags.p, ++flags.n * sizeof(*flags.p));
  flags.p[flags.n - 1] = s;
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
    command.p[command.n] = '\n';
    command.n += 1;
  }
}

int main(int argc, char *argv[]) {
  int i, ws, pid;
  sigset_t mask, savemask;

  if (argc == 1) {
    write(2, MANUAL, sizeof(MANUAL) - 1);
    exit(1);
  }

  if (argc == 2 && !strcmp(argv[1], "--do-nothing")) {
    exit(0);
  }

  if (!isdirectory("o/third_party/gcc")) {
    system("third_party/gcc/unbundle.sh");
  }

  cc = argv[1];
  if (!strchr(cc, '/')) {
    if (!(cc = commandv(argv[1], ccpath))) exit(127);
  }

  ccversion = atoi(firstnonnull(emptytonull(getenv("CCVERSION")), "4"));
  isgcc = !!strstr(basename(cc), "gcc");
  isclang = !!strstr(basename(cc), "clang");
  iscc = isgcc | isclang;

  for (i = 1; i < argc; ++i) {
    if (argv[i][0] != '-') {
      AddFlag(argv[i]);
      continue;
    }
    if (!strcmp(argv[i], "-o")) {
      AddFlag(argv[i]);
      AddFlag((outpath = argv[++i]));
      continue;
    }
    if (!iscc) {
      AddFlag(argv[i]);
      continue;
    }
    if (isclang && IsGccOnlyFlag(argv[i])) {
      continue;
    }
    if (!strcmp(argv[i], "-w")) {
      AddFlag(argv[i]);
      AddFlag("-D__W__");
    } else if (!strcmp(argv[i], "-Oz")) {
      if (isclang) {
        AddFlag(argv[i]);
      } else {
        AddFlag("-Os");
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
        AddFlag("-Wa,-msse2avx");
        AddFlag("-D__MNO_VZEROUPPER__");
      } else if (isclang) {
        AddFlag("-mllvm");
        AddFlag("-x86-use-vzeroupper=0");
      }
    } else if (!strcmp(argv[i], "-msse2avx")) {
      if (isgcc) {
        AddFlag(argv[i]);
      } else if (isclang) {
        AddFlag("-Wa,-msse2avx");
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
    } else if (startswith(argv[i], "-fsanitize=implicit") &&
               strstr(argv[i], "integer")) {
      if (isgcc) AddFlag(argv[i]);
    } else if (startswith(argv[i], "-fvect-cost") ||
               startswith(argv[i], "-mstringop") ||
               startswith(argv[i], "-gz") ||
               strstr(argv[i], "stack-protector") ||
               strstr(argv[i], "sanitize") ||
               startswith(argv[i], "-fvect-cost") ||
               startswith(argv[i], "-fvect-cost")) {
      if (isgcc && ccversion >= 6) {
        AddFlag(argv[i]);
      }
    } else if (startswith(argv[i], "-fdiagnostic-color=")) {
      colorflag = argv[i];
    } else if (startswith(argv[i], "-R") ||
               !strcmp(argv[i], "-fsave-optimization-record")) {
      if (isclang) AddFlag(argv[i]);
    } else if (isclang && startswith(argv[i], "--debug-prefix-map")) {
      /* llvm doesn't provide a gas interface so simulate w/ clang */
      AddFlag(xasprintf("-f%s", argv[i] + 2));
    } else {
      AddFlag(argv[i]);
    }
  }

  if (iscc) {
    if (isclang) {
      /* AddFlag("-fno-integrated-as"); */
      AddFlag("-Wno-unused-command-line-argument");
      AddFlag("-Wno-incompatible-pointer-types-discards-qualifiers");
    }
    AddFlag("-no-canonical-prefixes");
    if (!IsTerminalInarticulate()) {
      AddFlag(firstnonnull(colorflag, "-fdiagnostics-color=always"));
    }
    if (wantpg && !wantnopg) {
      AddFlag("-pg");
      AddFlag("-D__PG__");
      if (wantnop && !isclang) {
        AddFlag("-mnop-mcount");
        AddFlag("-D__MNOP_MCOUNT__");
      }
      if (wantrecord) {
        AddFlag("-mrecord-mcount");
        AddFlag("-D__MRECORD_MCOUNT__");
      }
      if (wantfentry) {
        AddFlag("-mfentry");
        AddFlag("-D__MFENTRY__");
      }
    }
    if (wantasan) {
      AddFlag("-fsanitize=address");
      AddFlag("-D__FSANITIZE_ADDRESS__");
    }
    if (wantubsan) {
      AddFlag("-fsanitize=undefined");
      AddFlag("-fno-data-sections");
    }
    if (wantframe) {
      AddFlag("-fno-omit-frame-pointer");
    }
  }

  AddFlag(NULL);

  if (outpath) {
    outdir = xdirname(outpath);
    if (!isdirectory(outdir)) {
      makedirs(outdir, 0755);
    }
  }

  write(2, command.p, command.n);

  sigfillset(&mask);
  sigprocmask(SIG_BLOCK, &mask, &savemask);
  if ((pid = vfork()) == -1) exit(errno);
  if (!pid) {
    sigprocmask(SIG_SETMASK, &savemask, NULL);
    execv(cc, flags.p);
    _exit(127);
  }
  while (waitpid(pid, &ws, 0) == -1) {
    if (errno != EINTR) exit(errno);
  }

  if (WIFEXITED(ws)) {
    return WEXITSTATUS(ws);
  } else {
    return 128 + WTERMSIG(ws);
  }
}
