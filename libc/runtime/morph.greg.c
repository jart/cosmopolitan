/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asmflag.h"
#include "libc/intrin/kprintf.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"

__msabi extern typeof(VirtualProtect) *const __imp_VirtualProtect;

#ifdef __aarch64__
static privileged void __aarch64_sigprocmask(int how, const sigset_t *set,
                                             sigset_t *oldset) {
  register int r0 asm("x0") = how;
  register long r1 asm("x1") = (long)set;
  register long r2 asm("x2") = (long)oldset;
  register long r3 asm("x3") = 8;
  register long r8 asm("x8") = __NR_sigprocmask;
  register long r16 asm("x16") = __NR_sigprocmask;
  asm volatile("svc\t0"
               : "+r"(r0)
               : "r"(r1), "r"(r2), "r"(r3), "r"(r8), "r"(r16)
               : "memory");
}
#endif

static privileged void __morph_mprotect(void *addr, size_t size, int prot,
                                        int ntprot) {
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
#endif
    if (ax) notpossible;
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
 * @return 0 on success, or -1 w/ errno
 */
privileged void __morph_begin(sigset_t *save) {
  int ax;
  bool cf;
  intptr_t dx;
  sigset_t ss = {{-1, -1}};
#ifdef __x86_64__
  if (IsOpenbsd()) {
    asm volatile(CFLAG_ASM("syscall")
                 : CFLAG_CONSTRAINT(cf), "=a"(ax), "=d"(dx)
                 : "1"(__NR_sigprocmask), "D"(SIG_BLOCK), "S"(-1u)
                 : "rcx", "r8", "r9", "r10", "r11", "memory");
    save->__bits[0] = ax & 0xffffffff;
    if (cf) notpossible;
  } else if (!IsWindows() && !IsMetal()) {
    asm volatile("mov\t$8,%%r10d\n\t"
                 "syscall"
                 : "=a"(ax), "=d"(dx)
                 : "0"(__NR_sigprocmask), "D"(SIG_BLOCK), "S"(&ss), "1"(save)
                 : "rcx", "r8", "r9", "r10", "r11", "memory", "cc");
    if (ax) notpossible;
  }
#elif defined(__aarch64__)
  __aarch64_sigprocmask(SIG_BLOCK, &ss, save);
#else
#error "unsupported architecture"
#endif
  __morph_mprotect(__executable_start, __privileged_start - __executable_start,
                   PROT_READ | PROT_WRITE, kNtPageWritecopy);
}

/**
 * Begins code morphing executable.
 */
privileged void __morph_end(sigset_t *save) {
  int ax;
  long dx;
  bool cf;
  __morph_mprotect(__executable_start, __privileged_start - __executable_start,
                   PROT_READ | PROT_EXEC, kNtPageExecuteRead);
#ifdef __x86_64__
  if (IsOpenbsd()) {
    asm volatile(CFLAG_ASM("syscall")
                 : CFLAG_CONSTRAINT(cf), "=a"(ax), "=d"(dx)
                 : "1"(__NR_sigprocmask), "D"(SIG_SETMASK), "S"(save->__bits[0])
                 : "rcx", "r8", "r9", "r10", "r11", "memory");
    if (cf) notpossible;
  } else if (!IsWindows() && !IsMetal()) {
    asm volatile("mov\t$8,%%r10d\n\t"
                 "syscall"
                 : "=a"(ax), "=d"(dx)
                 : "0"(__NR_sigprocmask), "D"(SIG_SETMASK), "S"(save), "1"(0L)
                 : "rcx", "r8", "r9", "r10", "r11", "memory", "cc");
    if (ax) notpossible;
  }
#elif defined(__aarch64__)
  __aarch64_sigprocmask(SIG_SETMASK, save, 0);
#else
#error "unsupported architecture"
#endif
}
