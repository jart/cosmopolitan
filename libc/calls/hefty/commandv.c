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
#include "libc/alg/alg.h"
#include "libc/bits/progn.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/conv/conv.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/nt/ntdll.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/errfuns.h"

static struct critbit0 g_commandv;

textstartup static void g_commandv_init(void) {
  __cxa_atexit(critbit0_clear, &g_commandv, NULL);
}

const void *const g_commandv_ctor[] initarray = {g_commandv_init};

static int accessexe(char pathname[hasatleast PATH_MAX], size_t len,
                     const char *ext) {
  len = stpcpy(&pathname[len], ext) - &pathname[0];
  if (access(pathname, X_OK) != -1) {
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
  if (pathlen) *p++ = '/';
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
  char *ep, *path, *pathtok;
  struct critbit0 deduplicate;
  rc = -1;
  pathtok = ep =
      strdup(firstnonnull(getenv("PATH"), "/bin:/usr/local/bin:/usr/bin"));
  memset(&deduplicate, 0, sizeof(deduplicate));
  while ((path = strsep(&pathtok, IsWindows() ? ";" : ":"))) {
    if (strchr(path, '=')) continue;
    if (!critbit0_insert(&deduplicate, path)) continue;
    if ((rc = accesscmd(pathname, path, name, namelen)) != -1) {
      break;
    }
  }
  critbit0_clear(&deduplicate);
  free(ep);
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

static const char *cachecmd(const char *name, size_t namelen,
                            const char *pathname, size_t pathnamelen) {
  size_t entrylen;
  char *res, *entry;
  if ((entry = malloc((entrylen = namelen + 1 + pathnamelen) + 1))) {
    mkcmdquery(name, namelen, entry);
    res = memcpy(&entry[namelen + 1], pathname, pathnamelen + 1);
    critbit0_emplace(&g_commandv, entry, entrylen);
  } else {
    res = NULL;
  }
  return res;
}

static const char *getcmdcache(const char *name, size_t namelen,
                               char scratch[hasatleast PATH_MAX]) {
  const char *entry;
  if ((entry = critbit0_get(&g_commandv, mkcmdquery(name, namelen, scratch)))) {
    return &entry[namelen + 1];
  }
  return NULL;
}

noinline static const char *findcmdpath(const char *name,
                                        char pathname[hasatleast PATH_MAX]) {
  char *p;
  int rc, olderr;
  size_t len;
  olderr = errno;
  if (!(len = strlen(name))) return PROGN(enoent(), NULL);
  if (memchr(name, '=', len)) return PROGN(einval(), NULL);
  if ((p = getcmdcache(name, len, pathname)) ||
      (((IsWindows() &&
         ((rc = accesscmd(pathname, kNtSystemDirectory, name, len)) != -1 ||
          (rc = accesscmd(pathname, kNtWindowsDirectory, name, len)) != -1)) ||
        (rc = accesscmd(pathname, "", name, len)) != -1 ||
        (!strpbrk(name, "/\\") &&
         (rc = searchcmdpath(pathname, name, len)) != -1)) &&
       (p = cachecmd(name, len, pathname, rc)))) {
    errno = olderr;
    return p;
  } else {
    return NULL;
  }
}

/**
 * Resolves pathname of executable.
 *
 * This does the same thing as `command -v` in bourne shell. Path
 * lookups are cached for the lifetime of the process. Paths with
 * multiple components will skip the resolution process. Undotted
 * basenames get automatic .com and .exe suffix resolution on all
 * platforms. Windows' system directories will always trump PATH.
 *
 * @return execve()'able path, or NULL w/ errno
 * @errno ENOENT, EACCES, ENOMEM
 * @see free(), execvpe()
 */
const char *commandv(const char *name) {
  char pathname[PATH_MAX];
  return findcmdpath(name, pathname);
}
