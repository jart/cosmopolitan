/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/progn.internal.h"
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/conv/conv.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nt/ntdll.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/errfuns.h"

static int accessexe(char pathname[hasatleast PATH_MAX], size_t len,
                     const char *ext) {
  len = stpcpy(&pathname[len], ext) - &pathname[0];
  if (isexecutable(pathname)) {
    return len;
  } else {
    return -1;
  }
}

static int accesscmd(char pathname[hasatleast PATH_MAX], const char *path,
                     const char *name, size_t namelen) { /* cf. %PATHEXT% */
  int rc;
  char *p;
  bool hasdot;
  size_t pathlen, len;
  pathlen = strlen(path);
  if (pathlen + 1 + namelen + 1 + 4 + 1 > PATH_MAX) return -1;
  p = mempcpy(pathname, path, pathlen);
  if (pathlen && pathname[pathlen - 1] != '/') *p++ = '/';
  p = mempcpy(p, name, namelen);
  len = p - &pathname[0];
  hasdot = !!memchr(basename(name), '.', namelen);
  if ((hasdot && (rc = accessexe(pathname, len, "")) != -1) ||
      (!hasdot &&
       ((rc = accessexe(pathname, len, ".com")) != -1 ||
        (IsWindows() && (rc = accessexe(pathname, len, ".exe")) != -1) ||
        (!IsWindows() && (rc = accessexe(pathname, len, "")) != -1)))) {
    return rc;
  } else {
    return -1;
  }
}

static int searchcmdpath(char pathname[hasatleast PATH_MAX], const char *name,
                         size_t namelen) {
  int rc;
  char *path, *pathtok, ep[PATH_MAX];
  rc = -1;
  if (!memccpy(ep,
               firstnonnull(emptytonull(getenv("PATH")),
                            "/bin:/usr/local/bin:/usr/bin"),
               '\0', sizeof(ep))) {
    return enomem();
  }
  pathtok = ep;
  while ((path = strsep(&pathtok, IsWindows() ? ";" : ":"))) {
    if (strchr(path, '=')) continue;
    if ((rc = accesscmd(pathname, path, name, namelen)) != -1) {
      break;
    }
  }
  return rc;
}

static char *mkcmdquery(const char *name, size_t namelen,
                        char scratch[hasatleast PATH_MAX]) {
  char *p;
  if (namelen + 1 + 1 > PATH_MAX) return NULL;
  p = mempcpy(scratch, name, namelen);
  *p++ = '=';
  *p++ = '\0';
  if (IsWindows() || IsXnu()) strntolower(scratch, namelen);
  return &scratch[0];
}

/**
 * Resolves full pathname of executable.
 *
 * @return execve()'able path, or NULL w/ errno
 * @errno ENOENT, EACCES, ENOMEM
 * @see free(), execvpe()
 */
char *commandv(const char *name, char pathbuf[hasatleast PATH_MAX]) {
  char *p;
  size_t len;
  int rc, olderr;
  olderr = errno;
  if (!(len = strlen(name))) return PROGN(enoent(), NULL);
  if (memchr(name, '=', len)) return PROGN(einval(), NULL);
  if ((IsWindows() &&
       ((rc = accesscmd(pathbuf, kNtSystemDirectory, name, len)) != -1 ||
        (rc = accesscmd(pathbuf, kNtWindowsDirectory, name, len)) != -1)) ||
      (rc = accesscmd(pathbuf, "", name, len)) != -1 ||
      (!strpbrk(name, "/\\") &&
       (rc = searchcmdpath(pathbuf, name, len)) != -1)) {
    errno = olderr;
    return pathbuf;
  } else {
    return NULL;
  }
}
