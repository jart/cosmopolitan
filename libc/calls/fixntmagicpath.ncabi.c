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
#include "libc/calls/ntmagicpaths.h"
#include "libc/nexgen32e/tinystrcmp.h"
#include "libc/sysv/consts/o.h"

textwindows const char *(fixntmagicpath)(const char *path, unsigned flags) {
  const struct NtMagicPaths *mp = &kNtMagicPaths;
  asm("" : "+r"(mp));
  if (path[0] != '/') return path;
  if (tinystrcmp(path, mp->devtty) == 0) {
    if ((flags & O_ACCMODE) == O_RDONLY) {
      return mp->conin;
    } else if ((flags & O_ACCMODE) == O_WRONLY) {
      return mp->conout;
    }
  }
  if (tinystrcmp(path, mp->devnull) == 0) return mp->nul;
  if (tinystrcmp(path, mp->devstdin) == 0) return mp->conin;
  if (tinystrcmp(path, mp->devstdout) == 0) return mp->conout;
  return path;
}
