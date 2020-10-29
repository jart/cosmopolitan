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
#include "libc/log/check.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/str/str.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"

#define STACKALIGN      16
#define LINUX_AT_EXECFN 31

static size_t GetArgListLen(char **p) {
  size_t n;
  for (n = 0; *p; ++p) ++n;
  return n;
}

static int64_t PushString(struct Machine *m, const char *s) {
  size_t n;
  int64_t sp;
  n = strlen(s) + 1;
  sp = Read64(m->sp);
  sp -= n;
  Write64(m->sp, sp);
  VirtualRecv(m, sp, s, n);
  return sp;
}

void LoadArgv(struct Machine *m, const char *prog, char **args, char **vars) {
  int64_t i, n, sp, *p, *bloc;
  size_t narg, nenv, naux, nall;
  DCHECK_NOTNULL(prog);
  DCHECK_NOTNULL(args);
  DCHECK_NOTNULL(vars);
  naux = 1;
  nenv = GetArgListLen(vars);
  narg = GetArgListLen(args);
  nall = 1 + 1 + narg + 1 + nenv + 1 + (naux + 1) * 2;
  bloc = gc(malloc(sizeof(int64_t) * nall));
  p = bloc + nall;
  *--p = 0;
  *--p = 0;
  *--p = PushString(m, prog);
  *--p = LINUX_AT_EXECFN;
  for (*--p = 0, i = nenv; i--;) *--p = PushString(m, vars[i]);
  for (*--p = 0, i = narg; i--;) *--p = PushString(m, args[i]);
  *--p = PushString(m, prog);
  *--p = 1 + narg;
  DCHECK_EQ(bloc, p);
  sp = Read64(m->sp);
  while ((sp - nall * sizeof(int64_t)) & (STACKALIGN - 1)) --sp;
  sp -= nall * sizeof(int64_t);
  DCHECK_EQ(0, sp % STACKALIGN);
  Write64(m->sp, sp);
  Write64(m->di, 0); /* or ape detects freebsd */
  VirtualRecv(m, sp, bloc, sizeof(int64_t) * nall);
}
