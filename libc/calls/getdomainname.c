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
#include "libc/calls/internal.h"
#include "libc/calls/struct/utsname.h"
#include "libc/dce.h"
#include "libc/macros.h"
#include "libc/nt/enum/computernameformat.h"
#include "libc/nt/errors.h"
#include "libc/nt/runtime.h"
#include "libc/nt/systeminfo.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

int getdomainname(char *name, size_t len) {
  uint32_t nSize;
  char16_t name16[256];
  struct utsname u;
  if (len < 1) return einval();
  if (!name) return efault();
  if (!IsWindows()) {
    if (uname(&u) == -1) return -1;
    if (!memccpy(name, u.domainname[0] ? u.domainname : u.nodename, '\0',
                 len)) {
      name[len - 1] = '\0';
    }
    return 0;
  } else {
    nSize = ARRAYLEN(name16);
    if (!GetComputerNameEx(kNtComputerNameDnsFullyQualified, name16, &nSize)) {
      return __winerr();
    }
    tprecode16to8(name, MIN(MIN(ARRAYLEN(name16), nSize + 1), len), name16);
    return 0;
  }
}
