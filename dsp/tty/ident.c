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
#include "libc/calls/calls.h"
#include "libc/calls/termios.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/errfuns.h"

/* TODO(jart): DELETE */

static int ttyident_probe(struct TtyIdent *ti, int ttyinfd, int ttyoutfd,
                          const char *msg) {
  ssize_t rc;
  size_t got;
  char buf[64];
  int id, version;
  if ((rc = write(ttyoutfd, msg, strlen(msg))) != -1) {
  TryAgain:
    if ((rc = read(ttyinfd, buf, sizeof(buf))) != -1) {
      buf[min((got = (size_t)rc), sizeof(buf) - 1)] = '\0';
      if (sscanf(buf, "\e[>%d;%d", &id, &version) >= 1) {
        ti->id = id;
        ti->version = version;
        rc = 0;
      } else {
        rc = eio();
      }
    } else if (errno == EINTR) {
      goto TryAgain;
    } else if (errno == EAGAIN) {
      if (poll((struct pollfd[]){{ttyinfd, POLLIN}}, 1, 100) != 0) {
        goto TryAgain;
      } else {
        rc = etimedout();
      }
    }
  }
  return rc;
}

/**
 * Identifies teletypewriter.
 *
 * For example, we can tell if process is running in a GNU Screen
 * session Gnome Terminal.
 *
 * @return object if TTY responds, or NULL w/ errno
 * @see ttyidentclear()
 */
int ttyident(struct TtyIdent *ti, int ttyinfd, int ttyoutfd) {
  int rc;
  struct termios old;
  struct TtyIdent outer;
  rc = -1;
  if (!IsWindows()) {
    if (ttyconfig(ttyinfd, ttysetrawdeadline, 3, &old) != -1) {
      if (ttyident_probe(ti, ttyinfd, ttyoutfd, "\e[>c") != -1) {
        rc = 0;
        memset(&outer, 0, sizeof(outer));
        if (ti->id == 83 /* GNU Screen */ && (ti->next || _weaken(malloc)) &&
            ttyident_probe(&outer, ttyinfd, ttyoutfd, "\eP\e[>c\e\\") != -1 &&
            (ti->next =
                 (ti->next ? ti->next
                           : _weaken(malloc)(sizeof(struct TtyIdent))))) {
          memcpy(ti->next, &outer, sizeof(outer));
        } else {
          free(ti->next);
          ti->next = 0;
        }
      }
      ttyrestore(ttyinfd, &old);
    }
  }
  return rc;
}
