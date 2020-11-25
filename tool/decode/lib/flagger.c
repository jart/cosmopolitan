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
#include "libc/alg/arraylist2.internal.h"
#include "libc/fmt/fmt.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "tool/decode/lib/flagger.h"

/**
 * Formats numeric flags integer as symbolic code.
 *
 * @param names maps individual flags to string names in no order
 * @param id is the flags
 * @return NUL-terminated string that needs free()
 */
nodiscard char *RecreateFlags(const struct IdName *names, unsigned long id) {
  bool first;
  size_t bufi, bufn;
  char *bufp, extrabuf[20];
  bufi = 0;
  bufn = 0;
  bufp = NULL;
  first = true;
  for (; names->name; names++) {
    if ((id == 0 && names->id == 0) ||
        (id != 0 && names->id != 0 && (id & names->id) == names->id)) {
      id &= ~names->id;
      if (!first) {
        APPEND(&bufp, &bufi, &bufn, "|");
      } else {
        first = false;
      }
      CONCAT(&bufp, &bufi, &bufn, names->name, strlen(names->name));
    }
  }
  if (id) {
    if (bufi) APPEND(&bufp, &bufi, &bufn, "|");
    CONCAT(&bufp, &bufi, &bufn, extrabuf,
           snprintf(extrabuf, sizeof(extrabuf), "%#x", id));
  } else if (!bufi) {
    APPEND(&bufp, &bufi, &bufn, "0");
  }
  return bufp;
}
