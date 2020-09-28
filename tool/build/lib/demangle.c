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
#include "libc/calls/calls.h"
#include "libc/calls/hefty/spawn.h"
#include "libc/macros.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "tool/build/lib/demangle.h"

struct CxxFilt {
  int pid;
  int fds[3];
} g_cxxfilt;

void CloseCxxFilt(void) {
  close(g_cxxfilt.fds[0]);
  close(g_cxxfilt.fds[1]);
  g_cxxfilt.pid = -1;
}

void SpawnCxxFilt(void) {
  int pid;
  char path[PATH_MAX];
  if (commandv("c++filt", path)) {
    g_cxxfilt.fds[0] = -1;
    g_cxxfilt.fds[1] = -1;
    g_cxxfilt.fds[2] = 2;
    if ((pid = spawnve(0, g_cxxfilt.fds, path, (char *const[]){"c++filt", NULL},
                       environ)) != -1) {
      atexit(CloseCxxFilt);
    }
  } else {
    pid = -1;
  }
  g_cxxfilt.pid = pid;
}

char *DemangleCxxFilt(char *p, const char *symbol) {
  int n;
  char buf[512];
  bool iscomplicated;
  if (!g_cxxfilt.pid) SpawnCxxFilt();
  if (g_cxxfilt.pid == -1) return NULL;
  if ((n = strlen(symbol)) >= ARRAYLEN(buf)) return NULL;
  memcpy(buf, symbol, n);
  buf[n] = '\n';
  write(g_cxxfilt.fds[0], buf, n + 1);
  n = read(g_cxxfilt.fds[1], buf, ARRAYLEN(buf));
  if (n > 1 && buf[n - 1] == '\n') {
    if (buf[n - 2] == '\r') --n;
    --n;
    iscomplicated = memchr(buf, ' ', n) || memchr(buf, '(', n);
    if (iscomplicated) *p++ = '"';
    p = mempcpy(p, buf, n);
    if (iscomplicated) *p++ = '"';
    *p = '\0';
    return p;
  } else {
    CloseCxxFilt();
    return NULL;
  }
}

/**
 * Decrypts C++ symbol.
 *
 * Decoding these takes roughly the same lines of code as an entire
 * x86_64 disassembler. That's just for the GNU encoding scheme. So
 * what we'll do, is just offload this work to the c++filt program.
 */
char *Demangle(char *p, const char *symbol) {
  char *r;
  if (startswith(symbol, "_Z")) {
    if ((r = DemangleCxxFilt(p, symbol))) return r;
  }
  return stpcpy(p, symbol);
}
