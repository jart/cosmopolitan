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
#include "libc/calls/internal.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/sysv/errfuns.h"

textwindows int chdir$nt(const char *path) {
  int len;
  char16_t path16[PATH_MAX];
  if ((len = mkntpath(path, path16)) == -1) return -1;
  if (path16[len - 1] != u'/' && path16[len - 1] != u'\\') {
    path16[len + 0] = u'/';
    path16[len + 1] = u'\0';
  }
  if (SetCurrentDirectory(path16)) {
    return 0;
  } else {
    return __winerr();
  }
}
