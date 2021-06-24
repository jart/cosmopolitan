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
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nexgen32e/kcpuids.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/nexgen32e/vendor.internal.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/nexgen32e/x86info.h"
#include "libc/nt/runtime.h"
#include "libc/rand/rand.h"
#include "libc/rand/xorshift.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/grnd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

static bool have_getrandom;

/**
 * Returns cryptographic random data.
 *
 * This random number seed generator blends information from:
 *
 * - getrandom() on Linux
 * - getentropy() on XNU and OpenBSD
 * - sysctl(KERN_ARND) on FreeBSD and NetBSD
 * - RDSEED on Broadwell+ and Xen+ unless GRND_NORDRND
 * - RDRAND on Ivybridge+ and Xen+ unless GRND_NORDRND|GRND_RANDOM
 *
 * The following flags may be specified:
 *
 * - GRND_NORDRND: Don't source rando from hardware.
 * - GRND_NOSYSTEM: Don't source rando from operating system.
 * - GRND_RANDOM: Halt the entire system while I tap an entropy pool
 *   so small that it's hard to use statistics to test if it's random
 * - GRND_NONBLOCK: Do not wait for i/o events or me to jiggle my
 *   mouse, and instead return immediately the moment data isn't
 *   available, even if the result needs to be -1 w/ EAGAIN
 *
 * This function is safe to use with fork() and vfork(). It will also
 * close any file descriptor it ends up needing before it returns.
 *
 * @asyncsignalsafe
 * @vforksafe
 */
ssize_t getrandom(void *p, size_t n, unsigned f) {
  char cf;
  ssize_t rc;
  uint64_t x;
  size_t i, j, m;
  int fd, cmd[2];
  sigset_t neu, old;
  if (n > 256) n = 256;
  if (!IsTiny() &&
      (f & ~(GRND_RANDOM | GRND_NONBLOCK | GRND_NORDRND | GRND_NOSYSTEM))) {
    return einval();
  }
  if (!(f & GRND_NOSYSTEM)) {
    if (IsWindows()) {
      if (RtlGenRandom(p, n)) {
        rc = n;
      } else {
        return __winerr();
      }
    } else if (IsFreebsd() || IsNetbsd()) {
      if (IsFreebsd()) {
        cmd[0] = 1;  /* CTL_KERN */
        cmd[1] = 37; /* KERN_ARND */
      } else {
        cmd[0] = 1;  /* CTL_KERN */
        cmd[1] = 81; /* KERN_ARND */
      }
      m = n;
      if ((rc = sysctl(cmd, 2, p, &m, 0, 0)) != -1) {
        rc = m;
      }
    } else if (have_getrandom) {
      if ((rc = sys_getrandom(p, n, f & (GRND_RANDOM | GRND_NONBLOCK))) != -1) {
        if (!rc && (IsXnu() || IsOpenbsd())) {
          rc = n;
        }
      }
    } else if ((fd = __sys_openat(
                    AT_FDCWD,
                    (f & GRND_RANDOM) ? "/dev/random" : "/dev/urandom",
                    O_RDONLY | ((f & GRND_NONBLOCK) ? O_NONBLOCK : 0), 0)) !=
               -1) {
      rc = sys_read(fd, p, n);
      sys_close(fd);
    } else {
      return enosys();
    }
  } else {
    memset(p, 0, n);
    rc = n;
  }
  if (rc != -1) {
    if (!IsTiny()) {
      if (rc < 0 || rc > n) {
        abort();
      }
      if (f & (GRND_RANDOM | GRND_NONBLOCK)) {
        if (n && !rc) {
          abort();
        }
      } else {
        if (rc != n) {
          abort();
        }
      }
    }
    if (!(f & GRND_NORDRND)) {
      if (X86_HAVE(RDSEED)) {
        for (i = j = 0; i < rc; ++j) {
          /* CF=1: Destination register valid. Quoth Intel DRNG-SIG 4.1.3 */
          asm volatile(CFLAG_ASM("rdseed\t%1")
                       : CFLAG_CONSTRAINT(cf), "=r"(x)
                       : /* no inputs */
                       : "cc");
          if (cf) {
            j = 0;
            if (i + 8 <= rc) {
              x ^= READ64LE((char *)p + i);
              WRITE64LE((char *)p + i, x);
              i += 8;
            } else {
              for (; i < rc; x >>= 8) {
                ((char *)p)[i++] ^= x;
              }
            }
          } else if (j == 10) {
            asm volatile("pause");
          }
        }
        rc = i;
      } else if (X86_HAVE(RDRND)) {
        for (i = j = 0; i < rc; ++j) {
          /* CF=1: Destination register valid. Quoth Intel DRNG-SIG 4.1.3 */
          asm volatile(CFLAG_ASM("rdrand\t%1")
                       : CFLAG_CONSTRAINT(cf), "=r"(x)
                       : /* no inputs */
                       : "cc");
          if (cf) {
            j = 0;
            if (i + 8 <= rc) {
              x ^= READ64LE((char *)p + i);
              WRITE64LE((char *)p + i, x);
              i += 8;
            } else {
              for (; i < rc; x >>= 8) {
                ((char *)p)[i++] ^= x;
              }
            }
          } else if (j == 10) {
            asm volatile("pause");
          }
        }
        rc = i;
      } else if (f & GRND_NOSYSTEM) {
        return enosys();
      }
    }
  }
  return rc;
}

static textstartup void getrandom_init(int argc, char **argv, char **envp,
                                       intptr_t *auxv) {
  extern unsigned kMutableCpuids[KCPUIDS_LEN][4] asm("kCpuids");
  /*
   * Clear RDRAND on AMD models before Zen and then some
   * since it's not only slow but can freeze after sleep
   * https://bugzilla.redhat.com/show_bug.cgi?id=1150286
   */
  if ((X86_HAVE(RDRND) || X86_HAVE(RDSEED)) &&
      (IsAuthenticAMD() &&
       (kX86CpuFamily < 0x17 ||
        (kX86CpuFamily == 0x17 &&
         (0x70 <= kX86CpuModel && kX86CpuModel <= 0x7F))))) {
    kMutableCpuids[KCPUIDS_1H][KCPUIDS_ECX] &= ~(1u << 30);
    kMutableCpuids[KCPUIDS_7H][KCPUIDS_EBX] &= ~(1u << 18);
  }
  if (sys_getrandom(0, 0, 0) == 0) {
    have_getrandom = true;
  }
}

const void *const g_getrandom_init[] initarray = {getrandom_init};
