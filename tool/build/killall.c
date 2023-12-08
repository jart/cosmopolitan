/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/dce.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nt/enum/formatmessageflags.h"
#include "libc/nt/enum/lang.h"
#include "libc/nt/enum/processaccess.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"
#include "libc/x/x.h"
#include "third_party/getopt/getopt.internal.h"

/**
 * @fileoverview Mass Process Killer for Windows.
 *
 * Bad things sometimes happen during development, such as fork bombs.
 * Under such circumstances, the Windows operating system itself remains
 * remarkably stable (much more so than Linux would in these cases) but
 * the tools on Windows for managing processes do not scale gracefully;
 * GUIs like the Task Manager and Process Explorer become unresponsive
 * when the system has a nontrivial number of processes, leaving no way
 * to kill the processes. This tool is designed to make it easy to kill
 * processes, particularly in large numbers, via a simple CLI interface.
 */

static const char *prog;
static char16_t **filters;
static uint32_t pids[10000];

static wontreturn void PrintUsage(int rc, FILE *f) {
  fprintf(f,
          "Usage: %s [-nshv] NAME...\n"
          "\tNAME\tcase-insensitive process name substring filter\n"
          "\t-n\tdry run (print matching processes but do not kill)\n"
          "\t-v\tverbose mode\n"
          "\t-s\tsilent mode\n"
          "\t-h\tshow help\n",
          prog);
  exit(rc);
}

static wontreturn void OutOfMemory(void) {
  fprintf(stderr, "%s: out of memory\n", prog);
  exit(1);
}

static void *Calloc(size_t n, size_t z) {
  void *p;
  if (!(p = calloc(n, z))) OutOfMemory();
  return p;
}

static void ConvertStringToLowercase16(char16_t *s) {
  while (*s) {
    *s = towlower(*s);
    ++s;
  }
}

static bool ShouldKillProcess(char16_t *name) {
  if (!*filters) {
    return true;
  }
  for (char16_t **f = filters; *f; ++f) {
    if (strstr16(name, *f)) {
      return true;
    }
  }
  return false;
}

static int64_t MyOpenProcess(uint32_t pid) {
  return OpenProcess(
      kNtProcessTerminate | kNtProcessQueryInformation | kNtProcessVmRead,
      false, pid);
}

static bool GetProcessName(int64_t hand, char16_t name[hasatleast PATH_MAX]) {
  int64_t hMod;
  uint32_t cbNeeded;
  *name = 0;
  if (EnumProcessModules(hand, &hMod, sizeof(hMod), &cbNeeded)) {
    GetModuleBaseName(hand, hMod, name, PATH_MAX);
  }
  return !!name[0];
}

static char16_t *DescribeError(int err) {
  static char16_t msg[256];
  FormatMessage(kNtFormatMessageFromSystem | kNtFormatMessageIgnoreInserts, 0,
                err, MAKELANGID(kNtLangNeutral, kNtSublangDefault), msg,
                ARRAYLEN(msg), 0);
  return chomp16(msg);
}

int main(int argc, char *argv[]) {
  int i, j;

  // get program name
  prog = argv[0] ? argv[0] : "killall";

  // sanity check environment
  if (!IsWindows()) {
    fprintf(stderr, "%s: this program is intended for windows\n", prog);
    exit(1);
  }

  // try to minimize terminal writes slowing us down
  setvbuf(stdin, NULL, _IONBF, 0);

  // get flags
  int opt;
  bool dryrun = false;
  bool silent = false;
  bool verbose = false;
  while ((opt = getopt(argc, argv, "nhsv")) != -1) {
    switch (opt) {
      case 'n':
        dryrun = true;
        break;
      case 's':
        silent = true;
        break;
      case 'v':
        verbose = true;
        break;
      case 'h':
        PrintUsage(0, stdout);
      default:
        PrintUsage(1, stderr);
    }
  }

  // get names of things to kill
  filters = Calloc(argc, sizeof(char16_t *));
  for (j = 0, i = optind; i < argc; ++i) {
    char16_t *filter;
    if ((filter = utf8to16(argv[i], -1, 0)) && *filter) {
      ConvertStringToLowercase16(filter);
      filters[j++] = filter;
    }
  }
  if (!j && !dryrun) {
    fprintf(stderr, "%s: missing operand\n", prog);
    exit(1);
  }

  // outer loop
  int count = 0;
  int subcount;
  do {

    // get pids of all processes on system
    uint32_t n;
    if (!EnumProcesses(pids, sizeof(pids), &n)) {
      fprintf(stderr, "%s: EnumProcesses() failed: %hs\n", prog,
              DescribeError(GetLastError()));
      exit(1);
    }
    n /= sizeof(uint32_t);

    // kill matching processes
    int64_t hProcess;
    char16_t name[PATH_MAX];
    for (subcount = i = 0; i < n; i++) {
      if (!pids[i]) continue;
      if ((hProcess = MyOpenProcess(pids[i]))) {
        if (GetProcessName(hProcess, name)) {
          ConvertStringToLowercase16(name);
          if (ShouldKillProcess(name)) {
            if (dryrun) {
              if (!silent) {
                printf("%5u %hs\n", pids[i], name);
              }
              ++subcount;
            } else if (TerminateProcess(hProcess, SIGKILL)) {
              if (!silent) {
                printf("%5u %hs killed\n", pids[i], name);
              }
              ++subcount;
            } else {
              printf("%5u %hs %hs\n", pids[i], name,
                     DescribeError(GetLastError()));
            }
          }
        } else if (verbose) {
          fprintf(stderr, "%s: GetProcessName(%u) failed: %hs\n", prog, pids[i],
                  DescribeError(GetLastError()));
        }
        CloseHandle(hProcess);
      } else if (verbose) {
        fprintf(stderr, "%s: OpenProcess(%u) failed: %hs\n", prog, pids[i],
                DescribeError(GetLastError()));
      }
    }

    // we don't stop until we've confirmed they're all gone
    count += subcount;
  } while (!dryrun && subcount);

  if (!silent && !count) {
    fprintf(stderr, "%s: no processes found\n", prog);
  }

  return 0;
}
