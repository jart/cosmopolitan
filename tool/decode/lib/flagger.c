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
#include "libc/alg/arraylist.h"
#include "libc/fmt/fmt.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "tool/decode/lib/flagger.h"

struct FlagNameBuf {
  size_t i, n;
  char *p;
};

/**
 * Formats numeric flags integer as symbolic code.
 *
 * @param names maps individual flags to string names in no order
 * @param id is the flags
 * @return NUL-terminated string that needs free()
 */
nodiscard char *recreateflags(const struct IdName *names, unsigned long id) {
  struct FlagNameBuf buf = {};
  char extrabuf[20];
  bool first;
  first = true;
  for (; names->name; names++) {
    if ((id == 0 && names->id == 0) ||
        (id != 0 && names->id != 0 && (id & names->id) == names->id)) {
      id &= ~names->id;
      if (!first) {
        append(&buf, "|");
      } else {
        first = false;
      }
      concat(&buf, names->name, strlen(names->name));
    }
  }
  if (id) {
    if (buf.i) append(&buf, "|");
    concat(&buf, extrabuf, snprintf(extrabuf, sizeof(extrabuf), "%#x", id));
  } else if (!buf.i) {
    append(&buf, "0");
  }
  return buf.p;
}
