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
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/sysinfo.h"
#include "libc/calls/struct/sysinfo.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/mem/mem.h"
#include "libc/nt/accounting.h"
#include "libc/nt/process.h"
#include "libc/nt/struct/memorystatusex.h"
#include "libc/nt/synchronization.h"

static textwindows uint16_t GetProcessCount(void) {
  uint16_t res;
  uint32_t have, got, *pids;
  uint32_t stack_memory[1000];
  have = 0xFFFF * 4;
  if (!_weaken(malloc) || !(pids = _weaken(malloc)(have))) {
    pids = stack_memory;
    have = sizeof(stack_memory);
  }
  if (EnumProcesses(pids, have, &got)) {
    res = got / 4;
  } else {
    res = 0;
  }
  if (pids != stack_memory && _weaken(free)) {
    _weaken(free)(pids);
  }
  return res;
}

textwindows int sys_sysinfo_nt(struct sysinfo *info) {
  int i;
  double load[3];
  struct NtMemoryStatusEx memstat;
  BLOCK_SIGNALS;
  if (sys_getloadavg_nt(load, 3) != -1) {
    for (i = 0; i < 3; ++i) {
      info->loads[i] = load[i] * 65536;
    }
  }
  memstat.dwLength = sizeof(struct NtMemoryStatusEx);
  if (GlobalMemoryStatusEx(&memstat)) {
    info->totalram = memstat.ullTotalPhys;
    info->freeram = memstat.ullAvailPhys;
  }
  info->uptime = GetTickCount64() / 1000;
  info->procs = GetProcessCount();
  info->mem_unit = 1;
  ALLOW_SIGNALS;
  return 0;
}
