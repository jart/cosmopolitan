/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "dsp/tty/tty.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/termios.h"
#include "libc/calls/ucontext.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/x/x.h"
#include "libc/x/xsigaction.h"

/* TODO(jart): DELETE */

#define FD STDOUT_FILENO

static struct TtyRaw {
  bool setup;
  bool hasold;
  bool noreentry;
  bool initialized;
  enum TtyRawFlags flags;
  sigaction_f next[10];
  unsigned char sigs[10];
  struct termios old;
} g_ttyraw;

static textstartup int ttyraw_setup(void) {
  struct termios *old;
  if (isatty(FD)) {
    old = !g_ttyraw.hasold ? &g_ttyraw.old : NULL;
    if (ttyconfig(FD, ttysetrawmode, g_ttyraw.flags, old) != -1) {
      g_ttyraw.hasold = true;
      return 0;
    }
  }
  return -1;
}

static textstartup int ttyraw_enable(void) {
  int rc;
  g_ttyraw.setup = (rc = ttyraw_setup()) != -1 || g_ttyraw.setup;
  return rc;
}

static textstartup void ttyraw_hidecursor(void) {
  if (!g_ttyraw.setup) return;
  if (g_ttyraw.flags & kTtyCursor) return;
  ttyhidecursor(FD);
}

static textexit int ttyraw_disable(void) {
  if (!g_ttyraw.setup) return 0;
  ttyshowcursor(FD);
  return ttyrestore(FD, &g_ttyraw.old);
}

static textexit void ttyraw_onexit(void) {
  ttyraw_disable();
}

static relegated void ttyraw_onsig(int sig, struct siginfo *info,
                                   struct ucontext *ctx) {
  size_t i;
  if (g_ttyraw.noreentry) _Exit(128 + sig);
  g_ttyraw.noreentry = true;
  if (g_ttyraw.flags != -1) {
    if (sig == SIGCONT) {
      ttyraw_enable();
    } else {
      ttyraw_disable();
    }
  }
  for (i = 0; i < ARRAYLEN(g_ttyraw.sigs); ++i) {
    if (g_ttyraw.sigs[i] == sig) {
      if (g_ttyraw.next[i] != (void *)SIG_IGN) {
        if (g_ttyraw.next[i] != (void *)SIG_DFL) {
          if (g_ttyraw.next[i]) {
            g_ttyraw.next[i](sig, info, ctx);
          }
        } else if (sig != SIGCONT) {
          _Exit(128 + sig);
        }
      }
      break;
    }
  }
  g_ttyraw.noreentry = false;
}

static textstartup void ttyraw_initsig(int sig, unsigned flags, unsigned mask) {
  static unsigned i;
  struct sigaction old;
  g_ttyraw.next[i] = xsigaction(sig, ttyraw_onsig, flags, mask, &old) != -1
                         ? old.sa_sigaction
                         : (void *)SIG_DFL;
  g_ttyraw.sigs[i++] = sig;
}

static textstartup void ttyraw_init(void) {
  unsigned crashmask = ~(SIGILL | SIGBUS | SIGSEGV | SIGABRT);
  ttyraw_initsig(SIGILL, SA_SIGINFO | SA_NODEFER | SA_ONSTACK, crashmask);
  ttyraw_initsig(SIGSEGV, SA_SIGINFO | SA_NODEFER | SA_ONSTACK, crashmask);
  ttyraw_initsig(SIGBUS, SA_SIGINFO | SA_NODEFER | SA_ONSTACK, crashmask);
  ttyraw_initsig(SIGABRT, SA_SIGINFO | SA_NODEFER, crashmask);
  ttyraw_initsig(SIGFPE, SA_SIGINFO | SA_RESTART, crashmask);
  ttyraw_initsig(SIGTRAP, SA_SIGINFO | SA_RESTART, crashmask);
  ttyraw_initsig(SIGHUP, SA_SIGINFO | SA_RESTART, crashmask);
  ttyraw_initsig(SIGINT, SA_SIGINFO | SA_RESTART, crashmask);
  ttyraw_initsig(SIGQUIT, SA_SIGINFO | SA_RESTART, crashmask);
  ttyraw_initsig(SIGCONT, SA_SIGINFO | SA_RESTART, crashmask);
  atexit(ttyraw_onexit);
}

/**
 * Sets raw mode, safely, the easy way.
 *
 * This should be called after your signal handlers have been installed.
 */
textstartup int ttyraw(enum TtyRawFlags flags) {
  int rc;
  if ((g_ttyraw.flags = flags) != -1) {
    if (!g_ttyraw.initialized) {
      g_ttyraw.initialized = true;
      ttyraw_init();
    }
    if ((rc = ttyraw_enable()) != -1) {
      ttyraw_hidecursor();
    }
  } else {
    rc = ttyraw_disable();
  }
  return rc;
}
