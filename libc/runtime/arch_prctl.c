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
#include "ape/lib/pc.h"
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/nexgen32e/msr.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/interruptiblecall.h"
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

int arch_prctl$sysv(int, int64_t) hidden;

static inline int arch_prctl$fsgsbase(int code, int64_t addr) {
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

static int arch_prctl$msr(int code, int64_t addr) {
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

static int arch_prctl$freebsd(int code, int64_t addr) {
  switch (code) {
    case ARCH_GET_FS:
      return arch_prctl$sysv(128, addr);
    case ARCH_SET_FS:
      return arch_prctl$sysv(129, addr);
    case ARCH_GET_GS:
      return arch_prctl$sysv(130, addr);
    case ARCH_SET_GS:
      return arch_prctl$sysv(131, addr);
    default:
      return einval();
  }
}

static int arch_prctl$xnu(int code, int64_t addr) {
  int ax;
  switch (code) {
    case ARCH_SET_GS:
      asm volatile("syscall"
                   : "=a"(ax)
                   : "0"(0x3000003), "D"(addr - 0x8a0 /* wat */)
                   : "rcx", "r11", "memory", "cc");
      return ax;
    case ARCH_GET_FS:
    case ARCH_SET_FS:
    case ARCH_GET_GS:
      return enosys();
    default:
      return einval();
  }
}

static int arch_prctl$openbsd(int code, int64_t addr) {
  int64_t rax;
  switch (code) {
    case ARCH_GET_FS:
      asm volatile("syscall"
                   : "=a"(rax)
                   : "0"(0x014a /* __get_tcb */)
                   : "rcx", "r11", "cc", "memory");
      *(int64_t *)addr = rax;
      return 0;
    case ARCH_SET_FS:
      asm volatile("syscall"
                   : "=a"(rax)
                   : "0"(0x0149 /* __set_tcb */), "D"(addr)
                   : "rcx", "r11", "cc", "memory");
      return 0;
    case ARCH_GET_GS:
    case ARCH_SET_GS:
      return enosys();
    default:
      return einval();
  }
}

static char g_fsgs_once;
static struct InterruptibleCall g_fsgs_icall;

/**
 * Don't bother.
 */
int arch_prctl(int code, int64_t addr) {
  void *fn = arch_prctl$fsgsbase;
  if (!g_fsgs_once) {
    g_fsgs_once = true;
    if (X86_HAVE(FSGSBASE)) {
      g_fsgs_icall.sig = SIGILL;
      if (interruptiblecall(&g_fsgs_icall, fn, code, addr, 0, 0) != -1 &&
          g_fsgs_icall.returnval != -1) {
        /* ivybridge+ (2012) lets us change segment registers without
           needing a 700ns system call. cpuid and /proc/cpuinfo will both
           report it's available; unfortunately, operating systems have an
           added ability to restrict this feature in %cr4, which we're not
           even allowed to read lool */
        g_fsgs_once = 2;
        return 0;
      }
    }
  }
  if (g_fsgs_once == 2) {
    return arch_prctl$fsgsbase(code, addr);
  }
  switch (__hostos) {
    case METAL:
      return arch_prctl$msr(code, addr);
    case FREEBSD:
      /* claims support but it appears not */
      return arch_prctl$freebsd(code, addr);
    case OPENBSD:
      return arch_prctl$openbsd(code, addr);
    case LINUX:
      return arch_prctl$sysv(code, addr);
    case XNU:
      /* probably won't work */
      return arch_prctl$xnu(code, addr);
    default:
      return enosys();
  }
}
