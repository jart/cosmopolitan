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
#include "libc/macros.h"
#include "libc/nt/enum/offerpriority.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/memoryrangeentry.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/errfuns.h"

forceinline typeof(PrefetchVirtualMemory) *GetPrefetchVirtualMemory(void) {
  static bool once;
  static typeof(PrefetchVirtualMemory) *PrefetchVirtualMemory_;
  if (!once) {
    once = true;
    PrefetchVirtualMemory_ = /* win8.1+ */
        GetProcAddressModule("KernelBase.dll", "PrefetchVirtualMemory");
  }
  return PrefetchVirtualMemory_;
}

forceinline typeof(OfferVirtualMemory) *GetOfferVirtualMemory(void) {
  static bool once;
  static typeof(OfferVirtualMemory) *OfferVirtualMemory_;
  if (!once) {
    once = true;
    OfferVirtualMemory_ = /* win8.1+ */
        GetProcAddressModule("KernelBase.dll", "OfferVirtualMemory");
  }
  return OfferVirtualMemory_;
}

textwindows int madvise$nt(void *addr, size_t length, int advice) {
  uint32_t rangecount;
  struct NtMemoryRangeEntry ranges[1];
  if ((advice & (int)MADV_WILLNEED) == (int)MADV_WILLNEED ||
      (advice & (int)MADV_SEQUENTIAL) == (int)MADV_SEQUENTIAL) {
    typeof(PrefetchVirtualMemory) *fn = GetPrefetchVirtualMemory();
    if (fn) {
      ranges[0].VirtualAddress = addr;
      ranges[0].NumberOfBytes = length;
      rangecount = ARRAYLEN(ranges);
      if (fn(GetCurrentProcess(), &rangecount, ranges, 0)) {
        return 0;
      } else {
        return __winerr();
      }
    } else {
      return enosys();
    }
  } else if ((advice & (int)MADV_FREE) == (int)MADV_FREE) {
    typeof(OfferVirtualMemory) *fn = GetOfferVirtualMemory();
    if (fn) {
      if (fn(addr, length, kNtVmOfferPriorityNormal)) {
        return 0;
      } else {
        return __winerr();
      }
    } else {
      return enosys();
    }
  } else {
    return einval();
  }
}
