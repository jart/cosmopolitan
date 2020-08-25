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
 * Installs plaintext function tracer. Do not call.
 * @see libc/runtime/_init.S for documentation
 */
textstartup void ftrace_init(void) {
  g_buf[0] = '+';
  g_buf[1] = ' ';
  if ((g_symbols = opensymboltable(finddebugbinary()))) {
    __hook(ftrace_hook, g_symbols);
  }
}
