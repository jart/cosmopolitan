/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/calls.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"

static const char *fixpathname(const char *pathname, int flags) {
  if ((flags & O_ACCMODE) == O_RDONLY && strcmp(pathname, "-") == 0) {
    return "/dev/stdin";
  } else if ((flags & O_ACCMODE) == O_WRONLY && strcmp(pathname, "-") == 0) {
    return "/dev/stdout";
  } else {
    return pathname;
  }
}

static int openpathname(const char *pathname, int flags, bool *out_noclose) {
  if ((flags & O_ACCMODE) == O_RDONLY && strcmp(pathname, "/dev/stdin") == 0) {
    *out_noclose = true;
    return fileno(stdin);
  } else if ((flags & O_ACCMODE) == O_WRONLY &&
             strcmp(pathname, "/dev/stdout") == 0) {
    *out_noclose = true;
    return fileno(stdout);
  } else {
    *out_noclose = false;
    return open(pathname, flags, 0666);
  }
}

/**
 * Opens file as stream object.
 *
 * @param pathname is a utf-8 ideally relative filename
 * @param mode is the string mode/flag DSL see fopenflags()
 * @return new object to be free'd by fclose() or NULL w/ errno
 * @note microsoft unilaterally deprecated this function lool
 */
FILE *fopen(const char *pathname, const char *mode) {
  FILE *f;
  bool noclose;
  int fd, flags;
  flags = fopenflags(mode);
  pathname = fixpathname(pathname, flags);
  if ((fd = openpathname(pathname, flags, &noclose)) != -1) {
    if ((f = fdopen(fd, mode)) != NULL) {
      f->noclose = noclose;
      return f;
    } else {
      if (!noclose) close(fd);
      return NULL;
    }
  } else {
    return NULL;
  }
}
