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
  This launches gcc or clang while filtering out\n\
  flags they whine about.\n\
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
  isgcc = strstr(basename(cc), "gcc");
  isclang = strstr(basename(cc), "clang");
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
    if (iscc) {
      AddFlag(argv[i]);
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
    } else if (isclang &&
               (!strcmp(argv[i], "-gstabs") || !strcmp(argv[i], "-ftrapv") ||
                !strcmp(argv[i], "-fsignaling-nans") ||
                !strcmp(argv[i], "-fcx-limited-range") ||
                !strcmp(argv[i], "-fno-fp-int-builtin-inexact") ||
                !strcmp(argv[i], "-Wno-unused-but-set-variable") ||
                !strcmp(argv[i], "-Wunsafe-loop-optimizations") ||
                !strcmp(argv[i], "-mdispatch-scheduler") ||
                !strcmp(argv[i], "-ftracer") ||
                !strcmp(argv[i], "-frounding-math") ||
                !strcmp(argv[i], "-fmerge-constants") ||
                !strcmp(argv[i], "-fmodulo-sched") ||
                !strcmp(argv[i], "-fopt-info-vec") ||
                !strcmp(argv[i], "-fopt-info-vec-missed") ||
                !strcmp(argv[i], "-fmodulo-sched-allow-regmoves") ||
                !strcmp(argv[i], "-freschedule-modulo-scheduled-loops") ||
                !strcmp(argv[i], "-fipa-pta") ||
                !strcmp(argv[i], "-fsched2-use-superblocks") ||
                !strcmp(argv[i], "-fbranch-target-load-optimize") ||
                !strcmp(argv[i], "-fdelete-dead-exceptions") ||
                !strcmp(argv[i], "-funsafe-loop-optimizations") ||
                !strcmp(argv[i], "-mmitigate-rop") ||
                !strcmp(argv[i], "-fno-align-jumps") ||
                !strcmp(argv[i], "-fno-align-labels") ||
                !strcmp(argv[i], "-fno-align-loops") ||
                !strcmp(argv[i], "-fivopts") ||
                !strcmp(argv[i], "-fschedule-insns") ||
                !strcmp(argv[i], "-fno-semantic-interposition") ||
                !strcmp(argv[i], "-mno-fentry") ||
                !strcmp(argv[i], "-fversion-loops-for-strides") ||
                !strcmp(argv[i], "-femit-struct-debug-baseonly") ||
                !strcmp(argv[i], "-ftree-loop-vectorize") ||
                !strcmp(argv[i], "-gdescribe-dies") ||
                !strcmp(argv[i], "-flimit-function-alignment") ||
                !strcmp(argv[i], "-ftree-loop-im") ||
                !strcmp(argv[i], "-fno-instrument-functions") ||
                !strcmp(argv[i], "-fstack-clash-protection") ||
                !strcmp(argv[i], "-mfpmath=sse+387") ||
                !strcmp(argv[i], "-Wa,--noexecstack") ||
                !strcmp(argv[i], "-freg-struct-return") ||
                !strcmp(argv[i], "-mcall-ms2sysv-xlogues") ||
                startswith(argv[i], "-ffixed-") ||
                startswith(argv[i], "-fcall-saved") ||
                startswith(argv[i], "-fcall-used") ||
                startswith(argv[i], "-fgcse-") ||
                strstr(argv[i], "shrink-wrap") ||
                strstr(argv[i], "schedule-insns2") ||
                startswith(argv[i], "-fvect-cost-model=") ||
                startswith(argv[i], "-fsimd-cost-model=") ||
                startswith(argv[i], "-fopt-info") ||
                startswith(argv[i], "-mstringop-strategy=") ||
                startswith(argv[i], "-mpreferred-stack-boundary=") ||
                strstr(argv[i], "gnu-unique") ||
                startswith(argv[i], "-Wframe-larger-than=") ||
                strstr(argv[i], "whole-program") ||
                startswith(argv[i], "-Wa,--size-check=") ||
                startswith(argv[i], "-Wa,--listing"))) {
      /* ignore flag so clang won't whine */
    } else {
      AddFlag(argv[i]);
    }
  }

  if (iscc) {
    if (isclang) {
      AddFlag("-fno-integrated-as");
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
