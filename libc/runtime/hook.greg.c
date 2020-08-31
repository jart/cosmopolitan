/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.h"
#include "libc/sysv/consts/prot.h"

/**
 * Rewrites code in memory to hook function calls.
 *
 * We do this by searching each function for the nop instruction
 * inserted by GCC when we use the -pg -mnop-mcount flags. There's no
 * risk of corrupting data since the linker scripts won't mix code and
 * data.
 *
 * Modules built with -O3 and without the profiling flags might have
 * these same nop instructions, but that shouldn't be problematic since
 * they're only there for the puposes of aligning jumps, and therefore
 * aren't actually executed. However codebases that use huge function
 * alignments with wide-nop slides could pose minor issues. Further note
 * that Cosmopolitan sources are almost never intentionally written to
 * use code alignment, since we've only seen a few cases where it helps.
 *
 * @see ape/ape.lds
 */
privileged void __hook(void ifunc(void), struct SymbolTable *symbols) {
  size_t i;
  intptr_t addr;
  sigset_t oldmask;
  uint64_t code, mcode;
  unsigned char *p, *pe;
  const intptr_t kMcount = (intptr_t)&mcount;
  const intptr_t kProgramCodeStart = (intptr_t)&_ereal;
  const intptr_t kPrivilegedStart = (intptr_t)&__privileged_start;
  const bool kIsBinaryAligned = !(kPrivilegedStart & (PAGESIZE - 1));
  sigprocmask(SIG_BLOCK, &kSigsetFull, &oldmask);
  if (mprotect((void *)symbols->addr_base,
               kPrivilegedStart - symbols->addr_base,
               kIsBinaryAligned ? PROT_READ | PROT_WRITE
                                : PROT_READ | PROT_WRITE | PROT_EXEC) != -1) {
    for (i = 0; i < symbols->count - 1; ++i) {
      if (symbols->addr_base + symbols->symbols[i].addr_rva <
          kProgramCodeStart) {
        continue; /* skip over real mode symbols */
      }
      if (symbols->addr_base + symbols->symbols[i].addr_rva >=
          kPrivilegedStart) {
        break; /* stop before privileged symbols */
      }
      for (p = (unsigned char *)(symbols->addr_base +
                                 symbols->symbols[i].addr_rva),
          pe = (unsigned char *)(symbols->addr_base +
                                 symbols->symbols[i + 1].addr_rva);
           p < pe - 8; ++p) {
        code = READ64LE(p);

        /*
         * Test for -mrecord-mcount (w/ -fpie or -fpic)
         *
         *   nopw 0x00(%rax,%rax,1)  ← morphed by package.com
         *   call *mcount(%rip)      ← linked w/o -static
         *   addr32 call mcount      ← relaxed w/ -static
         *   addr32 call mcount      ← relaxed w/ -static
         *
         * Note that gcc refuses to insert the six byte nop.
         */
        if ((code & 0x0000FFFFFFFFFFFF) == 0x0000441F0F66 ||
            (code & 0x0000FFFFFFFFFFFF) ==
                ((((kMcount - ((intptr_t)&p[2] + 4)) << 16) | 0xE867) &
                 0x0000FFFFFFFFFFFF) ||
            (code & 0x0000FFFFFFFFFFFF) ==
                ((((kMcount - ((intptr_t)&p[2] + 4)) << 16) | 0xFF15) &
                 0x0000FFFFFFFFFFFF)) {
          p[0] = 0x67;
          p[1] = 0xE8;
          addr = (intptr_t)ifunc - ((intptr_t)&p[2] + 4);
          p[2] = addr >> 000;
          p[3] = addr >> 010;
          p[4] = addr >> 020;
          p[5] = addr >> 030;
          break;
        }

        /*
         * Test for -mnop-mcount (w/ -fno-pie)
         */
        mcode = code & 0x000000FFFFFFFFFF;
        if ((mcode == 0x00441F0F /*   nopl 0x00(%eax,%eax,1) [canonical] */) ||
            (mcode == 0x00041F0F67 /* nopl (%eax,%eax,1)     [older gcc] */)) {
          if (p[-1] != 0x66 /*        nopw 0x0(%rax,%rax,1)  [donotwant] */) {
            p[0] = 0xE8 /* call Jvds */;
            addr = (intptr_t)ifunc - ((intptr_t)&p[1] + 4);
            p[1] = addr >> 000;
            p[2] = addr >> 010;
            p[3] = addr >> 020;
            p[4] = addr >> 030;
          }
          break;
        }
      }
    }
    mprotect((void *)symbols->addr_base, kPrivilegedStart - symbols->addr_base,
             PROT_READ | PROT_EXEC);
  }
  sigprocmask(SIG_SETMASK, &oldmask, NULL);
}
