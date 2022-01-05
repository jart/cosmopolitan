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
#include "libc/calls/sysdebug.internal.h"
#include "libc/dce.h"
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

/**
 * Absolute path of executable.
 *
 * This variable is initialized automatically at startup. The path is
 * guaranteed to exist, except on XNU and OpenBSD. It may be a symlink.
 * It may be spoofed.
 */
char program_executable_name[SIZE];

static textwindows bool GetNtExePath(char executable[SIZE]) {
  uint64_t w;
  wint_t x, y;
  uint32_t i, j;
  char16_t path16[PATH_MAX + 1];
  if (!GetModuleFileName(0, path16, ARRAYLEN(path16))) return 0;
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

static textstartup void GetProgramExecutableName(char executable[SIZE],
                                                 char *p) {
  char *t;
  size_t m;
  int cmd[4];
  ssize_t n = 0;
  if (IsWindows() && GetNtExePath(executable)) return;
  if (fileexists(p)) {
    if (!_isabspath(p)) {
      if (getcwd(executable, SIZE - 1)) {
        n = strlen(executable);
        executable[n++] = '/';
      }
    }
  } else if ((n = sys_readlinkat(AT_FDCWD, "/proc/self/exe", executable,
                                 SIZE - 1)) > 0) {
    executable[n] = 0;
    return;
  } else if ((n = sys_readlinkat(AT_FDCWD, "/proc/curproc/file", executable,
                                 SIZE - 1)) > 0) {
    executable[n] = 0;
    return;
  } else if (IsFreebsd() || IsNetbsd()) {
    cmd[0] = CTL_KERN;
    cmd[1] = KERN_PROC;
    if (IsFreebsd()) {
      cmd[2] = KERN_PROC_PATHNAME_FREEBSD;
    } else {
      cmd[2] = KERN_PROC_PATHNAME_NETBSD;
    }
    cmd[3] = -1;
    m = SIZE;
    if (sysctl(cmd, ARRAYLEN(cmd), executable, &m, 0, 0) != -1) {
      return;
    }
  }
  if (n < 0) n = 0;
  for (; *p; ++p) {
    if (n + 1 < SIZE) {
      executable[n++] = *p;
    }
  }
  executable[n] = 0;
}

textstartup void program_executable_name_init(int argc, char **argv,
                                              char **envp, intptr_t *auxv) {
  static bool once;
  char executable[SIZE];
  if (!cmpxchg(&once, 0, 1)) return;
  __stpcpy(program_executable_name, argv[0]);
  GetProgramExecutableName(executable, argv[0]);
  __stpcpy(program_executable_name, executable);
  SYSDEBUG("GetProgramExecutableName() -> %s", program_executable_name);
}

const void *const program_executable_name_init_ctor[] initarray = {
    program_executable_name_init,
};
