/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/sysv/consts/hwcap.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/auxv.h"

int main(int argc, char *argv[]) {
#ifdef __aarch64__

  long x = getauxval(AT_HWCAP);

  if (x & HWCAP_FP) puts("HWCAP_FP");
  if (x & HWCAP_ASIMD) puts("HWCAP_ASIMD");
  if (x & HWCAP_EVTSTRM) puts("HWCAP_EVTSTRM");
  if (x & HWCAP_AES) puts("HWCAP_AES");
  if (x & HWCAP_PMULL) puts("HWCAP_PMULL");
  if (x & HWCAP_SHA1) puts("HWCAP_SHA1");
  if (x & HWCAP_SHA2) puts("HWCAP_SHA2");
  if (x & HWCAP_CRC32) puts("HWCAP_CRC32");
  if (x & HWCAP_ATOMICS) puts("HWCAP_ATOMICS");
  if (x & HWCAP_FPHP) puts("HWCAP_FPHP");
  if (x & HWCAP_ASIMDHP) puts("HWCAP_ASIMDHP");
  if (x & HWCAP_CPUID) puts("HWCAP_CPUID");
  if (x & HWCAP_ASIMDRDM) puts("HWCAP_ASIMDRDM");
  if (x & HWCAP_JSCVT) puts("HWCAP_JSCVT");
  if (x & HWCAP_FCMA) puts("HWCAP_FCMA");
  if (x & HWCAP_LRCPC) puts("HWCAP_LRCPC");
  if (x & HWCAP_DCPOP) puts("HWCAP_DCPOP");
  if (x & HWCAP_SHA3) puts("HWCAP_SHA3");
  if (x & HWCAP_SM3) puts("HWCAP_SM3");
  if (x & HWCAP_SM4) puts("HWCAP_SM4");
  if (x & HWCAP_ASIMDDP) puts("HWCAP_ASIMDDP");
  if (x & HWCAP_SHA512) puts("HWCAP_SHA512");
  if (x & HWCAP_SVE) puts("HWCAP_SVE");
  if (x & HWCAP_ASIMDFHM) puts("HWCAP_ASIMDFHM");
  if (x & HWCAP_DIT) puts("HWCAP_DIT");
  if (x & HWCAP_USCAT) puts("HWCAP_USCAT");
  if (x & HWCAP_ILRCPC) puts("HWCAP_ILRCPC");
  if (x & HWCAP_FLAGM) puts("HWCAP_FLAGM");
  if (x & HWCAP_SSBS) puts("HWCAP_SSBS");
  if (x & HWCAP_SB) puts("HWCAP_SB");
  if (x & HWCAP_PACA) puts("HWCAP_PACA");
  if (x & HWCAP_PACG) puts("HWCAP_PACG");

  x = getauxval(AT_HWCAP2);

  if (x & HWCAP2_DCPODP) puts("HWCAP2_DCPODP");
  if (x & HWCAP2_SVE2) puts("HWCAP2_SVE2");
  if (x & HWCAP2_SVEAES) puts("HWCAP2_SVEAES");
  if (x & HWCAP2_SVEPMULL) puts("HWCAP2_SVEPMULL");
  if (x & HWCAP2_SVEBITPERM) puts("HWCAP2_SVEBITPERM");
  if (x & HWCAP2_SVESHA3) puts("HWCAP2_SVESHA3");
  if (x & HWCAP2_SVESM4) puts("HWCAP2_SVESM4");
  if (x & HWCAP2_FLAGM2) puts("HWCAP2_FLAGM2");
  if (x & HWCAP2_FRINT) puts("HWCAP2_FRINT");
  if (x & HWCAP2_SVEI8MM) puts("HWCAP2_SVEI8MM");
  if (x & HWCAP2_SVEF32MM) puts("HWCAP2_SVEF32MM");
  if (x & HWCAP2_SVEF64MM) puts("HWCAP2_SVEF64MM");
  if (x & HWCAP2_SVEBF16) puts("HWCAP2_SVEBF16");
  if (x & HWCAP2_I8MM) puts("HWCAP2_I8MM");
  if (x & HWCAP2_BF16) puts("HWCAP2_BF16");
  if (x & HWCAP2_DGH) puts("HWCAP2_DGH");
  if (x & HWCAP2_RNG) puts("HWCAP2_RNG");
  if (x & HWCAP2_BTI) puts("HWCAP2_BTI");
  if (x & HWCAP2_MTE) puts("HWCAP2_MTE");

#endif /* __aarch64__ */
}
