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
#include "libc/conv/itoa.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

static uintptr_t lastfree_;

void *AddressBirthAction(void *addr) {
  char buf[64], *p;
  p = buf;
  p = stpcpy(p, __FUNCTION__);
  p = stpcpy(p, ": 0x");
  p += uint64toarray_radix16((uintptr_t)addr, p);
  *p++ = '\n';
  __print(buf, p - buf);
  if (lastfree_ == (uintptr_t)addr) {
    lastfree_ = 0;
  }
  return addr;
}

void *AddressDeathAction(void *addr) {
  char buf[64], *p;
  p = buf;
  p = stpcpy(p, __FUNCTION__);
  p = stpcpy(p, ": 0x");
  p += uint64toarray_radix16((uintptr_t)addr, p);
  if (lastfree_ != (uintptr_t)addr) {
    lastfree_ = (uintptr_t)addr;
  } else {
    p = stpcpy(p, " [OBVIOUS DOUBLE FREE]");
  }
  *p++ = '\n';
  __print(buf, p - buf);
  return addr;
}
