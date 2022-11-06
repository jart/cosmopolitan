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
#include "libc/calls/metalfile.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/macros.internal.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/ok.h"

#define SIZE                       1024
#define CTL_KERN                   1
#define KERN_PROC                  14
#define KERN_PROC_PATHNAME_FREEBSD 12
#define KERN_PROC_PATHNAME_NETBSD  5

char program_executable_name[PATH_MAX];

static inline int IsAlpha(int c) {
  return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

static inline char *StrCat(char buf[PATH_MAX], const char *a, const char *b) {
  char *p, *e;
  p = buf;
  e = buf + PATH_MAX;
  while (*a && p < e) *p++ = *a++;
  while (*b && p < e) *p++ = *b++;
  return buf;
}

static inline void GetProgramExecutableNameImpl(char *p, char *e) {
  char *q;
  ssize_t rc;
  size_t i, n;
  union {
    int cmd[4];
    char path[PATH_MAX];
    char16_t path16[PATH_MAX];
  } u;

  if (IsWindows()) {
    n = GetModuleFileName(0, u.path16, ARRAYLEN(u.path16));
    for (i = 0; i < n; ++i) {
      // turn c:\foo\bar into c:/foo/bar
      if (u.path16[i] == '\\') {
        u.path16[i] = '/';
      }
    }
    if (IsAlpha(u.path16[0]) && u.path16[1] == ':' && u.path16[2] == '/') {
      // turn c:/... into /c/...
      u.path16[1] = u.path16[0];
      u.path16[0] = '/';
      u.path16[2] = '/';
    }
    tprecode16to8(p, e - p, u.path16);
    return;
  }

  if (IsMetal()) {
    if (!memccpy(p, APE_COM_NAME, 0, e - p - 1)) e[-1] = 0;
    return;
  }

  // if argv[0] exists then turn it into an absolute path. we also try
  // adding a .com suffix since the ape auto-appends it when resolving
  if (__argc && (((q = __argv[0]) && !sys_faccessat(AT_FDCWD, q, F_OK, 0)) ||
                 ((q = StrCat(u.path, __argv[0], ".com")) &&
                  !sys_faccessat(AT_FDCWD, q, F_OK, 0)))) {
    if (*q != '/') {
      if (q[0] == '.' && q[1] == '/') {
        q += 2;
      }
      if (getcwd(p, e - p)) {
        while (*p) ++p;
        *p++ = '/';
      }
    }
    for (i = 0; *q && p + 1 < e; ++p, ++q) {
      *p = *q;
    }
    *p = 0;
    return;
  }

  // if argv[0] doesn't exist, then fallback to interpreter name
  if ((rc = sys_readlinkat(AT_FDCWD, "/proc/self/exe", p, e - p - 1)) > 0 ||
      (rc = sys_readlinkat(AT_FDCWD, "/proc/curproc/file", p, e - p - 1)) > 0) {
    p[rc] = 0;
    return;
  }
  if (IsFreebsd() || IsNetbsd()) {
    u.cmd[0] = CTL_KERN;
    u.cmd[1] = KERN_PROC;
    if (IsFreebsd()) {
      u.cmd[2] = KERN_PROC_PATHNAME_FREEBSD;
    } else {
      u.cmd[2] = KERN_PROC_PATHNAME_NETBSD;
    }
    u.cmd[3] = -1;  // current process
    n = e - p;
    if (sys_sysctl(u.cmd, ARRAYLEN(u.cmd), p, &n, 0, 0) != -1) {
      return;
    }
  }
}

/**
 * Returns absolute path of program.
 */
char *GetProgramExecutableName(void) {
  int e;
  static bool once;
  if (!once) {
    e = errno;
    GetProgramExecutableNameImpl(
        program_executable_name,
        program_executable_name + sizeof(program_executable_name));
    errno = e;
    once = true;
  }
  return program_executable_name;
}

/* const void *const GetProgramExecutableNameCtor[] initarray = { */
/*     GetProgramExecutableName, */
/* }; */
