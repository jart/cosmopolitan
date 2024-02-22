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
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/log/log.h"
#include "libc/proc/posix_spawn.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"

static volatile bool g_timed_out;

static void finish(void) {
  if (!IsWindows()) {
    _exit(0);
  }
}

static void timeout(int sig) {
  g_timed_out = true;
}

static void failure(const char *url, const char *cmd, const char *reason) {
  WARNF("(srvr) failed to open %s in a browser tab using %s: %s", url, cmd,
        reason);
}

/**
 * Opens browser tab on host system.
 */
void launch_browser(const char *url) {

  // perform this task from a subprocess so it doesn't block server
  if (!IsWindows()) {
    switch (fork()) {
      case 0:
        break;
      default:
        return;
      case -1:
        perror("fork");
        return;
    }
  }

  // determine which command opens browser tab
  const char *cmd;
  if (IsWindows()) {
    cmd = "/c/windows/explorer.exe";
  } else if (IsXnu()) {
    cmd = "open";
  } else {
    cmd = "xdg-open";
  }

  // spawn process
  // set process group so ctrl-c won't kill browser
  int pid, err;
  posix_spawnattr_t sa;
  char *args[] = {(char *)cmd, (char *)url, 0};
  posix_spawnattr_init(&sa);
  posix_spawnattr_setflags(&sa, POSIX_SPAWN_SETPGROUP);
  err = posix_spawnp(&pid, cmd, 0, &sa, args, environ);
  posix_spawnattr_destroy(&sa);
  if (err) {
    failure(url, cmd, strerror(err));
    return finish();
  }

  // kill command if it takes more than three seconds
  // we need it because xdg-open acts weird on headless systems
  struct sigaction hand;
  hand.sa_flags = 0;
  sigemptyset(&hand.sa_mask);
  hand.sa_handler = timeout;
  sigaction(SIGALRM, &hand, 0);
  alarm(3);

  // wait for tab to return finish opening
  // the browser will still be running after this completes
  int ws;
  while (waitpid(pid, &ws, 0) == -1) {
    if (errno != EINTR) {
      failure(url, cmd, strerror(errno));
      kill(pid, SIGKILL);
      return finish();
    }
    if (g_timed_out) {
      failure(url, cmd, "process timed out");
      kill(pid, SIGKILL);
      return finish();
    }
  }
  if (ws) {
    failure(url, cmd, "process exited with non-zero status");
  }

  // we're done
  return finish();
}
