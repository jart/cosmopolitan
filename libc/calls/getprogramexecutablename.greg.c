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
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/metalfile.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/libgen.h"
#include "libc/intrin/getenv.internal.h"
#include "libc/serialize.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/ok.h"

#define CTL_KERN                   1
#define KERN_PROC                  14
#define KERN_PROC_PATHNAME_FREEBSD 12
#define KERN_PROC_PATHNAME_NETBSD  5

static struct {
  atomic_uint once;
  union {
    char buf[PATH_MAX];
    char16_t buf16[PATH_MAX / 2];
  } u;
} g_prog;

static inline int IsAlpha(int c) {
  return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

static inline int AllNumDot(const char *s) {
  while (true) {
    switch (*s++) {
      default:  return 0;
      case 0:   return 1;
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9': case '.':
        /* continue */
      }
  }
}

static int IsApeLoader(char *s) {
  char *b;
  return !strcmp(s, "/usr/bin/ape") ||
         (!strncmp((b = basename(s)), ".ape-", 5) &&
          AllNumDot(b + 5));
}

static inline void InitProgramExecutableNameImpl(void) {
  size_t n;
  ssize_t got;
  char c, *q, *b;

  if (__program_executable_name) {
    /* already set by the loader */
    return;
  }
  if (IsWindows()) {
    int n = GetModuleFileName(0, g_prog.u.buf16, ARRAYLEN(g_prog.u.buf16));
    for (int i = 0; i < n; ++i) {
      // turn c:\foo\bar into c:/foo/bar
      if (g_prog.u.buf16[i] == '\\') {
        g_prog.u.buf16[i] = '/';
      }
    }
    if (IsAlpha(g_prog.u.buf16[0]) &&  //
        g_prog.u.buf16[1] == ':' &&    //
        g_prog.u.buf16[2] == '/') {
      // turn c:/... into /c/...
      g_prog.u.buf16[1] = g_prog.u.buf16[0];
      g_prog.u.buf16[0] = '/';
      g_prog.u.buf16[2] = '/';
    }
    tprecode16to8(g_prog.u.buf, sizeof(g_prog.u.buf), g_prog.u.buf16);
    goto UseBuf;
  }
  if (IsMetal()) {
    __program_executable_name = APE_COM_NAME;
    return;
  }

  b = g_prog.u.buf;
  n = sizeof(g_prog.u.buf) - 1;
  if (IsFreebsd() || IsNetbsd()) {
    int cmd[4];
    cmd[0] = CTL_KERN;
    cmd[1] = KERN_PROC;
    if (IsFreebsd()) {
      cmd[2] = KERN_PROC_PATHNAME_FREEBSD;
    } else {
      cmd[2] = KERN_PROC_PATHNAME_NETBSD;
    }
    cmd[3] = -1;  // current process
    if (sys_sysctl(cmd, ARRAYLEN(cmd), b, &n, 0, 0) != -1) {
      if (!IsApeLoader(b)) {
        goto UseBuf;
      }
    }
  }
  if (IsLinux()) {
    if ((got = sys_readlinkat(AT_FDCWD, "/proc/self/exe", b, n)) > 0 ||
        (got = sys_readlinkat(AT_FDCWD, "/proc/curproc/file", b, n)) > 0) {
      b[got] = 0;
      if (!IsApeLoader(b)) {
        goto UseBuf;
      }
    }
  }

  if (issetugid()) {
    /* give up prior to using less secure methods */
    goto UseEmpty;
  }

  // if argv[0] exists then turn it into an absolute path. we also try
  // adding a .com suffix since the ape auto-appends it when resolving
  if ((q = __argv[0])) {
    char *p = g_prog.u.buf;
    char *e = p + sizeof(g_prog.u.buf);
    if (*q != '/') {
      if (q[0] == '.' && q[1] == '/') {
        q += 2;
      }
      int got = __getcwd(p, e - p - 1 - 4);  // for / and .com
      if (got != -1) {
        p += got - 1;
        *p++ = '/';
      }
    }
    while ((c = *q++)) {
      if (p + 1 + 4 < e) {  // for nul and .com
        *p++ = c;
      }
    }
    *p = 0;
    if (!sys_faccessat(AT_FDCWD, g_prog.u.buf, F_OK, 0)) goto UseBuf;
    p = WRITE32LE(p, READ32LE(".com"));
    *p = 0;
    if (!sys_faccessat(AT_FDCWD, g_prog.u.buf, F_OK, 0)) goto UseBuf;
  }

  /* the previous loader supplied the full program path as the first
     environment variable. we also try "_". */
  if ((q = __getenv(__envp, "COSMOPOLITAN_PROGRAM_EXECUTABLE").s) ||
      (q = __getenv(__envp, "_").s)) {
    goto CopyString;
  }

  // give up and just copy argv[0] into it
  if ((q = __argv[0])) {
  CopyString:
    char *p = g_prog.u.buf;
    char *e = p + sizeof(g_prog.u.buf);
    while ((c = *q++)) {
      if (p + 1 < e) {
        *p++ = c;
      }
    }
    *p = 0;
    goto UseBuf;
  }

  // if we don't even have that then empty the string
UseEmpty:
  g_prog.u.buf[0] = 0;

UseBuf:
  __program_executable_name = g_prog.u.buf;
}

static void InitProgramExecutableName(void) {
  int e = errno;
  InitProgramExecutableNameImpl();
  errno = e;
}

/**
 * Returns absolute path of program.
 */
char *GetProgramExecutableName(void) {
  cosmo_once(&g_prog.once, InitProgramExecutableName);
  return __program_executable_name;
}
