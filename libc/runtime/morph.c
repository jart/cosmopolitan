/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#define ShouldUseMsabiAttribute() 1
#include "ape/sections.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asmflag.h"
#include "libc/intrin/kprintf.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/memory.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/prot.h"

__msabi extern typeof(VirtualProtect) *const __imp_VirtualProtect;

__funline void __morph_mprotect(void *addr, size_t size, int prot, int ntprot) {
#ifdef __x86_64__
  bool cf;
  int ax, dx;
  uint32_t op;
  if (!IsWindows()) {
    asm volatile(CFLAG_ASM("clc\n\t"
                           "syscall")
                 : CFLAG_CONSTRAINT(cf), "=a"(ax), "=d"(dx)
                 : "1"(__NR_mprotect), "D"(addr), "S"(size), "2"(prot)
                 : "rcx", "r8", "r9", "r10", "r11", "memory");
#ifndef NDEBUG
    if (cf) ax = -ax;
    if (ax == -EPERM) {
      kprintf("error: need pledge(prot_exec) permission to code morph\n");
    }
    if (ax < 0) {
      kprintf("error: __morph_mprotect(%p, %#zx, %d) failed: errno=%d\n", addr,
              size, prot, -ax);
    }
#endif
  } else {
    __imp_VirtualProtect(addr, size, ntprot, &op);
  }
#elif defined(__aarch64__)
  register long r0 asm("x0") = (long)addr;
  register long r1 asm("x1") = (long)size;
  register long r2 asm("x2") = (long)prot;
  register long r8 asm("x8") = (long)__NR_mprotect;
  register long r16 asm("x16") = (long)__NR_mprotect;
  asm volatile("svc\t0"
               : "+r"(r0)
               : "r"(r1), "r"(r2), "r"(r8), "r"(r16)
               : "memory");
#endif
}

/**
 * Begins code morphing executable.
 *
 * The following example
 *
 *     #include <cosmo.h>
 *     #include <stdlib.h>
 *
 *     privileged int main() {  // privileged code is unmodifiable
 *       ShowCrashReports();    // print report if trapped
 *       __morph_begin(0);      // make executable code R+W
 *       *(char *)exit = 0xCC;  // turn exit() into an INT3 trap
 *       __morph_end();         // make executable code R+X
 *       exit(0);               // won't actually exit
 *     }
 *
 * shows how the exit() function can be recompiled at runtime to become
 * an int3 (x86-64) debugger trap. What makes it tricky is Cosmopolitan
 * maintains a R^X invariant, in order to support OpenBSD. So when code
 * wants to modify some part of the executable image in memory the vast
 * majority of the code stops being executable during that time, unless
 * it's been linked into a special privileged section of the binary. It
 * is only possible to code morph from privileged functions. Privileged
 * functions are also only allowed to call other privileged functions.
 */
privileged void __morph_begin(void) {
  __morph_mprotect(__executable_start, __privileged_start - __executable_start,
                   PROT_READ | PROT_WRITE, kNtPageWritecopy);
}

/**
 * Finishes code morphing executable.
 */
privileged void __morph_end(void) {
  __morph_mprotect(__executable_start, __privileged_start - __executable_start,
                   PROT_READ | PROT_EXEC, kNtPageExecuteRead);
}
