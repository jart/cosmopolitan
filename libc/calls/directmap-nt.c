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
#include "libc/runtime/directmap.internal.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"

textwindows noasan struct DirectMap sys_mmap_nt(void *addr, size_t size,
                                                int prot, int flags,
                                                int64_t handle, int64_t off) {
  size_t i;
  struct DirectMap dm;
  struct ProtectNt fl;
  const struct NtSecurityAttributes *sec;

  if (flags & MAP_PRIVATE) {
    sec = 0;  // MAP_PRIVATE isn't inherited across fork()
  } else {
    sec = &kNtIsInheritable;  // MAP_SHARED gives us zero-copy fork()
  }

  if ((prot & PROT_WRITE) && (flags & MAP_PRIVATE) && handle != -1) {
    // windows has cow pages but they can't propagate across fork()
    if (prot & PROT_EXEC) {
      fl = (struct ProtectNt){kNtPageExecuteWritecopy,
                              kNtFileMapCopy | kNtFileMapExecute};
    } else {
      fl = (struct ProtectNt){kNtPageWritecopy, kNtFileMapCopy};
    }
  } else {
    fl = __nt2prot(prot);
  }

  if ((dm.maphandle = CreateFileMapping(handle, sec, fl.flags1,
                                        (size + off) >> 32, (size + off), 0))) {
    if ((dm.addr = MapViewOfFileEx(dm.maphandle, fl.flags2, off >> 32, off,
                                   size, addr))) {
      return dm;
    }
    CloseHandle(dm.maphandle);
  }

  dm.maphandle = kNtInvalidHandleValue;
  dm.addr = (void *)(intptr_t)-1;
  return dm;
}
