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
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/directmap.h"

static textwindows struct DirectMap DirectMapNt(void *addr, size_t size,
                                                unsigned prot, unsigned flags,
                                                int fd, int64_t off) {
  struct DirectMap res;
  if ((res.maphandle = CreateFileMappingNuma(
           fd != -1 ? g_fds.p[fd].handle : kNtInvalidHandleValue,
           &kNtIsInheritable, prot2nt(prot, flags), size >> 32, size, NULL,
           kNtNumaNoPreferredNode))) {
    if (!(res.addr = MapViewOfFileExNuma(res.maphandle, fprot2nt(prot, flags),
                                         off >> 32, off, size, addr,
                                         kNtNumaNoPreferredNode))) {
      CloseHandle(res.maphandle);
      res.maphandle = kNtInvalidHandleValue;
      res.addr = (void *)(intptr_t)winerr();
    }
  } else {
    res.maphandle = kNtInvalidHandleValue;
    res.addr = (void *)(intptr_t)winerr();
  }
  return res;
}

struct DirectMap DirectMap(void *addr, size_t size, unsigned prot,
                           unsigned flags, int fd, int64_t off) {
  if (!IsWindows()) {
    return (struct DirectMap){mmap$sysv(addr, size, prot, flags, fd, off),
                              kNtInvalidHandleValue};
  } else {
    return DirectMapNt(addr, size, prot, flags, fd, off);
  }
}
