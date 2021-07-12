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
#include "libc/alg/bisectcarleft.internal.h"
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/repmovsb.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/nexgen32e/rdtscp.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/str/str.h"
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

void ftrace_hook(void);

static int noreentry;
static uint64_t laststamp;
static char g_buf[512];
static const char *g_lastsymbol;
static struct SymbolTable *g_symbols;

static noasan int GetNestingLevel(struct StackFrame *frame) {
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
privileged noasan void ftracer(void) {
  char *p;
  uint64_t stamp;
  const char *symbol;
  struct StackFrame *frame;
  size_t nesting, symbolsize;
  if (!cmpxchg(&noreentry, 0, 1)) return;
  if (g_symbols) {
    stamp = rdtsc();
    frame = __builtin_frame_address(0);
    frame = frame->next;
    symbol =
        &g_symbols->name_base[g_symbols
                                  ->symbols[bisectcarleft(
                                      (const int32_t(*)[2])g_symbols->symbols,
                                      g_symbols->count,
                                      frame->addr - g_symbols->addr_base)]
                                  .name_rva];
    if (symbol != g_lastsymbol) {
      symbolsize = strlen(symbol);
      nesting = GetNestingLevel(frame);
      if (2 + nesting * 2 + symbolsize + 1 + 21 + 2 <= ARRAYLEN(g_buf)) {
        p = g_buf;
        *p++ = '+';
        *p++ = ' ';
        memset(p, ' ', nesting * 2);
        p += nesting * 2;
        p = mempcpy(p, symbol, symbolsize);
        *p++ = ' ';
        p += uint64toarray_radix10((stamp - laststamp) / 3.3, p);
        *p++ = '\r';
        *p++ = '\n';
        write(2, g_buf, p - g_buf);
      }
      g_lastsymbol = symbol;
      laststamp = X86_HAVE(RDTSCP) ? rdtscp(0) : rdtsc();
    }
  }
  noreentry = 0;
}

textstartup void ftrace_install(void) {
  if ((g_symbols = OpenSymbolTable(FindDebugBinary()))) {
    __hook(ftrace_hook, g_symbols);
  } else {
    write(2, "error: --ftrace needs the concomitant .com.dbg binary\n", 54);
  }
}
