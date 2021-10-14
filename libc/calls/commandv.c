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
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/log/libfatal.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

static noasan bool EndsWithIgnoreCase(const char *p, size_t n, const char *s) {
  size_t i, m;
  m = __strlen(s);
  if (n >= m) {
    for (i = n - m; i < n; ++i) {
      if (kToLower[p[i] & 255] != (*s++ & 255)) {
        return false;
      }
    }
    return true;
  } else {
    return false;
  }
}

static noasan bool AccessCommand(const char *name,
                                 char path[hasatleast PATH_MAX], size_t namelen,
                                 const char *suffix, size_t pathlen) {
  size_t suffixlen;
  suffixlen = __strlen(suffix);
  if (pathlen + 1 + namelen + suffixlen + 1 > PATH_MAX) return -1;
  if (pathlen && (path[pathlen - 1] != '/' && path[pathlen - 1] != '\\')) {
    path[pathlen] = !IsWindows()                    ? '/'
                    : __memchr(path, '\\', pathlen) ? '\\'
                                                    : '/';
    pathlen++;
  }
  __repmovsb(path + pathlen, name, namelen);
  __repmovsb(path + pathlen + namelen, suffix, suffixlen + 1);
  return isexecutable(path);
}

static noasan bool SearchPath(const char *name, char path[hasatleast PATH_MAX],
                              size_t namelen, const char *suffix) {
  size_t i;
  const char *p;
  p = firstnonnull(emptytonull(getenv("PATH")), "/bin:/usr/local/bin:/usr/bin");
  for (;;) {
    for (i = 0; p[i] && p[i] != ':' && p[i] != ';'; ++i) {
      if (i < PATH_MAX) {
        path[i] = p[i];
      }
    }
    if (AccessCommand(name, path, namelen, suffix, i)) {
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

static noasan bool FindCommand(const char *name,
                               char pathbuf[hasatleast PATH_MAX],
                               size_t namelen, const char *suffix) {
  if (memchr(name, '/', namelen) || memchr(name, '\\', namelen)) {
    pathbuf[0] = 0;
    return AccessCommand(name, pathbuf, namelen, suffix, 0);
  }
  return ((IsWindows() &&
           (AccessCommand(name, pathbuf, namelen, suffix,
                          stpcpy(pathbuf, kNtSystemDirectory) - pathbuf) ||
            AccessCommand(name, pathbuf, namelen, suffix,
                          stpcpy(pathbuf, kNtWindowsDirectory) - pathbuf) ||
            AccessCommand(name, pathbuf, namelen, suffix,
                          stpcpy(pathbuf, ".") - pathbuf))) ||
          SearchPath(name, pathbuf, namelen, suffix));
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
noasan char *commandv(const char *name, char pathbuf[hasatleast PATH_MAX]) {
  int olderr;
  size_t namelen;
  if (!name) {
    efault();
    return 0;
  }
  if (!(namelen = __strlen(name))) {
    enoent();
    return 0;
  }
  if (namelen + 1 > PATH_MAX) {
    enametoolong();
    return 0;
  }
  if (FindCommand(name, pathbuf, namelen, "") ||
      (!EndsWithIgnoreCase(name, namelen, ".exe") &&
       !EndsWithIgnoreCase(name, namelen, ".com") &&
       !EndsWithIgnoreCase(name, namelen, ".com.dbg") &&
       (FindCommand(name, pathbuf, namelen, ".com") ||
        FindCommand(name, pathbuf, namelen, ".exe")))) {
    return pathbuf;
  } else {
    return 0;
  }
}
