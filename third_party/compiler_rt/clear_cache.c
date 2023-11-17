//===-- clear_cache.c - Implement __clear_cache ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "int_lib.h"
#include "libc/dce.h"
#include "libc/runtime/syslib.internal.h"

// The compiler generates calls to __clear_cache() when creating
// trampoline functions on the stack for use with nested functions.
// It is expected to invalidate the instruction cache for the
// specified range.

void __clear_cache(void *start, void *end) {

#ifdef __aarch64__
  if (IsXnu()) {
    __syslib->__sys_icache_invalidate((char *)start,
                                      (char *)end - (char *)start);
    return;
  }
  uint64_t xstart = (uint64_t)(uintptr_t)start;
  uint64_t xend = (uint64_t)(uintptr_t)end;
  uint64_t addr;
  // Get Cache Type Info
  uint64_t ctr_el0;
  __asm __volatile("mrs %0, ctr_el0" : "=r"(ctr_el0));
  // dc & ic instructions must use 64bit registers so we don't use
  // uintptr_t in case this runs in an IPL32 environment.
  const size_t dcache_line_size = 4 << ((ctr_el0 >> 16) & 15);
  for (addr = xstart & ~(dcache_line_size - 1); addr < xend;
       addr += dcache_line_size)
    __asm __volatile("dc cvau, %0" ::"r"(addr));
  __asm __volatile("dsb ish");
  const size_t icache_line_size = 4 << ((ctr_el0 >> 0) & 15);
  for (addr = xstart & ~(icache_line_size - 1); addr < xend;
       addr += icache_line_size)
    __asm __volatile("ic ivau, %0" ::"r"(addr));
  __asm __volatile("isb sy");

#elif defined(__powerpc64__)
  const size_t line_size = 32;
  const size_t len = (uintptr_t)end - (uintptr_t)start;

  const uintptr_t mask = ~(line_size - 1);
  const uintptr_t start_line = ((uintptr_t)start) & mask;
  const uintptr_t end_line = ((uintptr_t)start + len + line_size - 1) & mask;

  for (uintptr_t line = start_line; line < end_line; line += line_size) {
    __asm__ volatile("dcbf 0, %0" : : "r"(line));
  }
  __asm__ volatile("sync");
  for (uintptr_t line = start_line; line < end_line; line += line_size) {
    __asm__ volatile("icbi 0, %0" : : "r"(line));
  }
  __asm__ volatile("isync");

#else
  compilerrt_abort();
#endif
}
