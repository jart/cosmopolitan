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
