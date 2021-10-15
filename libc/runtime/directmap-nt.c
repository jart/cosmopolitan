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
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sysdebug.internal.h"
#include "libc/macros.internal.h"
#include "libc/nt/enum/filemapflags.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/runtime/directmap.internal.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"

textwindows noasan struct DirectMap sys_mmap_nt(void *addr, size_t size,
                                                int prot, int flags,
                                                int64_t handle, int64_t off) {
  /* asan runtime depends on this function */
  uint32_t got;
  size_t i, upsize;
  struct DirectMap dm;
  struct NtOverlapped op;
  if ((prot & PROT_WRITE) && (flags & MAP_PRIVATE) && handle != -1) {
    /*
     * WIN32 claims it can do COW mappings but we still haven't found a
     * combination of flags, that'll cause Windows to actually do this!
     */
    upsize = ROUNDUP(size, FRAMESIZE);
    dm.maphandle = CreateFileMappingNuma(-1, &kNtIsInheritable,
                                         kNtPageExecuteReadwrite, upsize >> 32,
                                         upsize, NULL, kNtNumaNoPreferredNode);
    SYSDEBUG("CreateFileMappingNuma(-1, kNtPageExecuteReadwrite, 0x%x/0x%x) -> "
             "0x%x",
             upsize, size, dm.maphandle);
    if (dm.maphandle) {
      dm.addr =
          MapViewOfFileExNuma(dm.maphandle, kNtFileMapWrite | kNtFileMapExecute,
                              0, 0, upsize, addr, kNtNumaNoPreferredNode);
      SYSDEBUG("MapViewOfFileExNuma(WX, 0x%x) -> addr:0x%x", addr, dm.addr);
      if (dm.addr) {
        for (i = 0; i < size; i += got) {
          got = 0;
          op.Internal = 0;
          op.InternalHigh = 0;
          op.Pointer = (void *)(uintptr_t)i;
          op.hEvent = 0;
          if (!ReadFile(handle, (char *)dm.addr + i, size - i, &got, &op)) {
            break;
          }
        }
        if (i == size) {
          return dm;
        }
        UnmapViewOfFile(dm.addr);
      }
      CloseHandle(dm.maphandle);
    }
  } else {
    dm.maphandle = CreateFileMappingNuma(
        handle, &kNtIsInheritable,
        (prot & PROT_WRITE) ? kNtPageExecuteReadwrite : kNtPageExecuteRead,
        handle != -1 ? 0 : size >> 32, handle != -1 ? 0 : size, NULL,
        kNtNumaNoPreferredNode);
    SYSDEBUG("CreateFileMappingNuma(fhand:%d, prot:%s, size:0x%x) -> "
             "handle:0x%x",
             handle, (prot & PROT_WRITE) ? "XRW" : "XR",
             handle != -1 ? 0 : size);
    if (dm.maphandle) {
      dm.addr = MapViewOfFileExNuma(
          dm.maphandle,
          (prot & PROT_WRITE) ? kNtFileMapWrite | kNtFileMapExecute
                              : kNtFileMapRead | kNtFileMapExecute,
          off >> 32, off, size, addr, kNtNumaNoPreferredNode);
      SYSDEBUG(
          "MapViewOfFileExNuma(prot:%s, off:0x%x, size:0x%x, addr:0x%x) -> "
          "addr:0x%x",
          (prot & PROT_WRITE) ? "WX" : "RX", off, size, addr, dm.addr);
      if (dm.addr) {
        return dm;
      } else {
        CloseHandle(dm.maphandle);
      }
    }
  }
  dm.maphandle = kNtInvalidHandleValue;
  dm.addr = (void *)(intptr_t)__winerr();
  return dm;
}
