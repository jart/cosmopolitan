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
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/errfuns.h"

static bool IsExePath(const char *s, size_t n) {
  return n >= 4 && (READ32LE(s + n - 4) == READ32LE(".exe") ||
                    READ32LE(s + n - 4) == READ32LE(".EXE"));
}

static bool IsComPath(const char *s, size_t n) {
  return n >= 4 && (READ32LE(s + n - 4) == READ32LE(".com") ||
                    READ32LE(s + n - 4) == READ32LE(".COM"));
}

static bool IsComDbgPath(const char *s, size_t n) {
  return n >= 8 && (READ64LE(s + n - 8) == READ64LE(".com.dbg") ||
                    READ64LE(s + n - 8) == READ64LE(".COM.DBG"));
}

static bool AccessCommand(const char *name, char *path, size_t pathsz,
                          size_t namelen, int *err, const char *suffix,
                          size_t pathlen) {
  size_t suffixlen;
  suffixlen = strlen(suffix);
  if (IsWindows() && suffixlen == 0 && !IsExePath(name, namelen) &&
      !IsComPath(name, namelen))
    return false;
  if (pathlen + 1 + namelen + suffixlen + 1 > pathsz) return false;
  if (pathlen && (path[pathlen - 1] != '/' && path[pathlen - 1] != '\\')) {
    path[pathlen] = !IsWindows()                  ? '/'
                    : memchr(path, '\\', pathlen) ? '\\'
                                                  : '/';
    pathlen++;
  }
  memcpy(path + pathlen, name, namelen);
  memcpy(path + pathlen + namelen, suffix, suffixlen + 1);
  if (!access(path, X_OK)) {
    struct stat st;
    if (!stat(path, &st)) {
      if (S_ISREG(st.st_mode)) {
        return true;
      } else {
        errno = EACCES;
      }
    }
  }
  if (errno == EACCES || *err != EACCES) *err = errno;
  return false;
}

static bool SearchPath(const char *name, char *path, size_t pathsz,
                       size_t namelen, int *err, const char *suffix) {
  char sep;
  size_t i;
  const char *p;
  if (!(p = getenv("PATH"))) p = "/bin:/usr/local/bin:/usr/bin";
  sep = IsWindows() && strchr(p, ';') ? ';' : ':';
  for (;;) {
    for (i = 0; p[i] && p[i] != sep; ++i) {
      if (i < pathsz) {
        path[i] = p[i];
      }
    }
    if (AccessCommand(name, path, pathsz, namelen, err, suffix, i)) {
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

static bool FindCommand(const char *name, char *pb, size_t pbsz, size_t namelen,
                        bool pri, const char *suffix, int *err) {
  if (pri && (memchr(name, '/', namelen) || memchr(name, '\\', namelen))) {
    pb[0] = 0;
    return AccessCommand(name, pb, pbsz, namelen, err, suffix, 0);
  }
  if (IsWindows() && pri &&
      pbsz > max(strlen(kNtSystemDirectory), strlen(kNtWindowsDirectory))) {
    return AccessCommand(name, pb, pbsz, namelen, err, suffix,
                         stpcpy(pb, kNtSystemDirectory) - pb) ||
           AccessCommand(name, pb, pbsz, namelen, err, suffix,
                         stpcpy(pb, kNtWindowsDirectory) - pb);
  }
  return (IsWindows() &&
          (pbsz > 1 && AccessCommand(name, pb, pbsz, namelen, err, suffix,
                                     stpcpy(pb, ".") - pb))) ||
         SearchPath(name, pb, pbsz, namelen, err, suffix);
}

static bool FindVerbatim(const char *name, char *pb, size_t pbsz,
                         size_t namelen, bool pri, int *err) {
  return FindCommand(name, pb, pbsz, namelen, pri, "", err);
}

static bool FindSuffixed(const char *name, char *pb, size_t pbsz,
                         size_t namelen, bool pri, int *err) {
  return !IsExePath(name, namelen) && !IsComPath(name, namelen) &&
         !IsComDbgPath(name, namelen) &&
         (FindCommand(name, pb, pbsz, namelen, pri, ".com", err) ||
          FindCommand(name, pb, pbsz, namelen, pri, ".exe", err));
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
char *commandv(const char *name, char *pathbuf, size_t pathbufsz) {
  int e, f;
  char *res;
  size_t namelen;
  res = 0;
  if (!name) {
    efault();
  } else if (!(namelen = strlen(name))) {
    enoent();
  } else if (namelen + 1 > pathbufsz) {
    enametoolong();
  } else {
    e = errno;
    f = ENOENT;
    if ((IsWindows() &&
         (FindSuffixed(name, pathbuf, pathbufsz, namelen, true, &f) ||
          FindVerbatim(name, pathbuf, pathbufsz, namelen, true, &f) ||
          FindSuffixed(name, pathbuf, pathbufsz, namelen, false, &f) ||
          FindVerbatim(name, pathbuf, pathbufsz, namelen, false, &f))) ||
        (!IsWindows() &&
         (FindVerbatim(name, pathbuf, pathbufsz, namelen, true, &f) ||
          FindSuffixed(name, pathbuf, pathbufsz, namelen, true, &f) ||
          FindVerbatim(name, pathbuf, pathbufsz, namelen, false, &f) ||
          FindSuffixed(name, pathbuf, pathbufsz, namelen, false, &f)))) {
      errno = e;
      res = pathbuf;
    } else {
      errno = f;
    }
  }
  STRACE("commandv(%#s, %p, %'zu) → %#s% m", name, pathbuf, pathbufsz, res);
  return res;
}
