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
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/strace.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/log/libfatal.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/errfuns.h"

static noasan bool IsExePath(const char *s, size_t n) {
  return n >= 4 && (READ32LE(s + n - 4) == READ32LE(".exe") ||
                    READ32LE(s + n - 4) == READ32LE(".EXE"));
}

static noasan bool IsComPath(const char *s, size_t n) {
  return n >= 4 && (READ32LE(s + n - 4) == READ32LE(".com") ||
                    READ32LE(s + n - 4) == READ32LE(".COM"));
}

static noasan bool IsComDbgPath(const char *s, size_t n) {
  return n >= 8 && (READ64LE(s + n - 8) == READ64LE(".com.dbg") ||
                    READ64LE(s + n - 8) == READ64LE(".COM.DBG"));
}

static noasan bool AccessCommand(const char *name,
                                 char path[hasatleast PATH_MAX], size_t namelen,
                                 int *err, const char *suffix, size_t pathlen) {
  size_t suffixlen;
  suffixlen = strlen(suffix);
  if (pathlen + 1 + namelen + suffixlen + 1 > PATH_MAX) return false;
  if (pathlen && (path[pathlen - 1] != '/' && path[pathlen - 1] != '\\')) {
    path[pathlen] = !IsWindows()                  ? '/'
                    : memchr(path, '\\', pathlen) ? '\\'
                                                  : '/';
    pathlen++;
  }
  memcpy(path + pathlen, name, namelen);
  memcpy(path + pathlen + namelen, suffix, suffixlen + 1);
  if (!access(path, X_OK)) return true;
  if (errno == EACCES || *err != EACCES) *err = errno;
  return false;
}

static noasan bool SearchPath(const char *name, char path[hasatleast PATH_MAX],
                              size_t namelen, int *err, const char *suffix) {
  char sep;
  size_t i;
  const char *p;
  if (!(p = getenv("PATH"))) p = "/bin:/usr/local/bin:/usr/bin";
  sep = IsWindows() && strchr(p, ';') ? ';' : ':';
  for (;;) {
    for (i = 0; p[i] && p[i] != sep; ++i) {
      if (i < PATH_MAX) {
        path[i] = p[i];
      }
    }
    if (AccessCommand(name, path, namelen, err, suffix, i)) {
      return true;
    }
    if (p[i] == sep) {
      p += i + 1;
    } else {
      break;
    }
  }
  return false;
}

static noasan bool FindCommand(const char *name,
                               char pathbuf[hasatleast PATH_MAX],
                               size_t namelen, bool priorityonly,
                               const char *suffix, int *err) {
  if (priorityonly &&
      (memchr(name, '/', namelen) || memchr(name, '\\', namelen))) {
    pathbuf[0] = 0;
    return AccessCommand(name, pathbuf, namelen, err, suffix, 0);
  }
  if (IsWindows() && priorityonly) {
    return AccessCommand(name, pathbuf, namelen, err, suffix,
                         stpcpy(pathbuf, kNtSystemDirectory) - pathbuf) ||
           AccessCommand(name, pathbuf, namelen, err, suffix,
                         stpcpy(pathbuf, kNtWindowsDirectory) - pathbuf);
  }
  return (IsWindows() && AccessCommand(name, pathbuf, namelen, err, suffix,
                                       stpcpy(pathbuf, ".") - pathbuf)) ||
         SearchPath(name, pathbuf, namelen, err, suffix);
}

static noasan bool FindVerbatim(const char *name,
                                char pathbuf[hasatleast PATH_MAX],
                                size_t namelen, bool priorityonly, int *err) {
  return FindCommand(name, pathbuf, namelen, priorityonly, "", err);
}

static noasan bool FindSuffixed(const char *name,
                                char pathbuf[hasatleast PATH_MAX],
                                size_t namelen, bool priorityonly, int *err) {
  return !IsExePath(name, namelen) && !IsComPath(name, namelen) &&
         !IsComDbgPath(name, namelen) &&
         (FindCommand(name, pathbuf, namelen, priorityonly, ".com", err) ||
          FindCommand(name, pathbuf, namelen, priorityonly, ".exe", err));
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
  int e, f;
  char *res;
  size_t namelen;
  res = 0;
  if (!name) {
    efault();
  } else if (!(namelen = strlen(name))) {
    enoent();
  } else if (namelen + 1 > PATH_MAX) {
    enametoolong();
  } else {
    e = errno;
    f = ENOENT;
    if ((IsWindows() && (FindSuffixed(name, pathbuf, namelen, true, &f) ||
                         FindVerbatim(name, pathbuf, namelen, true, &f) ||
                         FindSuffixed(name, pathbuf, namelen, false, &f) ||
                         FindVerbatim(name, pathbuf, namelen, false, &f))) ||
        (!IsWindows() && (FindVerbatim(name, pathbuf, namelen, true, &f) ||
                          FindSuffixed(name, pathbuf, namelen, true, &f) ||
                          FindVerbatim(name, pathbuf, namelen, false, &f) ||
                          FindSuffixed(name, pathbuf, namelen, false, &f)))) {
      errno = e;
      res = pathbuf;
    } else {
      errno = f;
    }
  }
  STRACE("commandv(%#s, %p) → %#s% m", name, pathbuf, res);
  return res;
}
