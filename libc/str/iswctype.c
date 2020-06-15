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
#include "libc/macros.h"
#include "libc/str/str.h"

static const struct WcTypes {
  char name[8];
  wctype_t type;
} wctypes[] = {{"alnum", 0x006}, {"alpha", 0x002}, {"blank", 0x080},
               {"cntrl", 0x100}, {"digit", 0x004}, {"graph", 0x200},
               {"lower", 0x020}, {"print", 0x010}, {"punct", 0x300},
               {"space", 0x001}, {"upper", 0x040}, {"xdigit", 0x008}};

wctype_t wctype(const char *s) {
  char name[8];
  strncpy(name, s, sizeof(name));
  for (unsigned i = 0; i < ARRAYLEN(wctypes); ++i) {
    if (memcmp(name, wctypes[i].name, sizeof(name)) == 0) {
      return wctypes[i].type;
    }
  }
  return 0;
}

int iswctype(wint_t c, wctype_t type) {
  if (c < 128) {
    if (!(type & ~0xff)) {
      return kCtype[(unsigned char)type];
    }
    if (type == 0x100) return iscntrl((unsigned char)c);
    if (type == 0x200) return isgraph((unsigned char)c);
    if (type == 0x300) return ispunct((unsigned char)c);
  }
  return 0;
}
