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
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/asmflag.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nexgen32e/msr.internal.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/pc.internal.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"

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

int sys_set_tls();

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
      // sysarch(AMD64_GET_FSBASE)
      return sys_arch_prctl(128, addr);
    case ARCH_SET_FS:
      // sysarch(AMD64_SET_FSBASE)
      return sys_arch_prctl(129, (intptr_t)&addr);
    case ARCH_GET_GS:
      // sysarch(AMD64_GET_GSBASE)
      return sys_arch_prctl(130, addr);
    case ARCH_SET_GS:
      // sysarch(AMD64_SET_GSBASE)
      return sys_arch_prctl(131, (intptr_t)&addr);
    default:
      return einval();
  }
}

static int arch_prctl_netbsd(int code, int64_t addr) {
  switch (code) {
    case ARCH_GET_FS:
      // sysarch(X86_GET_FSBASE)
      return sys_arch_prctl(15, addr);
    case ARCH_SET_FS:
      // we use _lwp_setprivate() instead of sysarch(X86_SET_FSBASE)
      // because the latter has a bug where signal handlers cause it
      // to be clobbered. please note, this doesn't apply to %gs :-)
      return sys_set_tls(addr);
    case ARCH_GET_GS:
      // sysarch(X86_GET_GSBASE)
      return sys_arch_prctl(14, addr);
    case ARCH_SET_GS:
      // sysarch(X86_SET_GSBASE)
      return sys_arch_prctl(16, (intptr_t)&addr);
    default:
      return einval();
  }
}

static int arch_prctl_xnu(int code, int64_t addr) {
  int e;
  switch (code) {
    case ARCH_SET_GS:
      // thread_fast_set_cthread_self has a weird ABI
      e = errno;
      sys_set_tls(addr);
      errno = e;
      return 0;
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
                   : "rcx", "rdx", "rdi", "rsi", "r8", "r9", "r10", "r11", "cc",
                     "memory");
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
                   : "rcx", "rdx", "rsi", "r8", "r9", "r10", "r11", "cc",
                     "memory");
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

/**
 * Changes x86 segment registers.
 *
 * Support for segment registers is spotty across platforms. See the
 * table of tested combinations below.
 *
 * This wrapper has the same weird ABI as the Linux Kernel. The type
 * Cosmopolitan type signature of the prototype for this function is
 * variadic, so no value safety checking will be performed w/o asan.
 *
 * Cosmopolitan Libc initializes your process by default, to use the
 * segment register %fs, for thread-local storage. To safely disable
 * this TLS you need to either set `__tls_enabled` to 0, or you must
 * follow the same memory layout assumptions as your C library. When
 * TLS is disabled you can't use threads unless you call clone() and
 * that's not really a good idea since `errno` won't be thread-safe.
 *
 * Please note if you're only concerned about running on Linux, then
 * consider using Cosmopolitan's fsgsbase macros which don't need to
 * issue system calls to change %fs and %gs. See _have_fsgsbase() to
 * learn more.
 *
 * @param code may be
 *     - `ARCH_SET_FS` works on Linux, FreeBSD, NetBSD, OpenBSD, Metal
 *     - `ARCH_GET_FS` works on Linux, FreeBSD, NetBSD, OpenBSD, Metal
 *     - `ARCH_SET_GS` works on Linux, FreeBSD, NetBSD, XNU, Metal
 *     - `ARCH_GET_GS` works on Linux, FreeBSD, NetBSD, Metal
 * @param addr is treated as `intptr_t` when setting a register, and
 *     is an output parameter (i.e. `intptr_t *`) when reading one
 * @raise ENOSYS if operating system didn't support changing `code`
 * @raise EINVAL if `code` wasn't valid
 * @raise EFAULT if `ARCH_SET_FS` or `ARCH_SET_GS` was used and memory
 *     pointed to by `addr` was invalid
 * @see _have_fsgsbase()
 */
int arch_prctl(int code, int64_t addr) {
  int rc;
  if (IsAsan() &&               //
      (code == ARCH_GET_FS ||   //
       code == ARCH_GET_GS) &&  //
      !__asan_is_valid((int64_t *)addr, 8)) {
    rc = efault();
  } else {
    switch (__hostos) {
      case _HOSTMETAL:
        rc = arch_prctl_msr(code, addr);
        break;
      case _HOSTFREEBSD:
        rc = arch_prctl_freebsd(code, addr);
        break;
      case _HOSTNETBSD:
        rc = arch_prctl_netbsd(code, addr);
        break;
      case _HOSTOPENBSD:
        rc = arch_prctl_openbsd(code, addr);
        break;
      case _HOSTLINUX:
        rc = sys_arch_prctl(code, addr);
        break;
      case _HOSTXNU:
        rc = arch_prctl_xnu(code, addr);
        break;
      default:
        rc = enosys();
        break;
    }
  }
#ifdef SYSDEBUG
  if (!rc && (code == ARCH_GET_FS || code == ARCH_GET_GS)) {
    STRACE("arch_prctl(%s, [%p]) → %d% m", DescribeArchPrctlCode(code),
           *(int64_t *)addr, rc);
  } else {
    STRACE("arch_prctl(%s, %p) → %d% m", DescribeArchPrctlCode(code), addr, rc);
  }
#endif
  return rc;
}
