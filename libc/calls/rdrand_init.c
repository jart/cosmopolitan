/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/nexgen32e/kcpuids.h"
#include "libc/nexgen32e/vendor.internal.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/nexgen32e/x86info.h"
#include "libc/stdio/rand.h"

textstartup void rdrand_init(int argc, char **argv, char **envp,
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
}

const void *const g_rdrand_init[] initarray = {rdrand_init};
