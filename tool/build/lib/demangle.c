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
#include "tool/build/lib/demangle.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/iovec.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/limits.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"

struct CxxFilt {
  int pid;
  int reader;
  int writer;
} g_cxxfilt;

void CloseCxxFilt(void) {
  close(g_cxxfilt.reader);
  close(g_cxxfilt.writer);
  g_cxxfilt.pid = -1;
}

void SpawnCxxFilt(void) {
  int pipefds[2][2];
  const char *cxxfilt;
  char path[PATH_MAX];
  cxxfilt = firstnonnull(emptytonull(getenv("CXXFILT")), "c++filt");
  if (commandv(cxxfilt, path, sizeof(path))) {
    pipe2(pipefds[0], O_CLOEXEC);
    pipe2(pipefds[1], O_CLOEXEC);
    if (!(g_cxxfilt.pid = vfork())) {
      dup2(pipefds[1][0], 0);
      dup2(pipefds[0][1], 1);
      execv(path, (char *const[]){(char *)cxxfilt, 0});
      abort();
    }
    g_cxxfilt.reader = pipefds[0][0];
    g_cxxfilt.writer = pipefds[1][1];
    atexit(CloseCxxFilt);
  } else {
    g_cxxfilt.pid = -1;
  }
}

char *CopySymbol(char *p, size_t pn, const char *s, size_t sn) {
  size_t extra;
  bool showdots, iscomplicated;
  assert(pn >= 1 + 3 + 1 + 1);
  iscomplicated = memchr(s, ' ', sn) || memchr(s, '(', sn);
  extra = 1;
  if (iscomplicated) extra += 2;
  if (sn + extra > pn) {
    sn = pn - extra - 3;
    showdots = true;
  } else {
    showdots = false;
  }
  if (iscomplicated) *p++ = '"';
  p = mempcpy(p, s, sn);
  if (showdots) p = stpcpy(p, "...");
  if (iscomplicated) *p++ = '"';
  *p = '\0';
  return p;
}

char *DemangleCxxFilt(char *p, size_t pn, const char *s, size_t sn) {
  ssize_t rc;
  size_t got;
  struct iovec iov[2];
  static char buf[4096];
  if (!g_cxxfilt.pid) SpawnCxxFilt();
  if (g_cxxfilt.pid == -1) return NULL;
  buf[0] = '\n';
  iov[0].iov_base = (void *)s;
  iov[0].iov_len = sn;
  iov[1].iov_base = buf;
  iov[1].iov_len = 1;
  writev(g_cxxfilt.writer, iov, ARRAYLEN(iov));
  if ((rc = read(g_cxxfilt.reader, buf, sizeof(buf))) != -1) {
    got = rc;
    if (got >= 2 && buf[got - 1] == '\n') {
      if (buf[got - 2] == '\r') --got;
      --got;
      return CopySymbol(p, pn, buf, got);
    }
  }
  CloseCxxFilt();
  return NULL;
}

/**
 * Decrypts C++ symbol.
 *
 * Decoding these takes roughly the same lines of code as an entire
 * x86_64 disassembler. That's just for the GNU encoding scheme. So
 * what we'll do, is just offload this work to the c++filt program.
 */
char *Demangle(char *p, const char *symbol, size_t n) {
  char *r;
  size_t sn;
  sn = strlen(symbol);
  if (startswith(symbol, "_Z")) {
    if ((r = DemangleCxxFilt(p, n, symbol, sn))) return r;
  }
  return CopySymbol(p, n, symbol, sn);
}
