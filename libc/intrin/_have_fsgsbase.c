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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/fsgsbase.h"
#include "libc/nexgen32e/x86feature.h"

/**
 * Returns true if FSGSBASE ISA can be used.
 *
 * If this function returns true (Linux 5.9+ or FreeBSD) then you should
 * be able to read/write to the %gs / %fs x86 segment registers in about
 * one or two clock cycles which gives you a free addition operation for
 * all assembly ops that reference memory.
 *
 * The FSGSBASE ISA was introduced by Intel with Ivybridge (c. 2012) but
 * the Linux Kernel didn't authorize us to use it until 2020, once Intel
 * had to start backdooring customer kernels so that they could have it.
 * AMD introduced support for the FSGSBASE ISA in Excavator, aka bdver4.
 *
 * @return boolean indicating if feature can be used
 * @see _rdfsbase()
 * @see _rdgsbase()
 * @see _wrfsbase()
 * @see _wrgsbase()
 */
privileged int _have_fsgsbase(void) {
  // Linux 5.9 (c. 2020) introduced close_range() and fsgsbase support.
  // it's cheaper to test for close_range() than handle an op crashing.
  // Windows lets us use these instructions but they don't really work.
  int ax;
  if (X86_HAVE(FSGSBASE)) {
    if (IsLinux()) {
      asm volatile("syscall"
                   : "=a"(ax)
                   : "0"(436 /* close_range */), "D"(-1), "S"(-2), "d"(0)
                   : "rcx", "r11", "memory");
      return ax == -22;  // EINVAL
    } else if (IsFreebsd()) {
      return 1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}
