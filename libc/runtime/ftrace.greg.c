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
#include "libc/alg/bisectcarleft.h"
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/macros.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/missioncritical.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"

/**
 * @fileoverview Plain-text function call logging.
 *
 * Able to log ~2 million function calls per second, which is mostly
 * bottlenecked by system call overhead. Log size is reasonable if piped
 * into gzip.
 */

static char g_buf[512];
static const char *g_lastsymbol;
static struct SymbolTable *g_symbols;

forceinline int getnestinglevel(struct StackFrame *frame) {
  int nesting = -2;
  while (frame) {
    ++nesting;
    frame = frame->next;
  }
  return max(0, nesting);
}

/**
 * Prints name of function being called.
 *
 * We insert CALL instructions that point to this function, in the
 * prologues of other functions. We assume those functions behave
 * according to the System Five NexGen32e ABI.
 */
privileged interruptfn void ftrace_hook(void) {
  size_t i, j, nesting;
  const char *symbol;
  struct StackFrame *frame;
  LOAD_DEFAULT_RBX();
  if (g_symbols && !(g_runstate & RUNSTATE_BROKEN)) {
    frame = __builtin_frame_address(0);
    symbol =
        &g_symbols->name_base[g_symbols
                                  ->symbols[bisectcarleft(
                                      (const int32_t(*)[2])g_symbols->symbols,
                                      g_symbols->count,
                                      frame->addr - g_symbols->addr_base)]
                                  .name_rva];
    if (symbol != g_lastsymbol &&
        (nesting = getnestinglevel(frame)) * 2 < ARRAYLEN(g_buf) - 3) {
      i = 2;
      j = 0;
      while (nesting--) {
        g_buf[i++] = ' ';
        g_buf[i++] = ' ';
      }
      while (i < ARRAYLEN(g_buf) - 2 && symbol[j]) {
        g_buf[i++] = symbol[j++];
      }
      g_buf[i++] = '\n';
      __print(g_buf, i);
    }
    g_lastsymbol = symbol;
  }
  RESTORE_RBX();
}

/**
 * Rewrites code in memory to log function calls.
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
privileged void ftrace_install(void) {
  /* TODO(jart): Is -fschedule-insns2 so aggro that we need XED here? */
  size_t i;
  intptr_t addr;
  sigset_t oldmask;
  uint64_t code, mcode;
  unsigned char *p, *pe;
  const intptr_t kMcount = (intptr_t)&mcount;
  const intptr_t kFtraceHook = (intptr_t)&ftrace_hook;
  const intptr_t kProgramCodeStart = (intptr_t)&_ereal;
  const intptr_t kPrivilegedStart = (intptr_t)&__privileged_start;
  const bool kIsBinaryAligned = !(kPrivilegedStart & (PAGESIZE - 1));
  g_buf[0] = '+';
  g_buf[1] = ' ';
  sigprocmask(SIG_BLOCK, &kSigsetFull, &oldmask);
  if (__mprotect((void *)g_symbols->addr_base,
                 kPrivilegedStart - g_symbols->addr_base,
                 kIsBinaryAligned ? PROT_READ | PROT_WRITE
                                  : PROT_READ | PROT_WRITE | PROT_EXEC) != -1) {
    for (i = 0; i < g_symbols->count - 1; ++i) {
      if (g_symbols->addr_base + g_symbols->symbols[i].addr_rva <
          kProgramCodeStart) {
        continue; /* skip over real mode symbols */
      }
      if (g_symbols->addr_base + g_symbols->symbols[i].addr_rva >=
          kPrivilegedStart) {
        break; /* stop before privileged symbols */
      }
      for (p = (unsigned char *)(g_symbols->addr_base +
                                 g_symbols->symbols[i].addr_rva),
          pe = (unsigned char *)(g_symbols->addr_base +
                                 g_symbols->symbols[i + 1].addr_rva);
           p < pe - 8; ++p) {
        code = read64le(p);

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
          addr = kFtraceHook - ((intptr_t)&p[2] + 4);
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
            addr = kFtraceHook - ((intptr_t)&p[1] + 4);
            p[1] = addr >> 000;
            p[2] = addr >> 010;
            p[3] = addr >> 020;
            p[4] = addr >> 030;
          }
          break;
        }
      }
    }
    __mprotect((void *)g_symbols->addr_base,
               kPrivilegedStart - g_symbols->addr_base, PROT_READ | PROT_EXEC);
  }
  sigprocmask(SIG_SETMASK, &oldmask, NULL);
}

/**
 * Installs plaintext function tracer. Do not call.
 * @see libc/runtime/_init.S for documentation
 */
textstartup void ftrace_init(void) {
  if ((g_symbols = opensymboltable(finddebugbinary()))) {
    ftrace_install();
  }
}
