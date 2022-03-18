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
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/str/str.h"

static char *g_comdbg;
static char g_comdbg_buf[PATH_MAX + 1];

static optimizesize textstartup void g_comdbg_init() {
  char *p;
  size_t n;
  static bool once;
  if (!once) {
    if (!(g_comdbg = getenv("COMDBG"))) {
      p = program_executable_name;
      n = strlen(p);
      if (n > 4 && READ32LE(p + n - 4) == READ32LE(".dbg")) {
        g_comdbg = p;
      } else if (n > 4 && READ32LE(p + n - 4) == READ32LE(".com") &&
                 n + 4 <= PATH_MAX) {
        mempcpy(mempcpy(g_comdbg_buf, p, n), ".dbg", 5);
        if (fileexists(g_comdbg_buf)) {
          g_comdbg = g_comdbg_buf;
        }
      } else if (n + 8 <= PATH_MAX) {
        mempcpy(mempcpy(g_comdbg_buf, p, n), ".com.dbg", 9);
        if (fileexists(g_comdbg_buf)) {
          g_comdbg = g_comdbg_buf;
        }
      }
    }
    once = true;
  }
}

/**
 * Returns path of binary with the debug information, or null.
 *
 * @return path to debug binary, or NULL
 */
const char *FindDebugBinary(void) {
  g_comdbg_init();
  return g_comdbg;
}

const void *const g_comdbg_ctor[] initarray = {
    g_comdbg_init,
};
