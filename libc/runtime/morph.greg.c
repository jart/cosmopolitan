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
#include "libc/assert.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asmflag.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.internal.h"
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

static inline int __morph_rt_sigprocmask(int h, const sigset_t *s, sigset_t *o,
                                         size_t c) {
#ifdef __aarch64__
  register long r0 asm("x0") = (long)h;
  register long r1 asm("x1") = (long)s;
  register long r2 asm("x2") = (long)o;
  register long r3 asm("x3") = (long)c;
  register long res_x0 asm("x0");
  asm volatile("mov\tx8,%1\n\t"
               "svc\t0"
               : "=r"(res_x0)
               : "i"(135), "r"(r0), "r"(r1), "r"(r2), "r"(r3)
               : "x8", "memory");
  return res_x0;
#else
  return 0;
#endif
}

static inline int __morph_sigprocmask(int how, const sigset_t *set,
                                      sigset_t *oldset) {
  return __morph_rt_sigprocmask(how, set, oldset, sizeof(*set));
}

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
      _Exit(26);
    }
#endif
    _npassert(!ax);
  } else {
    __imp_VirtualProtect(addr, size, ntprot, &op);
  }
#elif defined(__aarch64__)
  register long r0 asm("x0") = (long)addr;
  register long r1 asm("x1") = (long)size;
  register long r2 asm("x2") = (long)prot;
  register long res_x0 asm("x0");
  asm volatile("mov\tx8,%1\n\t"
               "svc\t0"
               : "=r"(res_x0)
               : "i"(226), "r"(r0), "r"(r1), "r"(r2)
               : "x8", "memory");
  _npassert(!res_x0);
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
  STRACE("__morph_begin()");
#ifdef __x86_64__
  if (IsOpenbsd()) {
    asm volatile(CFLAG_ASM("syscall")
                 : CFLAG_CONSTRAINT(cf), "=a"(ax), "=d"(dx)
                 : "1"(__NR_sigprocmask), "D"(SIG_BLOCK), "S"(-1u)
                 : "rcx", "r8", "r9", "r10", "r11", "memory");
    save->__bits[0] = ax & 0xffffffff;
    _npassert(!cf);
  } else if (!IsWindows() && !IsMetal()) {
    asm volatile("mov\t$8,%%r10d\n\t"
                 "syscall"
                 : "=a"(ax), "=d"(dx)
                 : "0"(__NR_sigprocmask), "D"(SIG_BLOCK), "S"(&ss), "1"(save)
                 : "rcx", "r8", "r9", "r10", "r11", "memory", "cc");
    _npassert(!ax);
  }
#else
  __morph_sigprocmask(SIG_BLOCK, &ss, save);
#endif
  __morph_mprotect(_base, __privileged_addr - _base, PROT_READ | PROT_WRITE,
                   kNtPageWritecopy);
}

/**
 * Begins code morphing executable.
 */
privileged void __morph_end(sigset_t *save) {
  int ax;
  long dx;
  bool cf;
  __morph_mprotect(_base, __privileged_addr - _base, PROT_READ | PROT_EXEC,
                   kNtPageExecuteRead);
#ifdef __x86_64__
  if (IsOpenbsd()) {
    asm volatile(CFLAG_ASM("syscall")
                 : CFLAG_CONSTRAINT(cf), "=a"(ax), "=d"(dx)
                 : "1"(__NR_sigprocmask), "D"(SIG_SETMASK), "S"(save->__bits[0])
                 : "rcx", "r8", "r9", "r10", "r11", "memory");
    _npassert(!cf);
  } else if (!IsWindows() && !IsMetal()) {
    asm volatile("mov\t$8,%%r10d\n\t"
                 "syscall"
                 : "=a"(ax), "=d"(dx)
                 : "0"(__NR_sigprocmask), "D"(SIG_SETMASK), "S"(save), "1"(0)
                 : "rcx", "r8", "r9", "r10", "r11", "memory", "cc");
    _npassert(!ax);
  }
#else
  __morph_sigprocmask(SIG_SETMASK, save, 0);
#endif
  STRACE("__morph_end()");
}
