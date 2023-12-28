/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/nt/enum/offerpriority.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/memoryrangeentry.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/errfuns.h"

typedef bool32 (*__msabi PrefetchVirtualMemoryPtr)(
    int64_t hProcess, uintptr_t NumberOfEntries,
    struct NtMemoryRangeEntry *VirtualAddresses, uint32_t reserved_Flags);

textwindows static PrefetchVirtualMemoryPtr GetPrefetchVirtualMemory(void) {
  static PrefetchVirtualMemoryPtr PrefetchVirtualMemory_;
  if (!PrefetchVirtualMemory_) {
    PrefetchVirtualMemory_ = /* win8.1+ */
        GetProcAddressModule("Kernel32.dll", "PrefetchVirtualMemory");
  }
  return PrefetchVirtualMemory_;
}

typedef bool32 (*__msabi OfferVirtualMemoryPtr)(void *inout_VirtualAddress,
                                                size_t Size, int Priority);

textwindows static OfferVirtualMemoryPtr GetOfferVirtualMemory(void) {
  static OfferVirtualMemoryPtr OfferVirtualMemory_;
  if (!OfferVirtualMemory_) {
    OfferVirtualMemory_ = /* win8.1+ */
        GetProcAddressModule("Kernel32.dll", "OfferVirtualMemory");
  }
  return OfferVirtualMemory_;
}

textwindows int sys_madvise_nt(void *addr, size_t length, int advice) {
  if (advice == MADV_WILLNEED || advice == MADV_SEQUENTIAL) {
    PrefetchVirtualMemoryPtr fn = GetPrefetchVirtualMemory();
    if (fn) {
      if (fn(GetCurrentProcess(), 1, &(struct NtMemoryRangeEntry){addr, length},
             0)) {
        return 0;
      } else {
        return __winerr();
      }
    } else {
      return enosys();
    }
  } else if (advice == MADV_FREE) {
    OfferVirtualMemoryPtr fn = GetOfferVirtualMemory();
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
