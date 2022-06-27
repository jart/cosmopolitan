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
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/macros.internal.h"
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
    PrefetchVirtualMemory_ = /* win8.1+ */
        GetProcAddressModule("Kernel32.dll", "PrefetchVirtualMemory");
    once = true;
  }
  return PrefetchVirtualMemory_;
}

forceinline typeof(OfferVirtualMemory) *GetOfferVirtualMemory(void) {
  static bool once;
  static typeof(OfferVirtualMemory) *OfferVirtualMemory_;
  if (!once) {
    OfferVirtualMemory_ = /* win8.1+ */
        GetProcAddressModule("Kernel32.dll", "OfferVirtualMemory");
    once = true;
  }
  return OfferVirtualMemory_;
}

textwindows int sys_madvise_nt(void *addr, size_t length, int advice) {
  uint32_t rangecount;
  struct NtMemoryRangeEntry ranges[1];
  if (advice == MADV_WILLNEED || advice == MADV_SEQUENTIAL) {
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
  } else if (advice == MADV_FREE) {
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
