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
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/strace.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/libfatal.internal.h"
#include "libc/macros.internal.h"
#include "libc/mem/alloca.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/str/tpenc.h"
#include "libc/str/utf16.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/consts/prot.h"

#define SIZE                       1024
#define CTL_KERN                   1
#define KERN_PROC                  14
#define KERN_PROC_PATHNAME_FREEBSD 12
#define KERN_PROC_PATHNAME_NETBSD  5

char program_executable_name[SIZE];

static textwindows bool GetNtExePath(char executable[SIZE]) {
  bool32 rc;
  uint64_t w;
  wint_t x, y;
  uint32_t i, j;
  char16_t path16[PATH_MAX + 1];
  path16[0] = 0;
  rc = GetModuleFileName(0, path16, ARRAYLEN(path16));
  STRACE("GetModuleFileName(0, [%#hs]) → %hhhd", path16, rc);
  if (!rc) return false;
  for (i = j = 0; (x = path16[i++] & 0xffff);) {
    if (!IsUcs2(x)) {
      y = path16[i++] & 0xffff;
      x = MergeUtf16(x, y);
    }
    if (x == '\\') x = '/';
    w = tpenc(x);
    do {
      executable[j] = w;
      if (++j == SIZE) {
        return false;
      }
    } while ((w >>= 8));
  }
  executable[j] = 0;
  return true;
}

static void ReadProgramExecutableName(char executable[SIZE], char *argv0,
                                      uintptr_t *auxv) {
  size_t m;
  ssize_t n;
  int cmd[4];
  char *p, *t;
  if (IsWindows() && GetNtExePath(executable)) {
    return;
  }
  for (p = 0; *auxv; auxv += 2) {
    if (*auxv == AT_EXECFN) {
      p = (char *)auxv[1];
      break;
    }
  }
  n = 0;
  if (!p) p = argv0;
  if (p) {
    if (!_isabspath(p)) {
      if (getcwd(executable, SIZE - 1)) {
        n = strlen(executable);
        executable[n++] = '/';
      }
    }
    for (; *p; ++p) {
      if (n + 1 < SIZE) {
        executable[n++] = *p;
      }
    }
  }
  executable[n] = 0;
}

/**
 * Returns absolute path of executable.
 *
 * This variable is initialized automatically at startup. The path is
 * basically `argv[0]` except some extra vetting is done to provide
 * stronger assurance that the path can be counted upon to exist.
 *
 * For example, if your program is executed as a relative path and then
 * your program calls `chdir()`, then `argv[0]` will be incorrect; but
 * `program_executable_name` will work, because it prefixed `getcwd()`
 * early in the initialization phase.
 *
 * @see GetInterpreterExecutableName()
 * @see program_invocation_short_name
 * @see program_invocation_name
 */
char *GetProgramExecutableName(void) {
  int e;
  static bool once;
  char executable[SIZE];
  if (!once) {
    e = errno;
    ReadProgramExecutableName(executable, __argv[0], __auxv);
    errno = e;
    __stpcpy(program_executable_name, executable);
    once = true;
  }
  return program_executable_name;
}

// try our best to memoize it before a chdir() happens
const void *const program_executable_name_init_ctor[] initarray = {
    GetProgramExecutableName,
};
