/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/internal.h"
#include "libc/nt/enum/filemapflags.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/directmap.h"
#include "libc/sysv/consts/prot.h"

textwindows struct DirectMap __mmap$nt(void *addr, size_t size, unsigned prot,
                                       int64_t handle, int64_t off) {
  struct DirectMap dm;
  if ((dm.maphandle = CreateFileMappingNuma(
           handle, &kNtIsInheritable,
           (prot & PROT_WRITE) ? kNtPageExecuteReadwrite : kNtPageExecuteRead,
           size >> 32, size, NULL, kNtNumaNoPreferredNode))) {
    if (!(dm.addr = MapViewOfFileExNuma(
              dm.maphandle,
              (prot & PROT_WRITE)
                  ? kNtFileMapWrite | kNtFileMapExecute | kNtFileMapRead
                  : kNtFileMapExecute | kNtFileMapRead,
              off >> 32, off, size, addr, kNtNumaNoPreferredNode))) {
      CloseHandle(dm.maphandle);
      dm.maphandle = kNtInvalidHandleValue;
      dm.addr = (void *)(intptr_t)__winerr();
    }
  } else {
    dm.maphandle = kNtInvalidHandleValue;
    dm.addr = (void *)(intptr_t)__winerr();
  }
  return dm;
}
