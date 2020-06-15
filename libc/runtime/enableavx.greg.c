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
#include "ape/lib/pc.h"
#include "libc/dce.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/nt/struct/context.h"
#include "libc/nt/thunk/msabi.h"

#define kNtContextXstate 0x00100040
#define kNtXstateAvx     2
#define kNtXstateMaskAvx (1ull << kNtXstateAvx)

static void EnableAvx(void) {
  asm volatile("mov\t%%cr4,%%rax\n\t"
               "or\t%0,%%rax\n\t"
               "mov\t%%rax,%%cr4\n\t"
               "xor\t%%ecx,%%ecx\n\t"
               "xgetbv\n\t"
               "or\t%1,%%eax\n\t"
               "xsetbv"
               : /* no outputs */
               : "i"(CR4_OSXSAVE), "i"(XCR0_X87 | XCR0_SSE | XCR0_AVX)
               : "rax", "rcx", "rdx", "memory", "cc");
}

static void EnableAvxOnWindows(void) {
  /* typedef uint64_t (*getenabledxstatefeatures_f)(void) __msabi; */
  /* typedef bool32 (*initializecontext_f)(void *buffer, uint32_t flags, */
  /*                                       struct NtContext **out_context, */
  /*                                       uint32_t *inout_buffersize) __msabi;
   */
  /* typedef bool32 (*getxstatefeaturesmask_f)(struct NtContext * context, */
  /*                                           uint64_t * out_featuremask)
   * __msabi; */
  /* typedef bool32 (*setxstatefeaturesmask_f)(struct NtContext * context, */
  /*                                           uint64_t featuremask) __msabi; */
  /* getenabledxstatefeatures_f GetEnabledXStateFeatures; */
  /* initializecontext_f InitializeContext; */
  /* getxstatefeaturesmask_f GetXStateFeaturesMask; */
  /* setxstatefeaturesmask_f SetXStateFeaturesMask; */
}

/**
 * Requests authorization from operating system to do 256-bit math.
 * @assume avx cpuid check performed by caller
 */
int _init_enableavx(void) {
  if (IsMetal()) {
    EnableAvx();
  } else if (IsWindows()) {
    EnableAvxOnWindows();
  }
  return 0;
}
