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
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/zip.h"
#include "libc/zipos/zipos.internal.h"

/**
 * Returns pointer to zip central directory of current executable.
 */
struct Zipos *__zipos_get(void) {
  static struct Zipos zipos;
  static bool once;
  const char *exe;
  size_t mapsize;
  uint8_t *cdir;
  void *map;
  if (!once) {
    exe = (const char *)getauxval(AT_EXECFN);
    if ((zipos.fd = open(exe, O_RDONLY)) != -1) {
      if ((mapsize = getfiledescriptorsize(zipos.fd)) != SIZE_MAX &&
          (map = mmap(NULL, mapsize, PROT_READ, MAP_SHARED, zipos.fd, 0)) !=
              MAP_FAILED) {
        if ((cdir = zipfindcentraldir(map, mapsize))) {
          zipos.map = map;
          zipos.cdir = cdir;
        } else {
          munmap(map, mapsize);
        }
      }
    }
    once = true;
  }
  return zipos.cdir ? &zipos : NULL;
}
