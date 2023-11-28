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
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/metalfile.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/errno.h"
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

static inline void InitProgramExecutableNameImpl(void) {

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
    return;
  }

  char c, *q;
  if (IsMetal()) {
    q = APE_COM_NAME;
    goto CopyString;
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
    if (!sys_faccessat(AT_FDCWD, g_prog.u.buf, F_OK, 0)) return;
    p = WRITE32LE(p, READ32LE(".com"));
    *p = 0;
    if (!sys_faccessat(AT_FDCWD, g_prog.u.buf, F_OK, 0)) return;
  }

  // if getenv("_") exists then use that
  for (char **ep = __envp; (q = *ep); ++ep) {
    if (*q++ == '_' && *q++ == '=') {
      goto CopyString;
    }
  }

  // if argv[0] doesn't exist, then fallback to interpreter name
  ssize_t got;
  char *b = g_prog.u.buf;
  size_t n = sizeof(g_prog.u.buf) - 1;
  if ((got = sys_readlinkat(AT_FDCWD, "/proc/self/exe", b, n)) > 0 ||
      (got = sys_readlinkat(AT_FDCWD, "/proc/curproc/file", b, n)) > 0) {
    b[got] = 0;
    return;
  }
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
      return;
    }
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
  }

  // if we don't even have that then empty the string
  g_prog.u.buf[0] = 0;
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
  return g_prog.u.buf;
}
