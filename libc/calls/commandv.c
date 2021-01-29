/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bits/progn.internal.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

static bool AccessCommand(char path[hasatleast PATH_MAX], const char *name,
                          size_t namelen, size_t pathlen) {
  if (pathlen + 1 + namelen + 1 + 4 + 1 > PATH_MAX) return -1;
  if (pathlen && (path[pathlen - 1] != '/' && path[pathlen - 1] != '\\')) {
    path[pathlen] =
        !IsWindows() ? '/' : memchr(path, '\\', pathlen) ? '\\' : '/';
    pathlen++;
  }
  memcpy(path + pathlen, name, namelen + 1);
  if (isexecutable(path)) return true;
  memcpy(path + pathlen + namelen, ".com", 5);
  if (isexecutable(path)) return true;
  memcpy(path + pathlen + namelen, ".exe", 5);
  if (isexecutable(path)) return true;
  return false;
}

static bool SearchPath(char path[hasatleast PATH_MAX], const char *name,
                       size_t namelen) {
  size_t i;
  const char *p;
  p = firstnonnull(emptytonull(getenv("PATH")), "/bin:/usr/local/bin:/usr/bin");
  for (;;) {
    for (i = 0; p[i] && p[i] != ':' && p[i] != ';'; ++i) {
      if (i < PATH_MAX) path[i] = p[i];
    }
    if (AccessCommand(path, name, namelen, i)) {
      return true;
    }
    if (p[i] == ':' || p[i] == ';') {
      p += i + 1;
    } else {
      break;
    }
  }
  return false;
}

/**
 * Resolves full pathname of executable.
 *
 * @return execve()'able path, or NULL w/ errno
 * @errno ENOENT, EACCES, ENOMEM
 * @see free(), execvpe()
 * @asyncsignalsafe
 * @vforksafe
 */
char *commandv(const char *name, char pathbuf[hasatleast PATH_MAX]) {
  char *p;
  size_t namelen;
  int rc, olderr;
  if (!(namelen = strlen(name))) return PROGN(enoent(), NULL);
  if (namelen + 1 > PATH_MAX) return PROGN(enametoolong(), NULL);
  if (strchr(name, '/') || strchr(name, '\\')) {
    if (AccessCommand(strcpy(pathbuf, ""), name, namelen, 0)) {
      return pathbuf;
    } else {
      return NULL;
    }
  }
  olderr = errno;
  if ((IsWindows() &&
       (AccessCommand(pathbuf, name, namelen,
                      stpcpy(pathbuf, kNtSystemDirectory) - pathbuf) ||
        AccessCommand(pathbuf, name, namelen,
                      stpcpy(pathbuf, kNtWindowsDirectory) - pathbuf))) ||
      SearchPath(pathbuf, name, namelen)) {
    errno = olderr;
    return pathbuf;
  } else {
    return NULL;
  }
}
