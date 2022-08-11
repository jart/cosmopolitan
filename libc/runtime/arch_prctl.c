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
#include "libc/bits/asmflag.h"
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/nexgen32e/msr.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/pc.internal.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"

/**
 * @fileoverview Memory segmentation system calls.
 *
 * This whole file basically does:
 *
 *     mov foo,%fs
 *     mov foo,%gs
 *     mov %fs,foo
 *     mov %gs,foo
 *
 * Which is nontrivial due to the limitless authoritarianism of
 * operating systems.
 */

#define rdmsr(msr)                                         \
  ({                                                       \
    uint32_t lo, hi;                                       \
    asm volatile("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr)); \
    (uint64_t) hi << 32 | lo;                              \
  })

#define wrmsr(msr, val)                           \
  do {                                            \
    uint64_t val_ = (val);                        \
    asm volatile("wrmsr"                          \
                 : /* no outputs */               \
                 : "c"(msr), "a"((uint32_t)val_), \
                   "d"((uint32_t)(val_ >> 32)));  \
  } while (0)

static inline int arch_prctl_fsgsbase(int code, int64_t addr) {
  switch (code) {
    case ARCH_SET_GS:
      asm volatile("wrgsbase\t%0" : /* no outputs */ : "r"(addr));
      return 0;
    case ARCH_SET_FS:
      asm volatile("wrfsbase\t%0" : /* no outputs */ : "r"(addr));
      return 0;
    case ARCH_GET_GS:
      asm volatile("rdgsbase\t%0" : "=r"(*(int64_t *)addr));
      return 0;
    case ARCH_GET_FS:
      asm volatile("rdfsbase\t%0" : "=r"(*(int64_t *)addr));
      return 0;
    default:
      return einval();
  }
}

static int arch_prctl_msr(int code, int64_t addr) {
  switch (code) {
    case ARCH_SET_GS:
      wrmsr(MSR_IA32_GS_BASE, addr);
      return 0;
    case ARCH_SET_FS:
      wrmsr(MSR_IA32_FS_BASE, addr);
      return 0;
    case ARCH_GET_GS:
      *(int64_t *)addr = rdmsr(MSR_IA32_GS_BASE);
      return 0;
    case ARCH_GET_FS:
      *(int64_t *)addr = rdmsr(MSR_IA32_FS_BASE);
      return 0;
    default:
      return einval();
  }
}

static int arch_prctl_freebsd(int code, int64_t addr) {
  switch (code) {
    case ARCH_GET_FS:
      return sys_arch_prctl(128, addr);
    case ARCH_SET_FS:
      return sys_arch_prctl(129, addr);
    case ARCH_GET_GS:
      return sys_arch_prctl(130, addr);
    case ARCH_SET_GS:
      return sys_arch_prctl(131, addr);
    default:
      return einval();
  }
}

static privileged dontinline int arch_prctl_xnu(int code, int64_t addr) {
  int ax;
  bool failed;
  switch (code) {
    case ARCH_SET_GS:
      asm volatile(CFLAG_ASM("syscall")
                   : CFLAG_CONSTRAINT(failed), "=a"(ax)
                   : "1"(0x3000003), "D"(addr - 0x30)
                   : "rcx", "r11", "memory", "cc");
      if (failed) errno = ax, ax = -1;
      return ax;
    case ARCH_GET_FS:
    case ARCH_SET_FS:
    case ARCH_GET_GS:
      return enosys();
    default:
      return einval();
  }
}

static privileged dontinline int arch_prctl_openbsd(int code, int64_t addr) {
  bool failed;
  int64_t rax;
  switch (code) {
    case ARCH_GET_FS:
      asm volatile(CFLAG_ASM("syscall")
                   : CFLAG_CONSTRAINT(failed), "=a"(rax)
                   : "1"(0x014a /* __get_tcb */)
                   : "rcx", "r11", "cc", "memory");
      if (failed) {
        errno = rax;
        return -1;
      }
      *(int64_t *)addr = rax;
      return 0;
    case ARCH_SET_FS:
      asm volatile(CFLAG_ASM("syscall")
                   : CFLAG_CONSTRAINT(failed), "=a"(rax)
                   : "1"(0x0149 /* __set_tcb */), "D"(addr)
                   : "rcx", "r11", "cc", "memory");
      if (failed) {
        errno = rax;
        rax = -1;
      }
      return rax;
    case ARCH_GET_GS:
    case ARCH_SET_GS:
      return enosys();
    default:
      return einval();
  }
}

static char g_fsgs_once;

/**
 * Don't bother.
 */
int arch_prctl(int code, int64_t addr) {
  void *fn = arch_prctl_fsgsbase;
  switch (__hostos) {
    case METAL:
      return arch_prctl_msr(code, addr);
    case FREEBSD:
      // TODO(jart): this should use sysarch()
      return arch_prctl_freebsd(code, addr);
    case OPENBSD:
      return arch_prctl_openbsd(code, addr);
    case LINUX:
      return sys_arch_prctl(code, addr);
    case XNU:
      /* probably won't work */
      return arch_prctl_xnu(code, addr);
    default:
      return enosys();
  }
}
